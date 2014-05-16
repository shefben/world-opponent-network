#include "Animation.h"
#include "RawImage.h"

using namespace WONAPI;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AnimationFrame::AnimationFrame(Image *theImage, int theDelayTime)
{
	mImage = theImage;
	mDelayTime = theDelayTime;
	mDisposalMethod = DisposalMethod_None;
	mLeft = mTop = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AnimationFramePtr AnimationFrame::Duplicate()
{
	AnimationFrame *aFrame = new AnimationFrame(mImage,mDelayTime);
	aFrame->mDisposalMethod = mDisposalMethod;
	aFrame->mLeft = mLeft;
	aFrame->mTop = mTop;
	return aFrame;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AnimationFrame::Draw(Graphics &g, int x, int y)
{
	g.DrawImage(mImage,x+mLeft,y+mTop);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Animation::Animation(const AnimationFrameList &theList,  int theWidth, int theHeight) : mFrameList(theList)
{
	if(theWidth==-1 || theHeight==-1)
	{
		if(!theList.empty())
		{
			Image *anImage = theList.front()->mImage;
			mWidth = anImage->GetWidth();
			mHeight = anImage->GetHeight();
		}
		else
			mWidth = mHeight = 0;
	}
	else
	{
		mWidth = theWidth;
		mHeight = theHeight;
	}

	mFrameItr = mFrameList.begin();
	mNumLoops = 0;
	mLoopCount = 0;

	mNeedAccumulate = false;
	mCopyFirstImageToAccumulate = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Animation::RealizeNative(DisplayContext *theContext)
{
	if(mFrameList.empty())
		return;

	if(dynamic_cast<RawImage*>(mFrameList.front()->mImage.get())!=NULL)
	{
		for(AnimationFrameList::iterator anItr = mFrameList.begin(); anItr!=mFrameList.end(); ++anItr)
		{
			AnimationFrame *aFrame = *anItr;
			aFrame->mImage = ((RawImage*)aFrame->mImage.get())->GetNative(theContext);
		}
	}

	if(mNeedAccumulate && !mCopyFirstImageToAccumulate && mAccumulateImage.get()==NULL)
		mAccumulateImage = theContext->CreateImage(mWidth,mHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Animation::Rewind(bool resetLoopCount)
{
	if(resetLoopCount)
		mLoopCount = 0;

	mFrameItr = mFrameList.begin();

	if(mCopyFirstImageToAccumulate && !mFrameList.empty())
		mAccumulateImage = (NativeImage*)mFrameList.front()->mImage->Duplicate().get();
	else if(mAccumulateImage.get()!=NULL)
	{
		AnimationFrame *aFrame = *mFrameItr;
		aFrame->Draw(mAccumulateImage->GetGraphics(),0,0);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Animation::NextFrame()
{
	if(mFrameList.size()==1 || ++mFrameItr==mFrameList.end())
	{
		if(mNumLoops>0 && ++mLoopCount>=mNumLoops)
			return false;
		else
			Rewind(false);
	}

	if(mAccumulateImage.get()!=NULL)
	{
		AnimationFrame *aFrame = *mFrameItr;
		aFrame->Draw(mAccumulateImage->GetGraphics(),0,0);
	}	

	return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int Animation::GetFrameDelayTime()
{
	if(mFrameItr==mFrameList.end())
		return 0;

	AnimationFrame *aFrame = *mFrameItr;
	return aFrame->mDelayTime;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Animation::Draw(Graphics &g, int x, int y)
{
	if(mAccumulateImage.get()!=NULL)
		g.DrawImage(mAccumulateImage,x,y);
	else
	{
		if(mFrameItr!=mFrameList.end())
		{
			AnimationFrame *aFrame = *mFrameItr;
			aFrame->Draw(g,x,y);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AnimationPtr Animation::Duplicate()
{
	AnimationPtr anAnim = new Animation(mFrameList,mWidth,mHeight);
	for(AnimationFrameList::iterator anItr = anAnim->mFrameList.begin(); anItr!=anAnim->mFrameList.end(); ++anItr)
		*anItr = (*anItr)->Duplicate();

	anAnim->mNumLoops = mNumLoops;
	anAnim->mLoopCount = mLoopCount;
	anAnim->mNeedAccumulate = mNeedAccumulate;
	anAnim->mCopyFirstImageToAccumulate = mCopyFirstImageToAccumulate;
	if(mAccumulateImage.get()!=NULL)
		anAnim->mAccumulateImage = (NativeImage*)mAccumulateImage->Duplicate().get();

	return anAnim;
}
