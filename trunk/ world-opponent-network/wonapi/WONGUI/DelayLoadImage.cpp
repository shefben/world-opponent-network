#include "DelayLoadImage.h"
#include "WindowManager.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DelayLoadImage::DelayLoadImage(const std::string &theImagePath, int theWidth, int theHeight) : mImagePath(theImagePath)
{
	mHaveLoaded = false;
	mWidth = theWidth;
	mHeight = theHeight;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DelayLoadImage::Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height)
{
	if(!mHaveLoaded)
	{
		mHaveLoaded = true;
		mImage = WindowManager::GetDefaultWindowManager()->DecodeImage(mImagePath.c_str());
	}

	if(mImage.get()!=NULL)
		mImage->Draw(theContext,x,y,left,top,width,height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr DelayLoadImage::GetNative(DisplayContext *theContext)
{
	if(!mHaveLoaded)
	{
		mHaveLoaded = true;
		mImage = WindowManager::GetDefaultWindowManager()->DecodeImage(mImagePath.c_str());
	}

	return mImage;
}
