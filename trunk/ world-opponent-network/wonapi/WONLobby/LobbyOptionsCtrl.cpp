#include "LobbyOptionsCtrl.h"

#include "LobbyEvent.h"
#include "LobbyMisc.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"
#include "LobbyScreen.h"
#include "LobbyStaging.h"
#include "LobbyDialog.h"
#include "ChooseNetworkAdapterDialog.h"

#include "WONGUI/ComboBox.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/Label.h"
#include "WONGUI/SimpleComponent.h"

using namespace WONAPI;

enum // internal control ids for easy event handling
{
	ID_AutoPingGamesCheck = 1,
	ID_ChatSoundEffectsCheck,
	ID_CrossPromotionCheck,
	ID_DoAutoAFKCheck,
	ID_LobbyMusicCheck,
	ID_LobbySoundEffectsCheck,
	ID_RemoveLANName,
	ID_ResetFiltersButton,
	ID_RestoreDefaultsButton,
	ID_ShowConfirmationsCheck,
	ID_ShowGamesInProgressCheck,
	ID_StagingSoundEffectsCheck,
	ID_ChooseNetworkAdapterButton
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyOptionsCtrl::LobbyOptionsCtrl(bool isLan)
{
	mIsLan = isLan;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyOptionsCtrl::~LobbyOptionsCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int LobbyOptions_GetMaxPingsForComboItem(int theComboItem)
{
	if(theComboItem<0)
		return 5;

	switch(theComboItem)
	{
		case 0: return 5; // 28.8
		case 1: return 15; // 33.6
		case 2: return 25; // 56.6
		case 3: return 35; // ISDN
		case 4: return 45; // DSL

		case 5: 
		default: return 55; // T1+
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int LobbyOptions_GetComboItemForMaxPings(int theMaxPings)
{
	int i;
	for(i=1; i<6; i++)
	{
		if(LobbyOptions_GetMaxPingsForComboItem(i)>theMaxPings)
			break;
	}
			
	return i-1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyOptionsCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		ButtonPtr aButton;
	
		WONComponentConfig_GetWithCtrlId(aConfig,mChatSoundEffects,				"ChatSoundEffectsCheck", ID_ChatSoundEffectsCheck);
		WONComponentConfig_Get(aConfig,mConnectionSpeedCombo,					"ConnectionSpeedCombo");
		WONComponentConfig_GetWithCtrlId(aConfig,mShowCrossPromotion,			"CrossPromotionCheck", ID_CrossPromotionCheck);
		WONComponentConfig_Get(aConfig,mFirewalledIcon,							"FirewalledIcon");
		WONComponentConfig_GetWithCtrlId(aConfig,mLobbyMusic,					"LobbyMusicCheck", ID_LobbyMusicCheck);
		WONComponentConfig_GetWithCtrlId(aConfig,mLobbySoundEffects,			"LobbySoundEffectsCheck", ID_LobbySoundEffectsCheck);
		WONComponentConfig_Get(aConfig,mLocalAddressDetailLabel,				"LocalAddressDetailLabel");
		WONComponentConfig_Get(aConfig,mPublicAddressDetailLabel,				"PublicAddressDetailLabel");
		WONComponentConfig_GetWithCtrlId(aConfig,aButton,						"ResetFiltersButton", ID_ResetFiltersButton);
		WONComponentConfig_GetWithCtrlId(aConfig,aButton,						"RestoreDefaultsButton", ID_RestoreDefaultsButton);
		WONComponentConfig_GetWithCtrlId(aConfig,mShowConfirmationsCheck,		"ShowConfirmationsCheck", ID_ShowConfirmationsCheck);
		WONComponentConfig_GetWithCtrlId(aConfig,mShowGamesInProgressCheck,		"ShowGamesInProgressCheck", ID_ShowGamesInProgressCheck);
		WONComponentConfig_GetWithCtrlId(aConfig,mStagingSoundEffects,			"StagingSoundEffectsCheck", ID_StagingSoundEffectsCheck);
		WONComponentConfig_Get(aConfig,mDefaultTabCombo,						"TabSelectCombo");

		if(mIsLan)
		{
			WONComponentConfig_Get(aConfig,mLanBroadcastDetailLabel,			"LANBroadcastDetailLabel");
			WONComponentConfig_GetWithCtrlId(aConfig,aButton,					"RemoveLanNameButton", ID_RemoveLANName);
			WONComponentConfig_Get(aConfig,mUserNameCombo,						"UserNameCombo");
		}
		else
		{
			WONComponentConfig_GetWithCtrlId(aConfig,mAnonymousToFriendsCheck,	"AnonymousToFriendsCheck", ID_AnonymousToFriendsCheck);
			WONComponentConfig_GetWithCtrlId(aConfig,mAutoPingGamesCheck,		"AutoPingGamesCheck", ID_AutoPingGamesCheck);
			WONComponentConfig_GetWithCtrlId(aConfig,mDoAutoAFK,				"DoAutoAFKCheck", ID_DoAutoAFKCheck);
			WONComponentConfig_GetWithCtrlId(aConfig,mFilterDirtyWords,			"FilterDirtyWordsCheck", ID_FilterDirtyWordsCheck);
		}

		mLocalAddressDetailLabel->SetText(IPAddr::GetLocalAddr().GetHostString());
		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyOptionsCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_ButtonPressed: 
			if(HandleButtonPress(theEvent->mComponent))
				return;
			break;

		case ComponentEvent_ComboSelChanged:
			if (theEvent->mComponent==mDefaultTabCombo.get())
			{
				LobbyPersistentData::SetDefaultTabName(mDefaultTabCombo->GetText());
			}
			else if(theEvent->mComponent==mConnectionSpeedCombo.get())
			{
				LobbyPersistentData::SetMaxPings(LobbyOptions_GetMaxPingsForComboItem(mConnectionSpeedCombo->GetSelItemPos()));
			}
			else if(theEvent->mComponent==mUserNameCombo.get())
			{
				LobbyPersistentData::SetLanName(mUserNameCombo->GetText());
			}
			else
				break;

			return;

		case LobbyEvent_SyncFirewallStatus: HandleSyncFirewallStatus(); return;
		case LobbyEvent_EnteredScreen: HandleEnteredScreen(); return;
		case LobbyEvent_SetLanOptions: SetLanOptions(); return;
	}
	Container::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyOptionsCtrl::HandleButtonPress(Component *theButton)
{
	if(theButton==NULL)
		return false;

	Button *aButton = dynamic_cast<Button*>(theButton);
	if(aButton==NULL)
		return false;

	bool checked = aButton->IsChecked();

	switch(theButton->GetControlId())
	{	
		case ID_CrossPromotionCheck:		LobbyPersistentData::SetShowCrossPromotion(checked); return false; // let GameScreen catch event as well
		case ID_FilterDirtyWordsCheck:		LobbyPersistentData::SetFilterDirtyWords(checked); return false; // let routing logic catch this
		case ID_DoAutoAFKCheck:				LobbyPersistentData::SetDoAutoAFK(checked); return true;
		case ID_AnonymousToFriendsCheck:	LobbyPersistentData::SetAnonymousToFriends(checked); return false; // let routing logic catch this as well
		case ID_AutoPingGamesCheck:			LobbyPersistentData::SetAutoPingGames(checked); return true;

		case ID_ShowGamesInProgressCheck:	
			LobbyPersistentData::SetShowGamesInProgress(checked); 
			LobbyEvent::BroadcastEvent(LobbyEvent_ResyncGameList);
			return true;

		case ID_RestoreDefaultsButton:		
		{			
			LobbyPersistentData::RestoreUserDefaults(); 
			LobbyMisc::CheckSetNetworkAdapter(LobbyPersistentData::GetNetworkAdapter());
			SyncOptions(); 
			return true;
		}

		case ID_ResetFiltersButton:			
		{
			LobbyPersistentData::ResetServerListFilters();
			Container *aCurScreen = LobbyScreen::GetCurScreen();
			if(aCurScreen!=NULL)
				aCurScreen->BroadcastEventDown(new LobbyEvent(LobbyEvent_LoadFilterCombos));

			return true;
		}
			
			
		case ID_LobbyMusicCheck:			
			LobbyPersistentData::SetLobbyMusic(checked); 
			if(checked)
				LobbyGlobal_LobbyMusic_Sound->Resume();
			else
				LobbyGlobal_LobbyMusic_Sound->Stop();
			return true;
	
		case ID_LobbySoundEffectsCheck:		
			LobbyPersistentData::SetLobbySoundEffects(checked); 
			LobbyGlobal_ButtonClick_Sound->SetMuted(!checked);
			if(!checked)	
				LobbyGlobal_SierraLogon_Sound->Stop();
			return true;

		case ID_ChatSoundEffectsCheck:		LobbyPersistentData::SetChatSoundEffects(checked); return true;
		case ID_StagingSoundEffectsCheck:	LobbyPersistentData::SetStagingSoundEffects(checked); return true;
		case ID_ShowConfirmationsCheck:		LobbyPersistentData::SetShowConfirmations(checked); return true;
		case ID_RemoveLANName:				LobbyPersistentData::RemoveLanName(); SetLanOptions(); return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyOptionsCtrl::SetLanOptions()
{
	if(!mIsLan)
		return;

	mUserNameCombo->Clear();
	const LobbyPersistentData::LanNameInfoMap &aMap = LobbyPersistentData::GetLanNameInfoMap();
	LobbyPersistentData::LanNameInfoMap::const_iterator anItr = aMap.begin();
	while(anItr!=aMap.end())
	{
		mUserNameCombo->InsertString(anItr->first);
		++anItr;
	}
	mUserNameCombo->SetTextAndSelItem(LobbyPersistentData::GetLanName());


	char buffer[20];
	sprintf(buffer,"%d",LobbyPersistentData::GetLanBroadcastPort());
	mLanBroadcastDetailLabel->SetText(buffer);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyOptionsCtrl::SyncOptions()
{
	if(mIsLan)
		SetLanOptions();	
	else
	{
		mFilterDirtyWords->SetCheck(LobbyPersistentData::GetFilterDirtyWords());
		mDoAutoAFK->SetCheck(LobbyPersistentData::GetDoAutoAFK());
		mAnonymousToFriendsCheck->SetCheck(LobbyPersistentData::GetAnonymousToFriends(), true);
		mAutoPingGamesCheck->SetCheck(LobbyPersistentData::GetAutoPingGames(), true);
	}

	mChatSoundEffects->SetCheck(LobbyPersistentData::GetChatSoundEffects(), true);
	mConnectionSpeedCombo->SetSelItem(LobbyOptions_GetComboItemForMaxPings(LobbyPersistentData::GetMaxPings()),false);
	mDefaultTabCombo->SetTextAndSelItem(LobbyPersistentData::GetDefaultTabName(),false);
	mLobbyMusic->SetCheck(LobbyPersistentData::GetLobbyMusic(), true);
	mLobbySoundEffects->SetCheck(LobbyPersistentData::GetLobbySoundEffects(), true);
	mShowConfirmationsCheck->SetCheck(LobbyPersistentData::GetShowConfirmations(), true);
	mShowCrossPromotion->SetCheck(LobbyPersistentData::GetShowCrossPromotion(), true);
	mShowGamesInProgressCheck->SetCheck(LobbyPersistentData::GetShowGamesInProgress(), true);
	mStagingSoundEffects->SetCheck(LobbyPersistentData::GetStagingSoundEffects(), true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyOptionsCtrl::HandleEnteredScreen()
{
	if(LobbyScreen::OnLoginScreen())
		return;

	if(LobbyScreen::OnLanScreen()!=mIsLan)
		return;

	SyncOptions();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyOptionsCtrl::HandleSyncFirewallStatus()
{
	if(LobbyStaging::IsFirewallTestDone())
	{
		if(LobbyStaging::IsBehindFirewall())
		{
			mFirewalledIcon->SetImage(LobbyGlobal_Firewall_Image);
			mFirewalledIcon->Invalidate();
			mFirewalledIcon->SetVisible(true);	
		}
		else
			mFirewalledIcon->SetVisible(false);
	}
	else
	{
		mFirewalledIcon->SetImage(LobbyGlobal_Searching_Image);
		mFirewalledIcon->Invalidate();
		mFirewalledIcon->SetVisible(true);	
	}


	IPAddr aPublicAddr = LobbyStaging::GetPublicAddr();
	if(!aPublicAddr.IsValid())
	{
		if(!LobbyStaging::IsFirewallTestDone())
			mPublicAddressDetailLabel->SetText(LobbyOptions_CheckingAddress_String);
		else 
			mPublicAddressDetailLabel->SetText(LobbyOptions_AddressUnavailable_String);
	}
	else
		mPublicAddressDetailLabel->SetText(aPublicAddr.GetHostString());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

