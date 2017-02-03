#ifndef TICKET_QUEUE_H
#define TIQUET_QUEUE_H

#include "array_utils.h"
#include "spin_mutex.h"
#include "ticket_decl.h"

#include <mutex>
#include <vector>

#include <mpi.h>

namespace nanos {
namespace mpi {

class TicketQueue {
   private:
      std::vector<MPI_Request> _requests;
      std::vector<Ticket*>     _tickets;
      mutable spin_mutex       _mutex;

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

      void add( Ticket& ticket, MPI_Request req )
      {
         std::lock_guard<spin_mutex> guard( _mutex );
         _requests.push_back( req );
         _tickets.push_back( &ticket );
      }

      void add( Ticket& ticket, MPI_Request* first, MPI_Request* last )
      {
         const size_t count = std::distance(first,last);
         std::lock_guard<spin_mutex> guard( _mutex );

         const size_t capacity = _requests.capacity() - _requests.size() + count;
         _requests.reserve(capacity);
         _tickets.reserve(capacity);

         while( first != last ) {
            if( *first != MPI_REQUEST_NULL ) {
               _requests.push_back( *first );
               _tickets.push_back( &ticket );
            }
            ++first;
         }
      }

      void poll() {
         if ( !_requests.empty() ) {
            std::unique_lock<spin_mutex> guard( _mutex, std::try_to_lock );
            if ( guard.owns_lock() && !_requests.empty() ) {
               int count = _requests.size();
               int completed = 0;
               int indices[count];
               int err = MPI_Testsome( count, _requests.data(), &completed,
                                       indices, MPI_STATUSES_IGNORE );

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
