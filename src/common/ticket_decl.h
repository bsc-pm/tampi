/*
 * Copyright (c) 2015 Barcelona Supercomputing Center. All rights
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
#ifndef TICKET_DECL_H
#define TICKET_DECL_H

#include "print.h"
#include "task_local.h"

#include <array>

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
   bool              _waiting;
   bool              _spinNotYield;

public:
   template < typename Request >
   TicketBase( Request& req ) :
      _waiter(),
      _pending(1),
      _waiting(false),
      _spinNotYield(false)
   {
      nanos_create_wait_condition(&_waiter);

      const tls_view task_local_storage;
      task_local_storage.load( _spinNotYield );
   }

   template < typename Request >
   TicketBase( Request* first, Request* last ) :
      _waiter(),
      _pending(std::distance(first,last)),
      _waiting(false),
      _spinNotYield(false)
   {
      const tls_view task_local_storage;
      task_local_storage.load( _spinNotYield );

      if( !_spinNotYield ) {
         nanos_create_wait_condition(&_waiter);
      }
   }

   TicketBase( const TicketBase & gt ) = delete;

   TicketBase& operator=( const TicketBase & gt ) = delete;

   ~TicketBase() {
      if( _pending > 0 )
         log::fatal( "Destroying unfinished ticket" );
   }

   int getPendingRequests() const {
      return _pending;
   }

   void notifyCompletion( int num = 1 ) {
      assert( _pending >= num );
      _pending -= num;

      if( finished() && _waiting ) {
         nanos_signal_wait_condition(&_waiter);
      }
   }

   bool finished() const {
      return _pending == 0;
   }

   bool spinNotYield() const {
      return _spinNotYield;
   }

   void blockTask() {
      _waiting = true;
      nanos_block_current_task( &_waiter );
      _waiting = false;
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

      void wait();
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

      void wait();
   };
} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_DECL_H

