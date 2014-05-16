

#include "GameSpySupport.h"

  
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GameSpySupport::GameSpySupport(const char* aGameName, const char* aGameVersion, const char* aSecretKey)
	:	mIP(""), mSecretKey(aSecretKey), mUserParam(NULL), mInitialized(FALSE), 
		mStatus(WS_None), mAutoPumpTime(0),	mGameSpyStatus(0)
{
	mInfoCallback	 = NULL;
	mRulesCallback	 = NULL;
	mPlayersCallback = NULL;

	SetBasicKeyValue("gamename", aGameName);
	SetBasicKeyValue("gamever" , aGameVersion);
}

GameSpySupport::~GameSpySupport()
{ }


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus GameSpySupport::Startup(const IPAddr& anIPAddr, void* aUserParamP)
{
	// Store the server address
	mIP = anIPAddr;

	// Store the user param 
	mUserParam = aUserParamP;

	// Initialize the gamespy sdk
	mGameSpyStatus = qr_init(&mQRInstance, mIP.GetHostString().c_str(), mIP.GetPort(), 
						GetBasicKeyValue("gamename"), mSecretKey.c_str(), 
						HandleQueryBasic, HandleQueryInfo, HandleQueryRules, HandleQueryPlayers, this);

	// Success
	if (0 == mGameSpyStatus)
	{
		mInitialized = TRUE;
		mStatus = WS_Success;

		// Should we autopump?
		if (IsAutoPumping())
			BeginAutoPump(mAutoPumpTime);
	}
	else
	{
		// Handle error
		switch(mGameSpyStatus)
		{
		case E_GOA_WSOCKERROR:	//1
			mStatus = WS_GameSpySupport_WinSockError;
		case E_GOA_BINDERROR:	//2
			mStatus = WS_GameSpySupport_BindError;
		case E_GOA_DNSERROR:	//3
			mStatus = WS_GameSpySupport_DNSError;
		case E_GOA_CONNERROR:	//4
			mStatus = WS_GameSpySupport_ConnError;
		default:	// Unknown error, maybe should create new status WS_Unhandled
			mStatus = WS_GameSpySupport_UnhandledError;
		}
	} 

	return mStatus;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::Shutdown()
{
	// Kill the timer op
	SetAutoPumpTime(0);				// stop future pump timers

	if (mPumpTimerOp.get() != NULL)
	{
		mPumpTimerOp->Kill();
		mPumpTimerOp = NULL;
	}

	// "exiting" causes gamespy to deregister
	SetGameMode("exiting");

	// Do a final pump to send state change to gamespy
	Pump();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::HandleQueryBasic(char *outbuf, int maxlen, void *pThat)
{
	GameSpySupport* pSupport = (GameSpySupport*)pThat;

	// Load all keys from mBasicQueryMap into the buffer
	pSupport->DumpKeyValueMapToBuffer(pSupport->mBasicQueryMap, outbuf, maxlen);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::HandleQueryInfo(char *outbuf, int maxlen, void *pThat)
{
	GameSpySupport* pSupport = (GameSpySupport*)pThat;

	// Load all keys from mBasicQueryMap into the buffer
	pSupport->DumpKeyValueMapToBuffer(pSupport->mInfoQueryMap, outbuf, maxlen);

	// Call the users installed callback
	if (pSupport->mInfoCallback != NULL)
		(pSupport->mInfoCallback)(outbuf, maxlen, pSupport->mUserParam);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::HandleQueryRules(char *outbuf, int maxlen, void *pThat)
{
	GameSpySupport* pSupport = (GameSpySupport*)pThat;

	// Load all keys from mBasicQueryMap into the buffer
	pSupport->DumpKeyValueMapToBuffer(pSupport->mRulesQueryMap, outbuf, maxlen);

	// Call the users installed callback
	if (pSupport->mRulesCallback != NULL)
		(pSupport->mRulesCallback)(outbuf, maxlen, pSupport->mUserParam);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::HandleQueryPlayers(char *outbuf, int maxlen, void *pThat)
{
	GameSpySupport* pSupport = (GameSpySupport*)pThat;

	// Load all keys from mBasicQueryMap into the buffer
	pSupport->DumpKeyValueMapToBuffer(pSupport->mPlayersQueryMap, outbuf, maxlen);

	// Call the users installed callback
	if (pSupport->mPlayersCallback != NULL)
		(pSupport->mPlayersCallback)(outbuf, maxlen, pSupport->mUserParam);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Sends all info
/*
void GameSpySupport::HandleQueryStatus(char *outbuf, int maxlen, void *pThat)
{
	GameSpySupport* pSupport = (GameSpySupport*)pThat;

	// Add extra pre-callback functionality here
	// ...

	// Call the users installed callback
	if (pSupport->mStatusCallback != NULL)
		(pSupport->mStatusCallback)(outbuf, maxlen, pSupport->mUserParam);
}
*/


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::BeginAutoPump(unsigned long theMilliseconds)
{
	SetAutoPumpTime(theMilliseconds);

	if (mPumpTimerOp.get() != NULL)
	{
		mPumpTimerOp->Kill();
		mPumpTimerOp = NULL;
	}

	if (mAutoPumpTime != 0)
	{
		mPumpTimerOp = new AsyncOp();
		mPumpTimerOp->SetCompletion(new ParamCompletion<AsyncOpPtr, GameSpySupport*>(PumpTimerCompletion,this));
		mPumpTimerOp->RunAsTimer(mAutoPumpTime);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::PumpTimerCompletion(AsyncOpPtr theOp, GameSpySupport* pThat)
{
	if (theOp.get()	== NULL)
		return;

	if (theOp->GetStatus() == WS_Killed)
		return;

	pThat->Pump();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::Pump()
{
	// process queries
	qr_process_queries(mQRInstance);

	// Is a pump timer needed?
	if (IsAutoPumping())
		mPumpTimerOp->RunAsTimer(mAutoPumpTime);

	// Is a state changed heartbeat needed?
	if (mStateChanged)
	{
		qr_send_statechanged(mQRInstance);
		mStateChanged = FALSE;
	}
}
		
		
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::SetGameMode(const char* aGameMode)
{
	SetInfoKeyValue("gamemode", aGameMode);

	// Signal for a state-change heartbeat
	mStateChanged = TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::SetBasicKeyValue  (std::string theKey, std::string theValue)
{
	mBasicQueryMap[theKey] = theValue;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::SetInfoKeyValue   (std::string theKey, std::string theValue)
{
	mInfoQueryMap[theKey] = theValue;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::SetRulesKeyValue  (std::string theKey, std::string theValue)
{
	mRulesQueryMap[theKey] = theValue;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::SetPlayersKeyValue(std::string theKey, std::string theValue)
{
	mPlayersQueryMap[theKey] = theValue;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char* GameSpySupport::GetBasicKeyValue  (const char* theKey)
{
	KeyValueMap::iterator anItr = mBasicQueryMap.find(theKey);
	if (anItr != mBasicQueryMap.end())
		return anItr->second.c_str();
	else
		return "";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char* GameSpySupport::GetInfoKeyValue   (const char* theKey)
{
	KeyValueMap::iterator anItr = mBasicQueryMap.find(theKey);
	if (anItr != mBasicQueryMap.end())
		return anItr->second.c_str();
	else
		return "";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char* GameSpySupport::GetRulesKeyValue  (const char* theKey)
{
	KeyValueMap::iterator anItr = mBasicQueryMap.find(theKey);
	if (anItr != mBasicQueryMap.end())
		return anItr->second.c_str();
	else
		return "";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char* GameSpySupport::GetPlayersKeyValue(const char* theKey)
{
	KeyValueMap::iterator anItr = mBasicQueryMap.find(theKey);
	if (anItr != mBasicQueryMap.end())
		return anItr->second.c_str();
	else
		return "";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameSpySupport::DumpKeyValueMapToBuffer(KeyValueMap& theMap, char* outbuf, unsigned int maxlen)
{
	unsigned int pos = 0;
	unsigned int pairlength = 0;

	KeyValueMap::iterator anItr = theMap.begin();
	for (; anItr != theMap.end(); ++anItr)
	{
		pairlength = anItr->first.length() + anItr->second.length() + 2; // add 2 for the seperating '\'
		
		if (pairlength < (maxlen-pos))
			sprintf(outbuf+pos, "\\%s\\%s", anItr->first.c_str(), anItr->second.c_str());

		pos += pairlength;
	}
}
