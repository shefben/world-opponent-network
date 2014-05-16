#ifndef __WON_InitLogic_H__
#define __WON_InitLogic_H__

#include "LobbyLogic.h"

#include "WONSocket/IPAddr.h"

namespace WONAPI
{
WON_PTR_FORWARD(AcceptOp);
WON_PTR_FORWARD(AsyncSocket);
WON_PTR_FORWARD(ChooseNetworkAdapterDialog);
WON_PTR_FORWARD(HTTPSession);
class ByteBuffer;
class GetHTTPDocumentOp;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class InitLogic : public LobbyLogic
{
protected:
	HTTPSessionPtr mHTTPSession;
	std::string mPromotionLink;
	AcceptOpPtr mAcceptOp;
	AsyncSocketPtr mAcceptSocket;
	IPAddr mFirewallDetectorAddr;

	// Network adapter dialog
	ChooseNetworkAdapterDialogPtr	mChooseNetworkAdapterDialog;

	enum InitErrorFlags
	{
		InitErrorFlag_NoInternetConnection		=	0x0001,
		InitErrorFlag_DNSLookupFailed			=	0x0002,
		InitErrorFlag_NoAccountServers			=	0x0004,
		InitErrorFlag_NoAuthServers				=	0x0008,
		InitErrorFlag_NoVersionServers			=	0x0010,
		InitErrorFlag_DirLookupFailed			=	0x0020,
		InitErrorFlag_VersionCheckFailure		=	0x0040
	};
	int mLastInitErrorFlags;
	int mIgnoreInitErrorFlags;

	bool SetLastInitError(int theFlags, const GUIString &theDesc); // returns true if the error isn't ignored

	void SetTOUTimes();

	void ProcessCrossPromotionDoc(const ByteBuffer* theMsg);
	void HandleTOU(GetHTTPDocumentOp *theOp);

	void DNSTestCompletion(AsyncOp *theOp);
	void GetHTTPDocCompletion(AsyncOp *theOp);
	void GetTitanServersCompletion(AsyncOp *theOp);
	void CheckVersionCompletion(AsyncOp *theOp);
	void DetectFirewallCompletion(AsyncOp *theOp);
	void GetCrossPromotionCompletion(AsyncOp *theOp);

	void AcceptCompletion(AsyncOp *theOp);

	void RunDNSTestOp();
	void RunHTTPOp();
	void RunTitanServersOp();
	void RunVersionOp();
	void RunDetectFirewallOp();

	void HandleChooseNetworkAdapter();

	void StartNetInit(bool isRetry);

	virtual void Init(ComponentConfig *theConfig);
	virtual void KillHook();
	virtual ~InitLogic();

public:
	InitLogic();
	void DoInit();
	void KillFirewallDetectOps();

	virtual void HandleControlEvent(int theId);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
};
typedef SmartPtr<InitLogic> InitLogicPtr;


} // namespace WONAPI

#endif