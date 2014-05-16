#ifndef __WON_ADMINACTIONDIALOG_H__
#define __WON_ADMINACTIONDIALOG_H__

#include "WONGUI/Dialog.h"

namespace WONAPI
{

class LobbyClient;

enum LobbyClientAction;

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(ComboBox);
WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Label);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AdminActionDialog : public Dialog
{
protected:
	LobbyClientAction	mActionType;
	LabelPtr			mTitleLabel;

	LabelPtr	mExplanationHeader;	// "Explanation (Leave blank for silent action)"
	InputBoxPtr mExplanation;
	bool		mIsReasonOptional;

	

	LabelPtr mDurationHeader;			// "Duration"
	ComboBoxPtr mDurationCombo;			// 1,5,10,30,Permanent, use can enter own values
	ComboBoxPtr mTimeSegmentCombo;		// Minute,Hour,Day

	ButtonPtr mDoActionButton;			// caption = action type "Mute", "Ban", "Warn"
	ButtonPtr mCancelButton;			// caption = cancel

	ButtonPtr	mApplyToRoom;
	ButtonPtr	mApplyToServer;


	// Operations
protected:
	virtual void	InitComponent(ComponentInit &theInit);
	void			UpdateDoActionButton();

public:

	void Reset(LobbyClientAction theAction, LobbyClient *theTargetClient);
	unsigned long GetNumSeconds();
	bool GetApplyToServer();
	const GUIString& GetExplanation();


	// Overrides
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	// Constructor
public:
	AdminActionDialog();
	~AdminActionDialog();
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef SmartPtr<AdminActionDialog> AdminActionDialogPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
}; // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif __ADMINACTIONDIALOG_H__