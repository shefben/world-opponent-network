#ifndef __WON_LOBBYFRIEND_H__
#define __WON_LOBBYFRIEND_H__

#include "WONGUI/GUIString.h"
#include "WONSocket/IPAddr.h"
#include <map>

namespace WONAPI
{

class LobbyClient;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum FriendFlag
{
	FriendFlag_None = 0,
	FriendFlag_InChat = 0x1,
	FriendFlag_InGame = 0x2,
	FriendFlag_Searching = 0x4
};

class LobbyFriend: public RefCount
{
protected:
	GUIString	mName;
	unsigned long mWONUserId;

	IPAddr		mServerIP;
	GUIString	mServerName;

	// not persisted
	int				mOpsOutstanding;
	unsigned char	mFlags;
	time_t			mLastLocateTime;
	int				mReportLocationRoomSpec;
	bool			mAnonymous;

public:
	LobbyFriend(const GUIString& theName, unsigned long theWONUserId);

	const GUIString& GetName() { return mName; }
	unsigned long GetWONUserId() { return mWONUserId; }

	bool IsOnline() { return (InGame() || InChat()); }
	bool InGame() { return (mFlags & FriendFlag_InGame) ? true : false; }
	bool InChat() { return (mFlags & FriendFlag_InChat) ? true : false; }
	bool IsSearching() { return (mFlags & FriendFlag_Searching)?true : false; }
	bool IsAnonymous() { return mAnonymous; }

	void UpdateFlagsFromClient(LobbyClient *theClient);
	void NotifyOnServer(const IPAddr &theAddr, const GUIString &theName, int theFlags);
	void NotifyLeftServer();
	void SetAnonymous(bool isAnonymous);

	const IPAddr& GetServerIP() { return mServerIP; }
	const GUIString& GetServerName() { return mServerName; }

	time_t GetLastLocateTime() { return mLastLocateTime; }
	void SetLastLocateTime(time_t theTime) { mLastLocateTime = theTime; }
	void StartSearch(int theReportLocationRoomSpec = 0);
	void EndSearch(time_t theTime);
	void SetNumOps(int theNum) { mOpsOutstanding = theNum; }
	int DecrementOps() { return --mOpsOutstanding; }
	int GetNumOps() { return mOpsOutstanding; }
	bool ReportLocationOnFind() { return mReportLocationRoomSpec!=0; }
	int GetReportLocationRoomSpec() { return mReportLocationRoomSpec; }
};	
typedef SmartPtr<LobbyFriend> LobbyFriendPtr;
typedef std::map<unsigned long, LobbyFriendPtr> LobbyFriendMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
}; // namespace WONAPI


#endif