#ifndef __CHATCTRL_H__
#define __CHATCTRL_H__

#include "WONGUI/Container.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"

namespace WONAPI
{


class AlertEvent;
class AnnounceClientLocationEvent;
class AnnounceFriendEvent;
class ChatCtrlConfig;
class ChatCommandErrorEvent;
class ChatCommandParser;
class ClientModeratedEvent;
class GroupInvitationEvent;
class GroupJoinAttemptEvent;
class JoinedRoomEvent;
class LeftRoomEvent;
class LobbyClient;
class PlayerTeamChangedEvent;
class RecvChatEvent;
class SetChatInputEvent;

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Label);
WON_PTR_FORWARD(TextArea);
WON_PTR_FORWARD(TextLink);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ChatCtrl : public Container
{
friend ChatCtrlConfig;

protected:
	FontPtr mBoldFont;
	FontPtr mItalicFont;

	int COLOR_DEFAULT;
	int COLOR_USER;
	int COLOR_WHISPER;
	int COLOR_EMOTE;
	int COLOR_WARNING;
	int COLOR_ERROR;
	int COLOR_HELP;
	int COLOR_GROUP;
	int COLOR_MESSAGE;
	int COLOR_ACCEPT;
	int COLOR_REJECT;

	InputBoxPtr		mChatInput;
	ButtonPtr		mSendButton;
	TextAreaPtr		mChatOutput;
	int				mRoomSpecFlags;
	unsigned short	mMyClientId;
	GUIString		mRoomName;

	bool IsTeam();

	void AddSegment(const GUIString &theStr, const GUIString **strings, int nstrings, TextLink **links, int nlinks, bool lineBreak = true);
	void AddSegment(const GUIString &theStr, const GUIString &theSub, bool doLineBreak = true);
	void AddSegment(const GUIString &theStr, TextLink *theSub, bool doLineBreak = true);

	TextLinkPtr GetClientLink(LobbyClient *theClient);
	void DeActivateLink(TextLink *theLink, int theAcceptColor, int theRejectColor);

	void HandlePlayerTeamChanged(PlayerTeamChangedEvent *theEvent); // when someone joins or leaves your team
	void HandleAnnounceClientLocation(AnnounceClientLocationEvent *theEvent);
	void HandleAnnounceFriend(AnnounceFriendEvent *theEvent);
	void HandleJoinedRoom(JoinedRoomEvent *theEvent);
	void HandleLeftRoom(LeftRoomEvent *theEvent);
	void HandleLink(ComponentEvent *theEvent);
	void HandleRecvChat(RecvChatEvent *theEvent);
	void HandleSendChat();
	void HandleSetChatInput(SetChatInputEvent *theEvent);
	void HandleChatError(ChatCommandParser *theParser);
	void HandleChatCommandError(ChatCommandErrorEvent *theEvent);
	void HandleGroupInvitation(GroupInvitationEvent *theEvent);
	void HandleGroupJoinAttempt(GroupJoinAttemptEvent *theEvent);
	void HandleClientModerated(ClientModeratedEvent *theEvent);
	void HandleAlert(AlertEvent *theEvent);
	
//	void HandleAnnouncePlayerBanned(AnnouncePlayerBannedEvent* theEvent);*/

	virtual ~ChatCtrl();

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);\
	void Clear();

public:
	ChatCtrl();

	void SetRoomSpecFlags(int theFlags) { mRoomSpecFlags = theFlags; }

};
typedef SmartPtr<ChatCtrl> ChatCtrlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ChatCtrlConfig : public ContainerConfig
{
protected:
	ChatCtrl* mChatCtrl;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { ContainerConfig::SetComponent(theComponent); mChatCtrl = (ChatCtrl*)theComponent; }

	static Component* CompFactory() { return new ChatCtrl; }
	static ComponentConfig* CfgFactory() { return new ChatCtrlConfig; }
};


};

#endif