#ifndef __WON_ROUTINGGETGROUPLISTREQOP_H__
#define __WON_ROUTINGGETGROUPLISTREQOP_H__

#include "WONServer/ServerRequestOp.h"
#include "RoutingGetGroupListOp.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingGetGroupListReqOp : public ServerRequestOp
{
protected:
	RoutingGetGroupListOpPtr mOp;

	virtual WONStatus GetNextRequest();
	virtual WONStatus CheckResponse();

public:
	RoutingGetGroupListReqOp(const IPAddr &theAddr, unsigned short theFlags = 0);

	void SetFlags(unsigned short theFlags) { mOp->SetFlags(theFlags); }
	unsigned short GetFlags() const { return mOp->GetFlags(); }

	const RoutingGroupMap& GetGroupMap() const { return mOp->GetGroupMap(); }
	unsigned short GetClientCount() const { return mOp->GetClientCount(); }
};
typedef SmartPtr<RoutingGetGroupListReqOp> RoutingGetGroupListReqOpPtr;

}; // namespace WONAPI

#endif
