#include "GUITypes.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DialogParams::DialogParams(Component *theDialog, int theFlags, int x, int y)
{
	mParent = NULL;
	mDlgWindow = NULL;
	mDialog = theDialog;
	mFlags = theFlags;
	mx = x;
	my = y;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PopupParams::PopupParams(Component *thePopup, int theFlags, Component *theOwnerComponent, int x, int y)
{
	mParent = NULL;
	mInputComponent = NULL;

	mPopup = thePopup;
	mFlags = theFlags;
	mOwnerComponent = theOwnerComponent;
	mx = x;
	my = y;
}
