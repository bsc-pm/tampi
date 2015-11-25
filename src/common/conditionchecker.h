
#ifndef CONDITION_CHECKER_H
#define CONDITION_CHECKER_H

namespace nanos {

  /*! \brief Represents an object that checks a given condition.
   */
   class ConditionChecker
   {
      public:
         /*! \brief ConditionChecker default constructor
          */
         ConditionChecker() {}
         /*! \brief ConditionChecker copy constructor
          */
         ConditionChecker ( const ConditionChecker & cc ) {}
         /*! \brief ConditionChecker copy assignment operator
          */
         ConditionChecker& operator=( const ConditionChecker & cc )
         {
            return *this;
         }
         /*! \brief ConditionChecker destructor
          */
         virtual ~ConditionChecker() {}
         /*! \brief interface used by the SynchronizedCondition to check the condition (pure virtual)
          */
         virtual bool checkCondition() = 0;
   };

} // namespace nanos

#endif // CONDITION_CHECKER_H
