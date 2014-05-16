#ifndef __WON_SIMPLECOMPONENT_H__
#define __WON_SIMPLECOMPONENT_H__

#include "Container.h"
#include "NativeImage.h"
#include "Animation.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RectangleComponent : public Component
{
private:
	int mSolidColor;
	int mOutlineColor;

public:
	RectangleComponent();
	RectangleComponent(int theSolidColor, int theOutlineColor = -1);

	void SetColor(int theSolidColor, int theOutlineColor = -1);
	void SetSolidColor(int theSolidColor);
	void SetOutlineColor(int theOutlineColor);

	virtual void Paint(Graphics &g);
};

typedef WONAPI::SmartPtr<RectangleComponent> RectangleComponentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RectangleComponent3D : public Component
{
private:
	int mSolidColor;
	int mLightColor;
	int mDarkColor;
	int mThickness;
	bool mUp;

public:
	RectangleComponent3D(bool up = true, int theThickness = 2);
	RectangleComponent3D(bool up, int theThickness, int theLightColor, int theDarkColor = -1, int theSolidColor = -1);

	void SetSolidColor(int theSolidColor) { mSolidColor = theSolidColor; }
	void SetLightColor(int theLightColor) { mLightColor = theLightColor; }
	void SetDarkColor(int theDarkColor) { mDarkColor = theDarkColor; }
	void SetThickness(int theThickness) { mThickness = theThickness; }
	int  GetThickness() const { return mThickness; }
	void SetIsUp(bool isUp) { mUp = isUp; }

	
	virtual void Paint(Graphics &g);
};
typedef WONAPI::SmartPtr<RectangleComponent3D> RectangleComponent3DPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScreenContainer : public Container
{
protected:
	Component *mCurScreen;

public:
	ScreenContainer();
	void AddScreen(Component *theScreen);
	void ShowScreen(Component *theScreen, bool takeFocus = true);

	Component* GetCurScreen() { return mCurScreen; }
};
typedef SmartPtr<ScreenContainer> ScreenContainerPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BorderComponent : public Container
{
public:
	enum BorderPos
	{
		Pos_LeftTop = 0, Pos_RightTop, Pos_LeftBottom, Pos_RightBottom,
		Pos_Left, Pos_Top, Pos_Right, Pos_Bottom, 
	};

protected:
	ComponentPtr mComponents[8];
	Component *mLeftTop, *mRightTop, *mLeftBottom, *mRightBottom, *mLeft, *mTop, *mRight, *mBottom;
	bool mControlsAdded;

public:
	virtual void SizeChanged();

public:
	BorderComponent();
	void SetComponent(BorderPos thePos, Component *theComponent, int theWidth = -1, int theHeight = -1);
	void AddControls();
};
typedef WONAPI::SmartPtr<BorderComponent> BorderComponentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageComponent : public Component
{
protected:
	ImagePtr mImage;
	bool mTile;
	bool mStretch;

	virtual void GetDesiredSize(int &width, int &height);

public:
	ImageComponent(Image *theImage = NULL, bool tile = false);


	virtual void SetImage(Image *theImage);
	virtual void SetImage(Image *theImage, bool tile);
	void SetTile(bool doTile) { mTile = doTile; }
	void SetStretch(bool doStretch) { mStretch = doStretch; }

	bool GetTile() { return mTile; }
	bool GetStretch() { return mStretch; }
	Image* GetImage() { return mImage; }

	virtual void Paint(Graphics &g);
};

typedef WONAPI::SmartPtr<ImageComponent> ImageComponentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AnimationComponent : public Component
{
protected:
	int mDelayAcc;
	bool mStarted;
	AnimationPtr mAnimation;

public:
	virtual void SizeChanged();
	virtual void Paint(Graphics &g);
	virtual bool TimerEvent(int theDelta);
	virtual void AddedToParent();

public:
	AnimationComponent(Animation* theAnimation = NULL, bool startNow = false);
	void SetAnimation(Animation *theAnimation);

	void Start(bool restart = true);
	void Stop();
};

typedef WONAPI::SmartPtr<AnimationComponent> AnimationComponentPtr;

}; // namespace WONAPI

#endif