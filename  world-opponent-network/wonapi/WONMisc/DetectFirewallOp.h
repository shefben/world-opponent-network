#ifndef __WON_DetectFirewallOP_H__
#define __WON_DetectFirewallOP_H__
#include "WONShared.h"
#include "WONServer/ServerRequestOp.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DetectFirewallOp : public ServerRequestOp
{
private:
	unsigned short mListenPort;
	unsigned long mMaxConnectWaitTime;
	BlockingSocketPtr mListenSocket;
	bool	mDoListen;
	bool	mWaitForConnect;
	bool	mUseUDP;
	RecvBytesFromOpPtr	mRecvBytesFromOp;
	IPAddr	mPublicAddr;

	void Init();

protected:
	virtual WONStatus GetNextRequest();
	virtual WONStatus CheckResponse();
	virtual void CleanupHook();
	virtual void RunHook();
	virtual bool CallbackHook(AsyncOp *theOp, int theParam);
	virtual WONStatus SetupReceiveTracker();

	enum DetectFirewallTrack
	{
		DetectFirewall_Track_RecvFrom = 1000
	};

public:
	DetectFirewallOp(ServerContext *theFirewallDetectorContext);
	DetectFirewallOp(const IPAddr &theAddr);

	void SetDoUDPDetect(bool useUDP)	{ mUseUDP = useUDP; }
	void SetListenPort(unsigned short thePort) { mListenPort = thePort;  }
	void SetBindPort  (unsigned short thePort) { SetListenPort(thePort); }
	void SetMaxConnectWaitTime(unsigned long theNumSeconds) { mMaxConnectWaitTime = theNumSeconds; }
	void SetDoListen(bool doListen) { mDoListen = doListen; }
	void SetWaitForConnect(bool waitForConnect) { mWaitForConnect = waitForConnect; }

	void CloseSocketNow(bool kill = true); // should only call from the API thread if the op is running

	IPAddr& GetPublicAddr() { return mPublicAddr; }
};

typedef SmartPtr<DetectFirewallOp> DetectFirewallOpPtr;

}; // namespace WONAPI

#endif
