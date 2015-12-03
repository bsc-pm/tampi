/*************************************************************************************/
/*      Copyright 2009-2015 Barcelona Supercomputing Center                          */
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

#ifndef MPIERROR_HPP
#define MPIERROR_HPP

#include <mpi.h>

namespace nanos {
namespace error {

/*!
 * \ingroup NanosExceptions
 * \brief C++ wrapper for MPI's MPI_Errhandler struct.
 * \details Provides transparent allocation/deallocation to
 *          MPI error handling objects.
 *          Error handling object construction depends on the type
 *          of handle that will be attached. Therefore, this class must be specialized
 *          for each one that the user needs to use.
 * * \tparam <MPIHandle> Type of handle (communicator, on-sided window...)
 *          that will be used for error handling.
 * \author Jorge Bellon
 */
template <typename MPIHandle>
class MPIErrorHandler;

/*!
 * \ingroup NanosExceptions
 * \brief C++ wrapper for MPI's MPI_Errhandler struct.
 * \details Provides transparent allocation/deallocation to
 *          MPI error handling objects.
 *          Specialization for MPI communicators.
 * \author Jorge Bellon
 */
template <>
class MPIErrorHandler<MPI_Comm> {
private:
   MPI_Errhandler mpiObject;//!< MPI original C struct used for error handling.
public:
   /**
    * \brief Constructor
    * Initializes the MPI error handling C struct.
    * @param[in] errorHandler pointer to the function that handles the error.
    */
   MPIErrorHandler( MPI_Comm_errhandler_fn *errorHandler ) :
         mpiObject()
   {
      MPI_Comm_create_errhandler( errorHandler, &mpiObject );
   }

   /**
    * \brief Destructor
    * Frees the MPI error handling C struct.
    */
   virtual ~MPIErrorHandler() {
      int isMpiFinalized;
      MPI_Finalized( &isMpiFinalized );
      if( !isMpiFinalized ) {
         MPI_Errhandler_free( &mpiObject );
      }
   }

   /**
    * \brief Makes a communicator to use this error handler.
    * @param[in] Communicator handle.
    */
   int attachHandle( MPI_Comm handle ) {
      return MPI_Comm_set_errhandler( handle, mpiObject );
   }
};

}// namespace error
}// namespace nanos

#endif // MPIERROR_HPP
