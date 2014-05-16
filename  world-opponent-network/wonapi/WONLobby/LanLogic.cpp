#include "LanLogic.h"

#include "ChatCommandParser.h"
#include "DirectConnectDialog.h"
#include "LanNameDialog.h"
#include "LanPortDialog.h"
#include "LanTypes.h"
#include "LobbyClient.h"
#include "LobbyDialog.h"
#include "LobbyEvent.h"
#include "LobbyGame.h"
#include "LobbyMisc.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"
#include "LobbyScreen.h"
#include "LobbyStagingPrv.h"
#include "PasswordDialog.h"

#include "WONAuth/GetCertOp.h"
#include "WONAuth/PeerAuthOp.h"
#include "WONAuth/PeerAuthServerOp.h"
#include "WONGUI/GUIConfig.h"
#include "WONMisc/DNSTestOp.h"
#include "WONSocket/BlockingSocket.h"

#include <time.h>

using namespace WONAPI;

namespace WONAPI
{

static const int gLanLengthFieldSize = 2;
typedef LogicCompletion<LanLogic> LanLogicCompletion;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyLanClient : public LobbyClient
{
public:
	LobbyLanSocket *mSocket;

	void SetName(const GUIString &theName) { mName = theName; }
	void SetClientId(unsigned short theId) { mClientId = theId; }


	LobbyLanClient() : mSocket(NULL) { }
};
typedef SmartPtr<LobbyLanClient> LobbyLanClientPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyLanSocket : public BlockingSocket
{
public:
	LobbyLanClientPtr mClient;
	time_t mLastRecvTime;
	
	LobbyLanSocket() { SetLengthFieldSize(gLanLengthFieldSize); }
	virtual AsyncSocket* Duplicate() { return new LobbyLanSocket; }

	void SetClient(LobbyLanClient *theClient);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyLanSocket::SetClient(LobbyLanClient *theClient)
{
	if(mClient.get()!=NULL)
		mClient->mSocket = NULL;

	mClient = theClient;
	if(mClient.get()!=NULL)
		mClient->mSocket = this;
}

} // namespace WONAPI



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LanLogic::LanLogic()
{	
	mNextClientId = 0;
	mLastClientSocketSendTime = 0;
	mIAmCaptain = false;
	mNeedAuth = false;
	mClientList = new LobbyClientList;
	mLanGameList = new LobbyGameList(LobbyGameType_Lan);
	mDirectConnectGameList = new LobbyGameList(LobbyGameType_DirectConnect);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LanLogic::~LanLogic()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::Init(ComponentConfig *theConfig)
{
	WONComponentConfig_Get(theConfig,mDirectConnectDialog,"DirectConnectDialog");
	WONComponentConfig_Get(theConfig,mLanNameDialog,"LanNameDialog");
	WONComponentConfig_Get(theConfig,mLanPortDialog,"LanPortDialog");
	WONComponentConfig_Get(theConfig,mPasswordDialog,"PasswordDialog");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned short LanLogic::GetFreeClientId()
{
	if(mNextClientId==LobbyServerId_Invalid || mNextClientId==0)  // skip invalid id and "group id"
		mNextClientId++;

	return mNextClientId++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LanLogic::InGame()
{
	return mListenSocket.get()!=NULL || mClientSocket.get()!=NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LanLogic::IAmCaptain()
{
	return mIAmCaptain;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClientList* LanLogic::GetClientList()
{
	return mClientList;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* LanLogic::NetGetMyClient()
{
	return mMyClient;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::KillNet()
{
	AuthContext *anAuth = LobbyMisc::GetAuthContext();
	if(anAuth!=NULL)
		anAuth->SetDoAutoRefresh(false); // stop auto refreshing the AuthContext

	mNextClientId = 0;
	mIAmCaptain = false;
	mNeedAuth = false;

	if(mListenSocket.get()!=NULL)
	{
		mListenSocket->Kill();
		mListenSocket = NULL;
	}

	if(mClientSocket.get()!=NULL)
	{
		mClientSocket->QueueClose();
		mClientSocket = NULL;
	}

	if(mClientKeepAliveTimer.get()!=NULL)
	{
		mClientKeepAliveTimer->Kill();
		mClientKeepAliveTimer = NULL;
	}

	for(SocketSet::iterator anItr = mSocketSet.begin(); anItr!=mSocketSet.end(); ++anItr)
	{
		QueueSocket *aSocket = *anItr;
		aSocket->QueueClose();
	}

	mMyClient = NULL;
	mSocketSet.clear();


	mClientList->Clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::CloseCompletion(AsyncOp *theOp)
{
	SocketOp *anOp = (SocketOp*)theOp;
	LobbyLanSocket *aSocket = (LobbyLanSocket*)anOp->GetSocket();
	if(aSocket==NULL)
		return;
	
	if(IAmCaptain())
	{
		if(aSocket->mClient.get()!=NULL) // tell everyone they left
		{
			WriteBuffer aClientLeft(gLanLengthFieldSize);
			aClientLeft.AppendByte(LanMsgType_ClientLeft);
			aClientLeft.AppendShort(aSocket->mClient->GetClientId());

			aSocket->SetClient(NULL);

			BroadcastMessage(aClientLeft.ToByteBuffer());
		}
		mSocketSet.erase(aSocket);
	}
	else // disconnected from captain
	{
		if(aSocket!=NULL && aSocket==mClientSocket)
		{
			mClientSocket->Kill();
			mClientSocket = NULL;
			LobbyStagingPrv::NotifyGameDisconnect();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::ClientKeepAliveCompletion(AsyncOp *theOp)
{
	if(theOp!=mClientKeepAliveTimer.get())
		return;


	if(IAmCaptain()) 
	{
		// Captain checks to make sure there are no dead clients
		time_t aTime = time(NULL);
		for(SocketSet::iterator anItr = mSocketSet.begin(); anItr!=mSocketSet.end(); ++anItr)
		{
			LobbyLanSocket *aSocket = *anItr;
			if(aTime - aSocket->mLastRecvTime > 300) // haven't heard anything in 5 minutes -> drop them
				aSocket->QueueClose(1000);
		}
	}
	else if(mClientSocket.get()!=NULL)
	{
		// Handle sending keep alives to the captain of LAN/DirectConnect game
		time_t aTime = time(NULL);
		if(aTime - mLastClientSocketSendTime >= 120) // send no-op ever couple of minutes if nothing else has been sent
		{
			WriteBuffer aMsg(gLanLengthFieldSize);
			aMsg.AppendByte(LanMsgType_KeepAlive);
			SendMessageToCaptain(aMsg.ToByteBuffer());
		}
	}

	StartClientKeepAliveTimer();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::StartClientKeepAliveTimer()
{
	if(mClientKeepAliveTimer.get()==NULL)
	{
		mClientKeepAliveTimer = new AsyncOp;
		mClientKeepAliveTimer->SetCompletion(new LanLogicCompletion(ClientKeepAliveCompletion,this));
	}

	mClientKeepAliveTimer->RunAsTimer(120*1000); // just check for keep alive every couple of minutes
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleRegisterRequest(ReadBuffer &theMsg, LobbyLanSocket *theSocket)
{
	if(mMyClient.get()==NULL)
		return;

	LobbyGame *aGame = LobbyStaging::GetGame();
	if(aGame==NULL)
		return;

	// Read Message
	std::wstring aClientName;
	std::wstring aPassword;
	theMsg.ReadWString(aClientName);
	theMsg.ReadWString(aPassword);

	// Construct RegisterReply
	WriteBuffer aReply(gLanLengthFieldSize);
	aReply.AppendByte(LanMsgType_RegisterReply);

	// check that the user is on the same subnet, 
	if(aGame->GetGameType()==LobbyGameType_Lan) 
	{
		IPAddr aLocalAddr = IPAddr::GetLocalAddr();
		const char* aJoinAddress = theSocket->GetDestAddr().GetSixByte();
		const char* aLocalAddress = aLocalAddr.GetSixByte(); 
		if(aJoinAddress[2]!=aLocalAddress[2] || aJoinAddress[3]!=aLocalAddress[3])
		{
			aReply.AppendShort(LanRegisterStatus_NotOnSubnet);
			theSocket->QueueOp(new SendMsgOp(aReply.ToByteBuffer()));
			return;
		}
	}
	
	// Check password
	if(aPassword!=(std::wstring)aGame->GetPassword())
	{
		// Bad Password
		aReply.AppendShort(LanRegisterStatus_BadPassword);
		theSocket->QueueOp(new SendMsgOp(aReply.ToByteBuffer()));
		return;
	}

	// Construct new client data structure
	LobbyLanClientPtr aClient = new LobbyLanClient;
	aClient->SetName(aClientName);
	aClient->SetClientId(GetFreeClientId());
	mClientList->AddClient(aClient);

	// Send Client Joined
	WriteBuffer aClientJoined(gLanLengthFieldSize);
	aClientJoined.AppendByte(LanMsgType_ClientJoined);
	aClientJoined.AppendShort(aClient->GetClientId());
	aClientJoined.AppendWString(aClient->GetName());
	BroadcastMessage(aClientJoined.ToByteBuffer());

	// Hook up the client to the socket and cancel the connect close timer
	theSocket->SetClient(aClient);

	// Send Reply to client
	aReply.AppendShort(LanRegisterStatus_Success); // successfully joined
	aReply.AppendShort(mMyClient->GetClientId());
	aReply.AppendShort(aClient->GetClientId());

	const LobbyClientMap &aMap = mClientList->GetClientMap();
	aReply.AppendShort(aMap.size());
	for(LobbyClientMap::const_iterator anItr = aMap.begin(); anItr!=aMap.end(); ++anItr)
	{
		LobbyClient *aClient = anItr->second;
		aReply.AppendShort(aClient->GetClientId());
		aReply.AppendWString(aClient->GetName());
	}

	SendMessageToClient(aClient,aReply.ToByteBuffer());

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LanRegisterStatus LanLogic::HandleRegisterReply(const ByteBuffer *theMsg, LobbyGame *theGame)
{
	if(theMsg==NULL || mClientSocket.get()==NULL || theGame==NULL)
		return LanRegisterStatus_None;

	ReadBuffer aMsg(theMsg->data(), theMsg->length());
	try
	{
		unsigned char aMsgType = aMsg.ReadByte();
		if(aMsgType!=LanMsgType_RegisterReply)
			return LanRegisterStatus_None;

		short aStatus = aMsg.ReadShort();
		if(aStatus!=LanRegisterStatus_Success)
			return (LanRegisterStatus)aStatus;

		unsigned short aCaptainId = aMsg.ReadShort();
		unsigned short myClientId = aMsg.ReadShort();
		theGame->SetCaptainId(aCaptainId);

		unsigned short aNumClients = aMsg.ReadShort();
		for(int i=0; i<aNumClients; i++)
		{
			unsigned short aClientId = aMsg.ReadShort();
			std::wstring aClientName;
			aMsg.ReadWString(aClientName);
			LobbyLanClientPtr aClient = new LobbyLanClient;
			aClient->SetClientId(aClientId);
			aClient->SetName(aClientName);
			mClientList->AddClient(aClient);
			
			if(aClientId==myClientId)
				mMyClient = aClient;
			else if(aClientId==aCaptainId)
			{
				mClientSocket->SetClient(aClient);
				aClient->SetIsCaptain(true,true);
			}
		}
		return LanRegisterStatus_Success;
	}
	catch(ReadBufferException&)
	{
		return LanRegisterStatus_None;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleNonClientRequest(const ByteBuffer *theReq, LobbyLanSocket *theSocket)
{
	if(theReq==NULL || theSocket==NULL || !IAmCaptain())
		return;

	ReadBuffer aMsg(theReq->data(),theReq->length());
	try
	{
		unsigned char aType = aMsg.ReadByte();
		switch(aType)
		{
			case LanMsgType_RegisterRequest: HandleRegisterRequest(aMsg,theSocket); break;
		}
	}
	catch(ReadBufferException&)
	{
	}
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::RecvCompletion(AsyncOp *theOp)
{
	if(!theOp->SucceededAlive())
		return;

	RecvMsgOp *anOp = (RecvMsgOp*)theOp;
	LobbyLanSocket *aSocket = (LobbyLanSocket*)anOp->GetSocket();

	if(IAmCaptain())
	{
		aSocket->mLastRecvTime = time(NULL);
		if(aSocket->mClient.get()==NULL)
		{
			HandleNonClientRequest(anOp->GetMsg(),aSocket);
			return;
		}
	}
	
	HandleRecvMessage(anOp->GetMsg(),aSocket->mClient);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::AddSocket(LobbyLanSocket *theSocket)
{
	theSocket->SetCloseCompletion(new LanLogicCompletion(CloseCompletion,this));
	theSocket->SetRepeatCompletion(new LanLogicCompletion(RecvCompletion,this));
	theSocket->SetRepeatOp(new RecvMsgOp);
	theSocket->mLastRecvTime = time(NULL);

	mSocketSet.insert(theSocket);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::AcceptAuthCompletion(AsyncOp *theOp)
{
	PeerAuthServerOp *anOp = (PeerAuthServerOp*)theOp;
	LobbyLanSocket *aSocket = (LobbyLanSocket*)anOp->GetSocket();
	if(!anOp->SucceededAlive())
	{
		aSocket->QueueClose(1000);
		return;
	}

	Auth2Certificate *aClientCert = anOp->GetClientCertificate();
	AuthContext *anAuth = LobbyMisc::GetAuthContext();
	if(anAuth==NULL)
		return;

	DWORD myCommunity = anAuth->GetPeerData()->GetCertificate2()->GetFirstCommunity();		
	if(!aClientCert->VerifyPermissions(0,myCommunity,0))
	{
		aSocket->QueueClose(1000); // not correct community
		return;
	}

	AddSocket(aSocket);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::AcceptCompletion(AsyncOp *theOp)
{
	if(!theOp->SucceededAlive())
		return;

	AcceptOp *anOp = (AcceptOp*)theOp;
	LobbyLanSocket *aSocket = (LobbyLanSocket*)anOp->GetAcceptedSocket();
	
	if(mNeedAuth)
	{
		AuthContext *anAuth = LobbyMisc::GetAuthContext();
		if(anAuth==NULL)
			return;

		PeerAuthServerOpPtr aPeerAuthOp = new PeerAuthServerOp(anAuth,aSocket);
		RunOpTrack(aPeerAuthOp,new LanLogicCompletion(AcceptAuthCompletion,this),30000);
	}
	else
		AddSocket(aSocket);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::SendMessageToClient(LobbyLanClient *theClient, const ByteBuffer *theMsg)
{
	if(!IAmCaptain() || !InGame())
		return;

	if(theClient==mMyClient.get())
		HandleRecvMessage(theMsg,mMyClient);
	else if(theClient->mSocket!=NULL)
		theClient->mSocket->QueueOp(new SendMsgOp(theMsg));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::SendMessageToCaptain(const ByteBuffer *theMsg)
{
	if(!InGame())
		return;

	if(IAmCaptain())
		HandleRecvMessage(theMsg,mMyClient);
	else if(mClientSocket.get()!=NULL)
	{
		mLastClientSocketSendTime = time(NULL);
		mClientSocket->QueueOp(new SendMsgOp(theMsg));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::BroadcastMessage(const ByteBuffer *theMsg, bool sendToMe)
{
	if(!IAmCaptain())
		return;


	if(sendToMe)
		HandleRecvMessage(theMsg,mMyClient);
	
	for(SocketSet::iterator anItr = mSocketSet.begin(); anItr!=mSocketSet.end(); ++anItr)
	{
		LobbyLanSocket *aSocket = *anItr;
		if(aSocket->mClient.get()!=NULL)
			aSocket->QueueOp(new SendMsgOp(theMsg));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetSendGameMessageToCaptain(const ByteBuffer *theMsg)
{
	WriteBuffer aBuf(gLanLengthFieldSize);
	aBuf.AppendByte(LanMsgType_GameMessage);
	aBuf.AppendBuffer(theMsg);

	SendMessageToCaptain(aBuf.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetBroadcastGameMessage(const ByteBuffer *theMsg)
{
	WriteBuffer aBuf(gLanLengthFieldSize);
	aBuf.AppendByte(LanMsgType_GameMessage);
	aBuf.AppendBuffer(theMsg);

	BroadcastMessage(aBuf.ToByteBuffer(),false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetSendGameMessageToClient(LobbyClient *theClient, const ByteBuffer *theMsg)
{
	WriteBuffer aBuf(gLanLengthFieldSize);
	aBuf.AppendByte(LanMsgType_GameMessage);
	aBuf.AppendBuffer(theMsg);

	SendMessageToClient((LobbyLanClient*)theClient,aBuf.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleClientJoined(ReadBuffer &theMsg)
{
	unsigned short anId = theMsg.ReadShort();
	std::wstring aClientName;
	theMsg.ReadWString(aClientName);
	
	LobbyClientPtr aClient;
	if(IAmCaptain())
		aClient = mClientList->GetClient(anId); // client was added in register request
	else
	{
		LobbyLanClientPtr aNewClient = new LobbyLanClient;
		aNewClient->SetClientId(anId);
		aNewClient->SetName(aClientName);
		aClient = aNewClient;
		mClientList->AddClient(aClient);
	}

	if(aClient.get()!=NULL)
		LobbyEvent::BroadcastEvent(new ClientChangedEvent(aClient,LobbyRoomSpecFlag_Lan, LobbyChangeType_Add));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleClientLeft(ReadBuffer &theMsg)
{
	unsigned short anId = theMsg.ReadShort();
	LobbyClientPtr aClient = mClientList->RemoveClient(anId);
	if(aClient.get()!=NULL)
	{
		LobbyEvent::BroadcastEvent(new ClientChangedEvent(aClient,LobbyRoomSpecFlag_Lan, LobbyChangeType_Delete));
		LobbyStagingPrv::NotifyClientLeftGame(aClient);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleChatRequest(ReadBuffer &theMsg, LobbyLanClient *theSender)
{
	if(!IAmCaptain())
		return;

	unsigned short aFlags = theMsg.ReadShort();
	std::wstring aChat;
	theMsg.ReadWString(aChat);
	unsigned short aDestId = theMsg.ReadShort();

	WriteBuffer aChatMsg(gLanLengthFieldSize);
	aChatMsg.AppendByte(LanMsgType_Chat);
	aChatMsg.AppendShort(aFlags);
	aChatMsg.AppendShort(theSender->GetClientId());
	aChatMsg.AppendWString(aChat);
	aChatMsg.AppendShort(aDestId);

	ByteBufferPtr aBuf = aChatMsg.ToByteBuffer();
	if(aFlags & LobbyChatFlag_IsWhisper)
	{
		LobbyLanClient *aDest = (LobbyLanClient*)mClientList->GetClient(aDestId);
		if(aDest==NULL)
			return;

		SendMessageToClient(aDest,aBuf);
		if(theSender!=aDest)
			SendMessageToClient(theSender,aBuf);
	}
	else if(aFlags & LobbyChatFlag_TeamChat)
	{
		LobbyClientListItr aClientList(mClientList);
		while(aClientList.HasMoreClients())
		{
			LobbyLanClient *aClient = (LobbyLanClient*)aClientList.GetNextClient();
			if(aClient->SameTeam(theSender))
				SendMessageToClient(aClient,aBuf);
		}	
	}
	else
		BroadcastMessage(aBuf);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleChat(ReadBuffer &theMsg)
{
	unsigned short aFlags = theMsg.ReadShort();
	unsigned short aSenderId = theMsg.ReadShort();
	std::wstring aChat;
	theMsg.ReadWString(aChat);
	unsigned short aDestId = theMsg.ReadShort();

	LobbyClient *aSender = mClientList->GetClient(aSenderId);
	if(aSender==NULL)
		return;

	if(aSender->IsIgnored())
		return;
 
	LobbyClient *aDestClient = NULL;
	if(aFlags & LobbyChatFlag_IsWhisper)
	{
		aFlags = LobbyChatFlag_IsWhisper;
		aDestClient = mClientList->GetClient(aDestId);
		if(aDestClient==NULL)
			return;
		
		bool toMe = aDestClient==mMyClient;
		if(aSender!=mMyClient.get())
		{
			if(LobbyPersistentData::GetChatSoundEffects())
				LobbyMisc::PlaySound(LobbyGlobal_WhisperRecv_Sound);
		
			ChatCommandParser *aParser = LobbyMisc::GetChatCommandParser();
			if(aParser!=NULL)
				aParser->SetLastWhisperClientName(aSender->GetName());			
		}

	}

	LobbyEvent::BroadcastEvent(new RecvChatEvent(LobbyRoomSpecFlag_Lan, aFlags, aSender,aChat,aDestClient));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleGameMessage(ReadBuffer &theMsg, LobbyLanClient *theSender)
{
	LobbyStagingPrv::HandleGameMessage(theMsg,theSender);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleRecvMessage(const ByteBuffer *theMsg, LobbyLanClient *theSender)
{
	if(theMsg==NULL || theSender==NULL)
		return;

	int anOffset = 0;
	if(IAmCaptain() && theSender==mMyClient.get())
		anOffset = gLanLengthFieldSize;

	ReadBuffer aMsg(theMsg->data() + anOffset, theMsg->length() - anOffset);
	try
	{
		unsigned char aMsgType = aMsg.ReadByte();
		switch(aMsgType)
		{
			case LanMsgType_ClientJoined: HandleClientJoined(aMsg); break;
			case LanMsgType_ClientLeft: HandleClientLeft(aMsg); break;
			case LanMsgType_ChatRequest: HandleChatRequest(aMsg,theSender); break;
			case LanMsgType_Chat: HandleChat(aMsg); break;
			case LanMsgType_GameMessage: HandleGameMessage(aMsg, theSender); break;
		}
	}
	catch(ReadBufferException&)
	{
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleControlEvent(int theControlId)
{
	switch(theControlId)
	{
		case ID_AddLANName: HandleAddLanName(); break;
		case ID_ChangeLANBroadcast: HandleChangeLanBroadcastPort(); break;
		case ID_RefreshGameList: HandleRefreshGameList(); break;
		case ID_ShowLanScreen: HandleShowLanScreen(); return;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_ChangeDirectConnect: HandleChangeDirectConnect((ChangeDirectConnectEvent*)theEvent); return;
		case LobbyEvent_ClientAction: HandleClientAction((ClientActionEvent*)theEvent); return;
		case LobbyEvent_DoChatCommand: HandleDoChatCommand(theEvent->mComponent); return;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleAddLanName()
{
	// do popup dialog for name entry (see password entry cfg)
	mLanNameDialog->Reset();
	int aResult = LobbyDialog::DoDialog(mLanNameDialog);
	if(aResult!=ControlId_Ok || mLanNameDialog->GetName().empty())
		return;

	LobbyPersistentData::SetLanName(mLanNameDialog->GetName());
	LobbyEvent::BroadcastEvent(LobbyEvent_SetLanOptions);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleChangeLanBroadcastPort()
{
	int anOldPort = LobbyPersistentData::GetLanBroadcastPort();

	// do popup dialog for port entry
	mLanPortDialog->SetGamePort(LobbyStaging::GetLobbyPort());
	mLanPortDialog->SetPort(anOldPort);
	mLanPortDialog->Reset();

	int aResult = LobbyDialog::DoDialog(mLanPortDialog);
	if(aResult!=ControlId_Ok)
		return;

	int aNewPort = mLanPortDialog->GetPort();
	if(aNewPort==anOldPort)
		return;

	LobbyPersistentData::SetLanBroadcastPort(aNewPort);
	LobbyEvent::BroadcastEvent(LobbyEvent_SetLanOptions);
	StartLanListen();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleChangeDirectConnect(ChangeDirectConnectEvent *theEvent)
{
	if(theEvent->IsDelete())
	{
		if(theEvent->mGame.get()==NULL)
			return;

		LobbyPersistentData::RemoveDirectConnection(theEvent->mGame->GetIPAddr());

		mDirectConnectGameList->RemoveGameByAddr(theEvent->mGame->GetIPAddr());
		LobbyEvent::BroadcastEvent(new GameChangedEvent(theEvent->mGame,LobbyChangeType_Delete));
	}
	else if(theEvent->IsAdd())
	{		
		mDirectConnectDialog->Reset();
		if(LobbyDialog::DoDialog(mDirectConnectDialog)!=ControlId_Ok)
			return;

		IPAddr anAddr;
		LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
		int aLobbyPort = LobbyStaging::GetLobbyPort();
		if(aConfig!=NULL)
			aLobbyPort = aConfig->mLobbyPort; // use default lobby port

		anAddr.SetWithDefaultPortNoDNS(mDirectConnectDialog->GetAddress(),aLobbyPort);
		if(!anAddr.IsValid())
		{
			DNSTestOpPtr anOp = new DNSTestOp(mDirectConnectDialog->GetAddress());
			LobbyDialog::DoStatusOp(anOp,LANLogic_CheckingAddressTitle_String,LANLogic_CheckingAddressDetails_String,CloseStatusDialogType_Success);
			if(!anOp->SucceededAlive())
				return;

			anAddr = anOp->GetHostAddr();
			if(anAddr.GetPort()==0)
				anAddr.SetThePort(aLobbyPort);
		}

		GUIString aName = mDirectConnectDialog->GetName();
		if(aName.empty())
			aName = mDirectConnectDialog->GetAddress();

		bool isModify = false;
		LobbyGame* aGame = mDirectConnectGameList->AddGameByAddr(anAddr,&isModify);
		aGame->SetName(aName);

		LobbyPersistentData::AddDirectConnection(anAddr,aName);

		LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,isModify?LobbyChangeType_Modify:LobbyChangeType_Add));
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "ClientOptionsPopup.h"
void LanLogic::ShowClientOptions(LobbyClient *theClient, int theRoomSpecFlags)
{
	if(mMyClient.get()==NULL || theClient==NULL)
		return;

	ClientOptionsPopupPtr aPopup = new ClientOptionsPopup;
	aPopup->Init(theClient,mMyClient,theRoomSpecFlags);
	LobbyDialog::DoPopup(aPopup,PopupFlag_StandardPopup);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleClientAction(ClientActionEvent *theEvent)
{
	if(!InGame())
		return;

	if(!theEvent->mRoomSpec.IsLan())
		return;

	LobbyClient *aClient = theEvent->mClient;
	if(aClient==NULL)
		return;

	LobbyClientAction anAction = theEvent->mAction;

	switch(anAction)
	{
		case LobbyClientAction_ShowMenu: ShowClientOptions(aClient,theEvent->mRoomSpec); break;

//		case LobbyClientAction_Block: LobbyMisc::SetBlocked(aClient,true,LobbyRoomSpecFlag_Internet); break;
//		case LobbyClientAction_ClearAFK: aConnection->SetClientFlags(RoutingClientFlag_IsAway,0,0,0); break;
		case LobbyClientAction_Ignore: LobbyMisc::SetIgnored(aClient,true,LobbyRoomSpecFlag_Lan); break;
//		case LobbyClientAction_Invite: InviteClient(aClient,true,""); break;
//		case LobbyClientAction_Uninvite: InviteClient(aClient,false,""); break;
//		case LobbyClientAction_SetAFK: aConnection->SetClientFlags(RoutingClientFlag_IsAway,RoutingClientFlag_IsAway,0,0);  break;
//		case LobbyClientAction_Unblock: LobbyMisc::SetBlocked(aClient,false,LobbyRoomSpecFlag_Internet); break;
		case LobbyClientAction_Unignore: LobbyMisc::SetIgnored(aClient,false,LobbyRoomSpecFlag_Lan); break;	
		case LobbyClientAction_Whisper: LobbyEvent::BroadcastEvent(new SetChatInputEvent(LobbyChatCommand_Whisper, aClient->GetName(), theEvent->mRoomSpec)); break;
		case LobbyClientAction_Kick: LobbyStaging::KickClient(aClient,false); break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::SendChatRequest(const GUIString &theText, unsigned short theFlags, unsigned short theDestId)
{
	WriteBuffer aMsg(gLanLengthFieldSize);

	aMsg.AppendByte(LanMsgType_ChatRequest);
	aMsg.AppendShort(theFlags);
	aMsg.AppendWString(theText);
	aMsg.AppendShort(theDestId);

	SendMessageToCaptain(aMsg.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleDoChatCommand(Component *theChatCtrl)
{
	if(!InGame())
		return;

	ChatCommandParser *aParser = LobbyMisc::GetChatCommandParser();
	if(aParser==NULL)
		return;

	if(!(aParser->GetRoomSpecFlags() & LobbyRoomSpecFlag_Lan))
		return;

	LobbyClient *aTargetClient = aParser->GetLastTargetClient();
		
	LobbyChatCommand aCommand = aParser->GetLastChatCommand();
	switch(aCommand)
	{
		case LobbyChatCommand_Broadcast:
		case LobbyChatCommand_Emote:
		{
			int aFlags = aCommand==LobbyChatCommand_Emote ? LobbyChatFlag_IsEmote : 0;
			if(aParser->GetRoomSpecFlags() & LobbyRoomSpecFlag_Team)
				aFlags |= LobbyChatFlag_TeamChat;

			SendChatRequest(aParser->GetLastChat(),aFlags);
		}
		break;

		case LobbyChatCommand_Whisper: 
			if(aTargetClient!=NULL)
				SendChatRequest(aParser->GetLastChat(), LobbyChatFlag_IsWhisper,aTargetClient->GetClientId());
			break;
		
		case LobbyChatCommand_Ignore:
		{
			if(aTargetClient!=NULL)
				LobbyMisc::SetIgnored(aTargetClient,!aTargetClient->IsIgnored(),LobbyRoomSpecFlag_Internet);
			break;
		}

		default:
			LobbyEvent::DeliverEventTo(new ChatCommandErrorEvent(RoutingLogic_InvalidCommand_String),theChatCtrl);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleRefreshGameList()
{
	LobbyGameType aGameType = LobbyScreen::GetGameScreenType();
	if(aGameType==LobbyGameType_Lan)
	{
		mLanGameList->Clear();
		LobbyEvent::BroadcastEvent(new SyncGameListEvent(mLanGameList));
	
		if(mBroadcastSocket.get()!=NULL)
		{
			// Request current games
			WriteBuffer aMsg;
			aMsg.AppendByte(LanBroadcastMsgType_GameInfoRequest);
			mBroadcastSocket->QueueOp(new SendBytesToOp(aMsg.ToByteBuffer(),mBroadcastAddr));
		}
	}
	else if(aGameType==LobbyGameType_DirectConnect)
	{
		mDirectConnectGameList->Rewind();
		while(mDirectConnectGameList->HasMoreGames())
		{
			LobbyGame *aGame = mDirectConnectGameList->NextGame();
			if(!aGame->IsFiltered())
				LobbyMisc::QueuePing(aGame);
		}

		LobbyEvent::BroadcastEvent(LobbyEvent_SyncFilteredGameList);
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleShowLanScreen()
{
	LobbyPersistentData::SetCurUser(NULL);
	mLanGameList->Clear();
	mDirectConnectGameList->Clear();

	const LobbyPersistentData::DirectConnectMap &aMap = LobbyPersistentData::GetDirectConnectMap();
	LobbyPersistentData::DirectConnectMap::const_iterator anItr;
	for(anItr = aMap.begin(); anItr != aMap.end(); ++anItr) 
	{
		LobbyGame* aGame = mDirectConnectGameList->AddGameByAddr(anItr->first);
		aGame->SetName(anItr->second);		
	}

	LobbyEvent::BroadcastEvent(new SyncGameListEvent(mDirectConnectGameList));
	StartLanListen();

	LobbyScreen::ShowLanScreen(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LanLogic::DoDirectConnectAuth()
{
	AuthContext *anAuth = LobbyMisc::GetAuthContext();
	if(anAuth==NULL)
		return false;

	if(!LobbyMisc::IsCDKeyValid())
	{
		LobbyDialog::DoErrorDialog(AccountLogic_InvalidCDKeyTitle_String,AccountLogic_InvalidCDKeyDetails_String); 
		return false;
	}

	if(!anAuth->IsExpired(120))
	{
		anAuth->SetDoAutoRefresh(true); // make sure to keep renewing the certificate 
		return true;
	}

	anAuth->SetUserName(L"");
	anAuth->SetPassword(L"");

	GetCertOpPtr anOp = new GetCertOp(anAuth);
	LobbyDialog::DoStatusOp(anOp,LobbyContainer_LoginTitle_String,LobbyContainer_LoginDetails_String,CloseStatusDialogType_Success);
	
	if(anOp->Succeeded())
	{
		anAuth->SetDoAutoRefresh(true); // make sure to keep renewing the certificate 
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LanLogic::NetCreateGame(LobbyGame *theGame)
{
	KillNet();

	// Check if we need to authenticate for direct connect games
	if(theGame->GetGameType()==LobbyGameType_DirectConnect && LobbyMisc::DoCDKeyCheck()) 
	{
		mNeedAuth = true;
		if(!DoDirectConnectAuth())
			return false;
	}
	

	LobbyMisc::KillFirewallDetectOps(); // need to get the game port for myself

	mIAmCaptain = true;

	mMyClient = new LobbyLanClient;
	mMyClient->SetClientId(GetFreeClientId());
	mMyClient->SetName(LobbyPersistentData::GetLanName());
	mMyClient->SetIsCaptain(true,true);
	mClientList->AddClient(mMyClient);

	theGame->SetClientList(mClientList);

	mListenSocket = new LobbyLanSocket;
	WONStatus aStatus = mListenSocket->Bind(LobbyStaging::GetLobbyPort());
	if(aStatus!=WS_Success)
	{
		LobbyDialog::ShowStatus(LANLogic_SocketBindError_String,aStatus);
		return false;
	}

	aStatus = mListenSocket->Listen();
	if(aStatus!=WS_Success)
	{
		LobbyDialog::ShowStatus(LANLogic_SocketListenError_String,aStatus);
		return false;
	}

	mListenSocket->SetRepeatCompletion(new LanLogicCompletion(AcceptCompletion,this));
	mListenSocket->SetRepeatOp(new AcceptOp);

	LobbyEvent::BroadcastEvent(new JoinedRoomEvent("", theGame->GetName(), 
		LobbyRoomSpecFlag_Lan | LobbyRoomSpecFlag_Game, mMyClient->GetClientId(), 
		theGame->GetClientList()));

	mLastHeartBeatTime = 0;
	StartClientKeepAliveTimer();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LanLogic::NetJoinGame(LobbyGame *theGame)
{
	KillNet();

	// Check if we need to authenticate for direct connect games
	AuthContextPtr anAuth;
	if(theGame->GetGameType()==LobbyGameType_DirectConnect && LobbyMisc::DoCDKeyCheck())
	{
		mNeedAuth = true;
		if(!DoDirectConnectAuth())
			return false;

		anAuth = LobbyMisc::GetAuthContext();
	}


	mClientSocket = new LobbyLanSocket;


	bool doPasswordDialog = theGame->HasPassword();
	bool success = false;
	bool needConnect = true;

	while(!success)
	{
		GUIString aPassword;
		if(doPasswordDialog)
		{
			mPasswordDialog->Reset();
			int aResult = LobbyDialog::DoDialog(mPasswordDialog);
			if(aResult!=ControlId_Ok)
				return false;

			aPassword = mPasswordDialog->GetPassword();
		}

		if(needConnect)
		{
			// Connect to host
			AsyncOpPtr aConnectOp;
			if(mNeedAuth)
				aConnectOp = new PeerAuthOp(theGame->GetIPAddr(),anAuth,AUTH_TYPE_PERSISTENT,mClientSocket);
			else
				aConnectOp = new ConnectOp(theGame->GetIPAddr(), mClientSocket);

			LobbyDialog::DoStatusOp(aConnectOp,LANLogic_ConnectToGameTitle_String, LANLogic_ConnectToGameDetails_String, CloseStatusDialogType_Success);
			if(!aConnectOp->Succeeded())
				return false;

			needConnect = false;
		}

		// Send register request
		WriteBuffer aRegisterReq(gLanLengthFieldSize);
		aRegisterReq.AppendByte(LanMsgType_RegisterRequest);
		aRegisterReq.AppendWString(LobbyPersistentData::GetLanName());
		aRegisterReq.AppendWString(aPassword);
		mClientSocket->QueueOp(new SendMsgOp(aRegisterReq.ToByteBuffer()));

		mLastClientSocketSendTime = time(NULL);

		RecvMsgOpPtr aRecvOp = new RecvMsgOp(mClientSocket);
		LobbyDialog::DoStatusOp(aRecvOp,LANLogic_RegisterTitle_String,LANLogic_RegisterDetails_String,CloseStatusDialogType_Success);
		if(!aRecvOp->Succeeded())
			return false;

		LanRegisterStatus aStatus = HandleRegisterReply(aRecvOp->GetMsg(),theGame);
		switch(aStatus)
		{
			case LanRegisterStatus_Success: success = true; break;
			case LanRegisterStatus_None: return false;
			case LanRegisterStatus_BadPassword: 
				LobbyDialog::DoErrorDialog(GameStagingLogic_JoinGameErrorTitle_String,LobbyGameStatus_InvalidPassword_String);				
				doPasswordDialog = true;  
				break;

			case LanRegisterStatus_NotOnSubnet:
				LobbyDialog::DoErrorDialog(LANLogic_RegisterTitle_String,LANLogic_InvalidSubnet_String); 
				return false;
		}
	}

	mClientSocket->SetCloseCompletion(new LanLogicCompletion(CloseCompletion,this));
	mClientSocket->SetRepeatCompletion(new LanLogicCompletion(RecvCompletion,this));
	mClientSocket->SetRepeatOp(new RecvMsgOp);
	StartClientKeepAliveTimer();

	theGame->SetClientList(mClientList);
	LobbyEvent::BroadcastEvent(new JoinedRoomEvent("", theGame->GetName(), 
		LobbyRoomSpecFlag_Lan | LobbyRoomSpecFlag_Game, mMyClient->GetClientId(), 
		theGame->GetClientList()));


	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetKickClient(LobbyClient *theClient, bool isBan)
{
	if(!IAmCaptain())
		return;

	LobbyLanClient *aClient = (LobbyLanClient*)theClient;
	aClient->mSocket->QueueClose(1000);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetLeaveGame(LobbyGame *theGame)
{
	KillNet();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetUpdateGameSummary(LobbyGame *theGame)
{
	if(theGame->GetGameType()==LobbyGameType_Lan)
		SendGameInfo(theGame, mBroadcastAddr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetDeleteGameSummary(LobbyGame *theGame)
{
	if(theGame->GetGameType()==LobbyGameType_Lan)
		BroadcastGameDeleted(theGame);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetCloseGame(bool close)
{
	// fixme: Close listen socket?  Or keep it open in order to send reject message.
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LanLogic::NeedBroadcast(LobbyGame *theGame)
{
	if(!IAmCaptain())
		return false;

	if(mBroadcastSocket.get()==NULL || theGame==NULL)
		return false;

	if(theGame->GetGameType()!=LobbyGameType_Lan)
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::SendGameInfo(LobbyGame *theGame, const IPAddr& theAddr)
{
	if(!NeedBroadcast(theGame))
		return;

	WriteBuffer aMsg;
	aMsg.AppendByte(LanBroadcastMsgType_GameInfo);
	aMsg.AppendShort(theGame->GetIPAddr().GetPort());
	theGame->WriteSummary(aMsg);
	mBroadcastSocket->QueueOp(new SendBytesToOp(aMsg.ToByteBuffer(),theAddr));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::BroadcastGameHeartbeat(LobbyGame *theGame)
{
	if(!NeedBroadcast(theGame))
		return;

	time_t aTime = time(NULL);
	if(mLastHeartBeatTime>0 && aTime - mLastHeartBeatTime >= 30) // haven't sent a heartbeat recently
		SendGameInfo(theGame, mBroadcastAddr);
	else
	{
		WriteBuffer aMsg;
		aMsg.AppendByte(LanBroadcastMsgType_Heartbeat);
		aMsg.AppendShort(theGame->GetIPAddr().GetPort());
		mBroadcastSocket->QueueOp(new SendBytesToOp(aMsg.ToByteBuffer(),mBroadcastAddr));
	}

	mLastHeartBeatTime = aTime;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::BroadcastGameDeleted(LobbyGame *theGame)
{
	if(!NeedBroadcast(theGame))
		return;

	WriteBuffer aMsg;
	aMsg.AppendByte(LanBroadcastMsgType_GameDeleted);
	aMsg.AppendShort(theGame->GetIPAddr().GetPort());
	mBroadcastSocket->QueueOp(new SendBytesToOp(aMsg.ToByteBuffer(),mBroadcastAddr));
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::BroacastTimerCompletion(AsyncOp *theOp)
{
	if(theOp!=mBroadcastTimer.get())
		return;

	// If we're the captain of a game, then send out a heartbeat to the lan to let 
	// everyone know that it's still there.
	if(IAmCaptain())
	{
		LobbyGame *aGame = LobbyStaging::GetGame();
		if(aGame!=NULL)
			BroadcastGameHeartbeat(aGame);
	}

	// Get rid of any LAN games that haven't sent a heartbeat for a while
	if(mLanGameList->PurgeGamesWithNoHeartbeat(30))
		LobbyEvent::BroadcastEvent(new SyncGameListEvent(mLanGameList));

	StartBroadcastTimer();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleGameInfoRequest(ReadBuffer &theMsg, const IPAddr &theAddr)
{
	LobbyGame *aGame = LobbyStaging::GetGame();
	if(aGame==NULL)
		return;

	SendGameInfo(aGame,theAddr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleGameInfo(ReadBuffer &theMsg, const IPAddr &theAddr)
{
	IPAddr aFromAddr = theAddr;
	aFromAddr.SetThePort(theMsg.ReadShort());

	bool isModify = false;
	LobbyGame* aGame = mLanGameList->AddGameByAddr(aFromAddr,&isModify);

	if(aGame->ReadSummary(theMsg))
	{
		aGame->SetLastHeartbeat(time(NULL));		
		LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,isModify?LobbyChangeType_Modify:LobbyChangeType_Add));
	}
	else
	{
		LobbyGamePtr aSaveGame = aGame; // make sure game is refcounted
		mLanGameList->RemoveGameByAddr(aFromAddr);
		if(isModify)
			LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,isModify?LobbyChangeType_Modify:LobbyChangeType_Delete));
	}		
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleGameHeartbeat(ReadBuffer &theMsg, const IPAddr &theAddr)
{
	IPAddr aFromAddr = theAddr;
	aFromAddr.SetThePort(theMsg.ReadShort());
	if(aFromAddr.GetHost()!=theAddr.GetHost())
		return;

	LobbyGame *aGame = mLanGameList->GetGameByAddr(aFromAddr);
	if(aGame==NULL)
		return;

	aGame->SetLastHeartbeat(time(NULL));

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::HandleGameDeleted(ReadBuffer &theMsg, const IPAddr &theAddr)
{
	IPAddr aFromAddr = theAddr;
	aFromAddr.SetThePort(theMsg.ReadShort());
	if(aFromAddr.GetHost()!=theAddr.GetHost())
		return;

	LobbyGamePtr aGame = mLanGameList->RemoveGameByAddr(aFromAddr);
	if(aGame.get()!=NULL)
		LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,LobbyChangeType_Delete));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::RecvBroadcastCompletion(AsyncOp *theOp)
{
	RecvBytesFromOp *anOp = (RecvBytesFromOp*)theOp;
	if(anOp->GetSocket()!=mBroadcastSocket.get() || !anOp->SucceededAlive())
		return;

	ReadBuffer aMsg(anOp->GetBytes()->data(), anOp->GetBytes()->length());
	const IPAddr &aFromAddr = anOp->GetAddr();
	try
	{
		unsigned char aMsgType = aMsg.ReadByte();
		switch(aMsgType)
		{
			case LanBroadcastMsgType_GameInfo:			HandleGameInfo(aMsg,aFromAddr); break;		
			case LanBroadcastMsgType_Heartbeat:			HandleGameHeartbeat(aMsg,aFromAddr); break;
			case LanBroadcastMsgType_GameDeleted:		HandleGameDeleted(aMsg,aFromAddr); break;
			case LanBroadcastMsgType_GameInfoRequest:	HandleGameInfoRequest(aMsg, aFromAddr); break;
		}
	}
	catch(ReadBufferException&)
	{
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::StartBroadcastTimer()
{
	if(mBroadcastTimer.get()==NULL)
	{
		mBroadcastTimer = new AsyncOp;
		mBroadcastTimer->SetCompletion(new LanLogicCompletion(BroacastTimerCompletion,this));
	}

	mBroadcastTimer->RunAsTimer(15000);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::StartLanListen()
{
	StopLanListen();

	mBroadcastAddr = IPAddr::GetBroadcastAddr(LobbyPersistentData::GetLanBroadcastPort());
	mBroadcastSocket = new QueueSocket(AsyncSocket::UDP);
	WONStatus aStatus = mBroadcastSocket->Bind(LobbyPersistentData::GetLanBroadcastPort());
	if(aStatus!=WS_Success)
	{
		StopLanListen(); // fixme, Show error?
		return;
	}

	mBroadcastSocket->SetRepeatCompletion(new LanLogicCompletion(RecvBroadcastCompletion,this));
	mBroadcastSocket->SetRepeatOp(new RecvBytesFromOp(1024));
	StartBroadcastTimer();	

	// Request current games
	WriteBuffer aMsg;
	aMsg.AppendByte(LanBroadcastMsgType_GameInfoRequest);
	mBroadcastSocket->QueueOp(new SendBytesToOp(aMsg.ToByteBuffer(),mBroadcastAddr));

	if(IAmCaptain()) // need to broadcast our game
	{
		LobbyGame *aGame = LobbyStaging::GetGame();
		if(aGame!=NULL)
			SendGameInfo(aGame,mBroadcastAddr);
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::StopLanListen()
{
	if(!mLanGameList->IsEmpty())
	{
		mLanGameList->Clear();
		LobbyEvent::BroadcastEvent(new SyncGameListEvent(mLanGameList));
	}

	if(mBroadcastTimer.get()!=NULL)
	{
		mBroadcastTimer->Kill();
		mBroadcastTimer = NULL;
	}

	if(mBroadcastSocket.get()!=NULL)
	{
		mBroadcastSocket->Kill();
		mBroadcastSocket = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetSuspendLobby()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetResumeLobby()
{
	LobbyStaging::LeaveGame(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LanLogic::NetLogout()
{
	StopLanListen();

	mDirectConnectGameList->Clear();
}

