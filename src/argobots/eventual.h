
#ifndef EVENTUAL_H
#define EVENTUAL_H

#include <abt.h>

namespace abt {

	template < typename T >
	class Eventual {
		private:
			ABT_eventual _eventual;
		public:
			Eventual() :
				_eventual()
			{
				ABT_eventual_create( sizeof(T), &_eventual );
			}

			Eventual( Eventual const& other ) = delete;

			Eventual const& operator=( Eventual const& other ) = delete;

			~Eventual()
			{
				ABT_eventual_free( &_eventual );
			}

			T const& wait()
			{
				T *buffer;
				int err = ABT_eventual_wait( _eventual, reinterpret_cast<void**>(&buffer) );
				if ( err != ABT_SUCCESS ) {
					// error: print something, throw exception, abort...
				}
				return *buffer;
			}

			void set( T const& value )
			{
				int err = ABT_eventual_set( _eventual, &value, sizeof(T) );
				if( err != ABT_SUCCESS ) {
					// error: print something, throw exception, abort...
				}
			}
	};

} // namespace abt

#endif // EVENTUAL_H
