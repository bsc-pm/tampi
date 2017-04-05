/*************************************************************************************/
/*      Copyright 2009-2017 Barcelona Supercomputing Center                          */
/*                                                                                   */
/*      This file is part of the NANOS++ library.                                    */
/*                                                                                   */
/*      NANOS++ is free software: you can redistribute it and/or modify              */
/*      it under the terms of the GNU Lesser General Public License as published by  */
/*      the Free Software Foundation, either version 3 of the License, or            */
/*      (at your option) any later version.                                          */
/*                                                                                   */
/*      NANOS++ is distributed in the hope that it will be useful,                   */
/*      but WITHOUT ANY WARRANTY; without even the implied warranty of               */
/*      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                */
/*      GNU Lesser General Public License for more details.                          */
/*                                                                                   */
/*      You should have received a copy of the GNU Lesser General Public License     */
/*      along with NANOS++.  If not, see <http://www.gnu.org/licenses/>.             */
/*************************************************************************************/

#ifndef MPI_ERROR_TRANSLATOR_HPP
#define MPI_ERROR_TRANSLATOR_HPP

#include "mpi/error.h"
#include "mpiexception.hpp"
#include "mpierrorhandler.hpp"

namespace nanos {
namespace error {

/*!
 * \ingroup NanosExceptions
 * \brief Translates an MPI error to an exception
 * \details MPIErrorTranslator manages access for singleton class
 *          SingletonTranslator. It is used to attach MPI objects
 *          to error handlers with the goal of turn an MPI error
 *          into a C++ exception.
 * \tparam <MPIHandle> Type of MPI handle (communicator, on-sided
 *          window...) that the error handler must manage.
 * \author Jorge Bellon
 * Based in the article [http://www.ibm.com/developerworks/library/l-cppexcep]
 */
template < class MPIHandle >
class MPIErrorTranslator {
   private:
      /*!
       * \brief Helper singleton class.
       * Assigns MPI handles to error handlers that
       * throw MPIError exceptions.
       * \tparam <MPIHandle> type of MPI handle that is
       * managed for error handling.
       */
      class SingletonTranslator {
         MPIErrorHandler<MPIHandle> handlerDescriptor;

         public:
            //! Constructor
            SingletonTranslator() : handlerDescriptor( mpiErrorHandler ) {}

            //! 
            int attachHandle( MPIHandle handle ) {
               return handlerDescriptor.attachHandle( handle );
            }

            /**
             * \brief MPI error handler.
             * Handles MPI errors and throws an MPIError exceptions
             * that contain information regarding the error.
             * @param[in] handle MPI object involved when the error was found
             * @param[in] error error code that identifies what happened
             */
            static void mpiErrorHandler( MPIHandle *handle, int *error, ... ) {
               throw MPIException<MPIHandle>( handle, error );
            }
      };

   public:
      /**
       * \brief Constructor
       * Assigns an MPI object to an error handler.
       */
      MPIErrorTranslator( MPIHandle handle )
      {
         static SingletonTranslator s_objTranslator;
         s_objTranslator.attachHandle( handle );
      }
};

}// namespace error
}// namespace nanos

#endif // MPI_ERROR_TRANSLATOR

