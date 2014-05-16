#ifndef __CREATEGAMEDIALOG_H__
#define __CREATEGAMEDIALOG_H__

#include "WONGUI/Dialog.h"

namespace WONAPI
{

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(ComboBox);
WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Label);
WON_PTR_FORWARD(LobbyGame);
enum LobbyGameType;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CreateGameDialog : public Dialog
{
protected:
	InputBoxPtr mGameNameInput;
	ComboBoxPtr mSkillCombo;
	InputBoxPtr mPasswordInput;
	LabelPtr mPasswordLabel;
	ButtonPtr mOpenToAllRadio;
	ButtonPtr mPasswordRadio;
	ButtonPtr mInviteOnlyRadio;
	ButtonPtr mAskToJoinRadio;
	ButtonPtr mCreateButton;
	LobbyGameType mGameType;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	void UpdateCreateButton();
	void UpdatePasswordInput();

	virtual ~CreateGameDialog();

public:
	CreateGameDialog();
	virtual void InitComponent(ComponentInit &theInit);

	void Reset(bool clearAll = false);
	void SetGameType(LobbyGameType theType);

	LobbyGamePtr GetGame();
};
typedef SmartPtr<CreateGameDialog> CreateGameDialogPtr;

}; // namespace WONAPI

#endif