#ifndef TICKET_QUEUE_H
#define TIQUET_QUEUE_H

#include "array_utils.h"
#include "mpi/request.h"
#include "spin_mutex.h"
#include "ticket_decl.h"

#include <mutex>
#include <vector>

#include <mpi.h>

namespace nanos {
namespace mpi {

template < typename Ticket >
int test( typename Ticket::Request* req, int* completed );

template< typename Ticket >
int testsome( int count, typename Ticket::Request* requests, int* completed, int* indices );

template<>
inline int test<C::Ticket>( MPI_Request* req, int* completed ) {
   return PMPI_Test( req, completed, MPI_STATUS_IGNORE );
}

template<>
inline int testsome<C::Ticket>( int count, MPI_Request* requests, int* completed, int* indices ) {
   return PMPI_Testsome( count, requests, indices, completed, MPI_STATUSES_IGNORE );
}

template<>
inline int test<Fortran::Ticket>( MPI_Fint* req, int* completed ) {
   MPI_Fint err;
   pmpi_test_( req, completed, MPI_F_STATUS_IGNORE, &err );
   return err;
}

template<>
inline int testsome<Fortran::Ticket>( int count, MPI_Fint* requests, int* completed, int* indices ) {
   MPI_Fint err;
   pmpi_testsome_( &count, requests, indices, completed, MPI_F_STATUSES_IGNORE, &err );
   return err;
}

template < typename Ticket >
class TicketQueue {
   private:
      typedef typename Ticket::Request Request;

      std::vector<Request> _requests;
      std::vector<Ticket*> _tickets;
      mutable spin_mutex   _mutex;

   public:
      TicketQueue() :
         _mutex(),
         _requests(),
         _tickets()
      {
      }

      TicketQueue ( const TicketQueue & ) = delete;

      const TicketQueue & operator= ( const TicketQueue & ) = delete;

      bool empty () const {
         return _requests.empty();
      }

      void add( Ticket& ticket, Request& req )
      {
         int completed = 0;
         int err = test<Ticket>( &req, &completed );
         if( !completed ) {
            std::lock_guard<spin_mutex> guard( _mutex );
            _requests.push_back( req );
            _tickets.push_back( &ticket );
         } else {
            ticket.notifyCompletion();
         }
      }

      void add( Ticket& ticket, Request* first, Request* last )
      {
         int count = std::distance(first,last);
         int indices[count];
         int completed = 0;

         testsome<Ticket>( count, first, indices, &completed );
         ticket.notifyCompletion(completed);

         if( count > completed ) {
            std::lock_guard<spin_mutex> guard( _mutex );

            const size_t capacity = _requests.capacity() - _requests.size() + count - completed;

            _requests.reserve(capacity);
            _tickets.reserve(capacity);

            for( int i : indices ) {
               _requests.push_back( first[i] );
               _tickets.push_back( &ticket );
            }
         }
      }

      void poll() {
         if ( !_requests.empty() ) {
            std::unique_lock<spin_mutex> guard( _mutex, std::try_to_lock );
            if ( guard.owns_lock() && !_requests.empty() ) {
               int count = _requests.size();
               int completed = 0;
               int indices[count];
               int err = testsome<Ticket>( count, _requests.data(), &completed, indices );

               if( completed == MPI_UNDEFINED ) {
                  // All requests are already released.
                  // Consider skip deletion of completed requests and only clear
                  // them whenever all of them are released.
                  _requests.clear();
                  _tickets.clear();
               } else {
                  // Assumes indices array is sorted from lower to higher values
                  // Iterate from the end to the beginning of the array to ensure
                  // indexed element positions are not changed after a deletion
                  for( int* indexIt = &indices[completed-1]; indexIt != &indices[-1]; --indexIt )
                  {
                     auto requestIt = std::next( _requests.begin(), *indexIt );
                     auto ticketIt  = std::next( _tickets.begin(),  *indexIt );

                     (*ticketIt)->notifyCompletion();

                     _requests.erase(requestIt);
                     _tickets.erase(ticketIt);
                  }
               }
            }
         }
      }
};

} // namespace mpi
} // namespace nanos

#endif // TICKET_QUEUE_H
