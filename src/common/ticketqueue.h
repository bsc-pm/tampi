/*
 * Copyright (c) 2017 Barcelona Supercomputing Center. All rights
 *                    reserved.
 *
 * This file is part of OmpSs-MPI interoperability library.
 *
 * OmpSs-MPI interop. library is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef TICKET_QUEUE_H
#define TICKET_QUEUE_H

#include "array_view.h"
#include "request.h"
#include "spin_mutex.h"
#include "ticket_decl.h"

#include <mutex>
#include <vector>

#include <mpi.h>

namespace nanos {
namespace mpi {

template < typename Ticket >
class TicketQueue {
   private:
      typedef typename Ticket::Request Request;
      typedef typename Ticket::Status  Status;

      struct TicketMapping {
         Ticket* ticket;
         int     request_position;

         TicketMapping( Ticket& t, int pos = 0 ) :
            ticket(&t), request_position(pos)
         {
         }
      };

      std::vector<Request>       _requests;
      std::vector<Status>        _statuses;
      std::vector<TicketMapping> _tickets;
      mutable spin_mutex   _mutex;

   public:
      TicketQueue() :
         _mutex(),
         _requests(),
         _statuses(),
         _tickets()
      {
      }

      TicketQueue ( const TicketQueue & ) = delete;

      const TicketQueue & operator= ( const TicketQueue & ) = delete;

      void add( Ticket& ticket, Request& req );

      void add( Ticket& ticket, util::array_view<Request> t_requests );

      void poll();

      void requestCompleted( int position ) {
         auto req_it  = std::next(_requests.begin(),position);
         auto stat_it = std::next(_statuses.begin(),position);
         auto tm_it   = std::next(_tickets.begin(),position);

         TicketMapping tm = *tm_it;
         tm.ticket->notifyCompletion( tm.request_position, _statuses[position] );

         _requests.erase(req_it);
         _statuses.erase(stat_it);
         _tickets.erase(tm_it);
      }
};

template<>
inline void TicketQueue<C::Ticket>::add( C::Ticket& ticket, C::Ticket::Request& req )
{
   int completed = 0;
   int err = PMPI_Test( &req, &completed, ticket.getStatus() );
   if( completed != 0 ) {
      std::lock_guard<spin_mutex> guard( _mutex );
      ticket.addPendingRequest();
      _requests.push_back( req );
      _statuses.emplace_back();
      _tickets.emplace_back( ticket );
   }
}

template<>
inline void TicketQueue<Fortran::Ticket>::add( Fortran::Ticket& ticket, Fortran::Ticket::Request& req )
{
   MPI_Fint completed = 0;
   MPI_Fint err = MPI_SUCCESS;
   pmpi_test_( &req, &completed, reinterpret_cast<MPI_Fint*>(ticket.getStatus()), &err );

   if( completed != 0 ) {
      std::lock_guard<spin_mutex> guard( _mutex );
      ticket.addPendingRequest();
      _requests.push_back( req );
      _statuses.emplace_back();
      _tickets.emplace_back( ticket );
   }
}

template<>
inline void TicketQueue<C::Ticket>::add( C::Ticket& ticket, util::array_view<C::Ticket::Request> t_requests )
{
   int count = t_requests.size();
   int indices[count]; // Array positions for completed requests
   int completed = 0;  // Number of completed requests
   int err = MPI_SUCCESS;

   err = PMPI_Testsome( count, t_requests.data(), indices, &completed, ticket.getStatus() );

   if( completed < count ) {
      std::lock_guard<spin_mutex> guard( _mutex );

      // Amortize cost of realloc when not enough capacity
      const size_t capacity = _requests.capacity();
      const size_t required_capacity = _requests.size() + (ticket.getPendingRequests() - completed);
      if( required_capacity > capacity ) {
         _requests.reserve( 2 * required_capacity );
         _statuses.reserve( 2 * required_capacity );
         _tickets.reserve ( 2 * required_capacity );
      }

      // Insert each uncompleted request
      // (requests not present in indices array)
      // only when its value is not MPI_REQUEST_NULL
      // (MPI_Testsome does not report inactive requests
      // as completed)
      int c = 0;
      for( int u = 0; u < count; ++u ) {
         if (c < completed && u == indices[c]) {
            c++;
         } else {
            int flag;
            C::Ticket::Request& req = t_requests[u];
            err = PMPI_Request_get_status( req, &flag, MPI_STATUS_IGNORE);
            if( flag == 0 ) {
               ticket.addPendingRequest();
               _requests.push_back( req );
               _statuses.emplace_back();
               _tickets.emplace_back( ticket, u );
            }
         }
      }
   }
}

template<>
inline void TicketQueue<Fortran::Ticket>::add( Fortran::Ticket& ticket, util::array_view<Fortran::Ticket::Request> t_requests )
{
   MPI_Fint count = t_requests.size();
   MPI_Fint indices[count];
   MPI_Fint completed = 0;
   MPI_Fint err = MPI_SUCCESS;

   pmpi_testsome_( &count, t_requests.data(), indices, &completed, reinterpret_cast<MPI_Fint*>(ticket.getStatus()), &err );

   if( completed < count ) {
      std::lock_guard<spin_mutex> guard( _mutex );

      // Amortize cost of realloc when not enough capacity
      const size_t capacity = _requests.capacity();
      const size_t required_capacity = _requests.size() + (ticket.getPendingRequests() - completed);
      if( required_capacity > capacity ) {
         _requests.reserve( 2 * required_capacity );
         _statuses.reserve( 2 * required_capacity );
         _tickets.reserve ( 2 * required_capacity );
      }

      // Insert each uncompleted request
      // (requests not present in indices array)
      // only when its value is not MPI_REQUEST_NULL
      // (MPI_Testsome does not report inactive requests
      // as completed)
      int c = 0;
      for( int u = 0; u < count; ++u ) {
         if (c < completed && u == (indices[c]-1)) {
            c++;
        } else {
           int flag;
           Fortran::Ticket::Request& req = t_requests[u];
           pmpi_request_get_status_( &req, &flag, MPI_F_STATUS_IGNORE, &err );
           if( flag == 0 ) {
              ticket.addPendingRequest();
              _requests.push_back( req );
              _statuses.emplace_back();
              _tickets.emplace_back( ticket, u );
           }
        }
      }
   }
}

template<>
inline void TicketQueue<C::Ticket>::poll() {
   if ( !_requests.empty() ) {
      std::unique_lock<spin_mutex> guard( _mutex, std::try_to_lock );

      if ( guard.owns_lock() && !_requests.empty() ) {
         int count = _requests.size();
         int completed = 0;
         int indices[count];
         int err = PMPI_Testsome( count, _requests.data(), &completed, indices, _statuses.data() );

         if( completed == MPI_UNDEFINED ) {
            // All requests were already released.
            // Consider skip deletion of completed requests and only clear
            // them whenever all of them are released.
            _requests.clear();
            _statuses.clear();
            _tickets.clear();
         } else {
            // Assumes indices array is sorted from lower to higher values
            // Iterate from the end to the beginning of the array to ensure
            // indexed element positions are not changed after a deletion
            for( int index : util::reverse(util::array_view<int>(indices,completed) ) )
            {
               requestCompleted( index );
            }
         }
      }
   }
}

template<>
inline void TicketQueue<Fortran::Ticket>::poll() {
   if ( !_requests.empty() ) {
      std::unique_lock<spin_mutex> guard( _mutex, std::try_to_lock );

      if ( guard.owns_lock() && !_requests.empty() ) {
         int count = _requests.size();
         int completed = 0;
         int indices[count];
         int err = MPI_SUCCESS;

         pmpi_testsome_( &count, _requests.data(), &completed, indices, MPI_F_STATUSES_IGNORE, &err );

         if( completed == MPI_UNDEFINED ) {
            // All requests are already released.
            // Consider skip deletion of completed requests and only clear
            // them whenever all of them are released.
            _requests.clear();
            _statuses.clear();
            _tickets.clear();
         } else {
            // Assumes indices array is sorted from lower to higher values
            // Iterate from the end to the beginning of the array to ensure
            // indexed element positions are not changed after a deletion
            for( int index : util::reverse(util::array_view<int>(indices,completed) ) )
            {
               // Take care with index values. Fortran API returns values in 1..N range
               // Fortran index values are 1 unit greater than C/C++ ones
               requestCompleted( index-1 );
            }
         }
      }
   }
}

} // namespace mpi
} // namespace nanos

#endif // TICKET_QUEUE_H
