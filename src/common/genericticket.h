#ifndef NANOS6_GENERIC_TICKET_H
#define NANOS6_GENERIC_TICKET_H

namespace nanos {

class GenericTicket
{
  public:
	GenericTicket()
	{

	}

	GenericTicket ( const GenericTicket & gt ) = delete;

	GenericTicket& operator=( const GenericTicket & gt ) = delete;

    virtual ~GenericTicket()
	{

	}

	virtual void wait() = 0;
	virtual bool check() = 0;
	virtual void signal() = 0;
};

} // namespace nanos


#endif //NANOS6_GENERIC_TICKET_H
