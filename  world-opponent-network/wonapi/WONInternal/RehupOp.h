#ifndef __WON_REHUP_OP_H__
#define __WON_REHUP_OP_H__

#include "WONServer/ServerRequestOp.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RehupOp : public ServerRequestOp
{
protected:
	virtual WONStatus GetNextRequest();
	virtual WONStatus CheckResponse();

public:
	RehupOp(ServerContext* theServerContextP);
	RehupOp(const IPAddr &theAddr);
};

typedef SmartPtr<RehupOp> RehupOpPtr;

}; // namespace WONAPI

#endif
