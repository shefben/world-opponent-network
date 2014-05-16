#include "BackgroundGrabber.h"
#include "Window.h"
#include "RawImage.h"

#include "SimpleComponent.h"
#include "GUICompat.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned __int64 BackgroundGrabber::mImageOrderCounter = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BackgroundGrabber::BackgroundGrabber()
{
	mImageX = mImageY = 0;
	mNeedPutBG = false;
	mHaveBG = false;
	mTranslucentColor = 0;
	mTranslucentLevel = 4;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BackgroundGrabber::SizeChanged(Component *theComponent)
{
//	mHaveBG = false; // Backgrond isn't valid anymore if we've moved around and stuff
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BackgroundGrabber::Clear()
{
	mHaveBG = mNeedPutBG = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BackgroundGrabber::PrePaint(Component *theComponent, Graphics &g)
{
	if(mHaveBG && mNeedPutBG)
	{
		int x = 0, y = 0;
		theComponent->ChildToRoot(x,y);
		g.DrawImage(mBGImage,mImageX-x, mImageY-y);

	//		g.DrawImage(mBGImage,mImageX-theComponent->Left(), mImageY-theComponent->Top());
		mHaveBG = false;
		mNeedPutBG = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BackgroundGrabber::SetTranslucentColor(int theColor, int theLevel)
{
	mTranslucentLevel = theLevel;
	mTranslucentColor = theColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define COLOR_1_8(c) (((c)>>3)&0xff1f1f1f)
#define COLOR_2_8(c) (((c)>>2)&0xff3f3f3f)
#define COLOR_3_8(c) (COLOR_1_8(c)+COLOR_2_8(c))
#define COLOR_4_8(c) (((c)>>1)&0xff7f7f7f)
#define COLOR_5_8(c) (COLOR_4_8(c)+COLOR_1_8(c))
#define COLOR_6_8(c) (COLOR_4_8(c)+COLOR_2_8(c))
#define COLOR_7_8(c) (COLOR_4_8(c)+COLOR_3_8(c))
//#define COLOR_7_8(c) ((c)-COLOR_1_8(c)) // doesn't work right because it rounds up

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int GetColor8th(int theColor, int theNumerator)
{
	switch(theNumerator)
	{
		case 1: return COLOR_1_8(theColor);
		case 2: return COLOR_2_8(theColor);
		case 3: return COLOR_3_8(theColor);
		case 4: return COLOR_4_8(theColor);
		case 5: return COLOR_5_8(theColor);
		case 6: return COLOR_6_8(theColor);
		case 7: return COLOR_7_8(theColor);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BackgroundGrabber::Paint(Component *theComponent, Graphics &g)
{
	if(!mHaveBG)
	{
		mClipRect = g.GetClipRect();
		mImageOrder = mImageOrderCounter++;
		int aWidth = theComponent->Width();
		int aHeight = theComponent->Height();
		if(mBGImage.get()==NULL || aWidth!=mBGImage->GetWidth() || aHeight!=mBGImage->GetHeight())
		{
			Window *aWindow = theComponent->GetWindow();
			mBGImage = aWindow->CreateImage(aWidth, aHeight);
			
			if(theComponent->ComponentFlagSet(ComponentFlag_DebugBG) && mDebugWindow.get()==NULL)
			{
				mDebugWindow = new PlatformWindow;
				mDebugWindow->SetParent(theComponent->GetWindow());
				mDebugWindow->SetCreateFlags(CreateWindow_SizeSpecClientArea);
				WONRectangle aRect(0,0,mBGImage->GetWidth(),mBGImage->GetHeight());
				mDebugWindow->Create(aRect);
			}
			
			if(mDebugWindow.get()!=NULL)
			{
				mDebugWindow->GetRoot()->RemoveAllChildren();
				mDebugWindow->AddComponent(new ImageComponent(mBGImage));
				int x=0,y=0;
				theComponent->ChildToRoot(x,y);
				WONRectangle aScreenRect;
				theComponent->GetWindow()->GetScreenPos(aScreenRect);
				mDebugWindow->Move(x+aScreenRect.Left(),y+aScreenRect.Top());
			}

		}

		NativeImage *anImage = theComponent->GetWindow()->GetOffscreenImage();
		int x = 0, y = 0;
		theComponent->RootToChild(x,y);
		mBGImage->GetGraphics().DrawImage(anImage, x, y);
		mImageX = 0;//theComponent->Left();
		mImageY = 0;//theComponent->Top();
		theComponent->ChildToRoot(mImageX,mImageY);

		if(mDebugWindow.get()!=NULL)
			mDebugWindow->GetRoot()->Invalidate();

		mHaveBG = true;

		if(theComponent->ComponentFlagSet(ComponentFlag_Translucent))
		{

			int aColor = GetColor8th(mTranslucentColor,8-mTranslucentLevel);
			if(g.BlendSupported())
			{
				g.SetColor(mTranslucentColor);
				g.FillRectBlend(0,0,theComponent->Width(),theComponent->Height(),(8-mTranslucentLevel)*255/8);
				return;
			}
	
			RawImage32Ptr aRawImage = mBGImage->GetRaw();
			int aNumPixels = aRawImage->GetWidth()*aRawImage->GetHeight();
			DWORD *aPixels =aRawImage->GetImageData();
			DWORD *anEndPixels = aPixels+aNumPixels;
			switch(mTranslucentLevel)
			{
			case 1:
				do
				{
					*aPixels = COLOR_1_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			case 2:
				do
				{
					*aPixels = COLOR_2_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			case 3:
				do
				{
					*aPixels = COLOR_3_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			case 4:
				do
				{
					*aPixels = COLOR_4_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			case 5:
				do
				{
					*aPixels = COLOR_5_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			case 6:
				do
				{
					*aPixels = COLOR_6_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;
			
			case 7:
				do
				{
					*aPixels = COLOR_7_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			}
			g.DrawImage(aRawImage,0,0);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BackgroundGrabber::Invalidate(Component *theComponent)
{
	if(mHaveBG)
		mNeedPutBG = true;
}