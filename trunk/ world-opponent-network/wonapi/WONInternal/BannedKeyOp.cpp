
#include "BannedKeyOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InsertBannedKeyOp::InsertBannedKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName(""),
	  mBannedUntil(0)
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InsertBannedKeyOp::Reset()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InsertBannedKeyOp::RunHook()
{
	SetMessageType(DBProxyAccount);
	SetSubMessageType(MSGTYPE);
	mStatusList.clear();

	// Stores the message data
	WriteBuffer requestData;
	requestData.AppendWString(mCommunityName);
	requestData.AppendString(mProductName);
	requestData.AppendLong(mBannedUntil);

	requestData.AppendShort(mKeyList.size());
	for(std::list<std::string>::const_iterator anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		requestData.AppendString(*anItr,1);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus InsertBannedKeyOp::CheckResponse()
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
		unsigned short aListSize = theData.ReadShort();
		for(unsigned short i = 0; i < aListSize; i++)
			mStatusList.push_back(theData.ReadShort());
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
QueryBannedKeyOp::QueryBannedKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueryBannedKeyOp::RunHook()
{
	SetMessageType(DBProxyAccount);
	SetSubMessageType(MSGTYPE);
	mKeyList.clear();

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
WONStatus QueryBannedKeyOp::CheckResponse()
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
		unsigned short aListSize = theData.ReadShort();
		BANNED_KEY_PAIR aBannedKey;
		for(unsigned short i = 0; i < aListSize; i++)
		{
			theData.ReadString(aBannedKey.first,1);
			aBannedKey.second = theData.ReadLong();
			mKeyList.push_back(aBannedKey);
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
QueryListBannedKeyOp::QueryListBannedKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueryListBannedKeyOp::RunHook()
{
	SetMessageType(DBProxyAccount);
	SetSubMessageType(MSGTYPE);
	mKeyDataList.clear();

	// Stores the message data
	WriteBuffer requestData;
	requestData.AppendWString(mCommunityName);
	requestData.AppendString(mProductName);

	requestData.AppendShort(mKeyList.size());
	for(std::list<std::string>::const_iterator anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		requestData.AppendString(*anItr,1);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus QueryListBannedKeyOp::CheckResponse()
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
		unsigned short aListSize = theData.ReadShort();
		BANNED_KEY_PAIR aBannedKey;
		for(unsigned short i = 0; i < aListSize; i++)
		{
			aBannedKey.first = theData.ReadLong();
			aBannedKey.second = theData.ReadShort();
			mKeyDataList.push_back(aBannedKey);
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
RemoveBannedKeyOp::RemoveBannedKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RemoveBannedKeyOp::Reset()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RemoveBannedKeyOp::RunHook()
{
	SetMessageType(DBProxyAccount);
	SetSubMessageType(MSGTYPE);
	mStatusList.clear();

	// Stores the message data
	WriteBuffer requestData;
	requestData.AppendWString(mCommunityName);
	requestData.AppendString(mProductName);

	requestData.AppendShort(mKeyList.size());
	for(std::list<std::string>::const_iterator anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		requestData.AppendString(*anItr,1);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RemoveBannedKeyOp::CheckResponse()
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
		unsigned short aListSize = theData.ReadShort();
		for(unsigned short i = 0; i < aListSize; i++)
			mStatusList.push_back(theData.ReadShort());
	}
	catch (ReadBufferException&)
	{
		return WS_InvalidMessage;
	}

	// Everything checked out
	return WS_Success;
}
