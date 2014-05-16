#ifndef __WON_ROUTINGGETCLIENTLISTOP_H__
#define __WON_ROUTINGGETCLIENTLISTOP_H__
#include "WONShared.h"
#include "RoutingOp.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingGetClientListOp : public RoutingOp
{
private:
	RoutingClientMap mClientMap;
	bool mIsEx;
	friend class RoutingRegisterClientOp;
	
	virtual void SendRequest();
	virtual WONStatus HandleReply(unsigned char theMsgType, ReadBuffer &theMsg);

public:
	RoutingGetClientListOp(RoutingConnection *theConnection, bool isEx = false) : RoutingOp(theConnection), mIsEx(isEx) {}
	const RoutingClientMap& GetClientMap() const { return mClientMap; }
	virtual RoutingOpType GetType() const { return RoutingOp_GetClientList; }
};

typedef SmartPtr<RoutingGetClientListOp> RoutingGetClientListOpPtr;

}; // namespace WONAPI



#endif
