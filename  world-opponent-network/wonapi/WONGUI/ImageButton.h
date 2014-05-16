#ifndef __WON_IMAGEBUTTON_H__
#define __WON_IMAGEBUTTON_H__

#include "Button.h"
#include "Image.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ImageButtonType
{
	ImageButtonType_Normal				= 0,
	ImageButtonType_Over				= 1,
	ImageButtonType_Down				= 2, 
	ImageButtonType_Disabled			= 3,
	ImageButtonType_Checked				= 4,
	ImageButtonType_None
};

class ImageButton : public Button
{
protected:
	virtual ~ImageButton();
	ImagePtr mImages[5];

	bool mStretch;

public:
	ImageButton();
	virtual void Paint(Graphics &g);
	virtual void GetDesiredSize(int &width, int &height);

	void SetImage(ImageButtonType theType, Image *theImage) { mImages[theType] = theImage; }
	void SetStretch(bool stretch) { mStretch = stretch; }
};

typedef WONAPI::SmartPtr<ImageButton> ImageButtonPtr;

}; // namespace WONAPI

#endif