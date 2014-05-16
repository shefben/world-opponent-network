#ifndef __WON_IMAGECONFIG_H__
#define __WON_IMAGECONFIG_H__

#include "ComponentConfig.h"
#include "WONGUI/StretchImage.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageConfig_Init(ResourceConfigTable *theTable);

class ImageConfig : public ComponentConfig
{
protected:
	ImagePtr mImage;
	
	virtual bool GetNamedIntValue(const std::string &theName, int &theValue);
	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool PostParse(ConfigParser &theParser);

public:
	ImageConfig();

	virtual Image* GetImage() { return mImage; }
	virtual RawImage* GetRawImage() { return NULL; }

	void SetImage(Image *theImage) { mImage = theImage;  }

	static ComponentConfig* CfgFactory() { return new ImageConfig; }
	static Component* CompFactory() { return NULL; }
};
typedef SmartPtr<ImageConfig> ImageConfigPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class StretchImageConfig : public ImageConfig
{
protected:
	StretchImage* mStretchImage;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	StretchImageConfig();
	
	static ComponentConfig* CfgFactory() { return new StretchImageConfig; }
};
typedef SmartPtr<StretchImageConfig> StretchImageConfigPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class StretchCenterImageConfig : public ImageConfig
{
protected:
	StretchCenterImage *mStretchCenterImage;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	StretchCenterImageConfig();
	static ComponentConfig* CfgFactory() { return new StretchCenterImageConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageFilterConfig : public ImageConfig
{
protected:
	RawImagePtr mRawImage;
	bool mImageIsRawImage;

	virtual bool HandleInstruction(const std::string &theInstruction);
	RawImagePtr HandleFilter();

public:
	ImageFilterConfig();
	void SetRawImage(RawImage *theImage);

	static ComponentConfig* CfgFactory() { return new ImageFilterConfig; }
	virtual Image* GetImage();
	virtual RawImage* GetRawImage() { return mRawImage; }
};
typedef SmartPtr<ImageFilterConfig> ImageFilterConfigPtr;



}; // namespace WONAPI

#endif