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

#ifndef GENERIC_EXCEPTION_HPP
#define GENERIC_EXCEPTION_HPP

#include "exceptiontracer.hpp"

#include <exception>

namespace nanos {
namespace error {

/*!
 * \defgroup NanosExceptions
 * \brief A generic class for exceptions.
 * \details GenericException is the base class of every exception used inside the runtime.
 * \author Jorge Bellon
 */
class GenericException : public ExceptionTracer, public std::exception {
   private:
      std::string errorMessage;//!< Explanatory string.
		// TODO: use a typedef to define a generic task (usable from nanos5 and nanos6)
      //WorkDescriptor &runningTaskOnError;//!< WorkDescriptor that was being executed when the error was found.

   public:
      /**
       * Constructor
       * @param[in] message Brief description of the error that was found.
       */
      GenericException( std::string const& message ) : 
            errorMessage( message )/*, 
            runningTaskonError( getMyThreadSafe()->getCurrentWD() )*/
      {}

      /**
       * Provides access to an explanatory string
       * \returns a brief description of the error that was found
       */
      virtual const char* what() const noexcept {
         return errorMessage.c_str();
      }
};

}// namespace error
}// namespace nanos

#endif // GENERICEXCEPTION_HPP
