#include "ResourceConfig.h"
#include "ComponentConfig.h"
#include "WONCommon/WONFile.h"
#include "WONCommon/FileReader.h"
#include "WONGUI/Window.h"

using namespace WONAPI;
using namespace std;

static const int GROUP_SIZE = 10000;

ResourceConfigTable::ControlIdMap ResourceConfigTable::mControlIdMap;
ResourceConfigTable::ConfigAllocatorMap ResourceConfigTable::mConfigAllocatorMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfig::ResourceConfig(ResourceConfigTable *theTable)
{
	mAllowOverwrite = false;
	mParseCount = 0;
	mConfigTable = theTable;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfig::~ResourceConfig()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int ResourceConfig_GetHexDigit(char theChar)
{
	if(theChar>='0' && theChar<='9')
		return theChar - '0';
	
	theChar = toupper(theChar);
	if(theChar>='A' && theChar<='F')
		return theChar - 'A' + 10;

	return -1;
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfig::EnsureComma(StringParser &theVal)
{
	if(!theVal.CheckNextChar(','))
		throw ConfigObjectException("Expecting ','");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfig::EnsureCloseParen(StringParser &theVal)
{
	if(!theVal.CheckNextChar(')'))
		throw ConfigObjectException("Expecting ')'");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ResourceConfig::SafeReadColor(StringParser &theVal)
{
	theVal.SkipWhitespace();
	if(theVal.GetChar()=='(')
	{
		// special color (std or dec)
		std::string aType;
		theVal.IncrementPos();
		theVal.ReadString(aType);
		int aColor = 0;
		if(stricmp(aType.c_str(),"STD")==0) // standard color
		{
			EnsureComma(theVal);
			std::string aName;
			theVal.ReadString(aName); 
			const char *aPtr = aName.c_str();

			if(stricmp(aPtr,"3DDarkShadow")==0) aColor = ColorScheme::GetColorRef(StandardColor_3DDarkShadow); 
			else if(stricmp(aPtr,"3DFace")==0) aColor = ColorScheme::GetColorRef(StandardColor_3DFace); 
			else if(stricmp(aPtr,"3DHilight")==0) aColor = ColorScheme::GetColorRef(StandardColor_3DHilight); 
			else if(stricmp(aPtr,"3DShadow")==0) aColor = ColorScheme::GetColorRef(StandardColor_3DShadow); 
			else if(stricmp(aPtr,"Scrollbar")==0) aColor = ColorScheme::GetColorRef(StandardColor_Scrollbar); 
			else if(stricmp(aPtr,"ButtonText")==0) aColor = ColorScheme::GetColorRef(StandardColor_ButtonText); 
			else if(stricmp(aPtr,"GrayText")==0) aColor = ColorScheme::GetColorRef(StandardColor_GrayText); 
			else if(stricmp(aPtr,"Hilight")==0) aColor = ColorScheme::GetColorRef(StandardColor_Hilight); 
			else if(stricmp(aPtr,"HilightText")==0) aColor = ColorScheme::GetColorRef(StandardColor_HilightText); 
			else if(stricmp(aPtr,"ToolTipBack")==0) aColor = ColorScheme::GetColorRef(StandardColor_ToolTipBack); 
			else if(stricmp(aPtr,"ToolTipText")==0) aColor = ColorScheme::GetColorRef(StandardColor_ToolTipText); 
			else if(stricmp(aPtr,"MenuBack")==0) aColor = ColorScheme::GetColorRef(StandardColor_MenuBack); 
			else if(stricmp(aPtr,"MenuText")==0) aColor = ColorScheme::GetColorRef(StandardColor_MenuText); 
			else if(stricmp(aPtr,"Back")==0) aColor = ColorScheme::GetColorRef(StandardColor_Back); 
			else if(stricmp(aPtr,"Text")==0) aColor = ColorScheme::GetColorRef(StandardColor_Text); 
			else if(stricmp(aPtr,"Link")==0) aColor = ColorScheme::GetColorRef(StandardColor_Link); 
			else if(stricmp(aPtr,"LinkDown")==0) aColor = ColorScheme::GetColorRef(StandardColor_LinkDown); 
			else
				throw ConfigObjectException("Invalid standard color: " + aType);
		}
		else if(stricmp(aType.c_str(),"DEC")==0)
		{
			EnsureComma(theVal); 
			int r,g,b;
			theVal.ReadValue(r); EnsureComma(theVal);
			theVal.ReadValue(g); EnsureComma(theVal);
			theVal.ReadValue(b); 
			aColor = ((r&0xff)<<16) | ((g&0xff)<<8) | (b&0xff);
		}

		EnsureCloseParen(theVal);
		return aColor;
	}

	int aColor = 0;
	for(int i=0; i<6; i++)
	{
		aColor<<=4;
		int aDigit = ResourceConfig_GetHexDigit(theVal.GetChar());
		if(aDigit<0)
			throw ConfigObjectException("Invalid color specification.");

		aColor |= aDigit;
		theVal.IncrementPos();
	}
	
	return aColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ResourceConfig::SafeReadFontStyle(StringParser &theVal)
{
	std::string aStyleStr;
	int aStyle = 0;
	while(true)
	{
		theVal.ReadString(aStyleStr,true);
		if(aStyleStr.empty())
			break;

		if(stricmp(aStyleStr.c_str(),"Bold")==0)
			aStyle |= FontStyle_Bold;
		else if(stricmp(aStyleStr.c_str(),"Italic")==0)
			aStyle |= FontStyle_Italic;
		else if(stricmp(aStyleStr.c_str(),"Plain")==0)
			aStyle |= FontStyle_Plain;
		else if(stricmp(aStyleStr.c_str(),"Underline")==0)
			aStyle |= FontStyle_Underline;
		else 
			throw ConfigObjectException("Unknown font style: " + aStyleStr);
	}
	
	return aStyle;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FontPtr ResourceConfig::SafeReadFont(StringParser &theVal)
{
	std::string aFace;
	theVal.ReadString(aFace); EnsureComma(theVal);
	int aStyle = SafeReadFontStyle(theVal); EnsureComma(theVal);
	
	int aSize;
	if(!theVal.ReadValue(aSize) || aSize<=0)
		throw ConfigObjectException("Invalid font size.");

	return Window::GetDefaultWindow()->GetFont(FontDescriptor(aFace,aStyle,aSize));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundPtr ResourceConfig::SafeReadSound(StringParser &theVal)
{
	theVal.SkipWhitespace();
	
	SoundDescriptor aDesc;
	std::string aName;
	if(theVal.GetChar()=='(')
	{
		theVal.IncrementPos();
		theVal.ReadString(aName); EnsureComma(theVal);
		int aFlags = 0;
		while(true)
		{
			std::string aFlagStr;
			theVal.ReadString(aFlagStr,true);
			if(aFlagStr.empty())
				break;

			if(stricmp(aFlagStr.c_str(),"Preload")==0)
				aFlags |= SoundFlag_Preload;
			else if(stricmp(aFlagStr.c_str(),"Music")==0)
				aFlags |= SoundFlag_Music;
			else if(stricmp(aFlagStr.c_str(),"Muted")==0)
				aFlags |= SoundFlag_Muted;
			else 
				throw ConfigObjectException("Unknown sound flag: " + aFlagStr);
		}

		aDesc.SetSoundFlags(aFlags,true);
		EnsureCloseParen(theVal);
	}
	else
		theVal.ReadString(aName);

	aDesc.mFilePath = ComponentConfig::GetResourceFile(aName);

	SoundPtr aSound = WindowManager::GetDefaultWindowManager()->DecodeSound(aDesc);
	if(aSound.get()==NULL)
		throw ConfigObjectException("Failed to decode sound: " + aDesc.mFilePath);

	return aSound;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr ResourceConfig::SafeReadImage(StringParser &theVal)
{
	std::string aPath;
	theVal.ReadString(aPath);

	ImagePtr anImage = WindowManager::GetDefaultWindowManager()->DecodeDelayImage(ComponentConfig::GetResourceFile(aPath).c_str());
	if(anImage.get()==NULL)
		throw ConfigObjectException("Failed to decode image: " + aPath);

	return anImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Background ResourceConfig::SafeReadBackground(StringParser &theVal)
{
	std::string aType;
	theVal.ReadString(aType);

	if(stricmp(aType.c_str(),"COLOR")==0)
	{
		EnsureComma(theVal);
		return SafeReadColor(theVal);
	}
	else if(stricmp(aType.c_str(),"IMAGE")==0)
	{
		EnsureComma(theVal);
		return SafeReadImage(theVal).get();
	}
	else if(stricmp(aType.c_str(),"STRETCHIMAGE")==0)
	{
		EnsureComma(theVal);
		Background aBackground(SafeReadImage(theVal));
		aBackground.SetStretchImage(true);
		return aBackground;
	}
	else if(stricmp(aType.c_str(),"WATERMARK")==0)
	{
		EnsureComma(theVal);
		Background aBackground(SafeReadImage(theVal));
		aBackground.SetUseOffsets(false);
		return aBackground;
	}
	else if(stricmp(aType.c_str(),"NONE")==0)
		return -1;
	else if(stricmp(aType.c_str(),"TRANSPARENT")==0)
	{
		Background aBackground(-1);
		aBackground.SetWantGrabBG(true);
		return aBackground;
	}
	else
		throw ConfigObjectException("Invalid background specification");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class Type, class Map>
static bool ResourceConfig_AddToMap(const char *theName, StringParser &theVal, Map &theMap, int theParseCount, Type* &theType)
{
	std::string aKey;
	theVal.ReadValue(aKey,true);
	if(aKey.empty())
		throw ConfigObjectException("Invalid resource key.");
	
	std::pair<Map::iterator,bool> aRet;
	aRet = theMap.insert(Map::value_type(aKey,Map::referent_type(Type(),theParseCount)));

	int &aParseCount = aRet.first->second.second;

	if(aRet.second)
	{
		theType = &aRet.first->second.first;		
		return true;
	}
	else if(aParseCount==theParseCount)
	{
		char aBuf[512];
		sprintf(aBuf,"%s resource already exists: %s",theName,aKey.c_str());
		throw ConfigObjectException(aBuf);
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfig* ResourceConfig::CheckResourceReference(StringParser &theVal, std::string &theResName)
{
	if(mConfigTable==NULL)
		return NULL;

	if(!theVal.CheckNextChar('@'))
		return NULL;

	std::string aName;
	theVal.ReadString(aName);
	int aDotPos = aName.find('.');
	if(aDotPos==string::npos)
		throw ConfigObjectException("Invalid resource reference: " + aName);

	std::string aGroup = aName.substr(0,aDotPos);
	theResName = aName.substr(aDotPos+1);

	return mConfigTable->SafeGetConfig(aGroup);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::HandleColor(StringParser &theVal)
{
	DWORD *aColor;
	std::string aRefName;
	if(ResourceConfig_AddToMap("Color",theVal,mColorMap,mParseCount,aColor))
	{
		ResourceConfig *aRef = CheckResourceReference(theVal,aRefName);
		if(aRef!=NULL)
			*aColor = aRef->SafeGetColor(aRefName);
		else
			*aColor = SafeReadColor(theVal);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::HandleInt(StringParser &theVal)
{
	int *anInt;
	std::string aRefName;
	if(ResourceConfig_AddToMap("Int",theVal,mIntMap,mParseCount,anInt))
	{
		ResourceConfig *aRef = CheckResourceReference(theVal,aRefName);
		if(aRef!=NULL)
			*anInt = aRef->SafeGetInt(aRefName);
		else
		{
			if(!theVal.ReadValue(*anInt))
				throw ConfigObjectException("Invalid integer value.");
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::HandleFont(StringParser &theVal)
{

	FontPtr *aFont;
	std::string aRefName;
	if(ResourceConfig_AddToMap("Font",theVal,mFontMap,mParseCount,aFont))
	{
		ResourceConfig *aRef = CheckResourceReference(theVal,aRefName);
		if(aRef!=NULL)
			*aFont = aRef->SafeGetFont(aRefName);
		else
			*aFont = SafeReadFont(theVal);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::HandleSound(StringParser &theVal)
{
	SoundPtr *aSound;
	std::string aRefName;
	if(ResourceConfig_AddToMap("Sound",theVal,mSoundMap,mParseCount,aSound))
	{
		ResourceConfig *aRef = CheckResourceReference(theVal,aRefName);
		if(aRef!=NULL)
			*aSound = aRef->SafeGetSound(aRefName);
		else
			*aSound = SafeReadSound(theVal);
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::HandleString(StringParser &theVal)
{
	GUIString *aString;
	std::string aRefName;
	if(ResourceConfig_AddToMap("String",theVal,mStringMap,mParseCount,aString))
	{

		ResourceConfig *aRef = CheckResourceReference(theVal,aRefName);
		if(aRef!=NULL)
			*aString = aRef->SafeGetString(aRefName);
		else
		{
			std::wstring aVal;
			theVal.ReadValue(aVal,false);
			*aString = aVal;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::HandleImage(StringParser &theVal)
{
	ImagePtr *anImage;
	std::string aRefName;
	if(ResourceConfig_AddToMap("Image",theVal,mImageMap,mParseCount,anImage))
	{
		ResourceConfig *aRef = CheckResourceReference(theVal,aRefName);
		if(aRef!=NULL)
			*anImage = aRef->SafeGetImage(aRefName);
		else
			*anImage = SafeReadImage(theVal);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::HandleBackground(StringParser &theVal)
{
	Background *aBackground;
	std::string aRefName;
	if(ResourceConfig_AddToMap("Background",theVal,mBackgroundMap,mParseCount,aBackground))
	{
		ResourceConfig *aRef = CheckResourceReference(theVal,aRefName);
		if(aRef!=NULL)
			*aBackground = aRef->SafeGetBackground(aRefName);
		else
			*aBackground = SafeReadBackground(theVal);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::PreParse(ConfigParser &theParser)
{
	mParseCount++;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::HandleKeyVal(ConfigParser &theParser, const std::string& theKey, StringParser &theVal)
{
	if(theKey=="STRING" || theKey=="CODESTRING")
		return HandleString(theVal);
	else if(theKey=="IMAGE" || theKey=="CODEIMAGE")
		return HandleImage(theVal);
	else if(theKey=="SOUND" || theKey=="CODESOUND")
		return HandleSound(theVal);
	else if(theKey=="FONT" || theKey=="CODEFONT")
		return HandleFont(theVal);
	else if(theKey=="COLOR" || theKey=="CODECOLOR")
		return HandleColor(theVal);
	else if(theKey=="BACKGROUND" || theKey=="CODEBACKGROUND")
		return HandleBackground(theVal);
	else if(theKey=="INT" || theKey=="CODEINT")
		return HandleInt(theVal);
	else if(theKey=="CONTROLID")
	{
	}
	else
		return ConfigObject::HandleKeyVal(theParser,theKey,theVal);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class Type, class Map>
static void ResourceConfig_AddToMap(Map &theMap, const std::string &theKey, Type &theType, int mParseCount)
{
	theMap[theKey] = Map::referent_type(theType,mParseCount);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfig::AddString(const std::string &theKey, const GUIString &theStr)
{
	ResourceConfig_AddToMap(mStringMap,theKey,theStr,mParseCount);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfig::AddImage(const std::string &theKey, Image *theImage)
{
	ResourceConfig_AddToMap(mImageMap,theKey,theImage,mParseCount);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfig::AddFont(const std::string &theKey, Font *theFont)
{
	ResourceConfig_AddToMap(mFontMap,theKey,theFont,mParseCount);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfig::AddSound(const std::string &theKey, Sound *theSound)
{
	ResourceConfig_AddToMap(mSoundMap,theKey,theSound,mParseCount);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfig::AddColor(const std::string &theKey, DWORD theColor)
{
	ResourceConfig_AddToMap(mColorMap,theKey,theColor,mParseCount);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfig::AddBackground(const std::string &theKey, const Background &theBackground)
{
	ResourceConfig_AddToMap(mBackgroundMap,theKey,theBackground,mParseCount);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfig::AddInt(const std::string &theKey, int theInt)
{
	ResourceConfig_AddToMap(mIntMap,theKey,theInt,mParseCount);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class Type, class Map>
static bool ResourceConfig_GetRes(const std::string &theKey, Type &theVal, Map &theMap)
{
	Map::iterator anItr = theMap.find(theKey);
	if(anItr!=theMap.end())
	{
		theVal = anItr->second.first;
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::GetString(const std::string& theKey, GUIString &theStr)
{
	return ResourceConfig_GetRes(theKey,theStr,mStringMap);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::GetImage(const std::string &theKey, Image* &theImage)
{
	return ResourceConfig_GetRes(theKey,theImage,mImageMap);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::GetFont(const std::string &theKey, Font* &theFont)
{
	return ResourceConfig_GetRes(theKey,theFont,mFontMap);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::GetSound(const std::string &theKey, Sound* &theSound)
{
	return ResourceConfig_GetRes(theKey,theSound,mSoundMap);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::GetColor(const std::string &theKey, DWORD &theColor)
{
	return ResourceConfig_GetRes(theKey,theColor,mColorMap);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::GetBackground(const std::string &theKey, Background &theBackground)
{
	return ResourceConfig_GetRes(theKey,theBackground,mBackgroundMap);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfig::GetInt(const std::string &theKey, int &theInt)
{
	return ResourceConfig_GetRes(theKey,theInt,mIntMap);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class Map>
static Map::referent_type::first_type& ResourceConfig_SafeGetRes(const char *theName, const std::string &theKey, Map &theMap, ResourceConfig *theConfig)
{
	Map::iterator anItr = theMap.find(theKey);
	if(anItr!=theMap.end())
		return anItr->second.first;
	else
	{
		char aBuf[512];
		sprintf(aBuf,"%s resource not found: %s (ResourceGroup: %s)",theName,theKey.c_str(),theConfig->GetName().c_str());
		throw ConfigObjectException(aBuf);
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const GUIString& ResourceConfig::SafeGetString(const std::string& theKey)
{
	return ResourceConfig_SafeGetRes("String",theKey,mStringMap,this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Image* ResourceConfig::SafeGetImage(const std::string &theKey)
{
	return ResourceConfig_SafeGetRes("Image",theKey,mImageMap,this);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Font* ResourceConfig::SafeGetFont(const std::string &theKey)
{
	return ResourceConfig_SafeGetRes("Font",theKey,mFontMap,this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Sound* ResourceConfig::SafeGetSound(const std::string &theKey)
{
	return ResourceConfig_SafeGetRes("Sound",theKey,mSoundMap,this);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DWORD ResourceConfig::SafeGetColor(const std::string &theKey)
{
	return ResourceConfig_SafeGetRes("Color",theKey,mColorMap,this);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const Background& ResourceConfig::SafeGetBackground(const std::string &theKey)
{
	return ResourceConfig_SafeGetRes("Background",theKey,mBackgroundMap,this);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ResourceConfig::SafeGetInt(const std::string &theKey)
{
	return ResourceConfig_SafeGetRes("Int",theKey,mIntMap,this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfigTable::ResourceConfigTable()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfigTable::~ResourceConfigTable()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfig* ResourceConfigTable::AddConfig(const std::string &theName, bool *alreadyExists)
{
	ResourceMap::iterator anItr = mResourceMap.insert(ResourceMap::value_type(theName,NULL)).first;

	if(anItr->second==NULL)
	{
		anItr->second = new ResourceConfig(this);
		anItr->second->SetName(theName);

		if(alreadyExists!=NULL)
			*alreadyExists = false;
	}
	else 
	{
		if(alreadyExists!=NULL)
			*alreadyExists = true;
	}

	return anItr->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfig* ResourceConfigTable::GetConfig(const std::string &theName)
{
	ResourceMap::iterator anItr = mResourceMap.find(theName);
	if(anItr!=mResourceMap.end())
		return anItr->second;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfig* ResourceConfigTable::SafeGetConfig(const std::string &theName)
{
	ResourceMap::iterator anItr = mResourceMap.find(theName);
	if(anItr!=mResourceMap.end())
		return anItr->second;
	else
		throw ConfigObjectException("Resource group not found: " + theName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfigTable::RegisterGlueResourceFunc(GlueResourceFunc theFunc)
{
	mGlueResourceFuncSet.insert(theFunc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfigTable::SafeGlueResources()
{
	GlueResourceFuncSet::const_iterator anItr = mGlueResourceFuncSet.begin();
	for(; anItr!=mGlueResourceFuncSet.end(); ++anItr)
		(*anItr)(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfigTable::AddAllocator(const std::string &theName, ComponentConfigFactory theConfigFactory, ComponentFactory theComponentFactory)
{
	ConfigAllocatorMap::iterator anItr = mConfigAllocatorMap.insert(ConfigAllocatorMap::value_type(theName,ConfigAllocator(NULL,NULL))).first;
	ConfigAllocator &anAlloc = anItr->second;
	if(anAlloc.mConfigFactory==NULL)
	{
		anAlloc.mConfigFactory = theConfigFactory;
		anAlloc.mComponentFactory = theComponentFactory;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfigTable::RegisterControlId(const std::string &theName, int theVal)
{
	mControlIdMap[theName] = theVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfigTable::AllocateConfig(const std::string &theName, ComponentConfigPtr &theConfig, ComponentPtr &theComponent)
{
	ConfigAllocatorMap::iterator anItr = mConfigAllocatorMap.find(theName);
	if(anItr==mConfigAllocatorMap.end())
		return false;

	ConfigAllocator &anAlloc = anItr->second;
	theConfig = anAlloc.mConfigFactory();
	theComponent = anAlloc.mComponentFactory();	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceConfigTable::GetControlId(const std::string &theName, int &theVal)
{
	ControlIdMap::iterator anItr = mControlIdMap.find(theName);
	if(anItr==mControlIdMap.end())
		return false;
	
	theVal = anItr->second;
	return true;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfigTable::ClearGlueResourceFuncs()
{
	mGlueResourceFuncSet.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfigTable::ClearAllocators()
{
	mConfigAllocatorMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfigTable::ClearControlIds()
{
	mControlIdMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfigTable::ClearResources()
{
	mResourceMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceConfigTable::ClearAll()
{
	ClearGlueResourceFuncs();
	ClearAllocators();
	ClearControlIds();
	ClearResources();
}
