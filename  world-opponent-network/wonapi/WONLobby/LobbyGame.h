#ifndef __WON_LOBBYGAME_H__
#define __WON_LOBBYGAME_H__

#include "WONGUI/GUIString.h"
#include "WONSocket/IPAddr.h"

#include <map>

namespace WONAPI
{

class ByteBuffer;
typedef ConstSmartPtr<ByteBuffer> ByteBufferPtr;

class LobbyClient;
class LobbyGroup;
class LobbyPlayer;
class ReadBuffer;
class WriteBuffer;


WON_PTR_FORWARD(LobbyClientList);
enum LobbyGameType;
enum LobbySkillLevel;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyGame : public RefCount
{
protected:
	GUIString mName;
	GUIString mPassword;
	bool mHasPassword;
	bool mInviteOnly;
	bool mAskToJoin;
	bool mInProgress;
	bool mIAmInvited;
	bool mIAmCaptain;
	bool mDetailsVisible;
	bool mHasDetails;
	bool mHadSuccessfulPing;
	time_t mLastHeartbeat;
	bool mIsFiltered;

	int mNumPlayers;
	int mMaxPlayers;

	LobbyGameType mGameType;
	LobbySkillLevel mSkillLevel;
	LobbyClientListPtr mClientList;

	IPAddr mIPAddr;
	unsigned short mCaptainId;
	unsigned short mGroupId;
	unsigned short mPing;

	typedef LobbyGame*(*LobbyGameFactory)();
	static LobbyGameFactory mGameFactory;
	static LobbyGame* DefaultGameFactory() { return new LobbyGame; }

protected: // hooks
	virtual bool PreNetJoinGameHook() { return true; }
	virtual bool PostNetJoinGameHook() { return true; }
	virtual bool PreNetCreateGameHook() { return true; }
	virtual bool PostNetCreateGameHook() { return true; }
	virtual void PreNetLeaveGameHook() { }
	virtual void PostNetLeaveGameHook() { }
	virtual void NotifyStartGameHook() { }

	virtual bool TryStartGameHook() { return true; }
	virtual bool CanStartGameHook() { return true; }
	virtual bool CanReadyHook() { return true; }
	virtual short GetOkToJoinStatusHook();
	virtual void HandleHostGameHook(LobbyClient * /*myClient*/) { }
	virtual void GetJoinGameRequestHook(WriteBuffer &/*theMsg*/) { }	

	virtual short HandleJoinGameRequestHook(ReadBuffer &/*theMsg*/, LobbyClient * /*theClient*/) { return 0; }
	virtual void GetJoinGameReplyHook(LobbyClient * /*theClient*/, WriteBuffer &/*theReply*/) { }
	
	virtual void CopyFromHook(LobbyGame * /*theGame*/) { }
	virtual void HandleJoinGameReplyHook(ReadBuffer &/*theMsg*/) { }
	virtual void HandlePlayerLeftHook(LobbyClient * /*theClient*/, LobbyPlayer * /*thePlayer*/) { }
	virtual void HandlePlayerJoinedHook(LobbyClient * /*theClient*/) { }
	virtual void WriteSummaryHook(WriteBuffer &/*theMsg*/) { }
	virtual void WriteDetailsHook(WriteBuffer &/*theMsg*/) { }
	virtual bool ReadSummaryHook(ReadBuffer &/*theMsg*/) { return true; }
	virtual bool ReadDetailsHook(ReadBuffer &/*theMsg*/) { return true; }
	virtual void HandleGameMessageHook(unsigned char /*theMsgType*/, ReadBuffer &/*theMsg*/, LobbyClient * /*theSender*/) { }
	virtual GUIString GetStatusStringHook(short /*theStatus*/) { return GUIString::EMPTY_STR; }


protected:
	short HandleJoinGameRequest(ReadBuffer &theMsg, LobbyClient *theClient, WriteBuffer &theReply);

	virtual ~LobbyGame();
	LobbyGame();


public:
	bool CheckRoomSpecFlags(int theFlags);

	void InitFromLobbyGroup(LobbyGroup *theGroup);
	void CopyFrom(LobbyGame *theGame);

	bool PreNetJoinGame();
	bool PostNetJoinGame();
	bool PreNetCreateGame();
	bool PostNetCreateGame();
	void PreNetLeaveGame();
	void PostNetLeaveGame();
	void NotifyStartGame();
	ByteBufferPtr GetJoinGameRequest();
	ByteBufferPtr HandleJoinGameRequest(ReadBuffer &theMsg, LobbyClient *theClient);
	short HandleJoinGameReply(ReadBuffer &theMsg);
	void HandleHostGame(LobbyClient *myClient);
	void HandlePlayerLeft(LobbyClient *theClient);
	void HandlePlayerJoined(LobbyClient *theClient);
	void HandleGameMessage(unsigned char theMsgType, ReadBuffer &theMsg, LobbyClient *theSender);
	bool CanStartGame();
	bool CanReady();
	bool TryStartGame();
	bool IsEveryoneReady();

	void WriteSummary(WriteBuffer &theMsg);
	void WriteDetails(WriteBuffer &theMsg);
	bool ReadSummary(ReadBuffer &theMsg);
	bool ReadSummary(const ByteBuffer *theData);
	bool ReadDetails(ReadBuffer &theMsg);

	short GetOkToJoinStatus();
	bool IsOkToJoin();
	GUIString GetStatusString(short theStatus);

	
	bool IsSameGame(LobbyGame *theGame);

	void SetName(const GUIString &theName) { mName = theName; }
	void SetPassword(const GUIString &thePassword) { mPassword = thePassword; }
	void SetAskToJoin(bool isAskToJoin) { mAskToJoin = isAskToJoin; }
	void SetInviteOnly(bool isInviteOnly) { mInviteOnly = isInviteOnly; }
	void SetHasPassword(bool hasPassword) { mHasPassword = hasPassword; }
	void SetSkillLevel(LobbySkillLevel theSkillLevel) { mSkillLevel = theSkillLevel; }
	void SetIPAddr(const IPAddr &theAddr) { mIPAddr = theAddr; }
	void SetGroupId(unsigned short theId) { mGroupId = theId; }
	void SetGameType(LobbyGameType theType) { mGameType = theType; }
	void SetNumPlayers(int theNumPlayers) { mNumPlayers = theNumPlayers; }
	void SetMaxPlayers(int theMaxPlayers) { mMaxPlayers = theMaxPlayers; }
	void SetPing(unsigned short thePing) { mPing = thePing; }
	void SetInProgress(bool inProgress) { mInProgress = inProgress; }
	void SetIAmCaptain(bool amCaptain) { mIAmCaptain = amCaptain; }
	void SetIAmInvited(bool amInvited) { mIAmInvited = amInvited; }
	void SetCaptainId(unsigned short theId) { mCaptainId = theId; }
	void SetDetailsVisible(bool detailsVisible) { mDetailsVisible = detailsVisible; }
	void SetLastHeartbeat(time_t theTime) { mLastHeartbeat = theTime; }
	void SetHadSuccessfulPing(bool success) { mHadSuccessfulPing = success; }
	void SetIsFiltered(bool isFiltered) { mIsFiltered = isFiltered; }

	const GUIString& GetName() { return mName; }
	const GUIString& GetPassword() { return mPassword; }
	bool IsAskToJoin() { return mAskToJoin; }
	bool IsInviteOnly() { return mInviteOnly; }
	bool HasPassword() { return mHasPassword; }
	LobbySkillLevel GetSkillLevel() { return mSkillLevel; }
	const IPAddr& GetIPAddr() { return mIPAddr; }
	unsigned short GetGroupId() { return mGroupId; }
	LobbyGameType GetGameType() { return mGameType; }
	int GetNumPlayers() { return mNumPlayers; }
	int GetMaxPlayers() { return mMaxPlayers; }
	unsigned short GetPing() { return mPing; }
	bool InProgress() { return mInProgress; }
	bool IAmCaptain() { return mIAmCaptain; }
	bool IAmInvited() { return mIAmInvited || IAmCaptain(); }
	unsigned short GetCaptainId() { return mCaptainId; }
	bool IsPinging();
	bool IsPingingDetails();
	bool GetDetailsVisible() { return mDetailsVisible; }
	time_t GetLastHeartbeat() { return mLastHeartbeat; }
	bool GetHasDetails() { return mHasDetails; }
	bool GetHadSuccessfulPing() { return mHadSuccessfulPing; }
	bool IsGameFull();
	bool IsFiltered()  { return mIsFiltered; }

	void SetClientList(LobbyClientList *theList);
	LobbyClientList* GetClientList() { return mClientList; }
	LobbyClient* GetClient(unsigned short theId);
	void DeletePlayers();

	static LobbyGame* CreateGame(LobbyGameType theType);
	static void SetGameFactory(LobbyGameFactory theFactory) { mGameFactory = theFactory; }
};
typedef SmartPtr<LobbyGame> LobbyGamePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyGameList : public RefCount
{
protected:
	LobbyGameType mGameType;

	typedef std::map<unsigned short,LobbyGamePtr> IdGameMap;
	typedef std::map<IPAddr,LobbyGamePtr> AddrGameMap;

	IdGameMap mIdMap;
	AddrGameMap mAddrMap;

	IdGameMap::iterator mIdItr;
	AddrGameMap::iterator mAddrItr;

	virtual ~LobbyGameList();

public:
	LobbyGameList(LobbyGameType theType);

	void AddGame(LobbyGame *theGame);
	LobbyGame* AddGameById(unsigned short theId, bool *isModify = NULL); 
	LobbyGame* AddGameByAddr(const IPAddr &theAddr, bool *isModify = NULL); 

	LobbyGame* GetGameById(unsigned short theId);
	LobbyGame* GetGameByAddr(const IPAddr &theAddr);

	LobbyGamePtr RemoveGameById(unsigned short theId);
	LobbyGamePtr RemoveGameByAddr(const IPAddr &theAddr);

	LobbyGameType GetGameType() { return mGameType; }
	void Clear();

	// Iteration
	void Rewind();
	bool HasMoreGames();
	LobbyGame* NextGame(); 
	bool IsEmpty();

	bool PurgeGamesWithNoHeartbeat(int theNumSeconds); // returns whether any games were purged, only works for address based games
};
typedef SmartPtr<LobbyGameList> LobbyGameListPtr;


} // namespace WONAPI


#endif