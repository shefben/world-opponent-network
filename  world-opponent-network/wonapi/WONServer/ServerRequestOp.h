#ifndef __WON_SERVERREQUESTOP_H__
#define __WON_SERVERREQUESTOP_H__
#include "WONShared.h"


#include "WONCommon/ByteBuffer.h"
#include "WONSocket/BlockingSocket.h"
#include "ServerOp.h"


namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class ServerRequestOp : public ServerOp
{
private:
	AddrList mAddrList;
	AddrList::iterator mAddrItr;
	AuthSessionPtr mCurSession;
	AuthSessionPtr mUsedSession;
	bool mReusingSession;
	bool mSessionExpired;
	IPAddr mCurAddr;
	bool mUseAuth2;
	bool mCloseConnection;

	WONStatus mGetCertStatus;

	bool mUseLastServerErrorForFinishStatus;
	WONStatus mLastServerError;

	enum ServerReqTrack
	{
		ServerReq_Track_Socket = 1,
		ServerReq_Track_PeerAuth = 2,
	};

	void QueueSocketOp(SocketOp *theOp, DWORD theTimeout = OP_TIMEOUT_INFINITE);
	void Init();	

protected:
	ServerContextPtr mServerContext;
	ByteBufferPtr mRequest;
	ByteBufferPtr mResponse;

protected:

	WONStatus Send();
	WONStatus Recv();
	bool TryConnect();
	bool TryPeerToPeerAuth();
	bool CheckExpiredSession(); 
	bool CallbackHook(AsyncOp *theOp, int theParam);
	bool TryNextServer();
	void FinishSaveSession(WONStatus theStatus);
	void SetLastServerError(WONStatus theError);
	WONStatus InvalidReplyHeader();

protected:
	virtual WONStatus GetNextRequest() { return WS_ServerReq_Recv; }
	virtual WONStatus CheckResponse() { return WS_Success; }
	virtual void RunHook();
	virtual void CleanupHook();
	virtual void Reset() { }
	virtual ~ServerRequestOp();
	
public:
	ServerRequestOp(ServerContext *theContext);
	ServerRequestOp(const IPAddr &theAddr);

	void SetServerContext(ServerContext *theContext);
	void SetAddr(const IPAddr &theAddr);
	IPAddr GetAddr() const;
	const IPAddr& GetCurAddr() const { return mCurAddr; } // if op succeeds then this will be the address it succeeded with

	void SetRequest(const ByteBuffer *theRequest) { mRequest = theRequest; }

	const ByteBuffer* GetRequest() const { return mRequest; }
	const ByteBuffer* GetResponse() const { return mResponse; }
	AuthSession* GetSession() { return mUsedSession; }

	void SetUseAuth2(bool useAuth2) { mUseAuth2 = useAuth2; }
	WONStatus GetGetCertStatus() const { return mGetCertStatus; }
	WONStatus GetLastServerError() const { return mLastServerError; }

	void SetCloseConnection(bool close) { mCloseConnection = close; }


};
typedef SmartPtr<ServerRequestOp> ServerRequestOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ServerConnectOp : public ServerRequestOp
{
protected:
	bool mSendData;
	bool mHaveSent;
	bool mSendNoOp;

	virtual WONStatus GetNextRequest();
	virtual void Reset();
	void Init();

public:
	ServerConnectOp(ServerContext *theContext); 
	ServerConnectOp(const IPAddr &theAddr);

	void SetSendData(bool send) { mSendData = send; }
	void SetSendNoOp(bool sendNoOp) { mSendNoOp = sendNoOp; }
};
typedef SmartPtr<ServerConnectOp> ServerConnectOpPtr;

}; // namespace WONAPI

#endif
