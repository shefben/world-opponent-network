#ifndef __WON_ROUTINGCLIENTJOINEDSERVEROP_H__
#define __WON_ROUTINGCLIENTJOINEDSERVEROP_H__
#include "WONShared.h"
#include "RoutingOp.h"
#include "WONCommon/StringUtil.h"

#include <string>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingClientJoinedServerOp : public RoutingOp
{
private:
	RoutingClientInfoPtr mClient;

	virtual WONStatus HandleReply(unsigned char theMsgType, ReadBuffer &theMsg);

public:
	RoutingClientJoinedServerOp(RoutingConnection *theConnection) : RoutingOp(theConnection) {}

	unsigned short GetClientId() const { return mClient.get()?mClient->mId:0; }
	const std::wstring& GetClientName() const { return mClient.get()?mClient->mName:GetEmptyWStr(); }
	unsigned long GetClientFlags() const { return mClient.get()?mClient->mFlags:0; }

	bool GetIsModerator() const { return (GetClientFlags() & RoutingClientFlag_IsModerator) != 0; }
	bool GetIsMuted() const     { return (GetClientFlags() & RoutingClientFlag_IsMuted) != 0; }
	bool GetIsAway() const      { return (GetClientFlags() & RoutingClientFlag_IsAway) != 0; }

	RoutingClientInfo* GetClientInfo() { return mClient; }

	virtual RoutingOpType GetType() const { return RoutingOp_ClientJoinedServer; }

	static void ParseExClientData(ReadBuffer &theMsg, RoutingClientInfo *theClient);
};

typedef SmartPtr<RoutingClientJoinedServerOp> RoutingClientJoinedServerOpPtr;

}; // namespace WONAPI


#endif
