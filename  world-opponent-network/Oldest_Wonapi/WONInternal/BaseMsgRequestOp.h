#ifndef __WON_BASEMSGREQUESTOP_H__
#define __WON_BASEMSGREQUESTOP_H__

#include "WONServer/ServerRequestOp.h"
#include <vector>

namespace WONMsg
{
	class BaseMessage;
};

namespace WONAPI
{

class BaseMsgRequestOp : public ServerRequestOp
{
public:
	typedef bool(*CheckReply)(const WONMsg::BaseMessage& theRequest, const WONMsg::BaseMessage& theResponse);
	typedef std::vector<WONMsg::BaseMessage*> ResponseList;

	BaseMsgRequestOp(ServerContext *theContext);
	BaseMsgRequestOp(const IPAddr &theAddr);
	virtual ~BaseMsgRequestOp();

	void SetRequestMsg(const WONMsg::BaseMessage &theRequest);
	void SetResponseMsg(const WONMsg::BaseMessage &theResponse);

	void SetCheckReply(CheckReply theCheck) { mCheckReply = theCheck; }

	// List will always be empty if CheckReply has not been set
	const ResponseList& GetResponseList() const { return mResponseList; }

	// ResponseMsg will contain last response received in case of multiple responses
	const WONMsg::BaseMessage* GetRequestMsg() const { return mRequestMsg; }
	const WONMsg::BaseMessage* GetResponseMsg() const { return mResponseMsg; }

private:
	WONMsg::BaseMessage *mRequestMsg;
	WONMsg::BaseMessage *mResponseMsg;
	CheckReply           mCheckReply;
	ResponseList         mResponseList;

protected:
	virtual WONStatus GetNextRequest();
	virtual WONStatus CheckResponse();
};

typedef SmartPtr<BaseMsgRequestOp> BaseMsgRequestOpPtr;

}; // namespace WONAPI

#endif
