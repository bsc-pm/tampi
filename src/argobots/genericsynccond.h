
#ifndef GENERIC_SYNC_COND_H
#define GENERIC_SYNC_COND_H

#include <atomic>

namespace abt {

/*! \brief Abstract synchronization class.
 */
class GenericSyncCond
{
   protected:
      std::atomic<int> _refcount; /**< References counter for waits that need to make sure there are no pending signals */

   public:
      /*! \brief GenericSyncCond default constructor
       */
      GenericSyncCond() :
			_refcount(0)
		{
		}

      /*! \brief GenericSyncCond copy constructor (disabled)
       */
      GenericSyncCond ( const GenericSyncCond & gsc ) = delete;
      /*! \brief GenericSyncCond copy assignment operator (disabled)
       */
      GenericSyncCond& operator=( const GenericSyncCond & gsc ) = delete;

      /*! \brief GenericSyncCond destructor
       */
      virtual ~GenericSyncCond()
		{
		}

      /*! \brief increase references
       */
      int reference()
		{
			return _refcount++;
		}

      /*! \brief decrease references
       */
      int unreference() {
			return _refcount--;
		}

      virtual void wait() = 0;
      virtual void signal() = 0;
      virtual void signal_one() = 0;
      virtual bool check() = 0;

};

} // namespace abt

#endif
