#ifndef __WON_DialogLogic_H__
#define __WON_DialogLogic_H__

#include "LobbyLogic.h"

namespace WONAPI
{

WON_PTR_FORWARD(OkCancelDialog);
WON_PTR_FORWARD(StatusDialog);
WON_PTR_FORWARD(Window);

class Component;
class ComponentConfig;
class DirectConnectDialog;
class GUIString;
enum WONStatus;
enum CloseStatusDialogType;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DialogLogic : public LobbyLogic
{
protected:
	struct DialogInfo
	{
		WindowPtr mWindow;
		ComponentPtr mDialog;
		WONRectangle mBounds;
		bool mActive;

		DialogInfo() : mActive(false) { }
	};
	typedef std::list<DialogInfo> DialogStack;
	DialogStack mDialogStack;
	bool mPopupInOwnWindow;
	bool mUserCanceledLastStatusOp;

	bool CheckIsCurrentDialog(Component *theDialog, bool &needCenter);
	void CloseInactiveDialogs();
	WindowPtr GetDialogWindow(bool isCurrentDialog);

	OkCancelDialogPtr mOkCancelDialog;
	StatusDialogPtr mStatusDialog;
	bool mDoingStatusDialog;
	AsyncOpPtr mStatusOp;
	CloseStatusDialogType mStatusCloseOnFinish;

	void StatusCompletion(AsyncOp *theOp);

	virtual void KillHook();
	virtual ~DialogLogic();

	int DoDialogInSeperateWindow(Component *theComponent);
	int DoDialogInLobbyWindow(Component *theComponent);

public:
	DialogLogic();
	void Init(ComponentConfig *theConfig);
	virtual void TimerEvent();

	int DoDialog(Component *theComponent);
	void DoStatusOp(AsyncOp *theOp, DWORD theTimeout, const GUIString &theTitle, const GUIString &theMessage, CloseStatusDialogType closeOnFinish, DWORD theTime);
	void DoStatusDialog(const GUIString &theTitle, const GUIString &theStatus, bool isCancel);
	void DoErrorDialog(const GUIString &theTitle, const GUIString &theStatus);
	int DoOkCancelDialog(const GUIString &theTitle, const GUIString &theDetails, bool doWarningSound);
	int DoYesNoDialog(const GUIString& theTitle, const GUIString& theDetails, bool doWarningSound);

	void ShowStatus(const GUIString &theTitle, WONStatus theStatus);
	void FinishStatusDialog(WONStatus theStatus);

	bool GetUserCanceledLastStatusOp() { return mUserCanceledLastStatusOp; }
};
typedef SmartPtr<DialogLogic> DialogLogicPtr;


} // namespace WONAPI

#endif