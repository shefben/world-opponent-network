
#include "MasterKeyOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InsertMasterKeyOp::InsertMasterKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName(""),
	  mSequenceNumber(0),
	  mIsBeta(false),
	  mActivateTime(0),
	  mDeactivateTime(0),
	  mKeyLength(0),
	  mCDKeyFlags(0x0001)
{
	memset(mKeyP, '\0', 20);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InsertMasterKeyOp::RunHook()
{
	SetMessageType(DBProxyAccount);
	SetSubMessageType(MSGTYPE);

	// Stores the message data
	WriteBuffer requestData;
	requestData.AppendWString(mCommunityName);
	requestData.AppendString(mProductName);
	requestData.AppendShort(mSequenceNumber);
	requestData.AppendBool(mIsBeta);
	requestData.AppendLong(mActivateTime);
	requestData.AppendLong(mDeactivateTime);
	requestData.AppendByte(mKeyLength);
	requestData.AppendBytes(mKeyP, mKeyLength);
	requestData.AppendLong(mCDKeyFlags);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus InsertMasterKeyOp::CheckResponse()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::CheckResponse();
	if (result != WS_Success)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != GetSubMessageType()+1)
		return InvalidReplyHeader();

	// Extended unpack: None

	// Everything checked out
	return WS_Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UpdateMasterKeyOp::UpdateMasterKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName(""),
	  mSequenceNumber(0),
	  mIsBeta(false),
	  mActivateTime(0),
	  mDeactivateTime(0),
	  mCDKeyFlags(0x0001)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateMasterKeyOp::RunHook()
{
	SetMessageType(DBProxyAccount);
	SetSubMessageType(MSGTYPE);

	// Stores the message data
	WriteBuffer requestData;
	requestData.AppendWString(mCommunityName);
	requestData.AppendString(mProductName);
	requestData.AppendShort(mSequenceNumber);
	requestData.AppendBool(mIsBeta);
	requestData.AppendLong(mActivateTime);
	requestData.AppendLong(mDeactivateTime);
	requestData.AppendLong(mCDKeyFlags);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus UpdateMasterKeyOp::CheckResponse()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::CheckResponse();
	if (result != WS_Success)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != GetSubMessageType()+1)
		return InvalidReplyHeader();

	// Extended unpack: None

	// Everything checked out
	return WS_Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DeleteMasterKeyOp::DeleteMasterKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName(""),
	  mSequenceNumber(0)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DeleteMasterKeyOp::RunHook()
{
	SetMessageType(DBProxyAccount);
	SetSubMessageType(MSGTYPE);

	// Stores the message data
	WriteBuffer requestData;
	requestData.AppendWString(mCommunityName);
	requestData.AppendString(mProductName);
	requestData.AppendShort(mSequenceNumber);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus DeleteMasterKeyOp::CheckResponse()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::CheckResponse();
	if (result != WS_Success)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != GetSubMessageType()+1)
		return InvalidReplyHeader();

	// Extended unpack: None

	// Everything checked out
	return WS_Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
QueryMasterKeyOp::QueryMasterKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueryMasterKeyOp::RunHook()
{
	SetMessageType(DBProxyAccount);
	SetSubMessageType(MSGTYPE);

	// Stores the message data
	WriteBuffer requestData;
	requestData.AppendWString(mCommunityName);
	requestData.AppendString(mProductName);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus QueryMasterKeyOp::CheckResponse()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::CheckResponse();
	if (result != WS_Success)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != GetSubMessageType()+1)
		return InvalidReplyHeader();

	// Extended unpack
	ReadBuffer theData(mReplyData->data(), mReplyData->length());

	try
	{
		MasterKey aKey;
		mMasterKeyList.clear();
		short aCount = theData.ReadShort();
		for(short i = 0; i < aCount; i++)
		{
			aKey.mSequenceNumber = theData.ReadShort();
			aKey.mActivateDate = theData.ReadLong();
			aKey.mDeactivateDate = theData.ReadLong();
			aKey.mIsBeta = (theData.ReadByte() == 0) ? false : true;
			aKey.mCDKeyFlags = theData.ReadLong();
			mMasterKeyList.push_back(aKey);
		}
	}
	catch (ReadBufferException&)
	{
		return WS_InvalidMessage;
	}

	// Everything checked out
	return WS_Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AuthCheckKeyOp::AuthCheckKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AuthCheckKeyOp::RunHook()
{
	SetMessageType(DBProxyAccount);
	SetSubMessageType(MSGTYPE);

	// Stores the message data
	WriteBuffer requestData;
	requestData.AppendWString(mCommunityName, 1);
	requestData.AppendString(mProductName, 1);
	requestData.AppendShort(mKeyList.size());

	for(KEY_LIST_CITR anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		requestData.AppendString(*anItr, 1);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus AuthCheckKeyOp::CheckResponse()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::CheckResponse();
	if (result != WS_Success)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != GetSubMessageType()+1)
		return InvalidReplyHeader();

	// Extended unpack
	ReadBuffer theData(mReplyData->data(), mReplyData->length());

	try
	{
		mKeyStatusList.clear();
		short aCount = theData.ReadShort();
		for(short i = 0; i < aCount; i++)
			mKeyStatusList.push_back(theData.ReadShort());
	}
	catch (ReadBufferException&)
	{
		return WS_InvalidMessage;
	}

	// Everything checked out
	return WS_Success;
}
