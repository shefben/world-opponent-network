#include "InitLogic.h"

#include "LobbyCompletion.h"
#include "LobbyConfig.h"
#include "LobbyEvent.h"
#include "LobbyMisc.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"
#include "LobbyScreen.h"
#include "LobbyStaging.h"
#include "WONLobby.h"
#include "WONStatusString.h"
#include "ChooseNetworkAdapterDialog.h"

#include "WONCommon/FileWriter.h"
#include "WONCommon/WONFile.h"
#include "WONDir/GetMultiDirOp.h"
#include "WONGUI/Window.h"
#include "WONMisc/DetectFirewallOp.h"
#include "WONMisc/DNSTestOp.h"
#include "WONMisc/CheckValidVersionOp.h"
#include "WONMisc/GetHTTPDocumentOp.h"
#include "WONMisc/HTTPEngine.h"
#include "WONSocket/AcceptOp.h"


using namespace WONAPI;

typedef LogicCompletion<InitLogic> InitLogicCompletion;
static int gFirewallAcceptTimeout = 20000;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InitLogic::InitLogic()
{
	mHTTPSession = new HTTPSession;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InitLogic::~InitLogic()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::Init(ComponentConfig *theConfig)
{
	WONComponentConfig_Get(theConfig,mChooseNetworkAdapterDialog,"ChooseNetworkAdapterDialog");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::HandleChooseNetworkAdapter()
{
	mChooseNetworkAdapterDialog->Reset();
	if(LobbyDialog::DoDialog(mChooseNetworkAdapterDialog)!=ControlId_Ok)
		return;

	long aHost = mChooseNetworkAdapterDialog->GetSelHost();
	LobbyMisc::CheckSetNetworkAdapter(aHost);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InitLogic::SetLastInitError(int theFlags, const GUIString &theDesc)
{
	mLastInitErrorFlags = (theFlags & ~mIgnoreInitErrorFlags);
	if(mLastInitErrorFlags!=0)
	{
		int anAllowIgnoreFlags = InitErrorFlag_NoInternetConnection | InitErrorFlag_NoAccountServers;
		bool allowIgnore = (mLastInitErrorFlags & anAllowIgnoreFlags)==mLastInitErrorFlags;
		LobbyEvent::BroadcastEvent(new LoginInitStatusEvent(LobbyContainer_InitFailed_String, LoginInitStatus_InitFailed, theDesc,allowIgnore));
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::StartNetInit(bool isRetry)
{
	LobbyMisc::SetVersionStatus(WS_None);
	mLastInitErrorFlags = 0;

	// Start initialization by checking if DNS lookup is working
	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aConfig!=NULL)
	{
		if(aConfig->mCheckForInternet && !AsyncSocket::HasInternetConnection())
		{
			if(SetLastInitError(InitErrorFlag_NoInternetConnection,LobbyContainer_NoConnection_String))
				return;
		}
	}

	// Clear out all server contexts
	if(!isRetry)
	{
		ServerContext *anAccountServers = LobbyMisc::GetAccountServers();
		ServerContext *anAuthServers = LobbyMisc::GetAuthServers();
		ServerContext *aFirewallServers = LobbyMisc::GetFirewallDetectServers();
		ServerContext *aPatchServers = LobbyMisc::GetPatchServers();
		if(anAccountServers) anAccountServers->Clear();
		if(anAuthServers) anAuthServers->Clear();
		if(aFirewallServers) aFirewallServers->Clear();
		if(aPatchServers) aPatchServers->Clear();
	}


	RunDNSTestOp();

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::DoInit()
{
	mIgnoreInitErrorFlags = 0;

	// Load the lobby's persistent data 
	LobbyPersistentData::ReadGlobal();

	// Set the preferred network adapter
	IPAddr::SetLocalHost(LobbyPersistentData::GetNetworkAdapter(),true);

	// Find the current modified times of the TOU
	SetTOUTimes();

	// Init UDP Manager
	// fixme, choose network adapter first
	LobbyMisc::RestartUDPManager();

	// Fill in the login names in the LoginCtrl
	LobbyEvent::BroadcastEvent(LobbyEvent_SetLoginNames);

	// Disable controls that enable you to talk to TitanServers.
	// The login button starts out disabled and enables itself when initialization 
	// finished successfully.
	Container *aLobby = LobbyScreen::GetLobbyContainer();
	if(aLobby==NULL)
		return;
	
	ControlIdSet aDisable;
	aDisable.AddIds(ID_ShowCreateAccount,ID_ShowLostPassword,ID_ShowLostUserName);
	aLobby->EnableControls(aDisable,false);

	LobbyGlobal_LobbyMusic_Sound->SetLooping(true);
	if(LobbyPersistentData::GetLobbyMusic())
		LobbyGlobal_LobbyMusic_Sound->Play();
	else
		LobbyGlobal_LobbyMusic_Sound->Stop(); // in case music is already playing from the game

	StartNetInit(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::DNSTestCompletion(AsyncOp* theOp)
{
	DNSTestOp *anOp = (DNSTestOp*)theOp;

	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	ServerContext *aDirServers = LobbyMisc::GetDirServers();
	ServerContext *aRoomDirServers = LobbyMisc::GetRoomDirServers();
	ServerContext *anAuthServers = LobbyMisc::GetAuthServers();
	if(aConfig==NULL || aDirServers==NULL || aRoomDirServers==NULL || anAuthServers==NULL)
		return;

	if(anOp->Succeeded())
	{
		IPAddr anAddr;
		anAddr.SetRememberHostString(true);
		aDirServers->RemoveAllAddresses();
		for(WONTypes::StringList::const_iterator anItr = aConfig->mDirServersList.begin(); anItr != aConfig->mDirServersList.end(); ++anItr)
			aDirServers->AddAddress(*anItr);

		for(anItr = aConfig->mRoomDirServersList.begin(); anItr != aConfig->mRoomDirServersList.end(); ++anItr)
			aRoomDirServers->AddAddress(*anItr);

		for(anItr = aConfig->mAuthServers.begin(); anItr != aConfig->mAuthServers.end(); ++anItr)
			anAuthServers->AddAddress(*anItr);

		if(aRoomDirServers->GetNumAddresses() < 1)
			LobbyMisc::SetRoomDirServersToDirServers();

		// Get Message of the Day and TOU
		RunHTTPOp();

		// Get TitanServers simultaneously with MOTD
		RunTitanServersOp();
	}
	else
		SetLastInitError(InitErrorFlag_DNSLookupFailed,LobbyContainer_DNSLookupFailed_String);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::GetCrossPromotionCompletion(AsyncOp *theOp)
{
	HTTPGetOp* anOp = (HTTPGetOp*)theOp;
	if(anOp->Succeeded())
	{
		ImagePtr anImage = Window::GetDefaultWindow()->DecodeImage(anOp->GetLocalPath().c_str());
		LobbyEvent::BroadcastEvent(new SetCrossPromotionEvent(anImage.get(),mPromotionLink));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::ProcessCrossPromotionDoc(const ByteBuffer* theMsg)
{
	WriteBuffer aNewBuffer;
	aNewBuffer.AppendBytes(theMsg->data(), theMsg->length());
	aNewBuffer.AppendByte(0);
	char* aData = aNewBuffer.data();

	char aSeparator[] = " \t\n=";
	std::string anImageRef("");
	mPromotionLink = "";

	char* aToken = strtok(aData,aSeparator);
	while(aToken!=NULL)
	{
		if(stricmp(aToken,"link")==0)
		{
			aToken = strtok(NULL,"\n");	// the rest of the line
			if(aToken!=NULL)
				mPromotionLink = aToken;
		}
		else if(stricmp(aToken,"image")==0)
		{
			aToken = strtok(NULL,"\n");	// the rest of the line
			if(aToken!=NULL)
				anImageRef = aToken;
		}
		if(aToken)
			aToken = strtok(NULL,aSeparator);
	}
	if(anImageRef.length())
	{
		HTTPGetOpPtr anImageOp = new HTTPGetOp(anImageRef);
		anImageOp->SetHTTPCache(HTTPCache::GetGlobalCache());
		anImageOp->SetCompletion(new InitLogicCompletion(GetCrossPromotionCompletion,this));
		mHTTPSession->AddOp(anImageOp);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::SetTOUTimes()
{
	time_t aSysTOU = 0, aGameTOU = 0;
	WONFile aFile(LobbyMisc::GetSysTOUPath());
	aSysTOU = aFile.GetModifyTime();

	aFile.SetFilePath(LobbyMisc::GetGameTOUPath());
	aGameTOU = aFile.GetModifyTime();
	LobbyMisc::SetTOUTimes(aSysTOU,aGameTOU);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::HandleTOU(GetHTTPDocumentOp *theOp)
{
	time_t oldSysTime, oldGameTime;
	LobbyMisc::GetTOUTimes(oldSysTime, oldGameTime);

	bool needSetTimes = false;


	// Copy SysTOU if newer
	if(theOp->GetDocumentStatus(HTTPDocOwner_Sys)==WS_Success && theOp->GetModifiedTime(HTTPDocOwner_Sys)>oldSysTime)
	{
		// write the sys document
		FileWriter aWriter;
		try
		{
			if(aWriter.Open(LobbyMisc::GetSysTOUPath().c_str(),true))
			{
				ByteBufferPtr aDoc = theOp->GetDocument(HTTPDocOwner_Sys);
				if(aDoc->length()>1)
					aWriter.WriteBytes(aDoc->data(), aDoc->length()-1); // don't write the null character at the end
			}
			needSetTimes = true;
		}
		catch(FileWriterException&)
		{
		}
	}

	// Copy GameTOU if newer
	if(theOp->GetDocumentStatus(HTTPDocOwner_Game)==WS_Success && theOp->GetModifiedTime(HTTPDocOwner_Game)>oldGameTime)
	{
		// write the game document
		FileWriter aWriter;
		try
		{
			if(aWriter.Open(LobbyMisc::GetGameTOUPath().c_str(),true))
			{
				ByteBufferPtr aDoc = theOp->GetDocument(HTTPDocOwner_Game);
				if(aDoc->length()>1)
					aWriter.WriteBytes(aDoc->data(), aDoc->length()-1); // don't write the null character at the end
			}
			needSetTimes = true;
		}
		catch(FileWriterException&)
		{
		}
	}

	if(needSetTimes)
		SetTOUTimes();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::GetHTTPDocCompletion(AsyncOp *theOp)
{
	GetHTTPDocumentOp *anOp = (GetHTTPDocumentOp*)theOp;
	LobbyEvent::BroadcastEvent(new GotHTTPDocumentEvent(anOp));

	switch(anOp->GetDocType())
	{
		case HTTPDocType_MOTD: break;
		
		case HTTPDocType_TOU: HandleTOU(anOp); break;

		case HTTPDocType_ADV:
			{
				ByteBufferPtr aDoc = anOp->GetDocument(HTTPDocOwner_Game);
				if(aDoc->length()>1)
				{
					ProcessCrossPromotionDoc(aDoc);
					break;
				}
				else
					LobbyEvent::BroadcastEvent(new SetCrossPromotionEvent(NULL,""));
			}
			break;
		default:
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::GetTitanServersCompletion(AsyncOp *theOp)
{
	ServerContext *anAccountServers = LobbyMisc::GetAccountServers();
	ServerContext *anAuthServers = LobbyMisc::GetAuthServers();
	ServerContext *aFirewallServers = LobbyMisc::GetFirewallDetectServers();
	ServerContext *aPatchServers = LobbyMisc::GetPatchServers();
	ServerContext *aDirServers = LobbyMisc::GetDirServers();

	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();

	if(aConfig==NULL || anAccountServers==NULL || anAuthServers==NULL || aFirewallServers==NULL || aDirServers==NULL)
		return;

	GetMultiDirOp *anOp = (GetMultiDirOp*)theOp;
	if(anOp->Succeeded() || anOp->GetStatus()==WS_DirServ_MultiGetPartialFailure)
	{
		const GetEntityRequestList &aList = anOp->GetRequestList();
		GetEntityRequestList::const_iterator anItr = aList.begin();

		int aCount = 0;
		while(anItr!=aList.end())
		{
			GetEntityRequest *aRequest = *anItr;
			if(aRequest->GetStatus()==WS_Success)
			{
				ServerContext *aContext = NULL;
				switch(aCount)
				{
					case 0: if(aConfig->mAuthServers.empty()) aContext = anAuthServers; break;
					case 1: aContext = anAccountServers; break;
					case 2: aContext = aFirewallServers; break;
					case 3: aContext = aPatchServers; break;
				}

				if(aContext!=NULL)
					aContext->AddAddressesFromDir(aRequest->GetDirEntityList());
			}
			
			++aCount;
			++anItr;
		}

		if(anAuthServers->IsEmpty() || anAccountServers->IsEmpty() || aPatchServers->IsEmpty())
		{
			aDirServers->NotifyFailed(anOp->GetCurAddr());
			GUIString aList;
			int anInitErrorFlags = 0;
			if(anAuthServers->IsEmpty())
			{
				aList.append(LobbyContainer_AuthServers_String);
				anInitErrorFlags |= InitErrorFlag_NoAuthServers;
			}
			if(anAccountServers->IsEmpty())
			{
				if(!aList.empty())
					aList.append(", ");
			
				aList.append(LobbyContainer_AccountServers_String);
				anInitErrorFlags |= InitErrorFlag_NoAccountServers;
			}
			if(aPatchServers->IsEmpty())
			{
				if(!aList.empty())
					aList.append(", ");

				aList.append(LobbyContainer_VersionServers_String);
				anInitErrorFlags |= InitErrorFlag_NoVersionServers;
			}

			GUIString aString = LobbyContainer_NoServers_String;
			aString.append(aList);
	
			if(SetLastInitError(anInitErrorFlags, aString))
				return;
		}

		RunVersionOp();
		RunDetectFirewallOp();
	}
	else
	{
		GUIString aString = StringLocalize::GetStr(LobbyContainer_DirLookupFailure_String,FriendlyWONStatusToString(anOp->GetStatus()));
		SetLastInitError(InitErrorFlag_DirLookupFailed,aString);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::CheckVersionCompletion(AsyncOp *theOp)
{
	Container *aLobbyContainer = LobbyScreen::GetLobbyContainer();
	if(aLobbyContainer==NULL)
		return;

	CheckValidVersionOp *anOp = (CheckValidVersionOp*)theOp;
	WONStatus aStatus = anOp->GetStatus();
	LobbyMisc::SetVersionStatus(aStatus);

	if(aStatus!=WS_Success && aStatus!=WS_DBProxyServ_ValidNotLatest && aStatus!=WS_DBProxyServ_OutOfDate)
	{
		// any other error is considered a get version error
		if(aStatus==WS_DBProxyServ_OutOfDateNoUpdate || aStatus==WS_DBProxyServ_DBError) // try next server on retry
		{
			ServerContext *aPatchServers = LobbyMisc::GetPatchServers();
			if(aPatchServers!=NULL)
				aPatchServers->NotifyFailed(anOp->GetCurAddr()); 
		}

		GUIString aString = StringLocalize::GetStr(LobbyContainer_VersionCheckFailure_String,FriendlyWONStatusToString(aStatus));
		SetLastInitError(InitErrorFlag_VersionCheckFailure,aString);

		return;
	}

	ControlIdSet anEnable;
	anEnable.AddIds(ID_ShowCreateAccount,ID_ShowLostPassword,ID_ShowLostUserName);
	aLobbyContainer->EnableControls(anEnable,true);

	LobbyEvent::BroadcastEvent(new LoginInitStatusEvent(LobbyContainer_InitComplete_String,LoginInitStatus_InitSuccess));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::AcceptCompletion(AsyncOp *theOp)
{
	if(theOp!=mAcceptOp.get())
		return;

	AcceptOp *anAcceptOp = mAcceptOp;
	mAcceptOp = NULL;

	if(!anAcceptOp->Succeeded())
	{
		LobbyStaging::SetBehindFirewall(true);
		LobbyEvent::BroadcastEvent(LobbyEvent_SyncFirewallStatus);
		return;
	}

	AsyncSocket *aSocket = anAcceptOp->GetAcceptedSocket();
	if(aSocket->GetDestAddr().GetHost()==mFirewallDetectorAddr.GetHost())
	{
		LobbyStaging::SetBehindFirewall(false);
		LobbyEvent::BroadcastEvent(LobbyEvent_SyncFirewallStatus);
		return;
	}

	// Not correct address... try again
	mAcceptOp = new AcceptOp(anAcceptOp->GetSocket());
	RunOp(mAcceptOp,new InitLogicCompletion(AcceptCompletion,this),gFirewallAcceptTimeout);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::DetectFirewallCompletion(AsyncOp *theOp)
{
	DetectFirewallOp* anOp = (DetectFirewallOp*)theOp;

	bool behindFirewall = !anOp->Succeeded();
	if(anOp->Succeeded())
	{
		LobbyStaging::SetPublicAddr(anOp->GetPublicAddr());
		if(mAcceptSocket.get()!=NULL)
		{
			mFirewallDetectorAddr = anOp->GetCurAddr();
			mAcceptOp = new AcceptOp(mAcceptSocket);
			RunOp(mAcceptOp,new InitLogicCompletion(AcceptCompletion,this),gFirewallAcceptTimeout);
			mAcceptSocket = NULL; // don't need reference anymore
		}
	}
	else
		LobbyStaging::SetBehindFirewall(true); 	// Detect failed

	LobbyEvent::BroadcastEvent(LobbyEvent_SyncFirewallStatus);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::RunDNSTestOp()
{
	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aConfig==NULL)
		return;

	if(aConfig->mDirServersList.empty())
		return;

	LobbyEvent::BroadcastEvent(new LoginInitStatusEvent(LobbyContainer_InitStart_String,LoginInitStatus_InitStart));

	DNSTestOpPtr aDNSTestOp = new DNSTestOp(*(aConfig->mDirServersList.begin()));
	RunOpTrack(aDNSTestOp,new InitLogicCompletion(DNSTestCompletion,this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::RunHTTPOp()
{
	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aConfig==NULL)
		return;

	GetHTTPDocumentOpPtr anMOTDOp = new GetHTTPDocumentOp(aConfig->mProductName,HTTPDocType_MOTD);
	anMOTDOp->SetExtraConfig(aConfig->mLanguage); // set language
	anMOTDOp->SetHTTPSession(mHTTPSession);
	RunOpTrack(anMOTDOp,new InitLogicCompletion(GetHTTPDocCompletion,this));

	GetHTTPDocumentOpPtr aTOUOp = new GetHTTPDocumentOp(aConfig->mProductName,HTTPDocType_TOU);
	aTOUOp->SetExtraConfig(aConfig->mLanguage); // set language
	aTOUOp->SetHTTPSession(mHTTPSession);
	RunOpTrack(aTOUOp,new InitLogicCompletion(GetHTTPDocCompletion,this));

	GetHTTPDocumentOpPtr anADVOp = new GetHTTPDocumentOp(aConfig->mProductName,HTTPDocType_ADV);
	anADVOp->SetExtraConfig(aConfig->mLanguage); // set language
	anADVOp->SetHTTPSession(mHTTPSession);
	RunOpTrack(anADVOp,new InitLogicCompletion(GetHTTPDocCompletion,this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::RunTitanServersOp()
{
	ServerContext *aDirServers = LobbyMisc::GetDirServers();
	if(aDirServers==NULL)
		return;

	LobbyEvent::BroadcastEvent(new LoginInitStatusEvent(LobbyContainer_QueryingServers_String));

	GetMultiDirOpPtr anOp = new GetMultiDirOp(aDirServers);
	anOp->SetFlags(DIR_GF_DECOMPSERVICES | DIR_GF_SERVADDNETADDR);
	anOp->AddPath(L"/TitanServers/Auth");
	anOp->AddPath(L"/TitanServers/DBProxy");
	anOp->AddPath(L"/TitanServers/Firewall");

	// Construct the patch server path and append to the list
	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aConfig!=NULL)
		anOp->AddPath(aConfig->mProductDirectory + L"/Patch");

	RunOpTrack(anOp,new InitLogicCompletion(GetTitanServersCompletion,this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::RunVersionOp()
{
	ServerContext *aPatchServers = LobbyMisc::GetPatchServers();
	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aPatchServers==NULL || aConfig==NULL)
		return;

	LobbyEvent::BroadcastEvent(new LoginInitStatusEvent(LobbyContainer_CheckingVersion_String));


	CheckValidVersionOpPtr anOp = new CheckValidVersionOp(aConfig->mProductName, aPatchServers);
	anOp->SetConfigName(aConfig->mLanguage); // set language
	anOp->SetVersion(aConfig->mVersion);
	anOp->SetGetPatchList(false);
	RunOpTrack(anOp,new InitLogicCompletion(CheckVersionCompletion,this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::RunDetectFirewallOp()
{
	ServerContext *aFirewallServers = LobbyMisc::GetFirewallDetectServers();
	if(aFirewallServers==NULL)
		return;

	KillFirewallDetectOps();

	LobbyStaging::StartFirewallDetect(); // reset publicaddr and detected status

//	DetectFirewallOpPtr anOp = new DetectFirewallOp("noodle:15800"); // use this for testing not behind firewall
	DetectFirewallOpPtr anOp = new DetectFirewallOp(aFirewallServers);
	anOp->SetListenPort(LobbyStaging::GetLobbyPort());
	anOp->SetDoListen(false);
	anOp->SetWaitForConnect(false);
	RunOpTrack(anOp,new InitLogicCompletion(DetectFirewallCompletion,this));

	LobbyEvent::BroadcastEvent(LobbyEvent_SyncFirewallStatus);

	mAcceptSocket = new AsyncSocket;
	WONStatus aStatus = mAcceptSocket->Bind(LobbyStaging::GetLobbyPort());
	if(aStatus==WS_Success)
	{
		aStatus = mAcceptSocket->Listen();
		if(aStatus==WS_Success)
			return;
	}
	mAcceptSocket = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::KillFirewallDetectOps()
{
	if(mAcceptOp.get()!=NULL)
	{
		mAcceptOp->Kill();
	
		AsyncSocket *aSocket = mAcceptOp->GetSocket();
		if(aSocket!=NULL)
			aSocket->Close();
		
		mAcceptOp = NULL;
	}

	mAcceptSocket = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::KillHook()
{
	mHTTPSession->Kill();
	KillFirewallDetectOps();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::HandleControlEvent(int theId)
{
	switch(theId)
	{
		case ID_ShowInternetScreen: LobbyScreen::ShowInternetScreen(); break;
		case ID_LoginScreenBack: WONLobby::EndLobby(LobbyStatus_None); break;
		case ID_ChooseNetworkAdapter: HandleChooseNetworkAdapter(); break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitLogic::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==LobbyEvent_RetryInitLogin)
		StartNetInit(true);
	else if(theEvent->mType==LobbyEvent_IgnoreInitError)
	{
		mIgnoreInitErrorFlags |= mLastInitErrorFlags;
		StartNetInit(true);
	}
}
