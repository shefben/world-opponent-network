
#include "KeySubscribeOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BaseKeySubscribeOp::BaseKeySubscribeOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BaseKeySubscribeOp::Pack(WriteBuffer& theBufferR)
{
	// Stores the message data
	theBufferR.AppendString(mProductName);
	theBufferR.AppendWString(mCommunityName);
	theBufferR.AppendShort(mKeyList.size());
	for(std::list<std::string>::iterator anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		theBufferR.AppendString(*anItr, 1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus BaseKeySubscribeOp::Unpack(ReadBuffer& theBufferR)
{
	try
	{
		mStatusList.clear();
		unsigned short aListSize = theBufferR.ReadShort();
		for(unsigned short i = 0; i < aListSize; i++)
			mStatusList.push_back(theBufferR.ReadShort());
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
InsertKeySubscribeOp::InsertKeySubscribeOp(ServerContext* theContext)
	: BaseKeySubscribeOp(theContext),
	mAllocateDate(-1),
	mAllocateDuration(0),
	mActivateTime(-1),
	mActivateDuration(0)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InsertKeySubscribeOp::RunHook()
{
	SetMessageType(DBProxyKeySubscribe);
	SetSubMessageType(MSGTYPE);

	// Stores the message data
	WriteBuffer requestData;
	BaseKeySubscribeOp::Pack(requestData);

	requestData.AppendLong(mAllocateDate);
	requestData.AppendLong(mAllocateDuration);
	requestData.AppendLong(mActivateTime);
	requestData.AppendLong(mActivateDuration);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus InsertKeySubscribeOp::CheckResponse()
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
	return BaseKeySubscribeOp::Unpack(theData);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UpdateKeySubscribeOp::UpdateKeySubscribeOp(ServerContext* theContext)
	: BaseKeySubscribeOp(theContext),
	mUpdateFields(0),
	mAllocateDate(-1),
	mAllocateDuration(0),
	mActivateTime(-1),
	mActivateDuration(0),
	mActivateUserSeq(0)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateKeySubscribeOp::RunHook()
{
	SetMessageType(DBProxyKeySubscribe);
	SetSubMessageType(MSGTYPE);

	// Stores the message data
	WriteBuffer requestData;
	BaseKeySubscribeOp::Pack(requestData);

	requestData.AppendShort(mUpdateFields);
	requestData.AppendLong(mAllocateDate);
	requestData.AppendLong(mAllocateDuration);
	requestData.AppendLong(mActivateTime);
	requestData.AppendLong(mActivateDuration);
	requestData.AppendLong(mActivateUserSeq);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus UpdateKeySubscribeOp::CheckResponse()
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
	return BaseKeySubscribeOp::Unpack(theData);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RemoveKeySubscribeOp::RemoveKeySubscribeOp(ServerContext* theContext)
	: BaseKeySubscribeOp(theContext)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RemoveKeySubscribeOp::RunHook()
{
	SetMessageType(DBProxyKeySubscribe);
	SetSubMessageType(MSGTYPE);

	// Stores the message data
	WriteBuffer requestData;
	BaseKeySubscribeOp::Pack(requestData);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RemoveKeySubscribeOp::CheckResponse()
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
	return BaseKeySubscribeOp::Unpack(theData);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
QueryKeySubscribeOp::QueryKeySubscribeOp(ServerContext* theContext)
	: DBProxyOp(theContext)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueryKeySubscribeOp::RunHook()
{
	SetMessageType(DBProxyKeySubscribe);
	SetSubMessageType(MSGTYPE);

	// Stores the message data
	WriteBuffer requestData;
	requestData.AppendString(mProductName);
	requestData.AppendWString(mCommunityName);
	requestData.AppendShort(mKeyList.size());
	for(std::list<std::string>::iterator anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		requestData.AppendString(*anItr, 1);

	// Copy buffer into the class
	SetProxyRequestData(requestData.ToByteBuffer());

	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus QueryKeySubscribeOp::CheckResponse()
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
		KEY_DATA aKey;
		mKeyDataList.clear();
		short aListSize = theData.ReadShort();
		for(short i = 0; i < aListSize; i++)
		{
			aKey.mStatus = (WONStatus)theData.ReadShort();
			aKey.mAllocateDate = theData.ReadLong();
			aKey.mAllocateDuration = theData.ReadLong();
			aKey.mActivateTime = theData.ReadLong();
			aKey.mActivateDuration = theData.ReadLong();
			aKey.mActivateUserSeq = theData.ReadLong();
			mKeyDataList.push_back(aKey);
		}
	}
	catch (ReadBufferException&)
	{
		return WS_InvalidMessage;
	}

	// Everything checked out
	return WS_Success;
}
