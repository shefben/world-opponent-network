#ifndef __WON_LOBBYPLAYER_H__
#define __WON_LOBBYPLAYER_H__

#include "WONCommon/SmartPtr.h"

namespace WONAPI
{

class ReadBuffer;
class WriteBuffer;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyPlayer : public RefCount
{
protected:
	unsigned short mPing;
	bool mReady;

	typedef LobbyPlayer*(*LobbyPlayerFactory)();
	static LobbyPlayerFactory mPlayerFactory;
	static LobbyPlayer* DefaultPlayerFactory() { return new LobbyPlayer; }

	virtual bool ReadDataHook(ReadBuffer &/*theBuf*/) { return true; }
	virtual void WriteDataHook(WriteBuffer &/*theBuf*/) { }
	virtual bool SameTeamHook(LobbyPlayer * /*thePlayer*/) { return false; }

	LobbyPlayer();
	virtual ~LobbyPlayer();


public:
	unsigned short GetPing() { return mPing; }
	bool IsReady() { return mReady; }

	void SetPing(unsigned short thePing) { mPing = thePing; }
	void SetReady(bool isReady) { mReady = isReady; }

	bool SameTeam(LobbyPlayer *thePlayer);
	bool ReadData(ReadBuffer &theBuf);
	void WriteData(WriteBuffer &theBuf);

	static LobbyPlayer* CreatePlayer();
	static void SetPlayerFactory(LobbyPlayerFactory theFactory) { mPlayerFactory = theFactory; }
};
typedef SmartPtr<LobbyPlayer> LobbyPlayerPtr;


} // namespace WONAPI


#endif