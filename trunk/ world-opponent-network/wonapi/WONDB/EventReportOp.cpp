#include "EventReportOp.h"
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void EventReportOp::Init()
{
	mLengthFieldSize = 4;
	mHasRelatedServer = false;
	mHasRelatedClient = false;
	mHasRelatedUser = false;

	mActivityType = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
EventReportOp::EventReportOp(ServerContext *theEventContext) : ServerRequestOp(theEventContext)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
EventReportOp::EventReportOp(const IPAddr &theAddr) : ServerRequestOp(theAddr)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus EventReportOp::GetNextRequest()
{
	WriteBuffer aBuf(mLengthFieldSize);
	aBuf.AppendByte(5);			// small message
	aBuf.AppendShort(4);		// Event Service
	aBuf.AppendShort(1);		// Report Event

	unsigned char aContentFlags = 0;
	if (mHasRelatedServer) aContentFlags |= 0x02;
	if (mHasRelatedClient) aContentFlags |= 0x04;
	if (mHasRelatedUser)   aContentFlags |= 0x08;
	aBuf.AppendByte(aContentFlags);

	aBuf.AppendByte(mDetailList.size());
	aBuf.AppendByte(mAttachmentList.size());
	
	// general info
	aBuf.AppendShort(mActivityType);
	
	// server info
	if (mHasRelatedServer)
	{
		aBuf.AppendShort(mServerType);
		aBuf.AppendWString(mServerLogicalName);
		aBuf.AppendString(mServerNetAddress);
	}

    // client info
	if (mHasRelatedClient)
	{
		aBuf.AppendWString(mClientName);
		aBuf.AppendString(mClientNetAddress);
	}

	// user info
	if (mHasRelatedUser)
	{
		aBuf.AppendShort(1); // Auth 1
		aBuf.AppendLong(mUserId);
		aBuf.AppendWString(mUserName);
	}

	// details
	DetailList::iterator aDetailItr = mDetailList.begin();
	while(aDetailItr!=mDetailList.end())
	{
		aBuf.AppendShort(aDetailItr->mDetailType);
		aBuf.AppendByte(aDetailItr->mDataType);
		switch(aDetailItr->mDataType)
		{
			case DetailStruct::DataType_Long: aBuf.AppendLong(aDetailItr->mNumber); break;
			case DetailStruct::DataType_WString: aBuf.AppendWString(aDetailItr->mString); break;
		}

		++aDetailItr;
	}

	// attachments
	AttachmentList::iterator anAttachmentItr = mAttachmentList.begin();
	while(anAttachmentItr != mAttachmentList.end())
	{
		aBuf.AppendWString(anAttachmentItr->mDescription);
		aBuf.AppendByte(anAttachmentItr->mContentType);
		aBuf.AppendBuffer(anAttachmentItr->mData,4);

		++anAttachmentItr;
	}

	mRequest = aBuf.ToByteBuffer();
	return WS_ServerReq_Recv;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus EventReportOp::CheckResponse()
{
	ReadBuffer aMsg(mResponse->data(),mResponse->length());
	unsigned char aHeaderType = aMsg.ReadByte();
	unsigned short aServiceType = aMsg.ReadShort();
	unsigned short aMessageType = aMsg.ReadShort();
	if(aHeaderType!=5 || aServiceType!=4 || aMessageType!=2)
		return InvalidReplyHeader();


	short aStatus = aMsg.ReadShort();
	return (WONStatus)aStatus;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void EventReportOp::SetServerInfo(unsigned short theServerType, const std::wstring &theServerLogicalName, 
								   const std::string &theServerNetAddr)
{
	mHasRelatedServer = true;
	mServerType = theServerType;
	mServerLogicalName = theServerLogicalName;
	mServerNetAddress = theServerNetAddr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void EventReportOp::SetClientInfo(const std::wstring &theClientName, const std::string &theClientNetAddr)
{
	mHasRelatedClient = true;
	mClientName = theClientName;
	mClientNetAddress = theClientNetAddr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void EventReportOp::SetUserInfo(unsigned long theUserId, const std::wstring &theUserName)
{
	mHasRelatedUser = true;
	mUserId = theUserId;
	mUserName = theUserName;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void EventReportOp::AddDetail(unsigned short theDetailType, unsigned long theDetail)
{
	mDetailList.push_back(DetailStruct(theDetailType,theDetail));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void EventReportOp::AddDetail(unsigned short theDetailType, const std::wstring &theDetail)
{
	mDetailList.push_back(DetailStruct(theDetailType,theDetail));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void EventReportOp:: AddAttachment(const std::wstring &theDescription, unsigned char theType, const ByteBuffer *theData)
{
	mAttachmentList.push_back(AttachmentStruct(theDescription,theType,theData));	
}
