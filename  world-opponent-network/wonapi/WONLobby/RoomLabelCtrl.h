#ifndef __RoomLabelCtrl_H__
#define __RoomLabelCtrl_H__

#include "WONGUI/WONGUIConfig/ContainerConfig.h"

namespace WONAPI
{

WON_PTR_FORWARD(Label);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoomLabelCtrl : public Container
{
protected:
	int mRoomSpecFlags;
	LabelPtr mLabel;
	GUIString mPrefix;
	GUIString mServer;
	GUIString mRoom;
	int mUserCt;
	bool mShowServer;

	void CalcLabel();

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);

public:
	RoomLabelCtrl();

	void SetPrefix(const GUIString &thePrefix) { mPrefix = thePrefix; }
	void SetRoomSpecFlags(int theFlags) { mRoomSpecFlags = theFlags; }
	void SetShowUserCt(bool flag) {if (! flag) mUserCt = -1; else if (mUserCt < 0) mUserCt = 0; }
	void SetShowServer(bool flag) { mShowServer = flag; }
};
typedef SmartPtr<RoomLabelCtrl> RoomLabelCtrlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoomLabelCtrlConfig : public ContainerConfig
{
protected:
	RoomLabelCtrl* mRoomLabel;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { mRoomLabel = (RoomLabelCtrl*)theComponent; ContainerConfig::SetComponent(theComponent); }

	static Component* CompFactory() { return new RoomLabelCtrl; }
	static ComponentConfig* CfgFactory() { return new RoomLabelCtrlConfig; }
};


};

#endif