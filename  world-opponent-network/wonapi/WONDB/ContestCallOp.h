#ifndef __WON_CONTESTCALLOP_H__
#define __WON_CONTESTCALLOP_H__
#include "WONShared.h"

#include "WONServer/ServerRequestOp.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ContestCallOp : public ServerRequestOp
{
private:
	unsigned short mProcNum;
	ByteBufferPtr mRequestData;
	ByteBufferPtr mReplyData;
	
	void Init();

protected:
	virtual WONStatus GetNextRequest();
	virtual WONStatus CheckResponse();
	virtual void RunHook();

public:
	ContestCallOp(ServerContext *theContestContext);
	ContestCallOp(const IPAddr &theAddr);

	void SetProcNum(unsigned short theNum) { mProcNum = theNum; }
	void SetRequestData(const ByteBuffer *theData) { mRequestData = theData; }

	unsigned short GetProcNum() const { return mProcNum; }
	ByteBufferPtr GetRequestData() const { return mRequestData; }
	ByteBufferPtr GetReplyData() const { return mReplyData; }
};

typedef SmartPtr<ContestCallOp> ContestCallOpPtr;


}; // namespace WONAPI

#endif
