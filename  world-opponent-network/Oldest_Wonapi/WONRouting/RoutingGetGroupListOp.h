#ifndef __WON_ROUTINGGETGROUPLISTOP_H__
#define __WON_ROUTINGGETGROUPLISTOP_H__
#include "WONShared.h"
#include "RoutingOp.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingGetGroupListOp : public RoutingOp
{
private:
	unsigned short mFlags;
	RoutingGroupMap mGroupMap;
	friend class RoutingRegisterClientOp;

	virtual void SendRequest();
	virtual WONStatus HandleReply(unsigned char theMsgType, ReadBuffer &theMsg);
	void ParseReplyExceptForStatus(ReadBuffer &theMsg);

	RoutingMemberInfoPtr GetNewMemberInfo();
	RoutingGroupInfoPtr GetNewGroupInfo();

public:
	RoutingGetGroupListOp() : mFlags(0) {}
	RoutingGetGroupListOp(RoutingConnection *theConnection, unsigned char theFlags = 0) : RoutingOp(theConnection), mFlags(theFlags) {}

	void SetFlags(unsigned char theFlags) { mFlags = theFlags; }
	bool ParseGroupListObj(const void *theData, unsigned long theDataLen);
	
	unsigned char GetFlags() const { return mFlags; }
	bool HasGroupName() const     { return (mFlags & RoutingGetGroupList_IncludeGroupName)?true:false; }
	bool HasCaptainId() const     { return (mFlags & RoutingGetGroupList_IncludeCaptainId)?true:false; }
	bool HasMaxPlayers() const    { return (mFlags & RoutingGetGroupList_IncludeMaxPlayers)?true:false; }
	bool HasGroupFlags() const    { return (mFlags & RoutingGetGroupList_IncludeGroupFlags)?true:false; }
	bool HasAsyncFlags() const    { return (mFlags & RoutingGetGroupList_IncludeAsyncFlags)?true:false; }
	bool HasMemberCount() const   { return (mFlags & RoutingGetGroupList_IncludeMemberCount)?true:false; }
	bool HasObserverCount() const { return (mFlags & RoutingGetGroupList_IncludeObserverCount)?true:false; }
	bool HasMembers() const       { return (mFlags & RoutingGetGroupList_IncludeMembers)?true:false; }

	const RoutingGroupMap& GetGroupMap() const { return mGroupMap; }

	virtual RoutingOpType GetType() const { return RoutingOp_GetGroupList; }
};

typedef SmartPtr<RoutingGetGroupListOp> RoutingGetGroupListOpPtr;

}; // namespace WONAPI


#endif
