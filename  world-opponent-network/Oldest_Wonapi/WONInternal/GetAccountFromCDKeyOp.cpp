

#include "GetAccountFromCDKeyOp.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetAccountFromCDKeyOp::GetAccountFromCDKeyOp(ServerContext* theContext, std::string theCDKey, unsigned long theCommunity)
	: DBProxyOp(theContext),
	  mCDKey(theCDKey),
	  mCommunity(theCommunity),
	  mUserName(L""),
	  mPassword(L""),
	  mEmail(""),
	  mLastLogin("")
{
	mProfileData = new ProfileData();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAccountFromCDKeyOp::Reset()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAccountFromCDKeyOp::RunHook()
{
	SetMessageType(DBProxyMsgType::DBProxyAccount);
	SetSubMessageType(MSGTYPE);

	// Stores the message data
	WriteBuffer requestData;

	// Append the CDKey (string)
	requestData.AppendString(mCDKey,2);

	// Append the community (unsigned long)
	requestData.AppendLong(mCommunity);
	
	// Append the profile data
	mProfileData->PackRequestData(requestData,true);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus GetAccountFromCDKeyOp::CheckResponse()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::CheckResponse();
	if (result != WS_Success)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != GetSubMessageType()+1)
		return InvalidReplyHeader();

	// Extended unpack:
	// [WString] UserName
	// [WString] Password
	// [String ] Email

	ReadBuffer theData(mReplyData->data(), mReplyData->length());

	try
	{
		theData.ReadWString(mUserName,2);
		theData.ReadString (mEmail,2);
		theData.ReadWString(mPassword,2);
		theData.ReadString (mLastLogin,2);
		theData.ReadString (mActivated,2);

		// Unpack the profile data (custom actions)
		mProfileData->UnpackGetData(theData); 
	}
	catch (ReadBufferException&)
	{
	}

	// Everything checked out
	return WS_Success;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
