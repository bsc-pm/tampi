#ifndef TICKET_DECL_H
#define TICKET_DECL_H

#include "print.h"

#ifdef HAVE_NANOX_NANOS_H
#include <nanox/nanos.h>
#define nanos_get_current_task nanos_current_wd
#endif

#ifdef HAVE_NANOS6_H
#include <nanos6.h>
typedef void* nanos_wait_cond_t;
#endif

namespace nanos {
namespace mpi {

namespace detail {

class TicketBase {
private:
   nanos_wait_cond_t _waiter;
   int               _pending;

public:
   template < typename Request >
   TicketBase( Request& req ) :
      _waiter(),
      _pending(1)
   {
      nanos_create_wait_condition(&_waiter);
   }

   template < typename Request >
   TicketBase( Request* first, Request* last ) :
      _waiter(),
      _pending(std::distance(first,last))
   {
      nanos_create_wait_condition(&_waiter);
   }

   TicketBase( const TicketBase & gt ) = delete;

   TicketBase& operator=( const TicketBase & gt ) = delete;

   ~TicketBase() {
      if( _pending > 0 )
         log::fatal( "Destroying unfinished ticket" );
   }

   void notifyCompletion( int num = 1 ) {
      assert( _pending >= num );
      _pending -= num;

      if( finished() && _waiter )
         nanos_signal_wait_condition(&_waiter);
   }

   bool finished() const {
      return _pending == 0;
   }

   void wait() {
      if( !finished() ) {
         nanos_block_current_task(&_waiter);
      }
   }
};

} // namespace detail

namespace C {
struct Ticket : public detail::TicketBase {
   typedef MPI_Request Request;
   typedef MPI_Status  Status;

   private:
      Status* _first_status;

   public:
      Ticket( Request& r, Status* s = MPI_STATUS_IGNORE );

      Ticket( Request* first_req, Request* last_req, Status* first_status = MPI_STATUSES_IGNORE );

      Status* getStatus() { return _first_status; }

      bool ignoreStatus() const {
         return _first_status == MPI_STATUS_IGNORE | _first_status == MPI_STATUSES_IGNORE;
      }

      using TicketBase::notifyCompletion;

      void notifyCompletion( int status_pos, const Status& status ) {
         if( !ignoreStatus() ) {
            const Status* first = &status;
            const Status* last  = std::next(first);
            Status* out   = _first_status;
            std::copy<const Status*,Status*>( first, last, out );
         }
         notifyCompletion();
      }
};
} // namespace C

namespace Fortran {
struct Ticket : public detail::TicketBase {
   typedef MPI_Fint  Request;
   typedef std::array<MPI_Fint,sizeof(MPI_Status)/sizeof(int)> Status;

   private:
      MPI_Fint* _first_status;

   public:
      Ticket( Request& r, MPI_Fint* s = MPI_F_STATUS_IGNORE );

      Ticket( Request* first_req, Request* last_req, MPI_Fint* first_status = MPI_F_STATUSES_IGNORE );

      MPI_Fint* getStatus() { return _first_status; }

      bool ignoreStatus() const {
         // MPI fortran API decays status array to a pointer to integer
         return _first_status == MPI_F_STATUS_IGNORE | _first_status == MPI_F_STATUSES_IGNORE;
      }

      using TicketBase::notifyCompletion;

      void notifyCompletion( int status_pos, const Status& status ) {
         if( !ignoreStatus() ) {
            const Status* first = &status;
            const Status* last  = std::next(first);
            Status* out   = reinterpret_cast<Status*>(_first_status);
            std::copy<const Status*,Status*>( first, last, out );
         }
         notifyCompletion();
      }
   };
} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_DECL_H

