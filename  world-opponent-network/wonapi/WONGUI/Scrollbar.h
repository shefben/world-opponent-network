#ifndef __WON_SCROLLBAR_H__
#define __WON_SCROLLBAR_H__

#include "Container.h"
#include "Button.h"

namespace WONAPI
{

class Scrollbar : public Container
{
protected:
	ButtonPtr mUpArrow;
	ButtonPtr mDownArrow;
	ButtonPtr mScrollRect;
	ComponentPtr mThumb;
	bool mResizeThumb;
	bool mVertical;
	unsigned char mMinThumbSize;

	int mPosition;
	int mPageSize;
	int mTotalSize;
	int mLineUpScrollSize;
	int mLineDownScrollSize;
	int mPageScrollSize;

	int mDragX, mDragY;

	virtual ~Scrollbar();

private:
	int S_Left(Component *theComponent) { return mVertical?theComponent->Left():theComponent->Top(); }
	int S_Right(Component *theComponent) { return mVertical?theComponent->Right():theComponent->Bottom(); }
	int S_Top(Component *theComponent) { return mVertical?theComponent->Top():theComponent->Left(); }
	int S_Bottom(Component *theComponent) { return mVertical?theComponent->Bottom():theComponent->Right(); }
	int S_Height(Component *theComponent) { return mVertical?theComponent->Height():theComponent->Width(); }
	int S_Width(Component *theComponent) { return mVertical?theComponent->Width():theComponent->Height(); }
	void S_SetPos(Component *theComponent, int x, int y);
	void S_SetSize(Component *theComponent, int width, int height);
	void S_SetPosSize(Component *theComponent, int x, int y, int width, int height);

public:
	virtual void SizeChanged();
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseDrag(int x, int y);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void AddedToParent();

	void CopyAttributes(Scrollbar *theCopyFrom);

public:
	Scrollbar(bool vertical = true);
	void LayoutThumb(bool changePos);

	bool IsVertical() const { return mVertical; }
	bool IsHorizontal() const { return !mVertical; }
	void SetVertical(bool isVertical) { mVertical = isVertical; }
	void Enable(bool isEnabled);

	void SetUpArrow(Button *theButton) { mUpArrow = theButton; }
	void SetDownArrow(Button *theButton) { mDownArrow = theButton; }
	void SetScrollRect(Button *theButton) { mScrollRect = theButton; }
	void SetThumb(Component *theThumb) { mThumb = theThumb; }
	void AddControls(); // Call after setting the controls

	Button* GetUpArrow() { return mUpArrow; }
	Button* GetDownArrow() { return mDownArrow; }
	Button* GetScrollRect() { return mScrollRect; }
	Component* GetThumb() { return mThumb; }

	void SetResizeThumb(bool doResize) { mResizeThumb = doResize; }
	void SetMinThumbSize(unsigned char theSize) { mMinThumbSize = theSize; }

	void SetPosition(int thePosition) { mPosition = thePosition; }
	void SetPageSize(int thePageSize) { mPageSize = thePageSize; }
	void SetTotalSize(int theTotalSize) { mTotalSize = theTotalSize; }
	void SetLineUpScrollSize(int theLineScrollSize) { mLineUpScrollSize = theLineScrollSize; }
	void SetLineDownScrollSize(int theLineScrollSize) { mLineDownScrollSize = theLineScrollSize; }
	void SetPageScrollSize(int thePageScrollSize) { mPageScrollSize = thePageScrollSize; }
	
	int GetPosition() { return mPosition; }
	int GetPageSize() { return mPageSize; }
	int GetTotalSize() { return mTotalSize; }
	int GetLineUpScrollSize() { return mLineUpScrollSize; }
	int GetLineDownScrollSize() { return mLineDownScrollSize; }
	int GetPageScrollSize() { return mPageScrollSize; }
};

typedef WONAPI::SmartPtr<Scrollbar> ScrollbarPtr;

}; // namespace WONAPI

#endif