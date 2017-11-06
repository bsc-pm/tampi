#include "environment.h"

template<>
TicketQueue<C::Ticket>* C::Environment::_queue = nullptr;
template<>
bool C::Environment::_enabled = false;

template<>
TicketQueue<Fortran::Ticket>* Fortran::Environment::_queue = nullptr;
template<>
bool Fortran::Environment::_enabled = false;
