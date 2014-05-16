#ifndef __WON_UDPMANAGER_H__
#define __WON_UDPMANAGER_H__

#include "QueueSocket.h"
#include "RecvBytesFromOp.h"
#include <map>

namespace WONAPI
{

class UDPManager;
typedef SmartPtr<UDPManager> UDPManagerPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UDPManagerRecvOp : public AsyncOp
{
protected:
	QueueSocketPtr mSocket;
	UDPManagerPtr mUDPManager;
	bool mOwnSocket;

	IPAddr mFromAddr;
	ByteBufferPtr mMsg;
	
	typedef WONStatus(*MessageVerifier)(ReadBuffer &theMsg, void *theParam);
	MessageVerifier mMessageVerifier;
	void *mMessageVerifierParam;

	virtual WONStatus CheckMsg(ReadBuffer &theMsg);

	void QueueRecvOp();
	static void StaticRecvCompletion(AsyncOpPtr theOp, RefCountPtr theManager);
	void RecvCompletion(RecvBytesFromOp *theOp);

	virtual void RunHook();
	virtual void CleanupHook();

	friend class UDPManager;

public:
	UDPManagerRecvOp();
	UDPManagerRecvOp(const IPAddr &theRecvFrom);

	void SetMessageVerifier(MessageVerifier theVerifier, void *theParam = NULL);
	bool HasVerifier() { return mMessageVerifier!=NULL; }

	void SetMsg(const ByteBuffer *theMsg) { mMsg = theMsg; }
	const ByteBuffer* GetMsg() { return mMsg; }

	void SetFromAddr(const IPAddr &theAddr) { mFromAddr = theAddr; }
	const IPAddr& GetFromAddr() { return mFromAddr; }

	void SetUDPManager(UDPManager *theManager) { mUDPManager = theManager; }
	void SetSocket(QueueSocket *theSocket) { mSocket = theSocket; mOwnSocket = false; }
};
typedef SmartPtr<UDPManagerRecvOp> UDPManagerRecvOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UDPManagerSendRecvOp : public AsyncOp
{
protected:
	UDPManagerPtr mUDPManager;
	QueueSocketPtr mSocket;
	ByteBufferPtr mSendMsg;
	IPAddr mDestAddr;
	UDPManagerRecvOpPtr mRecvOp;
	int mNumRetransmits;
	int mRetransmitCount;
	DWORD mMsgTimeout;

	virtual void RunHook();
	virtual void CleanupHook();
	virtual void RetransmitHook();

	void TrySendRecv();
	static void StaticRecvCompletion(AsyncOpPtr theOp, RefCountPtr theManager);
	void RecvCompletion(UDPManagerRecvOp *theOp);

	static WONStatus StaticUnpackMsg(ReadBuffer &theMsg, void *theParam);
	virtual WONStatus UnpackMsg(ReadBuffer &theMsg) { return WS_Success; }

public:
	UDPManagerSendRecvOp();

	void SetUDPManager(UDPManager *theManager) { mUDPManager = theManager; }
	void SetMsgTimeout(DWORD theTimeout) { mMsgTimeout = theTimeout; }
	void SetNumRetransmits(int theNum) { mNumRetransmits = theNum; }
	void SetDestAddr(const IPAddr &theAddr) { mDestAddr = theAddr; }
	void SetSendMsg(const ByteBuffer *theMsg) { mSendMsg = theMsg; }

	const ByteBuffer* GetRecvMsg();
	const IPAddr& GetDestAddr() { return mDestAddr; }

};
typedef SmartPtr<UDPManagerSendRecvOp> UDPManagerSendRecvOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UDPManager : public RefCount
{
protected:
	QueueSocketPtr mSocket;
	OpCompletionBasePtr mDefaultCompletion;
	UDPManagerRecvOpPtr mAutoRecvOp;

	typedef std::list<UDPManagerRecvOpPtr> OpList;
	typedef std::multimap<IPAddr,UDPManagerRecvOpPtr> OpMap;

	OpList::iterator mCleanListItr;
	OpMap::iterator mCleanMapItr;
	
	OpList mOpList; // non-specific address
	OpMap mOpMap;
	int mMaxRecvBytes;
	bool mAutoRecv;

	static void StaticRecvCompletion(AsyncOpPtr theOp, RefCountPtr theManager);
	void RecvCompletion(RecvBytesFromOp *theOp);

	void Erase(const OpList::iterator &theItr);
	void Erase(const OpMap::iterator &theItr);
	void CleanOps();

public:
	UDPManager();
	void SetMaxRecvBytes(int theMaxBytes);

	WONStatus Bind(int thePort, bool allowBroadcast = false);
	unsigned short GetLocalPort();
	void Close();
	void QueueOp(UDPManagerRecvOp *theOp, DWORD theTimeout);
	void SendBytesTo(const ByteBuffer *theBytes, const IPAddr &theAddr);
	void SetDefaultCompletion(OpCompletionBase *theCompletion) { mDefaultCompletion = theCompletion; }
	void SetAutoRecv(bool autoRecv);
};

} // namespace WONAPI

#endif