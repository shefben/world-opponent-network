#include "ChatCommandParser.h"

#include "LobbyClient.h"
#include "LobbyResource.h"
#include "WONCommon/StringParser.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChatCommandParser::ChatCommandParser()
{
	mServerWideCommands = false;
	mDoColonEmote = false;
	mMatchTolerance = MatchResult_Partial;
	mLastInputDuration = 0;
	mCommandChar = '/';
	mRoomSpecFlags = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChatCommandParser::~ChatCommandParser()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCommandParser::AddCommands()
{
	AddMultiCommand(ChatCommandLogic_Whisper_String,LobbyChatCommand_Whisper);
	AddMultiCommand(ChatCommandLogic_Block_String,LobbyChatCommand_Block);
	AddMultiCommand(ChatCommandLogic_Emote_String,LobbyChatCommand_Emote);
	AddMultiCommand(ChatCommandLogic_Ignore_String,LobbyChatCommand_Ignore);
	AddMultiCommand(ChatCommandLogic_Away_String,LobbyChatCommand_Away);
	AddMultiCommand(ChatCommandLogic_Invite_String,LobbyChatCommand_Invite);
	AddMultiCommand(ChatCommandLogic_Uninvite_String,LobbyChatCommand_Uninvite);
	AddMultiCommand(ChatCommandLogic_Reply_String,LobbyChatCommand_Reply);
	AddMultiCommand(ChatCommandLogic_Clear_String,LobbyChatCommand_Clear);
	AddMultiCommand(ChatCommandLogic_Help_String,LobbyChatCommand_Help);
	AddMultiCommand(ChatCommandLogic_ShowTeam_String,LobbyChatCommand_ShowTeam);

	AddMultiCommand(ChatCommandLogic_Moderator_String,LobbyChatCommand_BecomeModerator);
	AddMultiCommand(ChatCommandLogic_ServerMute_String,LobbyChatCommand_ServerMute);
	AddMultiCommand(ChatCommandLogic_ServerBan_String,LobbyChatCommand_ServerBan);
	AddMultiCommand(ChatCommandLogic_Mute_String,LobbyChatCommand_Mute);
	AddMultiCommand(ChatCommandLogic_Ban_String,LobbyChatCommand_Ban);
	AddMultiCommand(ChatCommandLogic_Unmute_String,LobbyChatCommand_Unmute);
	AddMultiCommand(ChatCommandLogic_ServerUnmute_String,LobbyChatCommand_ServerUnmute);
	AddMultiCommand(ChatCommandLogic_Unban_String,LobbyChatCommand_Unban);
	AddMultiCommand(ChatCommandLogic_ServerUnban_String,LobbyChatCommand_ServerUnban);
	AddMultiCommand(ChatCommandLogic_Alert_String,LobbyChatCommand_Alert);
	AddMultiCommand(L"",LobbyChatCommand_StartShutdown);
	AddMultiCommand(L"",LobbyChatCommand_AbortShutdown);
	AddMultiCommand(ChatCommandLogic_Warn_String,LobbyChatCommand_Warn);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCommandParser::SetRoomContext(LobbyClientList *theClientList, int theSpecFlags)
{
	mClientList = theClientList;
	mRoomSpecFlags = theSpecFlags;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCommandParser::AddCommandString(const GUIString &theStr, LobbyChatCommand theCommand)
{
	if(theStr.empty())
		return;

	GUIString aStr = theStr;
	mCommandStringMap[aStr] = theCommand;

	PreferredCommandStringMap::const_iterator anItr = mPreferredCommandStringMap.find(theCommand);
	if (anItr == mPreferredCommandStringMap.end())
		mPreferredCommandStringMap.insert(PreferredCommandStringMap::value_type(theCommand,theStr));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCommandParser::ClearCommandStrings()
{
	mCommandStringMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ChatCommandParser::GetStringForCommand(LobbyChatCommand theCommand)
{
	PreferredCommandStringMap::const_iterator aPreferredItr = mPreferredCommandStringMap.find(theCommand);
	if (aPreferredItr != mPreferredCommandStringMap.end())
		return aPreferredItr->second;

	CommandStringMap::const_iterator anItr = mCommandStringMap.begin();
	for (; anItr != mCommandStringMap.end(); ++anItr)
	{
		if (anItr->second == theCommand)
			return anItr->first;
	}

	return "";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char* ChatCommandParser::GetDefaultCommandString(LobbyChatCommand theCommand) // backup in case string resource is missing
{
	switch(theCommand)
	{
		case LobbyChatCommand_Whisper:					return "w"; 
		case LobbyChatCommand_Emote:					return "em";
		case LobbyChatCommand_Block:					return "b";
		case LobbyChatCommand_Ignore:					return "i";
		case LobbyChatCommand_Away:						return "afk";
		case LobbyChatCommand_BecomeModerator:			return "mod";
		case LobbyChatCommand_ServerMute:				return "smute";
		case LobbyChatCommand_ServerBan:				return "sban"; 
		case LobbyChatCommand_Mute:						return "mute";
		case LobbyChatCommand_Ban:						return "ban";
		case LobbyChatCommand_Unmute:					return "unmute";
		case LobbyChatCommand_ServerUnmute:				return "sunmute";
		case LobbyChatCommand_Unban:					return "unban";
		case LobbyChatCommand_ServerUnban:				return "sunban";
		case LobbyChatCommand_Invite:					return "invite";
		case LobbyChatCommand_Uninvite:					return "uninvite";
		case LobbyChatCommand_Reply:					return "r";
		case LobbyChatCommand_Clear:					return "cls";
		case LobbyChatCommand_Help:						return "?";
		case LobbyChatCommand_ShowTeam:					return "team";
		case LobbyChatCommand_Alert:					return "alert";
		case LobbyChatCommand_StartShutdown:			return "startshutdown";
		case LobbyChatCommand_AbortShutdown:			return "abortshutdown";
		case LobbyChatCommand_Warn:						return "warn";
	}

	return "";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCommandParser::AddMultiCommand(const std::wstring &theStr, LobbyChatCommand theCommand)
{
	StringParser aParser(theStr.c_str());
	WONTypes::WStringList aList;
	aParser.ReadValue(aList);
	
	if(theStr.empty() || aList.empty())
	{
		AddCommandString(GetDefaultCommandString(theCommand),theCommand);
		return;
	}

	for(WONTypes::WStringList::iterator anItr = aList.begin(); anItr != aList.end(); ++anItr)
		AddCommandString(*anItr,theCommand);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCommandParser::SetAllowPartialNameMatch(bool allow)
{
	if(allow)
		mMatchTolerance = MatchResult_Partial;
	else
		mMatchTolerance = MatchResult_Exact;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCommandParser::SetServerWideCommands(bool serverWideCommandsOn)
{
	mServerWideCommands = serverWideCommandsOn;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCommandParser::SetDoColonEmote(bool doColonEmote)
{
	mDoColonEmote = doColonEmote;		
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ChatCommandParser::Compare(const GUIString &s1, const GUIString &s2, bool &partialMatch)
{
	partialMatch = false;
	int i1 = 0, i2 = 0;
	int len1 = s1.length(), len2 = s2.length();

	while(i1<len1 && i2<len2)
	{
		if (towupper(s1.at(i1)) != towupper(s2.at(i2)))
			return false;

		i1++;  i2++;
	}

	if(len1<=len2)
		partialMatch = true;

	return len1==len2;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCommandParser::SkipWhitespace(const GUIString &s1, int &thePos)
{
	while(thePos<s1.length() && iswspace(s1.at(thePos)))
		thePos++;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ChatCommandParser::GetWord(const GUIString &s1, int &thePos, bool doToUpper, bool checkQuotes)
{
	SkipWhitespace(s1,thePos);
	if(thePos>=s1.length())
		return "";

	GUIString aWord;
	if(checkQuotes && s1.at(thePos)=='"')
	{
		int anEndQuotePos = s1.find('"',thePos+1);
		if(anEndQuotePos>=0)
		{
			aWord = s1.substr(thePos+1,anEndQuotePos-thePos-1);
			thePos = anEndQuotePos+1;
			if(doToUpper)
				aWord.toUpperCase();

			return aWord;
		}
	}

	while(thePos<s1.length())
	{
		wchar_t aChar = s1.at(thePos);
		if(doToUpper)
			aChar = towupper(aChar);

		if(aChar==' ')
			return aWord;
		
		aWord.append(aChar);
		thePos++;
	}

	return aWord;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ChatCommandParser::GetRestOfLine(const GUIString &s1, int thePos)
{
	SkipWhitespace(s1,thePos);
	if(thePos>=s1.length())
		return "";

	GUIString aWord;
	if(s1.at(thePos)=='"')
	{
		int anEndQuotePos = s1.find('"',thePos+1);
		if(anEndQuotePos>=0)
		{
			aWord = s1.substr(thePos+1,anEndQuotePos-thePos-1);
			return aWord;
		}
	}

	int aPos = s1.length()-1;
	while(aPos>thePos)
	{
		if(s1.at(aPos)!=' ')
			break;

		aPos--;
	}

	if(aPos>thePos)
		return s1.substr(thePos,aPos-thePos+1);
	else
		return "";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* ChatCommandParser::GetClientByName(const GUIString &theName, MatchResult *theResult)
{
	MatchResult aResult = MatchResult_NotFound;
	LobbyClient *aMatchClient = NULL;

	if(mClientList!=NULL)
	{
		const LobbyClientMap &aMap = mClientList->GetClientMap();
		LobbyClientMap::const_iterator anItr = aMap.begin();
		for(; anItr!=aMap.end(); ++anItr)
		{
			LobbyClient *aClient = anItr->second;

			bool partialMatch = false;
			bool exactMatch = false;

			exactMatch = Compare(theName,aClient->GetName(),partialMatch);
			if(exactMatch)
			{
				if(aResult==MatchResult_Exact)
				{
					aResult = MatchResult_Ambiguous;
					break;
				}

				aResult = MatchResult_Exact;
				aMatchClient = aClient;
			}
			else if(partialMatch)
			{
				if(aResult==MatchResult_NotFound)
				{
					aResult = MatchResult_Partial;
					aMatchClient = anItr->second;
				}
				else if(aResult==MatchResult_Partial)
				{
					aResult = MatchResult_Ambiguous;
					break;
				}
			}
		}
	}

	if(aResult==MatchResult_NotFound || aResult==MatchResult_Ambiguous)
		aMatchClient = NULL;

	if(theResult!=NULL)
		*theResult = aResult;

	return aMatchClient;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ChatCommandParser::HandleCommand(const GUIString &theInput)
{
	int aPos = 1;
	GUIString aCommand = GetWord(theInput,aPos,true,false);
	
	CommandStringMap::iterator anItr = mCommandStringMap.find(aCommand);
	if(anItr==mCommandStringMap.end())
	{
		mLastChatCommand = LobbyChatCommand_None;
		mLastInputError = InputError_InvalidCommand;
		return false;
	}

	mLastChatCommand = anItr->second;

	// Extract name if needed
	bool needName = false;
	switch(mLastChatCommand)
	{
		case LobbyChatCommand_Reply:
			mLastMatchName = mLastWhisperClientName;
			needName = true;
			break;

		case LobbyChatCommand_Ignore:
			mLastMatchName = GetRestOfLine(theInput,aPos);
			needName = (! mLastMatchName.empty());
			break;

		case LobbyChatCommand_Block:
			mLastMatchName = GetRestOfLine(theInput,aPos);
			needName = true;
			break;

		case LobbyChatCommand_Unmute:
		case LobbyChatCommand_ServerUnmute:
			mLastMatchName = GetRestOfLine(theInput,aPos);
			if(!mLastMatchName.empty())
				needName = true;
			break;

		case LobbyChatCommand_Warn:
		case LobbyChatCommand_Whisper:
		case LobbyChatCommand_Mute:
		case LobbyChatCommand_ServerMute:
		case LobbyChatCommand_Ban:
		case LobbyChatCommand_ServerBan:
		case LobbyChatCommand_Invite:
		case LobbyChatCommand_Uninvite:
			mLastMatchName = GetWord(theInput,aPos,false,true);
			needName = true;
			break;
	}
	
	// Find corresponding client/member
	if(needName)
	{
		if(mLastMatchName.empty())
		{
			mLastInputError = InputError_RequireName;
			return false;
		}

		MatchResult aResult;
		mLastTargetClient = GetClientByName(mLastMatchName, &aResult);
			
		if(aResult < mMatchTolerance)
		{
			if(aResult==MatchResult_Ambiguous)
				mLastInputError = InputError_ClientAmbiguous;
			else
				mLastInputError = InputError_ClientNotFound;

			return false;
		}
	}

	SkipWhitespace(theInput,aPos);

	switch(mLastChatCommand)
	{
		///////////////////////////////////////////
		///////////////////////////////////////////
		case LobbyChatCommand_Warn:
		case LobbyChatCommand_Whisper:
		case LobbyChatCommand_Alert:
		case LobbyChatCommand_Emote:
		{
			if(aPos>=theInput.length())
			{
				mLastInputError = InputError_NoChat;
				return false;
			}

			mLastChat = theInput.substr(aPos);
			return true;
		}

		///////////////////////////////////////////
		///////////////////////////////////////////
		case LobbyChatCommand_Reply:
		{
			mLastChat = theInput.substr(aPos);
			if(!mLastChat.empty())
				mLastChatCommand = LobbyChatCommand_Whisper;

			return true;
		}

		///////////////////////////////////////////
		///////////////////////////////////////////
		case LobbyChatCommand_BecomeModerator:
		case LobbyChatCommand_Help:
		case LobbyChatCommand_Ignore:
		case LobbyChatCommand_Away:
		case LobbyChatCommand_AbortShutdown:
			return true;

		///////////////////////////////////////////
		///////////////////////////////////////////
		case LobbyChatCommand_Invite:
		case LobbyChatCommand_Uninvite:
		{
			mLastChat = theInput.substr(aPos);
			mLastBool = mLastChatCommand==LobbyChatCommand_Invite;
			return true;
		}

		///////////////////////////////////////////
		///////////////////////////////////////////
		case LobbyChatCommand_Mute:
		case LobbyChatCommand_ServerMute:
		case LobbyChatCommand_Ban:
		case LobbyChatCommand_ServerBan:
		case LobbyChatCommand_StartShutdown:
		{			
			DWORD aSeconds = 3600; // 1 hour
			std::string aTimeStr = GetWord(theInput,aPos,true,false);
			if(aTimeStr=="INFINITE")
				aSeconds = 0;
			else if(aTimeStr.length()>0)
			{
				char aUnitChar = aTimeStr[aTimeStr.length()-1];
				if(isalpha(aUnitChar))
					aTimeStr = aTimeStr.substr(0,aTimeStr.length()-1);

				aSeconds = atoi(aTimeStr.c_str());
				switch(aUnitChar)
				{
					case 'D': aSeconds*=24;
					case 'H': aSeconds*=60;	
					case 'M': aSeconds*=60; 
					case 'S': break;
					default: 
						mLastInputError = InputError_BadTime;
						return false;
				}

				if(aSeconds==0)
				{
					mLastInputError = InputError_BadTime;
					return false;
				}
			}
				
			mLastInputDuration = aSeconds;
			mLastChat = GetRestOfLine(theInput,aPos);
			return true;
		}

		///////////////////////////////////////////
		///////////////////////////////////////////
		case LobbyChatCommand_Unmute:
		case LobbyChatCommand_ServerUnmute:
		{
			mLastChat = GetRestOfLine(theInput,aPos);
			return true;
		}
		
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ChatCommandParser::HandleInput(const GUIString &theInput)
{	
	mLastInputError = InputError_None;
	mLastTargetClient = NULL;
	mLastMatchName.erase();

	if(mClientList.get()==NULL)
	{
		mLastInputError = InputError_NotLoggedIn;
		return false;
	}
		
	if(theInput.at(0)==mCommandChar)
		return HandleCommand(theInput);
	
	if(mDoColonEmote && theInput.at(0)==L':')
	{
		mLastChatCommand = LobbyChatCommand_Emote;

		mLastChat = theInput.substr(1);
		int aPos = 0;
		SkipWhitespace(mLastChat,aPos);
		if(aPos==mLastChat.length())
		{
			mLastInputError = InputError_NoChat;
			return false;
		}

//		mServer->mConnection->SendChat(aChat,RoutingChatFlag_IsEmote,theGroupIdContext);
		return true;
	}

	mLastChatCommand = LobbyChatCommand_Broadcast;

	int aPos = 0;
	SkipWhitespace(theInput,aPos);
	if(aPos==theInput.length())
	{
		mLastInputError = InputError_NoChat;
		return false;
	}

	mLastChat = theInput;
//	mServer->mConnection->SendChat(theInput,0,theGroupIdContext);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChatCommandParser::AddCommandToHelpText(LobbyChatCommand theCommand, const GUIString &theDescription, GUIString &theHelpText)
{
	theHelpText.append(theDescription);
	theHelpText.append("<n>");
}

///////////////////////////////////////////////////////////////////////////////
// GetHelpText: Return a block of text that sums up the chat commands.
///////////////////////////////////////////////////////////////////////////////
GUIString ChatCommandParser::GetHelpText(unsigned short myClientId)
{
	bool isAdmin = false;
	bool isModerator = false;
	bool isCaptain = false;
	bool isGameCaptain = false;
	bool isLan = mRoomSpecFlags&LobbyRoomSpecFlag_Lan?true:false;
	if(mClientList.get()!=NULL)
	{
		LobbyClient *aClient = mClientList->GetClient(myClientId);
		if(aClient!=NULL)
		{
			isModerator = aClient->IsModerator();
			isCaptain = aClient->IsCaptain(mRoomSpecFlags&LobbyRoomSpecFlag_Game?true:false);
			isGameCaptain = aClient->IsCaptain(true);
			isAdmin = aClient->IsAdmin();
		}
	}


	
	GUIString aHelp = ChatCommandLogic_HelpHeader_String;
	aHelp.append("<n>");

	AddCommandToHelpText(LobbyChatCommand_Help,                ChatCommandLogic_HelpHelp_String, aHelp);
	AddCommandToHelpText(LobbyChatCommand_Whisper,             ChatCommandLogic_WhisperHelp_String, aHelp);
	AddCommandToHelpText(LobbyChatCommand_Reply,               ChatCommandLogic_ReplyHelp_String, aHelp);
	AddCommandToHelpText(LobbyChatCommand_Emote,               ChatCommandLogic_EmoteHelp_String, aHelp);
	AddCommandToHelpText(LobbyChatCommand_Ignore,              ChatCommandLogic_IgnoreHelp_String, aHelp);
	AddCommandToHelpText(LobbyChatCommand_Clear,               ChatCommandLogic_ClearHelp_String, aHelp);

	if(mRoomSpecFlags&LobbyRoomSpecFlag_Game)
		AddCommandToHelpText(LobbyChatCommand_ShowTeam,        ChatCommandLogic_ShowTeamHelp_String, aHelp);


	if(!isLan)	
	{
		AddCommandToHelpText(LobbyChatCommand_Block,               ChatCommandLogic_BlockHelp_String, aHelp);
		AddCommandToHelpText(LobbyChatCommand_Away,                ChatCommandLogic_AwayHelp_String, aHelp);
	
		if(isGameCaptain)
		{
			AddCommandToHelpText(LobbyChatCommand_Invite,              ChatCommandLogic_InviteHelp_String, aHelp);
			AddCommandToHelpText(LobbyChatCommand_Uninvite,            ChatCommandLogic_UninviteHelp_String, aHelp);
		}

		if(isCaptain || isModerator)
		{
			AddCommandToHelpText(LobbyChatCommand_Mute,                ChatCommandLogic_MuteHelp_String, aHelp);
			AddCommandToHelpText(LobbyChatCommand_Unmute,              ChatCommandLogic_UnmuteHelp_String, aHelp);
			AddCommandToHelpText(LobbyChatCommand_Ban,                 ChatCommandLogic_BanHelp_String, aHelp);
			AddCommandToHelpText(LobbyChatCommand_Unban,               ChatCommandLogic_UnbanHelp_String, aHelp);
		}

		if(isModerator)
		{
			AddCommandToHelpText(LobbyChatCommand_BecomeModerator, ChatCommandLogic_ModeratorHelp_String, aHelp);
			AddCommandToHelpText(LobbyChatCommand_ServerMute,      ChatCommandLogic_ServerMuteHelp_String, aHelp);
			AddCommandToHelpText(LobbyChatCommand_ServerUnmute,    ChatCommandLogic_ServerUnmuteHelp_String, aHelp);
			AddCommandToHelpText(LobbyChatCommand_ServerBan,       ChatCommandLogic_ServerBanHelp_String, aHelp);
			AddCommandToHelpText(LobbyChatCommand_ServerUnban,     ChatCommandLogic_ServerUnbanHelp_String, aHelp);
			AddCommandToHelpText(LobbyChatCommand_Warn,			   ChatCommandLogic_WarnHelp_String, aHelp);
		}

		if(isAdmin)
			AddCommandToHelpText(LobbyChatCommand_Alert, ChatCommandLogic_AlertHelp_String, aHelp);
	}
	
	return aHelp;
}
