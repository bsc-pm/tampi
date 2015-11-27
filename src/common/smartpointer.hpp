/*************************************************************************************/
/*      Copyright 2015 Barcelona Supercomputing Center                               */
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

#ifndef SMARTPOINTER
#define SMARTPOINTER

namespace abt {

template < typename T >
class shared_pointer {
   private:
      T *pointer;
   public:
      shared_pointer() :
			pointer(NULL)
		{
		}

      shared_pointer( T* ptr ) :
			pointer(ptr)
      {
         if( ptr )
            ptr->reference();
      }

      shared_pointer( shared_pointer const& other ) :
         pointer(other.pointer)
      {
         other.pointer->reference();
      }

      virtual ~shared_pointer()
      {
         if( pointer && pointer->unreference() == 0 ) 
            delete pointer;
      }

      shared_pointer const& operator=( shared_pointer const& other ) {
         if( pointer && pointer->unreference() == 0 )
            delete pointer;
         other.pointer->reference();
         pointer = other.pointer;
         return *this;
      }

      void setPointer( T *ptr )
      {
         if( ptr )
            ptr->reference();

         if( pointer && pointer->unreference() == 0 ) 
            delete_function( pointer );
         
         pointer = ptr;
      }

      T *getPointer() { return pointer; }

      T *operator->() { return pointer; }

      T &operator*()
      {
         if( pointer )
            return *pointer;
      }

      void swap( shared_pointer<T> &other ) {
         T *tmp_ptr = other.pointer;
         other.pointer = pointer;
         pointer = tmp_ptr;
      }
};

template < typename T >
class unique_pointer {
   private:
      T *pointer;
   public:
      unique_pointer() : pointer(NULL) {}

      unique_pointer( T* ptr ) : pointer( ptr )
      {
      }

      unique_pointer( unique_pointer &other ) :
         pointer(other.pointer)
      {
         other.pointer = NULL;
      }

      virtual ~unique_pointer() { if( pointer ) delete pointer; }

      unique_pointer const& operator=( unique_pointer &other ) {
         if( pointer )
            delete pointer;
         pointer = other.pointer;
         other.pointer = NULL;
         return *this;
      }

      void setPointer( T *ptr )
      {
         if( pointer ) 
            delete pointer;
         pointer = ptr;
      }

      T *getPointer() { return pointer; }

      T *operator->() { return pointer; }

      T &operator*()
      {
         if( pointer )
            return *pointer;
      }

      void swap( unique_pointer<T> &other ) {
         T *tmp_ptr = other.pointer;
         other.pointer = pointer;
         pointer = tmp_ptr;
      }
};

} // namespace nanos
#endif
