#include "AccountLogic.h"

#include "CreditsDialog.h"
#include "LobbyConfig.h"
#include "LobbyDialog.h"
#include "LobbyEvent.h"
#include "LobbyMisc.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"
#include "LobbyScreen.h"
#include "LobbyStagingPrv.h"
#include "TOUDialog.h"
#include "WONLobby.h"

#include "WONAuth/GetCertOp.h"
#include "WONDB/CreateAccountOp.h"
#include "WONDB/GetAccountOp.h"
#include "WONDB/RequestPasswordEmailOp.h"
#include "WONDB/RequestAccountInfoEmailOp.h"
#include "WONDB/UpdateAccountOp.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/Window.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AccountLogic::AccountLogic()
{
	mNeedTOUOnCreate = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AccountLogic::~AccountLogic()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AccountLogic::Init(ComponentConfig *theConfig)
{
	WONComponentConfig_Get(theConfig,mCreditsDialog,"CreditsDialog");
	WONComponentConfig_Get(theConfig,mTOUDialog,"TOUDialog");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AccountLogic::DoCredits()
{
	mCreditsDialog->Reset();
	mCreditsDialog->Start();
	LobbyDialog::DoDialog(mCreditsDialog);
	mCreditsDialog->Stop();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AccountLogic::DoTermsOfUse()
{
	mTOUDialog->Reset();
	int aResult = LobbyDialog::DoDialog(mTOUDialog);

	return aResult==1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AccountLogic::CheckVersionStatus()
{
	WONStatus aVersionStatus = LobbyMisc::GetVersionStatus();
	if(aVersionStatus==WS_DBProxyServ_OutOfDate)
	{
		if(LobbyDialog::DoYesNoDialog(LobbyContainer_VersionOutOfDataTitle_String,LobbyContainer_VersionOutOfDataDetails_String)==ControlId_Ok)
			WONLobby::EndLobby(LobbyStatus_Patch);
		
		return false;
	}
		
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AccountLogic::DoCreateAccount(CreateAccountEvent *theEvent)
{
	if(!CheckVersionStatus())
		return;

	if(mNeedTOUOnCreate)
	{
		if(!DoTermsOfUse())
			return;
	
		mNeedTOUOnCreate = false;
	}

	ServerContext *anAccountServers = LobbyMisc::GetAccountServers();
	if(anAccountServers==NULL)
		return;

	CreateAccountOpPtr anOp = new CreateAccountOp(anAccountServers);
	anOp->SetUserName(theEvent->mUserName);
	anOp->SetPassword(theEvent->mPassword);
	anOp->SetEmail(theEvent->mEmail);

	ProfileDataPtr aData = new ProfileData;
	aData->AddNewsletterSub(1, theEvent->mSubscribeSierraNewsletter); // correct newsletter id?
	aData->SetBirthDate(theEvent->mBirthMonth,theEvent->mBirthDay,theEvent->mBirthYear);
	anOp->SetProfileData(aData);

	LobbyDialog::DoStatusOp(anOp,LobbyContainer_CreateAccountTitle_String,LobbyContainer_CreateAccountDetails_String);

	if(anOp->Succeeded())
	{
		LobbyPersistentData::UserInfo *anInfo = LobbyPersistentData::AddUserInfo(theEvent->mUserName,"");
		if(anInfo!=NULL)
			LobbyMisc::GetTOUTimes(anInfo->mSysTOUTime, anInfo->mGameTOUTime);

		LobbyPersistentData::WriteGlobal(true);
		LobbyEvent::BroadcastEvent(LobbyEvent_SetLoginNames);

		// Propagate the name and password to the LoginCtrl so the user can now just hit the login button
		// without having to type his username and password.
		LobbyEvent::BroadcastEvent(new SetLoginFieldsEvent(theEvent->mUserName,theEvent->mPassword,false)); 
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AccountLogic::DoGetLostPassword(GetLostPasswordEvent *theEvent)
{
	ServerContext *anAccountServers = LobbyMisc::GetAccountServers();
	if(anAccountServers==NULL)
		return;

	if(!CheckVersionStatus())
		return;

	RequestPasswordEmailOpPtr anOp = new RequestPasswordEmailOp(anAccountServers);
	anOp->SetUserName(theEvent->mUserName);

	LobbyDialog::DoStatusOp(anOp,LobbyContainer_RetrievePasswordTitle_String,LobbyContainer_RetrievePasswordDetails_String);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AccountLogic::DoGetLostUserName(GetLostUserNameEvent *theEvent)
{
	ServerContext *anAccountServers = LobbyMisc::GetAccountServers();
	if(anAccountServers==NULL)
		return;

	if(!CheckVersionStatus())
		return;

	RequestAccountInfoEmailOpPtr anOp = new RequestAccountInfoEmailOp(anAccountServers);
	anOp->SetEmailAddress(theEvent->mEmail);

	LobbyDialog::DoStatusOp(anOp,LobbyContainer_RetrieveUsernameTitle_String,LobbyContainer_RetrieveUsernameDetails_String);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AccountLogic::DoLogin(LoginEvent *theEvent)
{
	AuthContext *anAuthContext = LobbyMisc::GetAuthContext();
	Window *aWindow = LobbyScreen::GetWindow();
	if(anAuthContext==NULL || aWindow==NULL)
		return;

	int aKeyMod = aWindow->GetKeyMod();
	int ModKeys = KEYMOD_SHIFT | KEYMOD_ALT | KEYMOD_CTRL;
	if ((aKeyMod & ModKeys) == ModKeys)
	{
		DoCredits();
		return;
	}

	if(theEvent->mUserName.empty())
		return;

	if(!CheckVersionStatus())
		return;

	bool isAdmin = false;
	if(theEvent->mUserName.at(0)=='@')
	{
		isAdmin = true;
		theEvent->mUserName = theEvent->mUserName.substr(1);
	}

	if(WStringCompareNoCase(theEvent->mUserName,mLastLoginName)!=0) // not same guy as last time
	{
		LobbyPersistentData::UserInfo *anInfo = LobbyPersistentData::GetUserInfo(theEvent->mUserName);
		if(anInfo==NULL || LobbyMisc::CheckDoTOU(anInfo->mSysTOUTime,anInfo->mGameTOUTime))
		{
			if(!DoTermsOfUse())
				return;
		}
		mLastLoginName = theEvent->mUserName;
	}


	anAuthContext->SetUserName(theEvent->mUserName);
	anAuthContext->SetPassword(theEvent->mPassword);
	if(LobbyMisc::DoCDKeyCheck() && !LobbyMisc::IsCDKeyValid())
	{
		LobbyDialog::DoErrorDialog(AccountLogic_InvalidCDKeyTitle_String,AccountLogic_InvalidCDKeyDetails_String); 
		return;
	}
	
	if(isAdmin)
		anAuthContext->AddCommunity(L"Admin");
	else
		anAuthContext->RemoveCommunity(L"Admin");
		

	GetCertOpPtr anOp = new GetCertOp(anAuthContext);
	LobbyDialog::DoStatusOp(anOp,LobbyContainer_LoginTitle_String,LobbyContainer_LoginDetails_String,CloseStatusDialogType_Success);

	if(anOp->Succeeded())
	{
		anAuthContext->SetDoAutoRefresh(true);

		GUIString aPassword;
		if(theEvent->mRememberPassword)
			aPassword = theEvent->mPassword;

		LobbyPersistentData::UserInfo *anInfo = LobbyPersistentData::AddUserInfo(theEvent->mUserName,aPassword);
		if(anInfo!=NULL)
		{
			LobbyMisc::GetTOUTimes(anInfo->mSysTOUTime, anInfo->mGameTOUTime);
			LobbyPersistentData::WriteGlobal(true);
			LobbyPersistentData::SetCurUser(anInfo);
		}

		LobbyEvent::BroadcastEvent(LobbyEvent_SetLoginNames);
		bool ctrlPressed = aKeyMod & KEYMOD_CTRL ? true : false;
		LobbyStagingPrv::EnterInternetScreen(!ctrlPressed);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AccountLogic::DoQueryAccount()
{
	ServerContext *anAccountServers = LobbyMisc::GetAccountServers();
	AuthContext *anAuthContext = LobbyMisc::GetAuthContext();
	if(anAccountServers==NULL || anAuthContext==NULL)
		return;

	GetAccountOpPtr anOp = new GetAccountOp(anAccountServers,anAuthContext);
	ProfileData *aData = anOp->GetProfileData();
	aData->AddNewsletterSub(1);
	LobbyDialog::DoStatusOp(anOp,LobbyContainer_QueryAccountTitle_String,LobbyContainer_QueryAccountDetails_String,CloseStatusDialogType_Success);

	if(anOp->Succeeded())
		LobbyEvent::BroadcastEvent(new QueryAccountResultEvent(anOp->GetEmail(),aData->GetNewsletterSub(1)));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AccountLogic::DoUpdateAccount(UpdateAccountEvent *theEvent)
{
	ServerContext *anAccountServers = LobbyMisc::GetAccountServers();
	AuthContext *anAuthContext = LobbyMisc::GetAuthContext();
	if(anAccountServers==NULL || anAuthContext==NULL)
		return;

	UpdateAccountOpPtr anOp = new UpdateAccountOp(anAccountServers,anAuthContext);
	anOp->SetEmail(theEvent->mEmail);
	anOp->SetPassword(theEvent->mPassword);
	anOp->GetProfileData()->AddNewsletterSub(1,theEvent->mSubscribeSierraNewsletter);
	LobbyDialog::DoStatusOp(anOp,LobbyContainer_UpdateAccountTitle_String,LobbyContainer_UpdateAccountDetails_String);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AccountLogic::HandleControlEvent(int theControlId)
{
	switch(theControlId)
	{
		case ID_ShowCreateAccount: 
			if(CheckVersionStatus())
			{
				LobbyScreen::ShowCreateAccountCtrl(); 
				mNeedTOUOnCreate = true; 
			}
			return;
		
		case ID_ShowLostPassword:  
			if(CheckVersionStatus())
				LobbyScreen::ShowLostPasswordCtrl(); 
	
			return;
	
		case ID_ShowLostUserName:  
			if(CheckVersionStatus())				
				LobbyScreen::ShowLostUsernameCtrl(); 
	
			return;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AccountLogic::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_CreateAccount:		DoCreateAccount((CreateAccountEvent*)theEvent); return;
		case LobbyEvent_GetLostPassword:	DoGetLostPassword((GetLostPasswordEvent*)theEvent); return;
		case LobbyEvent_GetLostUserName:	DoGetLostUserName((GetLostUserNameEvent*)theEvent); return;
		case LobbyEvent_Login:				DoLogin((LoginEvent*)theEvent); return;
		case LobbyEvent_QueryAccount:		DoQueryAccount(); return;
		case LobbyEvent_UpdateAccount:		DoUpdateAccount((UpdateAccountEvent*)theEvent); return;
	}
}
