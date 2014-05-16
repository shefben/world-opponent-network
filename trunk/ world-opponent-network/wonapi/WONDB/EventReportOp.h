#ifndef __WON_EVENTREPORTOP_H__
#define __WON_EVENTREPORTOP_H__
#include "WONShared.h"

#include "WONServer/ServerRequestOp.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class EventReportOp : public ServerRequestOp
{
private:
	unsigned short mActivityType;

	bool mHasRelatedServer;
	unsigned short mServerType;
	std::wstring mServerLogicalName;
	std::string mServerNetAddress;

	bool mHasRelatedClient;
	std::wstring mClientName;
	std::string mClientNetAddress;

	bool mHasRelatedUser;
	unsigned long mUserId;
	std::wstring mUserName;

	struct DetailStruct
	{
		unsigned short mDetailType;

		enum DataType
		{
			DataType_Long = 0,
			DataType_WString = 1
		};

		DataType mDataType;
		unsigned long mNumber;
		std::wstring mString;

		DetailStruct() : mDetailType(0), mDataType(DataType_Long), mNumber(0) {}
		DetailStruct(unsigned short theDetailType, unsigned long theNumber) : mDetailType(theDetailType), mDataType(DataType_Long), mNumber(theNumber) {}
		DetailStruct(unsigned short theDetailType, const std::wstring& theString) : mDetailType(theDetailType), mDataType(DataType_WString), mString(theString) {}
	};

	typedef std::list<DetailStruct> DetailList;
	DetailList mDetailList;

	struct AttachmentStruct
	{
		std::wstring mDescription;
		unsigned char mContentType;
		ByteBufferPtr mData;

		AttachmentStruct() : mContentType(0) {}
		AttachmentStruct(const std::wstring &theDescription, unsigned char theContentType, const ByteBuffer *theData) : mDescription(theDescription), mContentType(theContentType), mData(theData) {}
	};

	typedef std::list<AttachmentStruct> AttachmentList;
	AttachmentList mAttachmentList;

	void Init();

protected:
	virtual WONStatus GetNextRequest();
	virtual WONStatus CheckResponse();

public:
	EventReportOp(ServerContext *theEventContext);
	EventReportOp(const IPAddr &theAddr);

	void SetActivityType(unsigned short theType) { mActivityType = theType; }
	void SetServerInfo(unsigned short theServerType, const std::wstring &theServerLogicalName, const std::string &theServerNetAddr);
	void SetClientInfo(const std::wstring &theClientName, const std::string &theClientNetAddr);
	void SetUserInfo(unsigned long theUserId, const std::wstring &theUserName);

	void AddDetail(unsigned short theDetailType, unsigned long theDetail);
	void AddDetail(unsigned short theDetailType, const std::wstring &theDetail);

	void AddAttachment(const std::wstring &theDescription, unsigned char theType, const ByteBuffer *theData);
};

typedef SmartPtr<EventReportOp> EventReportOpPtr;

}; // namespace WONAPI

#endif
