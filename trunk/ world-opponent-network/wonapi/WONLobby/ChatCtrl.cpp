#include "ChatCtrl.h"

#include "ChatCommandParser.h"
#include "LobbyClient.h"
#include "LobbyEvent.h"
#include "LobbyFriend.h"
#include "LobbyGroup.h"
#include "LobbyMisc.h"
#include "LobbyResource.h"
#include "LobbyServer.h"
#include "LobbyStaging.h"
#include "LobbyPersistentData.h"

#include "WONGUI/Button.h"
#include "WONGUI/InputBox.h"
#include "WONGUI/Label.h"
#include "WONGUI/TextBox.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

enum ChatLinkType
{
	ChatLinkType_JoinGroup,
	ChatLinkType_AcceptClient,
	ChatLinkType_RejectClient,
	ChatLinkType_ClientOptions,
	ChatLinkType_FriendOptions,
	ChatLinkType_JoinFriend
};

class ChatLink;
typedef SmartPtr<ChatLink> ChatLinkPtr;

class ChatLink : public TextLink
{
public:
	ChatLinkType mType;
	IPAddr mServerIP;
	unsigned short mGroupId;
	LobbyClientPtr mClient;
	LobbyFriendPtr mFriend;
	bool mIsLink;
	ChatLinkPtr mPrevLink; // refcount previous link in case it gets erased from the textbox
	ChatLink *mNextLink;

	void InitClient(LobbyClient *theClient)
	{
		mClient = theClient;
	}

	void InitFriend(LobbyFriend *theFriend)
	{
		mFriend = theFriend;
	}

	ChatLink(ChatLinkType theType, const GUIString &theGroupName, unsigned short theGroupId, LobbyServer *theServer, int theColor, Font *theFont) : 
		mType(theType)
	{ 
		mText = theGroupName;
		mGroupId = theGroupId;
		mColor = theColor; 
		mFont = theFont; 
		mIsLink = true;
		mNextLink = NULL;
		if(theServer!=NULL)
			mServerIP = theServer->GetIPAddr();
	}
	
	
	ChatLink(ChatLinkType theType, LobbyClient *theClient, int theColor, Font *theFont) : 
		mType(theType)
	{
		InitClient(theClient);

		mText = theClient->GetName();
		mColor = theColor;
		mFont = theFont;
		mIsLink = true;
		mNextLink = NULL;
	}

	ChatLink(ChatLinkType theType, LobbyClient *theClient, const GUIString &theText, int theColor, Font *theFont) : 
		mType(theType)
	{
		InitClient(theClient);

		mText = theText;
		mColor = theColor;
		mFont = theFont;
		mIsLink = true;
		mNextLink = NULL;
	}

	ChatLink(ChatLinkType theType, LobbyFriend* theFriend, int theColor, Font *theFont) :
		mType(theType)
	{
		InitFriend(theFriend);
		mText = theFriend->GetName();
		mColor = theColor;
		mFont = theFont;
		mIsLink = true;
		mNextLink = NULL;
	}

	ChatLink(ChatLinkType theType, const IPAddr &theAddr, const GUIString &theServerName, LobbyFriend* theFriend, int theColor, Font *theFont) :
		mType(theType)
	{
		InitFriend(theFriend);

		mServerIP = theAddr;
		mText = theServerName;
		mColor = theColor;
		mFont = theFont;
		mIsLink = true;
		mNextLink = NULL;
	}

	virtual bool IsLink() { return mIsLink; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChatCtrl::ChatCtrl()
{
	COLOR_DEFAULT	= 0x000000;
	COLOR_ERROR		= 0xff0000;
	COLOR_HELP		= 0x404040;
	COLOR_USER		= 0x800000;
	COLOR_EMOTE		= 0x800080;
	COLOR_WHISPER	= 0x008000;
	COLOR_WARNING	= 0xff8000;	
	COLOR_GROUP		= 0x000080;
	COLOR_MESSAGE	= 0x0000ff;
	COLOR_ACCEPT    = 0x000000;
	COLOR_REJECT    = 0xa0a0a0;

	mMyClientId = LobbyServerId_Invalid;
	mRoomSpecFlags = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChatCtrl::~ChatCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mChatInput,"ChatInput");
		WONComponentConfig_Get(aConfig,mSendButton,"SendButton");
		WONComponentConfig_Get(aConfig,mChatOutput,"ChatOutput");

		mBoldFont = GetFontMod(mChatOutput->GetTextFont(), FontStyle_Bold);
		mItalicFont = GetFontMod(mChatOutput->GetTextFont(), FontStyle_Italic);

		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ChatCtrl::IsTeam()
{
	return mRoomSpecFlags&LobbyRoomSpecFlag_Team?true:false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::AddSegment(const GUIString &theStr, const GUIString **strings, int nstrings, TextLink **links, int nlinks, bool lineBreak)
{
	int colors[] = 
	{ 
		COLOR_DEFAULT, COLOR_USER, COLOR_WHISPER, COLOR_EMOTE, COLOR_WARNING,
		COLOR_ERROR, COLOR_HELP, COLOR_GROUP, COLOR_MESSAGE
	};
	int ncolors = sizeof(colors)/sizeof(colors[0]);

	Font *fonts[] = { mChatOutput->GetTextFont(), mBoldFont, mItalicFont };
	int nfonts = sizeof(fonts)/sizeof(fonts[0]);

	StringLocalize::AddText(mChatOutput, theStr, strings, nstrings, colors, ncolors, fonts, nfonts, links, nlinks, lineBreak); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::AddSegment(const GUIString &theStr, const GUIString &theSub, bool doLineBreak)
{
	const GUIString *subs[] = { &theSub };
	AddSegment(theStr,subs,1,NULL,0,doLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::AddSegment(const GUIString &theStr, TextLink *theSub, bool doLineBreak)
{
	TextLink *subs[] = { theSub };
	AddSegment(theStr,NULL,0,subs,1,doLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::Clear()
{
	mChatOutput->Clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextLinkPtr ChatCtrl::GetClientLink(LobbyClient *theClient)
{
	return new ChatLink(ChatLinkType_ClientOptions,theClient,COLOR_USER,mChatOutput->GetTextFont());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleAnnounceClientLocation(AnnounceClientLocationEvent *theEvent)
{
	if(theEvent->mHandled)
		return;

	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	LobbyClient *aClient = theEvent->mClient;
	if(aClient==NULL)
		return;

	if(!IsVisibleUpToRoot())
		return;

	theEvent->mHandled = true;

	LobbyServer *aServer = LobbyMisc::GetChatServer(); // is ok if it's null

	TextLinkPtr aClientLink = GetClientLink(theEvent->mClient);
	TextLinkPtr aGameGroup = new ChatLink(ChatLinkType_JoinGroup, theEvent->mGameName, aClient->GetGameGroupId(), aServer, COLOR_GROUP,mChatOutput->GetLinkFont());
	TextLinkPtr aChatGroup = new ChatLink(ChatLinkType_JoinGroup, theEvent->mChatName, aClient->GetChatGroupId(), aServer, COLOR_GROUP,mChatOutput->GetLinkFont());
	TextLink* links[3] = { aClientLink, aGameGroup, aChatGroup };

	GUIString aLocateStr;
	bool inGame = !theEvent->mGameName.empty();
	bool inChat = !theEvent->mChatName.empty();

	if(aClient->IsAnonymous())
		aLocateStr = ChatCtrl_ClientIsAnonymous_String;
	else if(inGame && inChat)
		aLocateStr = ChatCtrl_ClientInGameAndChat_String;
	else if(inChat && !inGame)
		aLocateStr = ChatCtrl_ClientInChat_String;
	else if(!inChat && inGame)
		aLocateStr = ChatCtrl_ClientInGame_String;
	else
		aLocateStr = ChatCtrl_ClientNotOnServer_String;

	AddSegment(aLocateStr,NULL,0,links,3);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleAnnounceFriend(AnnounceFriendEvent *theEvent)
{
	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	LobbyFriend *aFriend = theEvent->mFriend;
	if(aFriend==NULL)
		return;

	if(theEvent->IsAdd()) // friend joined the server
	{
		TextLinkPtr aLink = new ChatLink(ChatLinkType_FriendOptions, aFriend, COLOR_USER, mChatOutput->GetTextFont());
		AddSegment(ChatCtrl_FriendJoined_String,aLink);
		return;
	}
	else if(theEvent->IsDelete()) // friend left the server
	{
		TextLinkPtr aLink = new ChatLink(ChatLinkType_FriendOptions, aFriend, COLOR_USER, mChatOutput->GetTextFont());
		AddSegment(ChatCtrl_FriendLeft_String,aLink);
		return;
	}

	if(theEvent->mHandled)
		return;

	if(!IsVisibleUpToRoot())
		return;

	theEvent->mHandled = true;

	// Report friend location
	IPAddr aServerIP = aFriend->GetServerIP();
	GUIString aServerName = aFriend->GetServerName();
	bool online = aFriend->IsOnline() && aServerIP.IsValid() && !aServerName.empty();

	TextLinkPtr aClientLink = new ChatLink(ChatLinkType_FriendOptions, aFriend, COLOR_USER, mChatOutput->GetTextFont());
	TextLinkPtr aServerLink;
	if(online)
		aServerLink = new ChatLink(ChatLinkType_JoinFriend, aServerIP, aServerName, aFriend, COLOR_USER, mChatOutput->GetTextFont());

	TextLink *links[] = { aClientLink, aServerLink };

	GUIString aLocateStr;
	if(!online)
		aLocateStr = ChatCtrl_FriendIsNotOnline_String;
	else if(aFriend->InChat())
	{
		if(aFriend->InGame())
			aLocateStr = ChatCtrl_FriendIsInGameStaging_String;
		else
			aLocateStr = ChatCtrl_FriendIsChatting_String;
	}
	else
	{
		if(aFriend->InGame())
			aLocateStr = ChatCtrl_FriendIsPlaying_String;
		else if(online)
			aLocateStr = ChatCtrl_FriendIsOn_String;
		else
			aLocateStr = ChatCtrl_FriendIsNotOnline_String;
	}

	AddSegment(aLocateStr,NULL,0,links,2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
void ChatCtrl::HandleAnnouncePlayerBanned(AnnouncePlayerBannedEvent* theEvent)
{
	if (mCtrlLinkId != theEvent->mLinkId)
		return;

	TextLinkPtr aLink = GetClientLink(theEvent->mClient);
	if (theEvent->mKick)
		AddSegment(ChatCtrl_PlayerKicked_String,aLink);
	else
		AddSegment(ChatCtrl_PlayerBanned_String,aLink);
}
*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleGroupInvitation(GroupInvitationEvent *theEvent)
{
	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	TextLinkPtr aClientLink = GetClientLink(theEvent->mClient);
	TextLinkPtr aGroupLink = new ChatLink(ChatLinkType_JoinGroup, theEvent->mGroupName, theEvent->mGroupId, LobbyMisc::GetChatServer(), COLOR_GROUP,mChatOutput->GetLinkFont());

	TextLink *links[] = { aClientLink, aGroupLink };
	const GUIString *strings[] = { &theEvent->mGroupName, &theEvent->mComment };

	int aStrId;
	if(!theEvent->mFromMe)
	{
		if(theEvent->mInvited)
			aStrId = ChatCtrl_SomeoneInvitesYou_String_Id;
		else
			aStrId = ChatCtrl_SomeoneUninvitesYou_String_Id;
	}
	else
	{
		if(theEvent->mInvited)
			aStrId = ChatCtrl_YouInviteSomeone_String_Id;
		else
			aStrId = ChatCtrl_YouUninviteSomeone_String_Id;
	}
	if(!theEvent->mComment.empty())
		aStrId += 4; // use explanation version of the string
		
	GUIString anInviteStr = LobbyResource_GetString((LobbyResourceId)aStrId);
	AddSegment(anInviteStr,strings,2,links,2);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleGroupJoinAttempt(GroupJoinAttemptEvent *theEvent)
{
	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	// Team chat should not show join attempts
	if(IsTeam())
		return;

	TextLinkPtr aSender = GetClientLink(theEvent->mClient);
	ChatLinkPtr anAccept = new ChatLink(ChatLinkType_AcceptClient, theEvent->mClient, ChatCtrl_Accept_String, COLOR_MESSAGE,mChatOutput->GetLinkFont());
	ChatLinkPtr aReject = new ChatLink(ChatLinkType_RejectClient, theEvent->mClient, ChatCtrl_Reject_String, COLOR_MESSAGE,mChatOutput->GetLinkFont());
	anAccept->mNextLink = aReject;
	aReject->mPrevLink = anAccept;


	const GUIString *strings[] = { &theEvent->mComment };
	TextLink *links[] = { aSender, anAccept, aReject };

	GUIString aJoinStr = theEvent->mComment.empty()?ChatCtrl_SomeoneAsksToJoin_String:ChatCtrl_SomeoneAsksToJoinExpl_String;
	AddSegment(aJoinStr,strings,1,links,3);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandlePlayerTeamChanged(PlayerTeamChangedEvent *theEvent)
{
	if(IsTeam())
	{
		if(theEvent->IsAdd())
			AddSegment(ChatCtrl_TeamMemberJoined_String,GetClientLink(theEvent->mClient));
		else if(theEvent->IsDelete())
			AddSegment(ChatCtrl_TeamMemberLeft_String,GetClientLink(theEvent->mClient));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleSetChatInput(SetChatInputEvent *theEvent)
{
	if(theEvent->mHandled)
		return;

	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	if(!IsVisibleUpToRoot())
		return;

	theEvent->mHandled = true;

	ChatCommandParser *aParser = LobbyMisc::GetChatCommandParser();
	if(aParser==NULL)
		return;

	mChatInput->RequestFocus();

	GUIString aCommand;
	aCommand.append(aParser->GetCommandChar());
	aCommand.append(aParser->GetStringForCommand(theEvent->mCommand));
	aCommand.append(" \"");
	aCommand.append(theEvent->mName);
	aCommand.append("\" ");
	mChatInput->SetText(aCommand,false);
	mChatInput->SetSel(-1,-1);

	// Check to see if the current text is the same command
	if (mChatInput->GetText().find(aCommand)!=-1)
		return; // don't do anything

	// If the current text is another command, overwrite it, otherwise prepend the command
	if (mChatInput->GetText().at(0) != aParser->GetCommandChar())
		aCommand.append(mChatInput->GetText());
			
	mChatInput->SetText(aCommand,false);
	mChatInput->SetSel(-1,-1);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleRecvChat(RecvChatEvent *theEvent)
{
	if(theEvent->mSender.get()==NULL) // need a sender
		return;

	if(mMyClientId==LobbyServerId_Invalid) // not in a chat room
		return;

	if(theEvent->IsTeam())
	{
		if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags | LobbyRoomSpecFlag_Team))
			return;
	}
	else
	{
		if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
			return;
			
		if(IsTeam() && !theEvent->IsWhisper() && !theEvent->IsWarning()) // show whispers and warning in team chat as well
			return;
	}

	bool useTeamStrings = theEvent->IsTeam() && !IsTeam(); // indicate that the chat came from the team window

	TextLinkPtr aSenderLink = GetClientLink(theEvent->mSender);
	TextLinkPtr aDestLink;
	TextLink* links [] = { aSenderLink, NULL };
	int nlinks = 1;

	if(theEvent->mDestClient.get()!=NULL)
	{
		links[1] = aDestLink = GetClientLink(theEvent->mDestClient);
		nlinks = 2;
	}

	theEvent->mDestClient.get()?2:1;

	const GUIString* strings[] = { &theEvent->mText };
	int nstrings = 1;

	GUIString anInsertStr;

	if(theEvent->IsEmote())
		anInsertStr = useTeamStrings?ChatCtrl_TeamEmote_String:ChatCtrl_Emote_String;
	else if(theEvent->IsWhisper() && theEvent->mDestClient.get()!=NULL)
	{
		if(theEvent->mSender->GetClientId()==mMyClientId) // You whisper
		{
			if(theEvent->mDestClient.get()->GetClientId()==mMyClientId) // to yourself
				anInsertStr = ChatCtrl_YouWhisperToYourself_String;	
			else																	// to someone
				anInsertStr = ChatCtrl_YouWhisper_String;	
		}
		else																		// someone whispers
			anInsertStr = ChatCtrl_SomeoneWhispersToYou_String;						
	}
	else if (theEvent->IsWarning() && theEvent->mDestClient.get()!=NULL)
	{
		if (theEvent->mSender->GetClientId() == mMyClientId)						// you warn
		{	
			if(theEvent->mDestClient->GetClientId()==mMyClientId)					// you warn yourself
				anInsertStr = ChatCtrl_YouWarnYourself_String;
			else																	// you warn someone
				anInsertStr = ChatCtrl_YouWarnSomeone_String;
		}
		else																		// someone warns
		{
			if(theEvent->mDestClient->GetClientId()==mMyClientId)					// someone warns you
				anInsertStr = ChatCtrl_SomeoneWarnsYou_String;
			else																	// someone warnss someone
				anInsertStr = ChatCtrl_SomeoneWarnsSomeone_String;
		}
	}
	else // normal chat
		anInsertStr = useTeamStrings?ChatCtrl_TeamChat_String:ChatCtrl_NormalChat_String;

	if(!anInsertStr.empty())
		AddSegment(anInsertStr, strings, nstrings, links, nlinks);
}
/*
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleServerDisconnect(ServerDisconnectEvent *theEvent)
{
	if(mGroup.get()!=NULL && mGroup->mServer==theEvent->mServer)
	{
		AddSegment(ChatCtrl_Disconnected_String,mGroup->mName);
		SetGroup(NULL,false);
	}
}
*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleJoinedRoom(JoinedRoomEvent *theEvent)
{
	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;
	
	mMyClientId = theEvent->mMyClientId;
	mRoomName = theEvent->mRoomName;

	mSendButton->Enable(true);

	AddSegment(ChatCtrl_EnteringRoom_String, theEvent->mRoomName);
	mChatOutput->LineBreak(10);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleLeftRoom(LeftRoomEvent *theEvent)
{
	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	mMyClientId = LobbyServerId_Invalid;
	mSendButton->Enable(false);

	if(theEvent->mDisplayMessage)
	{
		if(theEvent->mDisconnect)
			AddSegment(ChatCtrl_Disconnected_String,mRoomName);
		else
			AddSegment(ChatCtrl_LeavingRoom_String, mRoomName);

		mChatOutput->LineBreak(10);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleChatError(ChatCommandParser *theParser)
{
	GUIString anError;
	switch(theParser->GetLastInputError())
	{
		case ChatCommandParser::InputError_ClientNotFound:
			anError = StringLocalize::GetStr(RoutingLogic_ClientNotFound_String, theParser->GetLastMatchName());
			break;

		case ChatCommandParser::InputError_ClientAmbiguous:
			anError = StringLocalize::GetStr(RoutingLogic_ClientAmbiguous_String, theParser->GetLastMatchName());
			break;

		case ChatCommandParser::InputError_InvalidCommand:
			anError = RoutingLogic_InvalidCommand_String;
			break;

		case ChatCommandParser::InputError_RequireName:
			anError = RoutingLogic_RequireName_String;
			break;

		case ChatCommandParser::InputError_BadTime:
			anError = RoutingLogic_BadTimeSpec_String;
			break;
	}
	if(anError.empty())
		anError = RoutingLogic_Error_String;

	mChatOutput->AddSegment(anError,COLOR_ERROR,true);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleChatCommandError(ChatCommandErrorEvent *theEvent)
{
	mChatOutput->AddSegment(theEvent->mError,COLOR_ERROR,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleSendChat()
{
	if(mChatInput->GetText().empty() || mSendButton->Disabled())
		return;

	ChatCommandParser *aParser = LobbyMisc::GetChatCommandParser(mRoomSpecFlags);
	if(aParser==NULL)
		return;

	GUIString aCommand = mChatInput->GetText();
	mChatInput->AddInputHistory(aCommand);
	mChatInput->Clear();

	if(!aParser->HandleInput(aCommand))
	{
		HandleChatError(aParser);
		return;
	}

	switch(aParser->GetLastChatCommand())
	{
		case LobbyChatCommand_Help: AddSegment(aParser->GetHelpText(mMyClientId),NULL,0,NULL,0); return;
		case LobbyChatCommand_Clear: mChatOutput->Clear(); return;
		case LobbyChatCommand_Reply: 
			if(aParser->GetLastChat().empty())
			{
				GUIString aCommand;
				aCommand.append(aParser->GetCommandChar());
				aCommand.append(aParser->GetStringForCommand(LobbyChatCommand_Whisper));
				aCommand.append(" \"");
				aCommand.append(aParser->GetLastMatchName());
				aCommand.append("\" ");
				mChatInput->SetText(aCommand,false);
				mChatInput->SetSel(-1,-1);
				return;
			}
			break;

		case LobbyChatCommand_ShowTeam:
			{
			LobbyClientListItr aList = aParser->GetClientList();
			LobbyClient *myClient = LobbyStaging::GetMyClient();
			if(myClient==NULL)
				return;

			GUIString aListStr;
			while(aList.HasMoreClients())
			{
				LobbyClient *aClient = aList.GetNextClient();
				if(myClient->SameTeam(aClient))
				{
					if(!aListStr.empty())
						aListStr.append(", ");

					aListStr.append(aClient->GetName());
				}
			}
			AddSegment(ChatCtrl_TeamMembers_String,aListStr, true);
			return;
			}

		case LobbyChatCommand_Ignore:
			if (aParser->GetLastMatchName().empty())
			{
				GUIString anIgnList;
				LobbyPersistentData::IgnoreNameSet::const_iterator anItr(LobbyPersistentData::GetIgnoreNameSet().begin());
				for (; anItr != LobbyPersistentData::GetIgnoreNameSet().end(); ++anItr)
				{
					if(!anIgnList.empty())
						anIgnList.append(", ");
					anIgnList.append(*anItr);
				}
				AddSegment(ChatCtrl_IgnoreList_String, anIgnList, true);
				return;
			}
			break;
	}

	FireEvent(LobbyEvent_DoChatCommand,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static BYTE GetTimedOffset(unsigned long theSeconds, bool isRoundUp, unsigned long& theTimeValue)
{
	// Indefinite duration
	if(theSeconds == 0)
	{
		theTimeValue = 0;
		return 6;
	}

	if(isRoundUp)
	{
		// Less than or equal to 1 minute (1 minute)
		if(theSeconds <= 60)
		{
			theTimeValue = 1;
			return 0;
		}
		// Less than or equal to 59 minutes (x minutes)
		if(theSeconds <= 59*60)
		{
			theTimeValue = theSeconds / 60;
			if((theSeconds % 60) > 0)
				theTimeValue++;

			return 1;
		}
		// Less than or equal to 1 hour (1 hour)
		if(theSeconds <= 60*60)
		{
			theTimeValue = 1;
			return 2;
		}
		// Less than or equal to 23 hours (x hours)
		if(theSeconds <= 23*60*60)
		{
			theTimeValue = theSeconds / (60*60);
			if((theSeconds % (60*60)) > 0)
				theTimeValue++;

			return 3;
		}
		// Less than or equal to 1 day (1 day)
		if(theSeconds <= 60*60*24)
		{
			theTimeValue = 1;
			return 4;
		}
		// More than 1 day (x days)
		theTimeValue = theSeconds / (60*60*24);
		if((theSeconds % (60*60*24)) > 0)
			theTimeValue++;

		return 5;
	}
	else
	{
		// Less than 2 minutes (1 minute)
		if(theSeconds < 2*60)
		{
			theTimeValue = 1;
			return 0;
		}
		// Less than 1 hour (x minutes)
		if(theSeconds < 60*60)
		{
			theTimeValue = theSeconds / 60;
			return 1;
		}
		// Less than 2 hours (1 hour)
		if(theSeconds < 2*60*60)
		{
			theTimeValue = 1;
			return 2;
		}
		// Less than 1 day (x hours)
		if(theSeconds < 60*60*24)
		{
			theTimeValue = theSeconds / (60*60);
			return 3;
		}
		// Less than 2 days (1 day)
		if(theSeconds < 2*60*60*24)
		{
			theTimeValue = 1;
			return 4;
		}
		// 2 or more days (x days)
		theTimeValue = theSeconds / (60*60*24);
		return 5;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleClientModerated(ClientModeratedEvent *theEvent)
{
	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	GUIString aStr;
	TextLinkPtr aLink = GetClientLink(theEvent->mClient);
	TextLink *links[] = { aLink };

	unsigned long aTimeVal = 0;
	if(theEvent->mIsMe)
	{
		switch(theEvent->mAction)
		{
		case ClientModeratedEvent::Action_Ban:
			aStr = LobbyResource_GetString((LobbyResourceId)(ChatCtrl_YouWereBannedMinute_String_Id + GetTimedOffset(theEvent->mDuration, true, aTimeVal)));
			break;
		case ClientModeratedEvent::Action_Mute:
			aStr = LobbyResource_GetString((LobbyResourceId)(ChatCtrl_YouWereMutedMinute_String_Id + GetTimedOffset(theEvent->mDuration, true, aTimeVal)));
			break;
		case ClientModeratedEvent::Action_Unmute:
			aStr = ChatCtrl_YouWereUnmuted_String;
			break;
		default:
		case ClientModeratedEvent::Action_Kick:
			aStr = ChatCtrl_YouWereKicked_String;
			break;
		}
	}
	else
	{
		switch(theEvent->mAction)
		{
		case ClientModeratedEvent::Action_Ban:
			aStr = LobbyResource_GetString((LobbyResourceId)(ChatCtrl_SomeoneWasBannedMinute_String_Id + GetTimedOffset(theEvent->mDuration, true, aTimeVal)));
			break;
		case ClientModeratedEvent::Action_Mute:
			aStr = LobbyResource_GetString((LobbyResourceId)(ChatCtrl_SomeoneWasMutedMinute_String_Id + GetTimedOffset(theEvent->mDuration, true, aTimeVal)));
			break;
		case ClientModeratedEvent::Action_Unmute:
			aStr = ChatCtrl_SomeoneWasUnmuted_String;
			break;
		default:
		case ClientModeratedEvent::Action_Kick:
			aStr = ChatCtrl_SomeoneWasKicked_String;
			break;
		}
	}

	if(aTimeVal == 0)
	{
		const GUIString *strings[] = { &theEvent->mRoomName };
		AddSegment(aStr,strings,1,links,1,true);
	}
	else
	{
		char aBuffer[20];
		sprintf(aBuffer, "%lu", aTimeVal);
		GUIString aVal(aBuffer);
		const GUIString *strings[] = { &theEvent->mRoomName, &aVal };
		AddSegment(aStr,strings,2,links,1,true);
	}

	// Add the comment if there is one
	if(!theEvent->mComment.empty())
	{
		const GUIString *comments[] = { &theEvent->mComment };
		aStr = ChatCtrl_ModeratorComment_String;
		AddSegment(aStr,comments,1,NULL,0,true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleAlert(AlertEvent *theEvent)
{
	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	GUIString aStr;
	const GUIString *comments[] = { &theEvent->mComment };

	switch(theEvent->mAction)
	{
	case AlertEvent::Action_Alert:
		aStr = ChatCtrl_ServerAlert_String;
		break;
	case AlertEvent::Action_ShutdownStarted:
		{
			unsigned long aTimeVal = 0;
			aStr = LobbyResource_GetString((LobbyResourceId)(ChatCtrl_ServerShutdownStartedMinute_String_Id + GetTimedOffset(theEvent->mTimeLeft, false, aTimeVal)));
			if(aTimeVal != 0)
			{
				char aBuffer[20];
				sprintf(aBuffer, "%lu", aTimeVal);
				GUIString aVal(aBuffer);
				const GUIString *strings[] = { &aVal };
				AddSegment(aStr,strings,1,NULL,0,true);
				aStr = ChatCtrl_AlertComment_String;
			}
		}
		break;
	default:
	case AlertEvent::Action_ShutdownAborted:
		aStr = ChatCtrl_ServerShutdownAborted_String;
		AddSegment(aStr,NULL,0,NULL,0,true);
		aStr = ChatCtrl_AlertComment_String;
		break;
	}

	if(!theEvent->mComment.empty())
		AddSegment(aStr,comments,1,NULL,0,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::DeActivateLink(TextLink *theLink, int theAcceptColor, int theRejectColor)
{
	ChatLink *aLinkOrig = (ChatLink*)theLink;
	aLinkOrig->mColor = theAcceptColor;
	aLinkOrig->mIsLink = false;

	ChatLink *aLink = aLinkOrig->mPrevLink;
	while(aLink!=NULL)
	{
		aLink->mIsLink = false;
		aLink->mColor = theRejectColor;
		aLink = aLink->mPrevLink;
	}

	aLink = aLinkOrig->mNextLink;
	while(aLink!=NULL)
	{
		aLink->mIsLink = false;
		aLink->mColor = theRejectColor;
		aLink = aLink->mNextLink;
	}

	mChatOutput->Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleLink(ComponentEvent *theEvent)
{
	TextLinkActivatedEvent *anEvent = (TextLinkActivatedEvent*)theEvent;
	ChatLink *aLink = dynamic_cast<ChatLink*>(anEvent->mLink.get());
	if(aLink==NULL)
		return;

	switch(aLink->mType)
	{
		case ChatLinkType_ClientOptions: FireEvent(new ClientActionEvent(LobbyClientAction_ShowMenu,mRoomSpecFlags,aLink->mClient)); break;
		case ChatLinkType_FriendOptions: FireEvent(new FriendActionEvent(LobbyFriendAction_ShowMenuTryClient,aLink->mFriend,mRoomSpecFlags)); break;
		case ChatLinkType_JoinFriend: FireEvent(new JoinFriendsRoomEvent(aLink->mFriend)); break;
		case ChatLinkType_JoinGroup: FireEvent(new JumpToRoomEvent(aLink->mText,aLink->mGroupId,aLink->mServerIP)); break;
		case ChatLinkType_AcceptClient: DeActivateLink(aLink,COLOR_ACCEPT,COLOR_REJECT); FireEvent(new AcceptClientEvent(aLink->mClient,true)); break; 
		case ChatLinkType_RejectClient: DeActivateLink(aLink,COLOR_ACCEPT,COLOR_REJECT); FireEvent(new AcceptClientEvent(aLink->mClient,false)); break;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_InputReturn: 
			if(theEvent->mComponent!=mChatInput)
				break;

			HandleSendChat();
			return;

		case ComponentEvent_ButtonPressed:
			if(theEvent->mComponent!=mSendButton)
				break;

			HandleSendChat();
			return;

		case ComponentEvent_TextLinkActivated: HandleLink(theEvent); return;

		case LobbyEvent_Alert: HandleAlert((AlertEvent*)theEvent); return;
		case LobbyEvent_AnnounceClientLocation: HandleAnnounceClientLocation((AnnounceClientLocationEvent*)theEvent); return;
		case LobbyEvent_AnnounceFriend: HandleAnnounceFriend((AnnounceFriendEvent*)theEvent); return;
		case LobbyEvent_ClientModerated: HandleClientModerated((ClientModeratedEvent*)theEvent); return;
		case LobbyEvent_GroupInvitation: HandleGroupInvitation((GroupInvitationEvent*)theEvent); return;
		case LobbyEvent_GroupJoinAttempt: HandleGroupJoinAttempt((GroupJoinAttemptEvent*)theEvent); return;
		case LobbyEvent_ChatCommandError: HandleChatCommandError((ChatCommandErrorEvent*)theEvent); return;
		case LobbyEvent_JoinedRoom: HandleJoinedRoom((JoinedRoomEvent*)theEvent); return;
		case LobbyEvent_LeftRoom: HandleLeftRoom((LeftRoomEvent*)theEvent); return;
		case LobbyEvent_PlayerTeamChanged: HandlePlayerTeamChanged((PlayerTeamChangedEvent*)theEvent); return;
		case LobbyEvent_RecvChat: HandleRecvChat((RecvChatEvent*)theEvent); return;
		case LobbyEvent_SetChatInput: HandleSetChatInput((SetChatInputEvent*)theEvent); return;
		case LobbyEvent_SetRoomSpecFlags:
			mRoomSpecFlags |= ((SetRoomSpecFlagsEvent*)theEvent)->mSpecFlags;
			return;

	}

/*	if(mScreenActive)
	{
		switch(theEvent->mType)
		{
			case LobbyEvent_GroupJoinAttempt: HandleGroupJoinAttempt((GroupJoinAttemptEvent*)theEvent); return;
			case LobbyEvent_AnnouncePlayerBanned : HandleAnnouncePlayerBanned((AnnouncePlayerBannedEvent*)theEvent); return;
		}
	}*/

	Container::HandleComponentEvent(theEvent);
}
/**/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ChatCtrlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(ContainerConfig::HandleInstruction(theInstruction))
		return true;
	
	if(theInstruction=="GROUPTYPE")
	{
		GUIString aStr = ReadString();
		if(aStr.compareNoCase("CHAT")==0)
			mChatCtrl->SetRoomSpecFlags(LobbyRoomSpecFlag_Chat);
		else if(aStr.compareNoCase("GAME")==0)
			mChatCtrl->SetRoomSpecFlags(LobbyRoomSpecFlag_Game);
		else if(aStr.compareNoCase("TEAM")==0)
			mChatCtrl->SetRoomSpecFlags(LobbyRoomSpecFlag_Game | LobbyRoomSpecFlag_Team);
		else
			throw ConfigObjectException("Invalid Group Type: " + (std::string)aStr);
	}
	else if(theInstruction=="CTRLLINKID")
	{}//mChatCtrl->SetCtrlLinkId((CtrlLinkId)ReadInt());
	else if(theInstruction=="COLOR_DEFAULT")
		mChatCtrl->COLOR_DEFAULT = ReadColor();
	else if(theInstruction=="COLOR_ERROR")
		mChatCtrl->COLOR_ERROR = ReadColor();
	else if(theInstruction=="COLOR_HELP")
		mChatCtrl->COLOR_HELP = ReadColor();
	else if(theInstruction=="COLOR_USER")
		mChatCtrl->COLOR_USER = ReadColor();
	else if(theInstruction=="COLOR_EMOTE")
		mChatCtrl->COLOR_EMOTE = ReadColor();
	else if(theInstruction=="COLOR_WHISPER")
		mChatCtrl->COLOR_WHISPER = ReadColor();
	else if(theInstruction=="COLOR_WARNING")
		mChatCtrl->COLOR_WARNING = ReadColor();
	else if(theInstruction=="COLOR_GROUP")
		mChatCtrl->COLOR_GROUP = ReadColor();
	else if(theInstruction=="COLOR_MESSAGE")
		mChatCtrl->COLOR_MESSAGE = ReadColor();
	else if(theInstruction=="COLOR_ACCEPT")
		mChatCtrl->COLOR_ACCEPT = ReadColor();
	else if(theInstruction=="COLOR_REJECT")
		mChatCtrl->COLOR_REJECT = ReadColor();
	else
		return false;

	return true;
}
