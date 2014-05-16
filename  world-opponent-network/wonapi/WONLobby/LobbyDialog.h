#ifndef __WON_LOBBYDIALOG_H__
#define __WON_LOBBYDIALOG_H__

#include "WONCommon/Platform.h"

#include "LobbyTypes.h"

namespace WONAPI
{

class AsyncOp;
class Component;
class GUIString;

enum CloseStatusDialogType;
enum WONStatus;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyDialog
{
public:
	static void DoPopup(Component *theComponent, int thePopupFlags, int x = -1, int y = -1);
	static int DoDialog(Component *theComponent);
	static void DoStatusOp(AsyncOp *theOp, const GUIString &theTitle, const GUIString &theMessage, CloseStatusDialogType closeOnFinish = CloseStatusDialogType_None, DWORD theTime = 0); // theTime = 0 -> Use Default
	static void DoStatusDialog(const GUIString &theTitle, const GUIString &theStatus, bool isCancel);
	static void DoErrorDialog(const GUIString &theTitle, const GUIString &theStatus);
	static void ShowStatus(const GUIString &theTitle, WONStatus theStatus);
	static void FinishStatusDialog(WONStatus theStatus);
	static bool UserCanceledLastStatusOp();

	static int DoOkCancelDialog(const GUIString &theTitle, const GUIString &theDetails, bool doWarningSound = true);
	static int DoYesNoDialog(const GUIString &theTitle, const GUIString &theDetails, bool doWarningSound = true);
};


} // namespace WONAPI

#endif