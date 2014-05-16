#include "ImageConfig.h"
#include "ResourceConfig.h"
#include "WONGUI/ImageFilter.h"
#include "WONGUI/Window.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::ImageConfig_Init(ResourceConfigTable *theTable)
{
	theTable->AddAllocator("Image",ImageConfig::CfgFactory, ImageConfig::CompFactory);
	theTable->AddAllocator("StretchImage",StretchImageConfig::CfgFactory, ImageConfig::CompFactory);
	theTable->AddAllocator("StretchCenterImage",StretchCenterImageConfig::CfgFactory, ImageConfig::CompFactory);
	theTable->AddAllocator("ImageFilter",ImageFilterConfig::CfgFactory, ImageConfig::CompFactory);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImageConfig::ImageConfig()
{
	mHaveInitedComponents = true; // prevent ComponentConfig::InitComponents from crashing
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ImageConfig::GetNamedIntValue(const std::string &theName, int &theValue)
{
	if(theName=="WIDTH")
		theValue = mImage->GetWidth();
	else if(theName=="HEIGHT")
		theValue = mImage->GetHeight();
	else
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ImageConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SRC")
		mImage = ReadImage();
	else if(theInstruction=="SIZE")
	{
		int width = ReadInt(); EnsureComma();
		int height = ReadInt();
		mImage->SetSize(width,height);
	}
	else
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ImageConfig::PostParse(ConfigParser &theParser)
{
	return true;	// prevent ComponentConfig::PostParse from crashing
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StretchImageConfig::StretchImageConfig()
{
	mImage = mStretchImage = new StretchImage(NULL,0,0,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool StretchImageConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SRC")
		mStretchImage->SetImage(ReadRawImage());
	else if(theInstruction=="TILE")
	{
		bool tilehorz = ReadBool(); EnsureComma();
		bool tilevert = ReadBool();
		mStretchImage->SetTile(tilehorz,tilevert);
	}
	else if(theInstruction=="BORDERS")
	{
		int left = ReadInt(); EnsureComma();
		int top = ReadInt(); EnsureComma();
		int right = ReadInt(); EnsureComma();
		int bottom = ReadInt();
		mStretchImage->SetBorders(left,top,right,bottom);

		if(!EndOfString())
		{
			EnsureComma();
			bool tilehorz = ReadBool(); EnsureComma();
			bool tilevert = ReadBool();
			mStretchImage->SetTile(tilehorz,tilevert);
		}
	}
	else if(theInstruction=="SKIPCENTER")
		mStretchImage->SetSkipCenter(ReadBool());
	else if(theInstruction=="MULTIEXPORT")
	{
		int aNum = ReadInt(); EnsureComma();
		std::string fromStr = ReadRawString(); EnsureComma();
		std::string toStr = ReadRawString();
		int aStartIndex = 0;
		if(!EndOfString())
		{
			EnsureComma();
			aStartIndex = ReadInt() - 1;
		}

		char from[512], to[512];
		for(int i=0; i<aNum; i++)
		{
			sprintf(from,"%s%d",fromStr.c_str(),i+1+aStartIndex);
			sprintf(to,"%s%d",toStr.c_str(),i+1);

			RawImagePtr anImage = GetRawImageFromStr(from);
		
			StretchImageConfigPtr aConfig = new StretchImageConfig;
			aConfig->mStretchImage->CopyAttributes(mStretchImage);
			aConfig->mStretchImage->SetImage(anImage);
			ComponentConfig *aParent = (ComponentConfig*)GetParent();
			aParent->AddChildToConfigMap(to,aConfig);
		}
	}
	else if(theInstruction=="MULTICROPEXPORT")
	{
		int aNum = ReadInt(); EnsureComma();
		RawImagePtr aMasterImage = ReadRawImage(); EnsureComma();
		std::string toStr = ReadRawString();
		int aWidth = aMasterImage->GetWidth()/aNum;
		CropImageFilterPtr aCrop = new CropImageFilter;
		aCrop->SetSize(aWidth,aMasterImage->GetHeight());

		char to[512];
		for(int i=0; i<aNum; i++)
		{
			aCrop->SetPos(i*aWidth,0);
			sprintf(to,"%s%d",toStr.c_str(),i+1);
			RawImagePtr anImage = aCrop->Filter(aMasterImage);
		
			StretchImageConfigPtr aConfig = new StretchImageConfig;
			aConfig->mStretchImage->CopyAttributes(mStretchImage);
			aConfig->mStretchImage->SetImage(anImage);
			ComponentConfig *aParent = (ComponentConfig*)GetParent();
			aParent->AddChildToConfigMap(to,aConfig);
		}
	}
	else if(theInstruction=="EXPORT")
	{
		GUIString aName = ReadString(); EnsureComma();
		StretchImageConfigPtr aConfig = new StretchImageConfig;
		aConfig->mStretchImage->CopyAttributes(mStretchImage);
		aConfig->mStretchImage->SetImage(ReadRawImage());
		ComponentConfig *aParent = (ComponentConfig*)GetParent();
		aParent->AddChildToConfigMap(aName,aConfig);
	}
	else
		return ImageConfig::HandleInstruction(theInstruction);

	return true;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StretchCenterImageConfig::StretchCenterImageConfig()
{
	mImage = mStretchCenterImage = new StretchCenterImage(NULL,NULL);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool StretchCenterImageConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="STRETCHIMAGE")
		mStretchCenterImage->SetStretchImage(ReadImage());
	else if(theInstruction=="CENTERIMAGE")
		mStretchCenterImage->SetCenterImage(ReadImage());
	else
		return ImageConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImageFilterConfig::ImageFilterConfig()
{
	mImageIsRawImage = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr ImageFilterConfig::HandleFilter()
{
	ImageFilterPtr aFilter;

	GUIString aName = ReadString(); EnsureComma();
	if(aName.compareNoCase("SCALEPERCENT")==0)
	{
		int percentWidth = ReadInt(); 
		int percentHeight = percentWidth;
		if(!EndOfString())
		{
			EnsureComma();
			percentHeight = ReadInt();
		}
		ScaleImageFilterPtr aScaleFilter = new ScaleImageFilter(0);
		aScaleFilter->SetPercentSize(percentWidth,percentHeight);
		aFilter = aScaleFilter;
	}
	else if(aName.compareNoCase("SCALEABS")==0)
	{
		int width = ReadInt(); EnsureComma();
		int height = ReadInt();
		aFilter = new ScaleImageFilter(width,height);
	}
	else if(aName.compareNoCase("TINT")==0)
	{
		int red = ReadInt(); EnsureComma(); 
		int green = ReadInt(); EnsureComma();
		int blue = ReadInt();
		aFilter = new TintImageFilter(red,green,blue);
	}
	else if(aName.compareNoCase("CROP")==0)
	{	
		int x = ReadInt(); EnsureComma();
		int y = ReadInt(); EnsureComma();
		int width = ReadInt(); EnsureComma();
		int height = ReadInt(); 
		aFilter = new CropImageFilter(x,y,width,height);
	}
	else if(aName.compareNoCase("PIXELTRANS")==0)
		aFilter = new TransparentImageFilter(ReadColor());


	if(aFilter.get()!=NULL)
		return aFilter->Filter(mRawImage);
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageFilterConfig::SetRawImage(RawImage *theImage)
{
	mImage = mRawImage = theImage;
	mImageIsRawImage = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ImageFilterConfig::HandleInstruction(const std::string &theInstruction)
{
	ImageFilterPtr aFilter;
	if(theInstruction=="SRC")
		SetRawImage(ReadRawImage());
	else if(theInstruction=="FILTER")
		SetRawImage(HandleFilter());
	else if(theInstruction=="DUPLICATE")
		SetRawImage((RawImage*)mRawImage->Duplicate().get());
	else if(theInstruction=="MULTIHORZCROP")
	{
		int aNumImages = ReadInt(); EnsureComma();
		std::string aBaseName = ReadString();
		int aWidth = mRawImage->GetWidth()/aNumImages;
		CropImageFilterPtr aCrop = new CropImageFilter;
		aCrop->SetSize(aWidth,mRawImage->GetHeight());
		char aName[512];
		for(int i=0; i<aNumImages; i++)
		{
			aCrop->SetPos(i*aWidth,0);
			RawImagePtr anImage = aCrop->Filter(mRawImage);
			sprintf(aName,"%s%d",aBaseName.c_str(),i+1);
			
			ImageFilterConfigPtr aConfig = new ImageFilterConfig;
			aConfig->SetRawImage(anImage);

			ComponentConfig *aParent = (ComponentConfig*)GetParent();
			aParent->AddChildToConfigMap(aName,aConfig);			
		}
	}
	else if(theInstruction=="EXPORT")
	{
		GUIString aName = ReadString(); EnsureComma();
		ImageFilterConfigPtr aConfig = new ImageFilterConfig;
		aConfig->SetRawImage(HandleFilter());

		ComponentConfig *aParent = (ComponentConfig*)GetParent();
		aParent->AddChildToConfigMap(aName,aConfig);
	}
	else
		return ImageConfig::HandleInstruction(theInstruction);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Image* ImageFilterConfig::GetImage() 
{ 
	if(mImageIsRawImage)
	{
		mImage = mRawImage->GetNative(Window::GetDefaultWindow()->GetDisplayContext());
		mImageIsRawImage = false;
	}
		
	return mImage;
}


