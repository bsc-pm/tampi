
#ifndef TASK_LOCAL_H
#define TASK_LOCAL_H

namespace nanos {

class tls_view {
private:
   void * _buffer;
   size_t _size;

public:
   tls_view() :
      _buffer(nullptr),
      _size(0)
   {
      nanos_get_task_local_storage( &_buffer, &_size );
   }

   template < typename T >
   void load( T& destination ) const {
      assert( sizeof(T) <= _size );
      T* source = static_cast<T*>(_buffer);
      std::copy( source, std::next(source), &destination );
   }

   template < typename T >
   void store( const T& source ) {
      assert( sizeof(T) <= _size );
      T* destination = static_cast<T*>(_buffer);
      std::copy( &source, std::next(&source), destination );
   }
};

} // namespace nanos

#endif // TASK_LOCAL_H

