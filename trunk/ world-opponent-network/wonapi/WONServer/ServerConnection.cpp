#include "ServerConnection.h"
#include "WONSocket/SocketOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerConnection::ServerConnection()
{
	mAuthType = AUTH_TYPE_NONE;
	mKeepAlivePeriod = 300;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::SetKeepAlivePeriod(DWORD theSeconds)
{
	AutoCrit aCrit(mDataCrit);
	mKeepAlivePeriod = theSeconds;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::StartKeepAlive()
{
	AutoCrit aCrit(mDataCrit);
	if(mKeepAliveOp.get()!=NULL)
	{
		mKeepAliveOp->Kill();
		mKeepAliveOp = NULL;
	}

	if(mKeepAlivePeriod!=0)
	{
		mKeepAliveOp = new AsyncOp;
		mKeepAliveOp->SetCompletion(new OpRefCompletion(StaticKeepAliveCallback,this));
		mKeepAliveOp->RunAsync(mKeepAlivePeriod*1000);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::SetAuth(AuthContext *theAuthContext, AuthType theAuthType)
{ 
	AutoCrit aCrit(mDataCrit);
	mAuthContext = theAuthContext; 
	mAuthType = theAuthType; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::StaticConnectCallback(AsyncOpPtr theOp, RefCountPtr theParam)
{
	PeerAuthOp *anOp = (PeerAuthOp*)theOp.get();
	ServerConnection *thisConnection = (ServerConnection*)theParam.get();

	thisConnection->ConnectCallback(anOp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::ConnectCallback(PeerAuthOp *theOp)
{
	AutoCrit aCrit(mDataCrit);
	if(theOp!=mPeerAuthOp.get())
		return;

	OpCompletionBasePtr aConnectCompletion = mConnectCompletion;
	mPeerAuthOp = NULL;

	aCrit.Leave();

	if(aConnectCompletion.get()!=NULL)
		aConnectCompletion->Complete(theOp);

	if(theOp->Succeeded())
	{
		SetRepeatCompletion(new OpRefCompletion(StaticMsgCallback,this));
		SetRepeatOp(new RecvMsgOp);
		StartKeepAlive();
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::StaticMsgCallback(AsyncOpPtr theOp, RefCountPtr theParam)
{
	RecvMsgOp *anOp = (RecvMsgOp*)theOp.get();
	ServerConnection *thisConnection = (ServerConnection*)theParam.get();

	if(anOp->GetStatus()==WS_Success)
		thisConnection->MsgCallback(anOp->GetMsg());
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::MsgCallback(const ByteBuffer*)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::StaticKeepAliveCallback(AsyncOpPtr theOp, RefCountPtr theParam)
{
	ServerConnection *thisConnection = (ServerConnection*)theParam.get();
	thisConnection->KeepAliveCallback(theOp);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::KeepAliveCallback(AsyncOp *theOp)
{
	AutoCrit aCrit(mDataCrit);
	if(theOp==mKeepAliveOp.get())
	{
		mKeepAliveOp = NULL;
		if(theOp->Killed())
			return;

		SendNoOp();
		StartKeepAlive();		
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::SendNoOp()
{
	WriteBuffer aBuf(mLengthFieldSize);
	aBuf.AppendByte(3); // mini message
	aBuf.AppendByte(1); // common service
	aBuf.AppendByte(7); // no-op
	QueueOp(new SendMsgOp(aBuf.ToByteBuffer()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PeerAuthOp* ServerConnection::ConnectAsync(const IPAddr &theAddr, DWORD theTimeout)
{
	AutoCrit aCrit(mDataCrit);
	Close();
	mPeerAuthOp = new PeerAuthOp(theAddr, mAuthContext, mAuthType, this);
	mPeerAuthOp->SetCompletion(new OpRefCompletion(StaticConnectCallback,this));
	mPeerAuthOp->RunAsync(theTimeout);
	return mPeerAuthOp;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus ServerConnection::ConnectBlock(const IPAddr &theAddr, DWORD theTimeout)
{
	AutoCrit aCrit(mDataCrit);
	Close();
	mPeerAuthOp = new PeerAuthOp(theAddr, mAuthContext, mAuthType, this);
	PeerAuthOpPtr aPeerAuthOp = mPeerAuthOp;
	aCrit.Leave();

	aPeerAuthOp->RunBlock(theTimeout);
	ConnectCallback(aPeerAuthOp);
	return aPeerAuthOp->GetStatus();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::SetConnectCompletion(OpCompletionBase *theCompletion)
{
	AutoCrit aCrit(mDataCrit);
	mConnectCompletion = theCompletion;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::CloseHook()
{
	if(mKeepAliveOp.get()!=NULL)
	{
		mKeepAliveOp->Kill();
		mKeepAliveOp = NULL;
	}
	if(mPeerAuthOp.get()!=NULL)
	{
		mPeerAuthOp->SetCompletion(mConnectCompletion);
	
		// This If-Pending check prevents the problem where the PeerAuthOp fails on a socket operation
		// so the ServerConnection closes and marks the PeerAuthOp as killed.  This can confuse programs
		// which ignore all ops that have been killed.
		if(mPeerAuthOp->Pending()) 
			mPeerAuthOp->Kill();
	
		mPeerAuthOp = NULL;
	}
	SetRepeatCompletion(NULL);
	BlockingSocket::CloseHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnection::KillHook()
{
	BlockingSocket::KillHook();
	mConnectCompletion = NULL;
	mAuthContext = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerConnectionConnectOp::ServerConnectionConnectOp(ServerConnection *theConnection, const IPAddr &theAddr) : 
	mAddr(theAddr), mConnection(theConnection)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnectionConnectOp::StaticConnectCompletion(AsyncOpPtr theOp, RefCountPtr that)
{

	((ServerConnectionConnectOp*)that.get())->ConnectCompletion(theOp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnectionConnectOp::ConnectCompletion(AsyncOp *theOp)
{
	if(!Pending())
		return;

	Finish(theOp->GetStatus());	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnectionConnectOp::RunHook()
{
	mConnection->SetConnectCompletion(new OpRefCompletion(StaticConnectCompletion,this));
	mConnection->ConnectAsync(mAddr,GetTimeout());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerConnectionConnectOp::CleanupHook()
{
	mConnection->SetConnectCompletion(NULL);
}

