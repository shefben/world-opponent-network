#ifndef __WON_TOUDialog_H__
#define __WON_TOUDialog_H__

#include "WONGUI/Dialog.h"

namespace WONAPI
{

class ByteBuffer;
typedef ConstSmartPtr<ByteBuffer> ByteBufferPtr;

WON_PTR_FORWARD(BrowserComponent);
WON_PTR_FORWARD(Button);
class GotHTTPDocumentEvent;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TOUDialog : public Dialog
{
protected:
	BrowserComponentPtr mBrowser;
	ButtonPtr mAgreeCheck;
	ButtonPtr mAcceptButton;
	ButtonPtr mRejectButton;
	bool mNeedGenHTML;

	void HandleHTTPDocEvent(GotHTTPDocumentEvent *theEvent);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void Paint(Graphics &g);

public:
	TOUDialog();
	virtual void InitComponent(ComponentInit &theInit);

	void Reset();
};
typedef SmartPtr<TOUDialog> TOUDialogPtr;

}; // namespace WONAPI

#endif