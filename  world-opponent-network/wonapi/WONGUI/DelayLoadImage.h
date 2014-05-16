#ifndef __WON_DELAYLOADIMAGE_H__
#define __WON_DELAYLOADIMAGE_H__
#include "NativeImage.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DelayLoadImage : public Image
{
protected:
	std::string mImagePath;
	bool mHaveLoaded;

	NativeImagePtr mImage;

public:	
	DelayLoadImage(const std::string &theImagePath, int theWidth, int theHeight);
	virtual void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);

	virtual NativeImagePtr GetNative(DisplayContext *theContext);
};
typedef SmartPtr<DelayLoadImage> DelayLoadImagePtr;

}

#endif