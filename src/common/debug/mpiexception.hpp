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

#ifndef MPI_ERROR_HPP
#define MPI_ERROR_HPP

#include "genericexception.hpp"

namespace nanos {
namespace error {

/*!
 * \ingroup NanosExceptions
 * \brief A generic class for MPI errors.
 * \details MPIException exception is thrown when an MPI routine fails.
 *          Contains useful information that can be used either for 
 *          recovery or for verbosity.
 * \tparam <MPIHandle> Type of MPI handle (communicator, on-sided 
 *          window, etc.) that took place when the error was found.
 * \author Jorge Bellon
 */
template <typename MPIHandle>
class MPIException : public GenericException {
   MPIHandle failedHandle;//!< Provides access to the MPIHandle
   nanos::mpi::Error errorCode;//!< Error code

public:
   /**
    * \brief Constructor.
    * @param[in] handle pointer to the handle that produced the error.
    * @param[in] error pointer to the error code.
    */
   MPIException( MPIHandle *handle, int *error ) :
         GenericException( nanos::mpi::Error::toString(*error) ),
         failedHandle(*handle),
         errorCode(*error)
   {
   }
};

}// namespace error
}// namespace nanos

#endif // MPI_ERROR_HPP
