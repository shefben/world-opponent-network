#ifndef __WON_ANIMATION_H__
#define __WON_ANIMATION_H__

#include "WONCommon/SmartPtr.h"
#include "Image.h"
#include "NativeImage.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AnimationFrame;
typedef WONAPI::SmartPtr<AnimationFrame> AnimationFramePtr;
class AnimationFrame : public WONAPI::RefCount
{
public:
	enum DisposalMethod
	{
		DisposalMethod_None	= 0,
		DisposalMethod_Leave = 1,
		DisposalMethod_RestorBGColor = 2,
		DisposalMethod_RestorePrevious = 3,
		DisposalMethod_Max
	};

	ImagePtr mImage;
	int mDelayTime;
	int mLeft, mTop;
	DisposalMethod mDisposalMethod;

	AnimationFrame(Image *theImage = NULL, int theDelayTime = 0);
	void Draw(Graphics &g, int x, int y);
	AnimationFramePtr Duplicate();
};
typedef std::list<AnimationFramePtr> AnimationFrameList;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Animation;
typedef WONAPI::SmartPtr<Animation> AnimationPtr;

class Animation : public WONAPI::RefCount
{
private:
	AnimationFrameList mFrameList;
	AnimationFrameList::iterator mFrameItr;

	int mNumLoops;
	int mLoopCount;

	bool mNeedAccumulate;
	bool mCopyFirstImageToAccumulate;
	NativeImagePtr mAccumulateImage;

	int mWidth, mHeight;

public:
	Animation(const AnimationFrameList &theList, int theWidth = -1, int theHeight = -1);

	void SetNumLoops(int theNumLoops) { mNumLoops = theNumLoops; }
	void SetNeedAccumulate(bool needAccumulate) { mNeedAccumulate = needAccumulate; }
	void SetCopyFirstImageToAccumulate(bool theVal) { mCopyFirstImageToAccumulate = theVal; }
	void RealizeNative(DisplayContext *theContext);

	void Rewind(bool resetLoopCount = true);
	bool NextFrame();
	int GetFrameDelayTime();
	int GetNumFrames() { return mFrameList.size(); }

	void Draw(Graphics &g, int x, int y);

	int GetWidth() { return mWidth; }
	int GetHeight() { return mHeight; }

	AnimationPtr Duplicate();
};

}; // namespace WONAPI

#endif