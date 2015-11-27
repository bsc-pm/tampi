
#ifndef LOCK_HPP
#define LOCK_HPP

#include <abt.h>

namespace abt {
 
class Mutex {
	private:
		ABT_mutex mutex;
	public:
		typedef ABT_mutex native_handle_type;

		Mutex() :
			mutex()
		{
			ABT_mutex_create( &mutex );
		}

		~Mutex()
		{
			ABT_mutex_free( &mutex );
		}

		bool operator==( Mutex const& other )
		{
			ABT_bool result;
			ABT_mutex_equal( mutex, other.mutex, &result );
			return result == ABT_TRUE;
		}

		void lock()
		{
			ABT_mutex_lock( mutex );
		}

		bool try_lock()
		{
			return ABT_SUCCESS == ABT_mutex_trylock( mutex );
		}

		void spin_lock()
		{
			ABT_mutex_spinlock( mutex );
		}

		void unlock()
		{
			ABT_mutex_unlock( mutex );
		}

		native_handle_type native_handle()
		{
			return mutex;
		}
};

}

#endif // LOCK_HPP
