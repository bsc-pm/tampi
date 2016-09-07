#ifndef NANOS6_TICKET_QUEUE_H
#define NANOS6_TIQUET_QUEUE_H

#include "genericticket.h"

#include "mpi/error.h"

#include <mutex>
#include <list>

namespace nanos {
namespace mpi {

class TicketQueue {

  private:
	friend class environment;
	typedef GenericTicket type;
    typedef std::list< type* > container;

	static TicketQueue* _queue;

	container _list;
	std::mutex _mutex;

  public:
	TicketQueue() :
      _list(),
      _mutex()
    {

    }

    TicketQueue ( const TicketQueue & ) = delete;

    const TicketQueue & operator= ( const TicketQueue & ) = delete;

    ~TicketQueue()
    {
		_list.clear();
    }

    bool empty () const
    {
        return _list.empty();
    }

    size_t size() const
    {
        return _list.empty();
    }

    void push_back( type &ticket )
    {
		while( !_mutex.try_lock() );
		std::lock_guard<std::mutex> guard( _mutex, std::adopt_lock );
		_list.push_back( &ticket );
    }

    bool poll()
    {
		if ( _list.empty() )
			return false;

        if( _mutex.try_lock() ) {
            if ( !_list.empty() ) {
                container::iterator it = _list.begin();

                while( it != _list.end() ) {
                    GenericTicket *ticket = *it;
                    if ( ticket->check() ) {
						ticket->signal();
                        _list.erase( it++ );
                        _mutex.unlock();
                        return true;
                    }
                    ++it;
                }
            }
            _mutex.unlock();
        }
        return false;
    }

	static void wait(GenericTicket &ticket)
    {
        if( !ticket.check() ) {
            _queue->push_back( ticket );
            ticket.wait();
        }
    }

	static TicketQueue& get()
	{
		return *_queue;
	}
};

} // namespace mpi
} // namespace nanos

#endif // NANOS6_TICKET_QUEUE_H
