#ifndef __WON_ROOTCONTAINER_H__
#define __WON_ROOTCONTAINER_H__

#include "Container.h"
#include "Rectangle.h"
#include "KeyboardListener.h"
#include <set>

namespace WONAPI
{

class Window;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class RootContainer : public Container
{
protected:
	WONRectangle mInvalidRect;
	Component *mModalComponent;
	Window *mWindow;
	Component *mCursorComponent;

	int mDragX, mDragY;
	bool mDragging;
	bool mAllowDrag;

	KeyboardListenerBasePtr mKeyboardListener;
	
	typedef std::set<Component*> ComponentSet;
	ComponentSet mPopupSet;

	struct BGStruct
	{
		int mImageX, mImageY;
		WONRectangle mClipRect;
		NativeImagePtr mImage;

		BGStruct() : mImageX(0), mImageY(0) { }
		BGStruct(int x, int y, NativeImage *theImage, const WONRectangle &theClipRect) : mImageX(x), mImageY(y), mImage(theImage), mClipRect(theClipRect) { }
	};
	typedef std::map<int,BGStruct> BGStructMap;
	BGStructMap mBGStructMap;


public:
	RootContainer(Window *theWindow);

	virtual Component* FindChildAt(int x, int y);
	virtual void PrePaint(Graphics &g);
	virtual void Paint(Graphics &g);
	virtual void InvalidateRect(WONRectangle &theRect);
	virtual void SizeChanged();
	virtual bool InvalidateUp(Component *theChild);

	void SetModalComponent(Component *theComponent);
	Component* GetModalComponent() { return mModalComponent; }

	void AddPopup(Component *theComponent);
	void RemovePopup(Component *theComponent);

	void Reset();

	void SetKeyboardListener(KeyboardListenerBase *theListener) { mKeyboardListener = theListener; }
	virtual bool KeyDown(int theKey);
	virtual bool KeyUp(int theKey);
	virtual bool KeyChar(int theKey);
	virtual void MouseMove(int x, int y);
	virtual void MouseDrag(int x, int y);
	virtual void MouseEnter(int x, int y);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseExit();
	virtual void RemoveChild(Component *theChild);

	WONRectangle& GetInvalidRect() { return mInvalidRect; }
	Window* GetWindow() { return mWindow; }
	virtual void SetCursor(Cursor *theCursor);
	void AddBG(BackgroundGrabber *theGrabber);
};

typedef WONAPI::SmartPtr<RootContainer> RootContainerPtr;

}; // namespace WONAPI

#endif