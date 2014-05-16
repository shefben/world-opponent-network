#ifndef __WON_ROOMDIALOG_H__
#define __WON_ROOMDIALOG_H__

#include "WONGUI/Dialog.h"

namespace WONAPI
{

WON_PTR_FORWARD(RoomCtrl);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoomDialog : public Dialog
{
protected:
	RoomCtrlPtr mRoomCtrl;

	virtual ~RoomDialog();

public:
	RoomDialog();
	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<RoomDialog> RoomDialogPtr;

}; // namespace WONAPI

#endif