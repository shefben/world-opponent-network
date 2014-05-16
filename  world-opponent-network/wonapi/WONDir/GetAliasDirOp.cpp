

#include "GetAliasDirOp.h"
#include "WONCommon/WriteBuffer.h"
#include "WONCommon/ReadBuffer.h"
//#include "DirEntityReplyParser.h"

using namespace std;
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAliasDirOp::Init()
{
	mLengthFieldSize   = 4;
	mNumFailedRequests = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GetAliasDirOp::GetAliasDirOp(ServerContext *theDirContext) : ServerRequestOp(theDirContext)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetAliasDirOp::GetAliasDirOp(const IPAddr &theAddr) : ServerRequestOp(theAddr)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAliasDirOp::Reset()
{
	GetEntityRequestList::const_iterator anItr = mRequestList.begin();
	for (;anItr != mRequestList.end(); ++anItr)
		(*anItr)->Reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAliasDirOp::AddRequest(GetEntityRequest* theRequest)
{
	mRequestList.push_back(theRequest);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAliasDirOp::AddService(const std::wstring& theService)
{
	GetEntityRequestPtr pRequest = new GetEntityRequest;
	pRequest->SetPath(theService);
	pRequest->SetFlags(mFlags);
	pRequest->SetDataTypes(mDataTypes);

	AddRequest(pRequest);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus GetAliasDirOp::GetNextRequest()
{
	// Once per op
	WriteBuffer aMsg(mLengthFieldSize);
	aMsg.AppendByte(5);						// Small Header
	aMsg.AppendShort(2);					// DirServer
	aMsg.AppendShort(114);					// AliasGetEntity

	aMsg.AppendShort(0);					// MaxEntitiesPerReply
	aMsg.AppendString(mProductName);
	aMsg.AppendByte(mRequestList.size());	// Number of entities requests

	// Once per request
	GetEntityRequestList::const_iterator anItr = mRequestList.begin();
	for (;anItr != mRequestList.end(); ++anItr)
	{	
		(*anItr)->Pack(&aMsg, GetEntityRequest::Pack_GetMultiDirOp);
	}
	mRequest = aMsg.ToByteBuffer();

	// Prepare the iterator for Recv
	mCurRequest = mRequestList.begin();

	return WS_ServerReq_Recv;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Parse the dir entities (One MultiEntityReply for each Request)
WONStatus GetAliasDirOp::CheckResponse()
{
	// Was the request list empty?
	if (mCurRequest == mRequestList.end())
	{
		// Construct a new request to parse out the status result
		GetEntityRequestPtr aRequest = new GetEntityRequest;
		return aRequest->ParseMultiEntityReply(mResponse->data(),mResponse->length());
	}

	(*mCurRequest)->ParseMultiEntityReply(mResponse->data(), mResponse->length());
		
	// Does this request have pending data?
	if ((*mCurRequest)->GetStatus() != WS_ServerReq_Recv)
	{
		if ((*mCurRequest)->GetStatus() != WS_Success)
			++mNumFailedRequests;
		++mCurRequest;
	}

	// Are all requests complete?
	if (mCurRequest == mRequestList.end())
	{
		if (mNumFailedRequests == 0)
			return WS_Success;
		else if (mNumFailedRequests != mRequestList.size())
			return WS_DirServ_MultiGetPartialFailure;
		else
			return WS_DirServ_MultiGetFailedAllRequests;
	}
	else
		return WS_ServerReq_Recv;

}
