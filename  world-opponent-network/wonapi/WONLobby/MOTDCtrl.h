#ifndef __WON_MOTDCTRL_H__
#define __WON_MOTDCTRL_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

class ByteBuffer;
typedef ConstSmartPtr<ByteBuffer> ByteBufferPtr;

WON_PTR_FORWARD(BrowserComponent);
class GetHTTPDocumentOp;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MOTDCtrl : public Container
{
protected:
	BrowserComponentPtr mBrowser;
	std::string mGameMOTDPath;
	std::string mSysMOTDPath;
	bool mNeedGenHTML;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void Paint(Graphics &g);

	virtual ~MOTDCtrl();

public:
	MOTDCtrl();
	virtual void InitComponent(ComponentInit &theInit);
	void ShowMessage(GetHTTPDocumentOp *theOp);
};
typedef SmartPtr<MOTDCtrl> MOTDCtrlPtr;

};

#endif
