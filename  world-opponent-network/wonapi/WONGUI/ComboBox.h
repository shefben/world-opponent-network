#ifndef __WON_COMBOBOX_H__
#define __WON_COMBOBOX_H__

#include "Container.h"
#include "InputBox.h"
#include "Button.h"
#include "ListBox.h"


namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComboSelChangedEvent : public ComponentEvent
{
public:
	TextListItemPtr mItem;
	bool mActiveSel; // user hit enter or clicked (as opposed to just using the arrow keys)

	ComboSelChangedEvent(Component *theCombo, TextListItem *theItem, bool activeSel) :
		ComponentEvent(theCombo, ComponentEvent_ComboSelChanged), mItem(theItem), mActiveSel(activeSel) { }

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComboBox : public Container
{
protected:
	InputBoxPtr mInputBox;
	ButtonPtr mButton;
	ScrollerPtr mListBox;
	ListAreaPtr mListArea;
	bool mListBoxVisible;
	bool mNoType;
	bool mShowButtonWhenDisabled;
	int mListBoxWidth;
	int mListBoxHeight;

	GUIString mUndoStr;

	void SetInputBoxToListSelection(bool activeSel = false, bool fireEvent = true);
	void ShowList();
	virtual ~ComboBox();

public:
	virtual void AddedToParent();
	virtual void SizeChanged();
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual	void MouseDown(int x, int y, MouseButton theButton);
	virtual void GotFocus();
	virtual void LostFocus();
	virtual bool KeyDown(int theKey);
	virtual bool MouseWheel(int theAmount);

public:
	ComboBox();

	InputBox* GetInputBox() { return mInputBox; }
	Button* GetButton() { return mButton; }
	Component* GetListBox() { return mListBox; }
	ListArea* GetListArea() { return mListArea; }

	int GetItemPos(ListItem *theItem) { return mListArea->GetItemPos(theItem); }
	TextListItem* GetItem(int thePos) { return (TextListItem*)mListArea->GetItem(thePos); }
	TextListItem* GetSelItem() { return (TextListItem*)mListArea->GetSelItem(); }
	int GetSelItemPos();
	int GetNumItems() { return mListArea->GetNumItems(); }

	ListItem* InsertString(const GUIString &theStr, int thePos = -1);
	void InsertItem(TextListItem *theItem, int thePos = -1);
	void UpdateItem(TextListItem *theItem);
	void DeleteItem(int thePos);
	void DeleteItem(ListItem *theItem) { mListArea->DeleteItem(theItem); }
	void SetSelItem(int theItemPos, bool fireEventOnChange = true);
	void Clear(bool clearInputAsWell = true);

	void SetText(const GUIString &theText, bool fireEvent = true);
	void SetTextAndSelItem(const GUIString &theText, bool fireEventOnChange = true);
	const GUIString& GetText() const { return mInputBox->GetText(); }

	void SetInputBox(InputBox *theInputBox) { mInputBox = theInputBox; }
	void SetButton(Button *theButton) { mButton = theButton; }
	void SetListArea(ListArea *theListArea); 
	void SetListBox(Scroller *theListBox);
	void SetListBoxDimensions(int theWidth, int theHeight);
	void SetListBoxWidth(int theWidth);
	void SetListBoxHeight(int theHeight);
	void CalcListBoxSize();
	void AddControls();
	void SetShowButtonWhenDisabled(bool show) { mShowButtonWhenDisabled = show; }

	void SetNoType(bool noType);
	virtual void Enable(bool isEnabled);
};
typedef WONAPI::SmartPtr<ComboBox> ComboBoxPtr;

}; // namespace WONAPI


#endif