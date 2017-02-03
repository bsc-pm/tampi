#ifndef TICKET_QUEUE_H
#define TIQUET_QUEUE_H

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

         _requests.insert( _requests.end(), first, last );
         _tickets.insert(  _tickets.end(),  count, &ticket );
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

               // Assumes indices array is sorted from lower to higher values
               // Iterate from the end to the beginning of the array to ensure
               // indexed element positions are not changed after a deletion
               typedef std::reverse_iterator<int*> index_iterator;
               index_iterator indexIt;
               for( indexIt =  index_iterator(indices+count-1);
                    indexIt != index_iterator(indices);
                    ++indexIt )
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
};

} // namespace mpi
} // namespace nanos

#endif // TICKET_QUEUE_H
