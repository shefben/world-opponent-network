#include "RoutingClientJoinedGroupOp.h"
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RoutingClientJoinedGroupOp::HandleReply(unsigned char theMsgType, ReadBuffer &theMsg)
{
	if(theMsgType!=RoutingClientJoinedGroup)
		return WS_RoutingOp_DontWantReply;

	unsigned char aFlags = theMsg.ReadByte();

	mGroupId = theMsg.ReadShort();
	mClientId = theMsg.ReadShort();
	mMemberFlags = theMsg.ReadByte();

	if(aFlags & RoutingGroupAsyncFlag_DistributeClientName)  
		theMsg.ReadWString(mClientName);

	if(aFlags & RoutingGroupAsyncFlag_DistributeClientFlags)
	{
		mClientFlags = theMsg.ReadLong();
		mHasClientFlags = true; 
	}

	return WS_Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoutingMemberInfoPtr RoutingClientJoinedGroupOp::GetMemberInfo()
{
	RoutingMemberInfoPtr aMember = GetNewMemberInfo();
	aMember->mClientId = mClientId;
	aMember->mFlags = mMemberFlags;
	if(!mClientName.empty() || HasClientFlags())
	{
		aMember->mClientInfo = GetNewClientInfo();
		if(HasClientFlags())
			aMember->mClientInfo->mFlags = mClientFlags;
		if(!mClientName.empty())
			aMember->mClientInfo->mName = mClientName;
	}

	return aMember;
}
