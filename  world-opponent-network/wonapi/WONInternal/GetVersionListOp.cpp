
#include "GetVersionListOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetVersionListOp::GetVersionListOp(const std::string& theProductName, ServerContext* theContext)
	: DBProxyOp(theContext),
	  mMsgType(9),
	  mProductName(theProductName),
	  mStateFilter(VersionState_BadState)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetVersionListOp::GetVersionListOp(const std::string& theProductName, const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(9),
	  mProductName(theProductName)
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Exceptions handled by ServerRequestOp
WONStatus GetVersionListOp::CheckResponse()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::CheckResponse();
	if (result != WS_Success)
		return result;
	
	if (mSubMessageReplyType != mMsgType+1)
		return InvalidReplyHeader();
	
	// Do extended unpack
	ReadBuffer aReadBuf(mReplyData->data(), mReplyData->length());
	unsigned short numVersions = aReadBuf.ReadShort();

	// Read in each version
	for (int i=0; i < numVersions; i++)
	{
		VersionDataPtr versionData = new VersionData;

		unsigned short versionDataSize = aReadBuf.ReadShort(); // record size for expandability

		versionData->ReadFromBuffer(ReadBuffer(aReadBuf.ReadBytes(versionDataSize), versionDataSize));
		mVersionDataList.push_back(versionData);
	}

	// Finished
	return WS_Success;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetVersionListOp::RunHook()
{
	SetMessageType(DBProxyPatchServer);
	SetSubMessageType(mMsgType);

	// Pack the message data
	WriteBuffer requestData;
	requestData.AppendString(mProductName);
	requestData.AppendString(mConfigName);
	requestData.AppendByte(mStateFilter);
	

	// Pack and call base class implementation
	SetProxyRequestData(requestData.ToByteBuffer());
	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetVersionListOp::Init()
{
	Reset();
	ServerRequestOp::mLengthFieldSize = 4;

	mConfigName = "%";	// % means get every config
	mStateFilter = VersionState_BadState;

}
