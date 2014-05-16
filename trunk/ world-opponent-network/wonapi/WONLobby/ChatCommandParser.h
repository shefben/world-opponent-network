#ifndef __WON_ChatCommandParser_H__
#define __WON_ChatCommandParser_H__

#include "LobbyTypes.h"

#include "WONCommon/SmartPtr.h"
#include "WONCommon/StringUtil.h"
#include "WONGUI/GUIString.h"

#include <map>
#include <set>

namespace WONAPI
{

WON_PTR_FORWARD(LobbyClientList);
WON_PTR_FORWARD(LobbyClient);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ChatCommandParser : public RefCount
{
public:
	enum MatchResult
	{
		MatchResult_NotFound = 0,
		MatchResult_Ambiguous,
		MatchResult_Partial,
		MatchResult_Exact
	};

	enum InputError
	{
		InputError_None,
		InputError_InvalidCommand,
		InputError_RequireName,
		InputError_ClientNotFound, 
		InputError_ClientAmbiguous, 
		InputError_NoChat,
		InputError_NotLoggedIn,
		InputError_BadTime,
	};

protected:
	LobbyClientListPtr mClientList;
	GUIString mLastWhisperClientName;

	typedef std::map<std::wstring, LobbyChatCommand, WStringLessNoCase> CommandStringMap;
	typedef std::map<LobbyChatCommand, std::wstring> PreferredCommandStringMap;

	CommandStringMap mCommandStringMap;
	PreferredCommandStringMap mPreferredCommandStringMap;

	MatchResult mMatchTolerance;
	bool mServerWideCommands;
	bool mDoColonEmote;

	InputError mLastInputError;
	LobbyChatCommand mLastChatCommand;
	LobbyClientPtr mLastTargetClient;
	GUIString mLastMatchName;
	GUIString mLastChat;
	bool mLastBool;
	DWORD mLastInputDuration;
	int mCommandChar;
	int mRoomSpecFlags;

	static bool Compare(const GUIString &s1, const GUIString &s2, bool &partialMatch);
	static void SkipWhitespace(const GUIString &s1, int &thePos);
	static GUIString GetWord(const GUIString &s1, int &thePos, bool doToUpper, bool checkQuotes);
	static GUIString GetRestOfLine(const GUIString &s1, int thePos);
	void SetIgnore(LobbyClient *theClient, bool on);
	void AddCommandToHelpText(LobbyChatCommand theCommand, const GUIString &theDesc, GUIString &theHelpText);

	const char* GetDefaultCommandString(LobbyChatCommand theCommand); // backup in case string resource is missing
	void AddMultiCommand(const std::wstring &theStr, LobbyChatCommand theCommand);

	virtual ~ChatCommandParser();

public:
	ChatCommandParser();
	void AddCommands();
	int GetCommandChar() { return mCommandChar; }
	void SetRoomContext(LobbyClientList *theList, int theSpecFlags);
	void SetLastWhisperClientName(const GUIString &theName) { mLastWhisperClientName = theName; }

	LobbyClient* GetClientByName(const GUIString &theName, MatchResult *theResult = NULL);
	LobbyClientList* GetClientList() { return mClientList; }

	void AddCommandString(const GUIString &theStr, LobbyChatCommand theCommand); // Associate command strings with commands (may have multiple strings for each command)
	void ClearCommandStrings();
	GUIString GetStringForCommand(LobbyChatCommand theCommand);
	void SetAllowPartialNameMatch(bool allow); // allow matching prefix of name (e.g. "/w Brian Hello" will whisper to Brian124 if there's no other user whose name starts with Brian.) 
	void SetServerWideCommands(bool serverWideCommandsOn); // commands such as whisper and ignore search the entire client list instead of just the members of the group specified in HandleInput
	void SetDoColonEmote(bool doColonEmote); // send emote when user starts chat with a ':'

	// Functions used for handling chat input
	InputError GetLastInputError() { return mLastInputError; }
	LobbyChatCommand GetLastChatCommand() { return mLastChatCommand; }
	LobbyClient* GetLastTargetClient() { return mLastTargetClient; }
	const GUIString& GetLastMatchName() { return mLastMatchName; }
	DWORD GetLastInputDuration() { return mLastInputDuration; }
	const GUIString& GetLastChat() { return mLastChat; }
	bool GetLastBool() { return mLastBool; }

	bool HandleCommand(const GUIString &theInput);
	bool HandleInput(const GUIString &theInput);

	GUIString GetHelpText(unsigned short myClientId);
	int GetRoomSpecFlags() { return mRoomSpecFlags; }
};
typedef SmartPtr<ChatCommandParser> ChatCommandParserPtr;

}; // namespace WONAPI

#endif