
#include "GetPatchServerModuleListOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetPatchServerModuleListOp::GetPatchServerModuleListOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mMsgType(1)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetPatchServerModuleListOp::GetPatchServerModuleListOp(const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(1)
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Exceptions handled by ServerRequestOp
WONStatus GetPatchServerModuleListOp::CheckResponse()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::CheckResponse();
	if (result != WS_Success)
		return result;
	
	if (mMessageReplyType != DBProxyOp::DBProxyModuleListReply)
		return InvalidReplyHeader();
	
	// Do extended unpack
	ReadBuffer aReadBuf(mReplyData->data(), mReplyData->length());
	unsigned short numModules = aReadBuf.ReadShort();

	// Read in each module
	for (int i=0; i < numModules; i++)
	{
		unsigned long moduleID = aReadBuf.ReadLong();
		mModuleList.push_back(moduleID);
	}

	// Finished
	return WS_Success;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetPatchServerModuleListOp::RunHook()
{
	SetMessageType(DBProxyModuleListRequest);
	SetSubMessageType(mMsgType);

	// Call base class implementation
	DBProxyOp::RunHook();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetPatchServerModuleListOp::Init()
{
	Reset();
	ServerRequestOp::mLengthFieldSize = 4;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GetPatchServerModuleListOp::IsModuleAvailable(unsigned short theMsgType, unsigned short theSubMsgType)
{
	std::list<unsigned long>::const_iterator anItr = mModuleList.begin();
	for (; anItr != mModuleList.end(); ++anItr)
	{
		// Search for this message type and sub message type
		if (theMsgType == LOWORD(*anItr) && theSubMsgType == HIWORD(*anItr))
			return true;
	}

	return false;
}
