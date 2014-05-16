#ifndef __WON_ROUTINGGETUSERSTATUSOP_H__
#define __WON_ROUTINGGETUSERSTATUSOP_H__
#include "WONShared.h"
#include "WONSocket/IPAddr.h"
#include "WONCommon/AsyncOpTracker.h"
#include "WONSocket/BlockingSocket.h"
#include "WONSocket/SendBytesToOp.h"
#include "WONSocket/RecvBytesFromOp.h"
#include "WONSocket/UDPManager.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Friend status flags
// 1 << 0 = FriendStatus_None
// 1 << 1 = FriendStatus_InChat
// 1 << 2 = FriendStatus_InGame

class RoutingGetUserStatusOp : public UDPManagerSendRecvOp
{
public:
	typedef std::map<unsigned long, unsigned char>	FriendStatusMap;

private:
	FriendStatusMap mFriendStatusMap;

public:
	const FriendStatusMap& GetFriendStatusMap() { return mFriendStatusMap; }
	void SetFriendStatusMap(const FriendStatusMap &theMap) { mFriendStatusMap = theMap; }
	void AddWONUserId(unsigned long theUserId);

protected:
	virtual void RunHook();
	virtual WONStatus UnpackMsg(ReadBuffer &theMsg);

public:
	RoutingGetUserStatusOp(const IPAddr& theAddress, UDPManager *theManager = NULL);
};

typedef SmartPtr<RoutingGetUserStatusOp> RoutingGetUserStatusOpPtr;

}; // namespace WONAPI

#endif
