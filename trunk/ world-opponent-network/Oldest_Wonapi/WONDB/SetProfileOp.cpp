#include "SetProfileOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SetProfileOp::Init()
{
	mLengthFieldSize = 2;
	mProfileData = new ProfileData;

	SetUseAuth2(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SetProfileOp::SetProfileOp(ServerContext *theProfileContext) : ServerRequestOp(theProfileContext)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SetProfileOp::SetProfileOp(const IPAddr &theAddr) : ServerRequestOp(theAddr)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SetProfileOp::AssociateNewUsernamePassword(const std::wstring &theUsername, const std::wstring &thePassword)
{
	mNewUsername = theUsername;
	mNewPassword = thePassword;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SetProfileOp::ClearNewUsernamePassword()
{ 
	mNewUsername = L""; 
	mNewPassword = L""; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus SetProfileOp::GetNextRequest()
{
	WriteBuffer aBuf(mLengthFieldSize);
	aBuf.AppendByte(5);			// small message
	aBuf.AppendShort(7);		// Profile Service
	aBuf.AppendShort(30);		// Auth update

	aBuf.AppendShort(0);		// no symmetric key
	aBuf.AppendShort(0);
	int aPos = aBuf.length();

	aBuf.AppendByte(5);			// small message
	aBuf.AppendShort(7);		// Profile Service
	aBuf.AppendShort(10);		// Set Profile
	const ProfileFieldMap &aMap = mProfileData->GetFieldMap();
	aBuf.AppendShort(aMap.size());
	ProfileFieldMap::const_iterator anItr = aMap.begin();
	while(anItr!=aMap.end())
	{
		aBuf.AppendLong(anItr->first);
		aBuf.AppendBuffer(anItr->second.mData,2);
		++anItr;
	}

	aBuf.SetShort(aPos-2,aBuf.length()-aPos);
	
	aBuf.AppendShort(0); 

	aPos = aBuf.length();
	aBuf.AppendWString(mNewUsername);
	aBuf.AppendWString(mNewPassword);
	aBuf.SetShort(aPos-2,aBuf.length()-aPos);

	mRequest = aBuf.ToByteBuffer();
	return WS_ServerReq_Recv;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus SetProfileOp::CheckResponse()
{
	ReadBuffer aMsg(mResponse->data(),mResponse->length());
	unsigned char aHeaderType = aMsg.ReadByte();
	unsigned short aServiceType = aMsg.ReadShort();
	unsigned short aMessageType = aMsg.ReadShort();
	if(aHeaderType!=5 || aServiceType!=7 || aMessageType!=31)
		return InvalidReplyHeader();

	mAPIErrorCode = aMsg.ReadLong();
	short aStatus = aMsg.ReadShort();
	aMsg.ReadString(mErrorString);

	mProfileData->UnpackSetReply(aMsg);
	return (WONStatus)aStatus;
}
