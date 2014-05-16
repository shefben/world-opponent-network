#ifndef __LobbyOptionsCTRL_H__
#define __LobbyOptionsCTRL_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(ComboBox);
WON_PTR_FORWARD(ImageComponent);
WON_PTR_FORWARD(Label);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyOptionsCtrl : public Container
{
protected:
	bool mIsLan;

	// Common Controls
	ComboBoxPtr			mDefaultTabCombo;
	ComboBoxPtr			mConnectionSpeedCombo;

	ButtonPtr			mShowCrossPromotion;
	ButtonPtr			mLobbySoundEffects;
	ButtonPtr			mChatSoundEffects;
	ButtonPtr			mStagingSoundEffects;
	ButtonPtr			mLobbyMusic;
	ButtonPtr			mShowConfirmationsCheck;
	ButtonPtr			mShowGamesInProgressCheck;
	ButtonPtr			mAnonymousToFriendsCheck;
	ButtonPtr			mAutoPingGamesCheck;

	LabelPtr			mLocalAddressDetailLabel;
	LabelPtr			mPublicAddressDetailLabel;
	ImageComponentPtr	mFirewalledIcon;

	// Lan Specific
	ComboBoxPtr			mUserNameCombo;
	LabelPtr			mLanBroadcastDetailLabel;

	// Internet Specific
	ButtonPtr			mFilterDirtyWords;
	ButtonPtr			mDoAutoAFK;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	void HandleSyncFirewallStatus();
	void HandleEnteredScreen();
	void SetLanOptions();
	void SyncOptions();
	bool HandleButtonPress(Component *theButton);

	virtual ~LobbyOptionsCtrl();

public:
	LobbyOptionsCtrl(bool isLan);
	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<LobbyOptionsCtrl> LobbyOptionsCtrlPtr;

};

#endif