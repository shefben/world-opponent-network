#ifndef __WON_GAMESPYSUPPORT_H__
#define __WON_GAMESPYSUPPORT_H__
#include "WONShared.h"

#include "WONCommon/SmartPtr.h"
#include "WONSocket/IPAddr.h"
#include "WONCommon/AsyncOp.h"

// Gamespy includes
#include "gqueryreporting.h"


namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GameSpySupport: public RefCount
{
protected:
	qr_t	mQRInstance;					// out parameter from qr_init (identifies instance)

	// Server information
	WONAPI::IPAddr	mIP;					// qr_init

	typedef std::map<std::string, std::string>	KeyValueMap;
	KeyValueMap		mBasicQueryMap;			// basic   query keys
	KeyValueMap		mInfoQueryMap;			// info    query keys
	KeyValueMap		mRulesQueryMap;			// rules   query keys
	KeyValueMap		mPlayersQueryMap;		// players query keys

	std::string		mSecretKey;				// (Gamespy) Secret key for validation
	BOOL			mStateChanged;			// has the mode changed since the last pump?
	void*			mUserParam;				// Passed through to all callbacks
	

	BOOL			mInitialized;			// Has qr_init been called?

	WONAPI::WONStatus	mStatus;			// WONStatus form of last error
	int					mGameSpyStatus;		// Gamespy value for last error
	unsigned long		mAutoPumpTime;		// Timeout for auto pump timer

	// Gamespy Callbacks
	typedef void(*GameSpyCallback)(char*,int,void*);
	GameSpyCallback		mInfoCallback;
	GameSpyCallback		mRulesCallback;
	GameSpyCallback		mPlayersCallback;

	WONAPI::AsyncOpPtr	mPumpTimerOp;

	//GameSpyCallback		mStatusCallback;
public:
	GameSpySupport(const char* aGameName, const char* aGameVersion, const char* aSecretKey);

	// Calls qr_init
	WONStatus Startup(const IPAddr& ServAddr, void* userparam);
	void	  Shutdown();	// sends status update with "gamemode" as "exiting"

	// Error codes
	WONAPI::WONStatus	GetStatus()			{ return mStatus;		 }
	int					GetGameSpyStatus()  { return mGameSpyStatus; }

	// Pump & Auto pump (calls process_queries)
	void	Pump();
	void	BeginAutoPump  (unsigned long theMilliseconds);
	void	SetAutoPumpTime(unsigned long theMilliseconds) { mAutoPumpTime = theMilliseconds; }
	BOOL	IsAutoPumping()	{ return ( mInitialized == TRUE && mAutoPumpTime != 0); }

	static void PumpTimerCompletion(AsyncOpPtr, GameSpySupport*); // Auto pumps completion signal

	// Key Value accessors
	void SetBasicKeyValue  (std::string theKey, std::string theValue);
	void SetInfoKeyValue   (std::string theKey, std::string theValue);
	void SetRulesKeyValue  (std::string theKey, std::string theValue);
	void SetPlayersKeyValue(std::string theKey, std::string theValue);

	const char* GetBasicKeyValue  (const char* theKey);
	const char* GetInfoKeyValue   (const char* theKey);
	const char* GetRulesKeyValue  (const char* theKey);
	const char* GetPlayersKeyValue(const char* theKey);

	void DumpKeyValueMapToBuffer(KeyValueMap& theMap, char* outbuf, unsigned int maxlen);

	// Set callback functions
	void InstallInfoQueryCallback   (const GameSpyCallback aCallback)	{ mInfoCallback    = aCallback; }
	void InstallRulesQueryCallback  (const GameSpyCallback aCallback)	{ mRulesCallback   = aCallback; }
	void InstallPlayersQueryCallback(const GameSpyCallback aCallback)	{ mPlayersCallback = aCallback; }

	// Default Query Handlers
	static void HandleQueryBasic  (char*,int,void*);	// does not call a user callback
	static void HandleQueryInfo   (char*,int,void*);
	static void HandleQueryRules  (char*,int,void*);
	static void HandleQueryPlayers(char*,int,void*);
	//static void HandleQueryStatus(char*,int,void*); // just calls all other queries

public:
	// Set the game state
	void	SetGameMode(const char* aGameMode);


protected:
	~GameSpySupport();

};

typedef SmartPtr<GameSpySupport>	GameSpySupportPtr;
}; // namespace WONAPI


#endif // __WONGAMESPYSUPPORT_H__
