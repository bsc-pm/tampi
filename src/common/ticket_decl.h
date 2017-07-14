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

#include "array_view.h"
#include "configuration.h"
#include "print.h"
#include "task_local.h"

#include <array>
#include <cassert>

#ifdef HAVE_NANOX_NANOS_H
#include <nanox/nanos.h>
#define nanos_get_current_task nanos_current_wd
#endif

#ifdef HAVE_NANOS6_H
#include <nanos6.h>
#endif

namespace nanos {
namespace mpi {

namespace detail {

struct WaitProperties {
   bool _spinForever;
   int  _numSpins;

   // If the task_level value is 0, then interoperability
   // is disabled, therefore the task has to spin until completion.
   WaitProperties( int task_level, int num_spins ) :
      _spinForever( task_level == 0 ),
      _numSpins(num_spins)
   {
   }

   WaitProperties() :
      _spinForever( config.disabled() ),
      _numSpins(0)
   {
   }

   bool spinNotYield() {
      return _spinForever || (_numSpins--) > 0;
   }

   bool spinForever() const {
      return _spinForever;
   }
};

class TicketBase {
private:
   void             *_waiter;
   int               _pending;
   WaitProperties    _waitMode;

public:
   TicketBase() :
      _waiter(),
      _pending(0),
      _waitMode()
   {
      _waiter = nanos_get_current_blocking_context();
      const tls_view task_local_storage;
      task_local_storage.load( _waitMode );
   }

   TicketBase( const TicketBase & gt ) = delete;

   TicketBase& operator=( const TicketBase & gt ) = delete;

   ~TicketBase() {
      if( !finished() )
         log::fatal( "Destroying unfinished ticket" );
   }

   bool finished() const {
      return _pending == 0;
   }

   void addPendingRequest() {
      _pending++;
   }

   void removePendingRequest() {
      assert( _pending > 0 );
      _pending--;
   }

   int getPendingRequests() const {
      return _pending;
   }

   void notifyCompletion() {
      removePendingRequest();
      if( finished() ) {
         nanos_unblock_task(_waiter);
      }
   }

   bool spinNotYield() {
      return _waitMode.spinNotYield();
   }

   void blockTask() {
      nanos_block_current_task( _waiter );
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

      Ticket( util::array_view<Request> t_requests, Status* first_status = MPI_STATUSES_IGNORE );

      Status* getStatus() { return _first_status; }

      bool ignoreStatus() const {
         return _first_status == MPI_STATUS_IGNORE || _first_status == MPI_STATUSES_IGNORE;
      }

      void notifyCompletion( int status_pos, const Status& status ) {
         if( !ignoreStatus() ) {
            const Status* first = &status;
            const Status* last  = std::next(first);
            Status* out   = std::next(_first_status, status_pos);
            std::copy<const Status*,Status*>( first, last, out );
         }
	 TicketBase::notifyCompletion();
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

      Ticket( util::array_view<Request> t_requests, MPI_Fint* first_status = MPI_F_STATUSES_IGNORE );

      MPI_Fint* getStatus() { return _first_status; }

      bool ignoreStatus() const {
         // MPI fortran API decays status array to a pointer to integer
         return _first_status == MPI_F_STATUS_IGNORE || _first_status == MPI_F_STATUSES_IGNORE;
      }

      void notifyCompletion( int status_pos, const Status& status ) {
         if( !ignoreStatus() ) {
            const Status* first = &status;
            const Status* last  = std::next(first);
            Status* out   = std::next(reinterpret_cast<Status*>(_first_status),status_pos);
            std::copy<const Status*,Status*>( first, last, out );
         }
	 TicketBase::notifyCompletion();
      }

      void wait();
   };
} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_DECL_H

