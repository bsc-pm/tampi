#ifndef MULTI_ADDER_QUEUE_HPP
#define MULTI_ADDER_QUEUE_HPP

#include "util/SpinLock.hpp"

#include <functional>
#include <mutex>

#include <boost/version.hpp>

// Support for BOOST 1.67
#if BOOST_VERSION == 106700
#include <boost/next_prior.hpp>
#endif

#include <boost/lockfree/spsc_queue.hpp>


template <typename T, size_t Size = 2048>
class MultiAdderQueue {
private:
	typedef boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Size> > queue_t;
	typedef util::SpinLock<> SpinLock;
	typedef std::function<void()> ProgressFunction;
	
	SpinLock _adderMutex;
	queue_t _queue;
	
public:
	MultiAdderQueue() :
		_adderMutex(),
		_queue()
	{}
	
	inline void add(const T &element, ProgressFunction progressFunction)
	{
		std::lock_guard<SpinLock> guard(_adderMutex);
		
		int pushed = 0;
		do {
			pushed = _queue.push(element);
			if (pushed < 1) {
				progressFunction();
			}
		} while (pushed == 0);
	}
	
	inline void add(const T elements[], int count, ProgressFunction progressFunction)
	{
		assert(elements != nullptr);
		std::lock_guard<SpinLock> guard(_adderMutex);
		
		int pushed = 0;
		do {
			pushed += _queue.push(&elements[pushed], count - pushed);
			if (pushed < count) {
				progressFunction();
			}
		} while (pushed < count);
	}
	
	inline int retrieve(T elements[], int count)
	{
		assert(elements != nullptr);
		return _queue.pop(elements, count);
	}
};

#endif // MULTI_ADDER_QUEUE_HPP
