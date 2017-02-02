#ifndef TICKET_MUTEX_HPP
#define TICKET_MUTEX_HPP

#include <atomic>
#include <cassert>
#include <cstddef>

namespace nanos {

template <typename TICKET_T = uint16_t>
class TicketMutex {

	private:
		std::atomic<TICKET_T> _currentTicket;
		std::atomic<TICKET_T> _nextFreeTicket;

	public:
		TicketMutex() :
			_currentTicket( 0 ),
			_nextFreeTicket( 0 )
		{

		}

		~TicketMutex()
		{
			// Not locked
			assert( _currentTicket == _nextFreeTicket );
		}
	
		inline void lock()
		{
			TICKET_T ticket = _nextFreeTicket++;
			while ( _currentTicket.load(std::memory_order_acquire) != ticket ) {
				// NOTE: there could be here some architecture-specific code to slow down the thread
			}
		}

		inline void unlock()
		{
			_currentTicket.fetch_add( 1, std::memory_order_release );
		}
};

} //namespace nanos

#endif // TICKET_SPIN_LOCK_HPP


