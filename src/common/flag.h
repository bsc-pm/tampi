#ifndef FLAG_H
#define FLAG_H

template <typename IntType>
struct Flag {
   IntType value;
public:
   Flag() : value(0) {}
   Flag( IntType const& v ) : value(v) {}
   Flag( bool b ) : value( b? 1: 0 ) {}
   Flag( Flag const& f ) : value(f.value) {}

   operator IntType*() { return &value; }
   operator IntType const&() const { return value; }
   operator bool() const { return value == 1; }
};

#endif // FLAG_H
