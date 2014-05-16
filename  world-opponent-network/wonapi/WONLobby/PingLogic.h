#ifndef __WON_PINGLOGIC_H__
#define __WON_PINGLOGIC_H__
#include "LobbyLogic.h"

#include <list>

namespace WONAPI
{

class IPAddr;
class ReadBuffer;

WON_PTR_FORWARD(LobbyGame);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PingLogic : public LobbyLogic
{
protected:
	typedef std::list<LobbyGamePtr> PingList;
	PingList mPingList;

	void PingCompletion(AsyncOp *theOp);
	void RecvUDPCompletion(AsyncOp *theOp);
	void HandlePingRequest(ReadBuffer &theMsg, const IPAddr &theReplyAddr);		  

	virtual ~PingLogic();

public:
	PingLogic();

	virtual void Init(ComponentConfig *theConfig);
	virtual void HandleControlEvent(int theControlId);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	void QueuePing(LobbyGame *theGame, bool getDetails, bool force = false);
	void KillPings(bool resetValues);
	int GetNumPingsToDo();
};
typedef SmartPtr<PingLogic> PingLogicPtr;



} // namespace WONAPI

#endif