
#ifndef LOCK_BLOCK_HPP
#define LOCK_BLOCK_HPP

#include "mutex.h"

namespace abt {

class LockBlock {
	private:
		Mutex &_mutex;
	public:
		LockBlock( Mutex &provided ) :
			_mutex( provided )
		{
			_mutex.lock();
		}

		~LockBlock()
		{
			_mutex.unlock();
		}
};

} // namespace abt

#endif // LOCK_BLOCK_HPP

