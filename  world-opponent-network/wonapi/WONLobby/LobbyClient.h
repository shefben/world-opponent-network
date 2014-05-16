#ifndef __WON_LOBBYCLIENT_H__
#define __WON_LOBBYCLIENT_H__

#include "WONGUI/GUIString.h"

#include <map>

namespace WONAPI
{
class LobbyPlayer;
struct RoutingClientInfo;
typedef SmartPtr<RoutingClientInfo> RoutingClientInfoPtr;
typedef std::map<unsigned short, RoutingClientInfoPtr> RoutingClientMap;

WON_PTR_FORWARD(LobbyPlayer);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyClient : public RefCount
{
protected:
	GUIString mName;
	unsigned short mClientId;
	unsigned long mClientFlags;
	unsigned long mWONUserId;
	unsigned long mCDKeyId;

	unsigned char mChatFlags;
	unsigned char mGameFlags;

	unsigned short mChatGroupId;
	unsigned short mGameGroupId;

	bool mIgnored;
	bool mBlocked;
	bool mIsFriend;
	bool mIsGameCaptain;
	bool mIsChatCaptain;
	bool mIsInvited;

	LobbyPlayerPtr mPlayer;

	virtual ~LobbyClient();

public:
	LobbyClient();

	void Init(const RoutingClientInfo *theClient);

	const GUIString& GetName() { return mName; }
	unsigned short GetClientId() { return mClientId; }
	unsigned long GetWONUserId() { return mWONUserId; }
	unsigned long GetCDKeyId() { return mCDKeyId; }

	void SetClientFlags(unsigned long theFlags) { mClientFlags = theFlags; }
	void SetMemberFlags(unsigned char theFlags, bool isGame) { isGame?mGameFlags=theFlags:mChatFlags=theFlags; }
	void SetGroupId(unsigned short theId, bool isGame) { isGame?mGameGroupId=theId:mChatGroupId=theId; }
	void SetIsCaptain(bool isCaptain, bool isGame);

	void ClearGroupInfo(unsigned short theId, bool isGame);

	void CheckPersistentData(); // ignored, blocked

	bool IsIgnored() { return mIgnored; }
	bool IsBlocked() { return mBlocked; }
	bool IsFriend();
	bool IsAnonymous();
	bool IsInvited() { return mIsInvited; }
	bool IsMuted(bool checkGame);
	bool IsAway();
	bool IsAdmin();
	bool IsModerator();
	bool IsCaptain(bool checkGame);
	bool IsGameCaptain() { return IsCaptain(true); }

	void SetPlayer(LobbyPlayer *thePlayer);
	LobbyPlayer* GetPlayer() { return mPlayer; }
	bool IsPlayer();
	bool IsPlayerReady();
	unsigned short GetPlayerPing();
	bool SameTeam(LobbyClient *theClient);

	bool InChat();
	bool InGame();
	unsigned short GetChatGroupId() { return mChatGroupId; }
	unsigned short GetGameGroupId() { return mGameGroupId; }

	void SetIsIgnored(bool isIgnored) { mIgnored = isIgnored; }
	void SetIsBlocked(bool isBlocked) { mBlocked = isBlocked; }
	void SetIsFriend(bool isFriend) { mIsFriend = isFriend; }
	void SetIsInvited(bool isInvited) { mIsInvited = isInvited; }
};
typedef SmartPtr<LobbyClient> LobbyClientPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef std::map<unsigned short, LobbyClientPtr> LobbyClientMap;

class LobbyClientList : public RefCount
{
protected:
	LobbyClientMap mClientMap;

	virtual ~LobbyClientList();

public:
	LobbyClientList();

	void UpdateClients(const RoutingClientMap &theMap);
	void CheckPersistentData();

	void AddClient(LobbyClient *theClient);
	LobbyClient* GetClient(unsigned short theId);
	LobbyClient* GetClientByWONId(unsigned long theWONId);
	LobbyClientPtr RemoveClient(unsigned short theId);
	void Clear();
	void ClearInvites();
	
	const LobbyClientMap& GetClientMap() { return mClientMap; }
	int GetNumClients() { return mClientMap.size(); }
};
typedef SmartPtr<LobbyClientList> LobbyClientListPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyClientListItr
{
private:
	const LobbyClientMap &mMap;
	LobbyClientMap::const_iterator mItr;

	LobbyClientListItr(const LobbyClientListItr&);
	void operator=(const LobbyClientListItr&);

public:
	LobbyClientListItr(LobbyClientList *theList);

	bool HasMoreClients();
	LobbyClient* GetNextClient();
	void Rewind();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// utility class for iterating over players in the ClientList 
class LobbyPlayerList
{
private:
	const LobbyClientMap &mMap;
	LobbyClientMap::const_iterator mItr;
	bool mFoundNextPlayer;

	LobbyPlayerList(const LobbyPlayerList&);
	void operator=(const LobbyPlayerList&);

	bool FindNextPlayer();

public:
	LobbyPlayerList(LobbyClientList *theList);

	bool HasMorePlayers();
	LobbyPlayer* GetNextPlayer(LobbyClient **theClient = NULL);
	void Rewind();
};


} // namespace WONAPI

#endif