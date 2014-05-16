#ifndef __WON_BACKGROUND_H__
#define __WON_BACKGROUND_H__

#include "Component.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Background
{
private:
	ImagePtr mImage;
	int mColor, mBorderColor;
	bool mUseOffsets, mStretchImage, mWantGrabBG;
	ComponentPtr mComponent;

	void Init();

public:
	Background();
	Background(Image *theImage);
	Background(int theColor, int theBorderColor = -1);
	Background(Component *theComponent);

	void SetColor(int theColor);
	void SetBorderColor(int theColor) { mBorderColor = theColor; }
	void SetImage(Image *theImage) { mImage = theImage; }
	void SetComponent(Component *theComponent) { mComponent = theComponent; }
	void SetUseOffsets(bool useOffsets) { mUseOffsets = useOffsets; }
	void SetStretchImage(bool stretch) { mStretchImage = stretch; }
	void SetWantGrabBG(bool grabBG) { mWantGrabBG = grabBG; }

	bool GetWantGrabBG() const { return mWantGrabBG; }

	void Paint(Graphics &g, int x, int y, int width, int height, int xoff=0, int yoff=0);

};

}; // namespace WONAPI

#endif