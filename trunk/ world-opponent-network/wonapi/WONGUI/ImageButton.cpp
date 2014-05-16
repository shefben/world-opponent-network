#include "ImageButton.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImageButton::ImageButton()
{
	SetButtonFlags(ButtonFlag_InvalidateOnMouseOver,true);
	SetComponentFlags(ComponentFlag_WantFocus,true);
	SetComponentFlags(ComponentFlag_WantTabFocus,false);

	mStretch = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImageButton::~ImageButton()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageButton::GetDesiredSize(int &width, int &height)
{
	int aMaxWidth = 0, aMaxHeight = 0;
	for(int i=0; i<5; i++)
	{
		Image *anImage = mImages[i];
		if(anImage!=NULL)
		{
			if(anImage->GetWidth() > aMaxWidth)
				aMaxWidth = anImage->GetWidth();
			if(anImage->GetHeight() > aMaxHeight)
				aMaxHeight = anImage->GetHeight();
		}
	}

	width = aMaxWidth;
	height = aMaxHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageButton::Paint(Graphics &g)
{
	Component::Paint(g);

	Image *anImage = NULL;
	if(Disabled())
		anImage = mImages[ImageButtonType_Disabled];
	else if(IsChecked() && mImages[ImageButtonType_Checked].get()!=NULL)
		anImage = mImages[4];
	else if(IsButtonDown())
		anImage = mImages[ImageButtonType_Down];
	else if(IsMouseOver())// || HasFocus())
		anImage = mImages[ImageButtonType_Over];

	if(anImage==NULL)
		anImage = mImages[ImageButtonType_Normal];

	if(anImage!=NULL)
	{
		if(mStretch && (anImage->GetWidth()!=Width() || anImage->GetHeight()!=Height()))
			anImage->SetSize(Width(),Height());

		g.DrawImage(anImage,0,0);
	}
}
