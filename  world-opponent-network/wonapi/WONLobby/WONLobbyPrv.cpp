#include "WONLobbyPrv.h"

#include "AccountLogic.h"
#include "AddFriendDialog.h"
#include "AdminActionDialog.h"
#include "BadUserDialog.h"
#include "ChatCommandParser.h"
#include "ChatCtrl.h"
#include "ChooseNetworkAdapterDialog.h"
#include "CreateAccountCtrl.h"
#include "CreateGameDialog.h"
#include "CreateRoomDialog.h"
#include "CreditsDialog.h"
#include "DialogLogic.h"
#include "DirectConnectCtrl.h"
#include "DirectConnectDialog.h"
#include "FriendsListCtrl.h"
#include "GameBrowserCtrl.h"
#include "GameStagingCtrl.h"
#include "LobbyStaging.h"
#include "LobbyStatus.h"
#include "StagingLogic.h"
#include "GameUserListCtrl.h"
#include "InitFailedCtrl.h"
#include "InitLogic.h"
#include "LanNameDialog.h"
#include "LanLogic.h"
#include "LanPortDialog.h"
#include "LobbyConfig.h"
#include "LobbyContainer.h"
#include "LobbyClient.h"
#include "LobbyEvent.h"
#include "LobbyGame.h"
#include "LobbyLogic.h"
#include "LobbyOptionsCtrl.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"
#include "LoginCtrl.h"
#include "LoginHelpCtrl.h"
#include "LoginScreen.h"
#include "LoginStatusCtrl.h"
#include "LostPasswordCtrl.h"
#include "LostUserNameCtrl.h"
#include "MOTDCtrl.h"
#include "OkCancelDialog.h"
#include "PasswordDialog.h"
#include "PingLogic.h"
#include "QuickPlayCtrl.h"
#include "RoomCtrl.h"
#include "RoomDialog.h"
#include "RoomLabelCtrl.h"
#include "RootScreen.h"
#include "RoutingLogic.h"
#include "ServerListCtrl.h"
#include "SpecialControls.h"
#include "StatusDialog.h"
#include "TOUDialog.h"
#include "UpdateAccountCtrl.h"
#include "UserListCtrl.h"


#include "WONAPI.h"
#include "WONAuth/AuthContext.h"
#include "WONCommon/CRC.h"
#include "WONCommon/WONBigFile.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/WONGUIConfig/BrowserComponentConfig.h"
#include "WONGUI/WONGUIConfig/ResourceConfig.h"
#include "WONGUI/WONGUIConfig/WONGUIConfig.h"

#include "WONSocket/UDPManager.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONLobbyPrv::WONLobbyPrv()
{
	mLobbyPort = 8888;
	mSysTOUTime = mGameTOUTime = 0;
	mBehindFirewall = false;
	mFirewallTestDone = false;
	mSuspended = false;
	mCurGameType = LobbyGameType_None;
	mLobbyStatus = LobbyStatus_None;
	mRunMode = LobbyRunMode_Block;
	mProductId = 0;
	mLanProductId = 0;

	mSysTOUPath = WONAPICore::GetDefaultFileDirectory() + "_wonsystou_m.txt";
	mGameTOUPath = WONAPICore::GetDefaultFileDirectory() + "_wongametou_m.txt";

	mLobbyConfig = LobbyConfig::mAllocator();
	mPersistentData = new LobbyPersistentData;
	mChatCommandParser = new ChatCommandParser;
	mUDPManager = new UDPManager;

	mAccountServers			= new ServerContext;
	mAuthServers			= new ServerContext;
	mDirServers				= new ServerContext;
	mRoomDirServers			= new ServerContext;
	mFirewallDetectServers	= new ServerContext;
	mPatchServers			= new ServerContext;

	mVersionStatus = WS_None;

	mAuthContext = new AuthContext;
	mAuthContext->SetServerContext(mAuthServers);

	mAccountLogic = new AccountLogic;
	mDialogLogic = new DialogLogic;
	mInitLogic = new InitLogic;
	mRoutingLogic = new RoutingLogic;
	mStagingLogic = new StagingLogic;
	mLanLogic = new LanLogic;
	mPingLogic = new PingLogic;

	mLogicMgr.AddLogic(mAccountLogic);
	mLogicMgr.AddLogic(mDialogLogic);
	mLogicMgr.AddLogic(mStagingLogic);
	mLogicMgr.AddLogic(mInitLogic);
	mLogicMgr.AddLogic(mLanLogic);
	mLogicMgr.AddLogic(mRoutingLogic);
	mLogicMgr.AddLogic(mPingLogic);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONLobbyPrv::~WONLobbyPrv()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace
{

Component* CreateAccountCtrlFactory()				{ return new CreateAccountCtrl; }
Component* DirectConnectCtrlFactory()				{ return new DirectConnectCtrl; }
Component* GameStagingCtrlFactory()					{ return new GameStagingCtrl; }
Component* InitFailedCtrlFactory()					{ return new InitFailedCtrl; }
Component* InternetOptionsCtrlFactory()				{ return new LobbyOptionsCtrl(false); }
Component* InternetScreenFactory()					{ return new RootScreen(false); }
Component* LanOptionsCtrlFactory()					{ return new LobbyOptionsCtrl(true); }
Component* LanScreenFactory()						{ return new RootScreen(true); }
Component* LoginCtrlFactory()						{ return new LoginCtrl; }
Component* LoginHelpCtrlFactory()					{ return new LoginHelpCtrl; }
Component* LoginScreenFactory()						{ return new LoginScreen; }
Component* LoginStatusCtrlFactory()					{ return new LoginStatusCtrl; }
Component* LostPasswordCtrlFactory()				{ return new LostPasswordCtrl; }
Component* LostUserNameCtrlFactory()				{ return new LostUserNameCtrl; }
Component* MOTDCtrlFactory()						{ return new MOTDCtrl; }
Component* QuickPlayCtrlFactory()					{ return new QuickPlayCtrl; }
Component* RoomCtrlFactory()						{ return new RoomCtrl; }
Component* UpdateAccountCtrlFactory()				{ return new UpdateAccountCtrl; }

Component* AddFriendDialogFactory()					{ return new AddFriendDialog; }
Component* AdminActionDialogFactory()				{ return new AdminActionDialog; }
Component* BadUserDialogFactory()					{ return new BadUserDialog; }
Component* ChooseNetworkAdapterDialogFactory()		{ return new ChooseNetworkAdapterDialog; }
Component* CreateGameDialogFactory()				{ return new CreateGameDialog; }
Component* CreateRoomDialogFactory()				{ return new CreateRoomDialog; }
Component* CreditsDialogFactory()					{ return new CreditsDialog; }
Component* DirectConnectDialogFactory()				{ return new DirectConnectDialog; }
Component* LanNameDialogFactory()					{ return new LanNameDialog; }
Component* LanPortDialogFactory()					{ return new LanPortDialog; }
Component* OkCancelDialogFactory()					{ return new OkCancelDialog; }
Component* PasswordDialogFactory()					{ return new PasswordDialog; }
Component* RoomDialogFactory()						{ return new RoomDialog; }
Component* StatusDialogFactory()					{ return new StatusDialog; }
Component* TOUDialogFactory()						{ return new TOUDialog; }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfigTablePtr GetLobbyResourceConfigTable(ResourceConfigTable *theTable)
{
	ResourceConfigTablePtr aTable = LobbyResource_Init(theTable);
	WONGUIConfig_Init(aTable);	
	BrowserComponentConfig::Init(aTable);

	aTable->AddAllocator("ChatCtrl",ChatCtrlConfig::CfgFactory,ChatCtrlConfig::CompFactory);
	aTable->AddAllocator("CreateAccountCtrl",ContainerConfig::CfgFactory,CreateAccountCtrlFactory);
	aTable->AddAllocator("DirectConnectCtrl",ContainerConfig::CfgFactory,DirectConnectCtrlFactory);
	aTable->AddAllocator("FriendsListCtrl",FriendsListCtrl::CfgFactory,FriendsListCtrl::CompFactory);
	aTable->AddAllocator("GameBrowserCtrl",GameBrowserCtrlConfig::CfgFactory,GameBrowserCtrlConfig::CompFactory);
	aTable->AddAllocator("GameStagingCtrl",ContainerConfig::CfgFactory,GameStagingCtrlFactory);
	aTable->AddAllocator("GameUserListCtrl",GameUserListCtrlConfig::CfgFactory,GameUserListCtrlConfig::CompFactory);
	aTable->AddAllocator("InitFailedCtrl",ContainerConfig::CfgFactory,InitFailedCtrlFactory);
	aTable->AddAllocator("InternetOptionsCtrl",ContainerConfig::CfgFactory,InternetOptionsCtrlFactory);
	aTable->AddAllocator("InternetScreen",ContainerConfig::CfgFactory,InternetScreenFactory);
	aTable->AddAllocator("LanOptionsCtrl",ContainerConfig::CfgFactory,LanOptionsCtrlFactory);
	aTable->AddAllocator("LanScreen",ContainerConfig::CfgFactory,LanScreenFactory);
	aTable->AddAllocator("LoginCtrl",DialogConfig::CfgFactory,LoginCtrlFactory);
	aTable->AddAllocator("LoginHelpCtrl",ContainerConfig::CfgFactory,LoginHelpCtrlFactory);	
	aTable->AddAllocator("LoginScreen",ContainerConfig::CfgFactory,LoginScreenFactory);
	aTable->AddAllocator("LoginStatusCtrl",ContainerConfig::CfgFactory,LoginStatusCtrlFactory);
	aTable->AddAllocator("LostPasswordCtrl",ContainerConfig::CfgFactory,LostPasswordCtrlFactory);
	aTable->AddAllocator("LostUserNameCtrl",ContainerConfig::CfgFactory,LostUserNameCtrlFactory);
	aTable->AddAllocator("MOTDCtrl",ContainerConfig::CfgFactory,MOTDCtrlFactory);
	aTable->AddAllocator("QuickPlayCtrl",DialogConfig::CfgFactory,QuickPlayCtrlFactory);
	aTable->AddAllocator("RoomCtrl",ContainerConfig::CfgFactory,RoomCtrlFactory);
	aTable->AddAllocator("RoomLabelCtrl",RoomLabelCtrlConfig::CfgFactory,RoomLabelCtrlConfig::CompFactory);
	aTable->AddAllocator("ServerListCtrl",ServerListCtrlConfig::CfgFactory,ServerListCtrlConfig::CompFactory);
	aTable->AddAllocator("UpdateAccountCtrl",ContainerConfig::CfgFactory,UpdateAccountCtrlFactory);
	aTable->AddAllocator("UserListCtrl",UserListCtrlConfig::CfgFactory,UserListCtrlConfig::CompFactory);

	aTable->AddAllocator("AddFriendDialog",DialogConfig::CfgFactory,AddFriendDialogFactory);
	aTable->AddAllocator("AdminActionDialog",DialogConfig::CfgFactory,AdminActionDialogFactory);
	aTable->AddAllocator("BadUserDialog",DialogConfig::CfgFactory,BadUserDialogFactory);
	aTable->AddAllocator("ChooseNetworkAdapterDialog",DialogConfig::CfgFactory,ChooseNetworkAdapterDialogFactory);
	aTable->AddAllocator("CreditsDialog",CreditsDialogConfig::CfgFactory,CreditsDialogFactory);
	aTable->AddAllocator("CreateGameDialog",DialogConfig::CfgFactory,CreateGameDialogFactory);
	aTable->AddAllocator("CreateRoomDialog",DialogConfig::CfgFactory,CreateRoomDialogFactory);
	aTable->AddAllocator("DirectConnectDialog",DialogConfig::CfgFactory,DirectConnectDialogFactory);
	aTable->AddAllocator("LanNameDialog",DialogConfig::CfgFactory,LanNameDialogFactory);
	aTable->AddAllocator("LanPortDialog",DialogConfig::CfgFactory,LanPortDialogFactory);
	aTable->AddAllocator("OkCancelDialog",DialogConfig::CfgFactory,OkCancelDialogFactory);
	aTable->AddAllocator("PasswordDialog",DialogConfig::CfgFactory,PasswordDialogFactory);
	aTable->AddAllocator("RoomDialog",DialogConfig::CfgFactory,RoomDialogFactory);
	aTable->AddAllocator("StatusDialog",DialogConfig::CfgFactory,StatusDialogFactory);
	aTable->AddAllocator("TOUDialog",DialogConfig::CfgFactory,TOUDialogFactory);


	aTable->AddAllocator("CrossPromotionButton",CrossPromotionButtonConfig::CfgFactory,CrossPromotionButtonConfig::CompFactory);
	aTable->AddAllocator("ImageTextButton",ImageTextButtonConfig::CfgFactory,ImageTextButtonConfig::CompFactory);
	aTable->AddAllocator("MaxSplitterButton",MaxSplitterButtonConfig::CfgFactory,MaxSplitterButtonConfig::CompFactory);

	aTable->AddAllocator("GameOptionsCtrl",ContainerConfig::CfgFactory,ContainerConfig::CompFactory);
	aTable->AddAllocator("PlayerOptionsCtrl",ContainerConfig::CfgFactory,ContainerConfig::CompFactory);

	return aTable;
}

} // anonymous namespace


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONLobbyPrv::Init(ResourceConfigTable *theTable, ComponentInitFunc theInitFunc)
{
	mInitError.erase();
	RemoveBigFiles();

	ConfigParser aParser;
	aParser.SetWarningsAsErrors(true);

	// Parse LobbyConfig
	if(!aParser.OpenFile(WONAPICore::GetDefaultFileDirectory() +  LobbyConfig::mDefaultFileName))
	{
		mInitError = "Couldn't find " + LobbyConfig::mDefaultFileName;
		return false;
	}

	if(!mLobbyConfig->ParseTopLevel(aParser))
	{
		mInitError = LobbyConfig::mDefaultFileName + " error: ";
		mInitError.append(aParser.GetAbortReason());
		return false;
	}

	// Add BigFiles
	std::string aResourceFolder = WONAPICore::GetDefaultFileDirectory() + mLobbyConfig->mResourceDirectory;
	for(WONTypes::StringList::iterator aStrItr = mLobbyConfig->mBigFiles.begin(); aStrItr != mLobbyConfig->mBigFiles.end(); ++aStrItr)
	{
		WONBigFilePtr aBigFile = new WONBigFile((WONAPICore::GetDefaultFileDirectory() + *aStrItr).c_str());
		mBigFileList.push_back(aBigFile);
		FileReader::AddBigFile(aBigFile,aResourceFolder.c_str());
	}

	mLobbyPort = mLobbyConfig->mLobbyPort;

	ComponentConfigPtr aContainerConfig = new ContainerConfig;
	mLobbyContainer = new LobbyContainer;

	// Load GUI config and resources
	ComponentConfig::SetResourceFolder(aResourceFolder);
	ResourceConfigTablePtr aTable = GetLobbyResourceConfigTable(theTable);

	ServerListCtrl::InitColumnMap(); // Add default columns to ServerList
//	aParser.AddDefine("Product", mLobbyConfig->mProductName);
	for(LobbyConfig::LobbyDefineMap::iterator anItr = mLobbyConfig->mDefineMap.begin(); anItr != mLobbyConfig->mDefineMap.end(); ++anItr)
		aParser.AddDefine(anItr->first,anItr->second);
	
	if(!aContainerConfig->ReadConfig_Unsafe(mLobbyContainer,"_LobbyMaster.cfg",aTable,&aParser))
	{
		mInitError = aContainerConfig->GetConfigError();
		return false;
	}

	// Init logic classes
	try
	{
		mLogicMgr.Init(aContainerConfig);
		if(theInitFunc!=NULL)
			theInitFunc(aContainerConfig);
	}
	catch(ConfigObjectException &anEx)
	{
		mInitError = anEx.what;
		return false;
	}

	// Calculate product id which is unique per product per version
	CRC16 aCRC;
	aCRC.Put(mLobbyConfig->mProductName.c_str(),mLobbyConfig->mProductName.length());
	aCRC.Put(mLobbyConfig->mVersion.c_str(),mLobbyConfig->mVersion.length());
	mProductId = aCRC.Get();

	aCRC.Reset();
	aCRC.Put(mLobbyConfig->mProductName.c_str(),mLobbyConfig->mProductName.length());
	aCRC.Put(mLobbyConfig->mLanVersion.c_str(),mLobbyConfig->mLanVersion.length());
	mLanProductId = aCRC.Get();

	mChatCommandParser->AddCommands();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONLobbyPrv::InitAuth(const std::wstring& theLoginCommunity, const char* theHashFileListStr /* = NULL */)
{
	mHasSetHashFileList = false;
	mHashFileList.clear();
	if(theHashFileListStr!=NULL)
	{
		StringParser aParser(theHashFileListStr);
		aParser.ReadValue(mHashFileList);
	}

	mLoginCommunity = theLoginCommunity;
	mAuthContext->SetCommunity(mLoginCommunity);
	if(mLobbyConfig->mIsCDKeyCheck)
	{
		mCDKey.SetProductString(mLobbyConfig->mProductName);
		mCDKey.LoadFromRegistry();

		mAuthContext->SetCDKey(mLoginCommunity,mCDKey);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONLobbyPrv::RemoveBigFiles()
{
	std::string aResourceFolder = WONAPICore::GetDefaultFileDirectory() + mLobbyConfig->mResourceDirectory;
	for(BigFileList::iterator anItr = mBigFileList.begin(); anItr != mBigFileList.end(); ++anItr)
		FileReader::RemoveBigFile(*anItr);

	mBigFileList.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONLobbyPrv::Destroy()
{
	mLogicMgr.Kill();

	if(mUDPManager.get()!=NULL)
	{
		mUDPManager->Close();
		mUDPManager = NULL;
	}

	if(mAuthContext.get()!=NULL)
	{
		mAuthContext->Kill();
		mAuthContext = NULL;
	}

	RemoveBigFiles();
}
