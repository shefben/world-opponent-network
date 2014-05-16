#ifndef __WON_DIALOG_H__
#define __WON_DIALOG_H__

#include "Container.h"
#include "Button.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Dialog : public Container
{
private:
	int mDragX, mDragY;
	bool mDragging;

protected:
	bool mAllowDrag;
	bool mKeepInRoot;
	bool mEndOnEscape;
	int mEscapeId;

	ButtonPtr mDefaultButton;

public:
	virtual void SizeChanged();
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseDrag(int x, int y);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseExit();

	virtual bool KeyDown(int theKey);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual bool AllowWindowClose();

	bool ActivateDefaultButton();

	virtual void GotFocus();
	virtual void LostFocus();


public:
	Dialog();
	
	void SetAllowDrag(bool allowDrag) { mAllowDrag = allowDrag; }
	void SetKeepInRoot(bool keepInRoot) { mKeepInRoot = keepInRoot; }
	void SetEndOnEscape(int theEscapeId);
	void SetDontEndOnEscape();
	void SetDefaultButton(Button *theButton);

	bool GetIsDragging() { return mDragging; }
	void SetIsDragging(bool isDragging) { mDragging = isDragging; }
};

typedef WONAPI::SmartPtr<Dialog> DialogPtr;

}; // namespace WONAPI

#endif