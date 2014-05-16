#include "Background.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Background::Init()
{
	mColor = -1;
	mBorderColor = -1;
	mUseOffsets = true;
	mStretchImage = false;
	mWantGrabBG = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Background::Background()
{
	Init();
	mColor = ColorScheme::GetColorRef(StandardColor_Back);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Background::Background(Image *theImage)
{
	Init();
	mImage = theImage;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Background::Background(int theColor, int theBorderColor)
{
	Init();
	mColor = theColor;
	mBorderColor = theBorderColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Background::Background(Component *theComponent)
{
	Init();
	mComponent = theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Background::SetColor(int theColor)
{
	mColor = theColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Background::Paint(Graphics &g, int xpos, int ypos, int width, int height, int xoff, int yoff)
{
	if(mStretchImage && mImage.get()!=NULL)
	{
		if(mImage->GetWidth()!=width || mImage->GetHeight()!=height)
			mImage->SetSize(width,height);

		g.DrawImage(mImage,xpos,ypos);
		return;
	}

	g.Translate(xpos,ypos);

	if(mColor!=-1)
	{
		g.SetColor(mColor);
		g.FillRect(0,0,width,height);
	}

	if(mBorderColor!=-1)
	{
		g.SetColor(mBorderColor);
		g.DrawRect(0,0,width,height);
	}
	
	if(mImage.get()!=NULL)
	{
		g.PushClipRect(0,0,width,height);
		int startX = mUseOffsets?-(xoff%mImage->GetWidth()):0;
		int startY = mUseOffsets?-(yoff%mImage->GetHeight()):0;
		for(int y = startY; y<height; y+=mImage->GetHeight())
			for(int x = startX; x<width; x+=mImage->GetWidth())
				g.DrawImage(mImage, x, y);

		g.PopClipRect();
	}

	if(mComponent.get()!=NULL)
	{
		mComponent->SetSize(width,height);
		mComponent->Invalidate();
		mComponent->Paint(g);
	}
	
	g.Translate(-xpos,-ypos);
}
