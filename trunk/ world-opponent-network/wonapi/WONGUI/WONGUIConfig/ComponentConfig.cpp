#include <assert.h>
#include <direct.h>
#include "ComponentConfig.h"
#include "FontConfig.h"
#include "ImageConfig.h"
#include "WONCommon/WONFile.h"
#include "WONCommon/PerfTimer.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/Window.h"
#include "ResourceConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfigTablePtr ComponentConfig::mResourceConfigTable;

std::string ComponentConfig::mResourceFolder;
static bool gShowParseTime = false;
static int gLinesParsed = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComponentConfig::ComponentConfig()
{
	mHasSetWidth = false;
	mHasSetHeight = false;
	mHaveInitedComponents = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComponentConfig::~ComponentConfig()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentConfig::ReadConfig(Component *theComponent, const char *theFileName, ResourceConfigTable *theTable, ConfigParser *theParser)
{
	if(!ReadConfig_Unsafe(theComponent, theFileName, theTable, theParser))
	{
		WindowManager::GetDefaultWindowManager()->InitSystemColorScheme();
		MSMessageBox::Msg(Window::GetDefaultWindow(),"Config Error",mConfigError,"Ok");
		WindowManager::GetDefaultWindowManager()->CloseAllWindows();
		exit(0);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::ReadConfig_Unsafe(Component *theComponent, const char *theFileName, ResourceConfigTable *theTable, ConfigParser *theParser)
{
	SetComponent(theComponent);

	WindowManager::GetDefaultWindowManager()->SetCacheImages(true);

	ResourceConfigTablePtr anOldResourceTable = mResourceConfigTable;

	if(theTable!=NULL)
		mResourceConfigTable = theTable;
	else
		mResourceConfigTable = new ResourceConfigTable;

	bool oldShowParseTime = gShowParseTime;

	gShowParseTime = false;
	static DWORD aTotalTime = 0;


	// Try to parse the configs
	std::string aFileName = GetResourceFile(theFileName);

	ConfigParser aLocalParser;
	ConfigParser &aParser = theParser?*theParser:aLocalParser;
	aParser.SetWarningsAsErrors(true);
	if(!ParseTopLevel(aParser,aFileName.c_str()))
	{
		aParser.CloseFile();
		mConfigError = aParser.GetAbortReason();
		return false;
	}

	try
	{
		mResourceConfigTable->SafeGlueResources();
		InitComponents();
	}
	catch(ConfigObjectException &anEx)
	{
		mConfigError = anEx.what;
		return false;
	}

	// Show parse time for profiling
	if(gShowParseTime)
	{
		char aBuf[512];
		sprintf(aBuf,"Lines parsed: %d\nConfig Parse Time: %.2f (%.2f) (= %%%d)",gLinesParsed,PerfTimer::GetElapsedTime(),PerfTimer::GetCountedTime(),(int)(PerfTimer::GetPercentTime()*100));
		MSMessageBox::Msg(Window::GetDefaultWindow(),"Config Parse Time",aBuf);
	}
	gShowParseTime = oldShowParseTime;

	WindowManager::GetDefaultWindowManager()->SetCacheImages(false);

	mResourceConfigTable = anOldResourceTable;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentConfig::InitComponents()
{
	if(mHaveInitedComponents)
		return;

	mHaveInitedComponents = true;
	mComponent->InitComponent(ComponentConfigInit(this));

	ConfigMap::iterator anItr = mConfigMap.begin();
	while(anItr!=mConfigMap.end())
	{
		ComponentConfig *aConfig = anItr->second;
		aConfig->InitComponents();
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentConfig::SetComponent(Component *theComponent)
{
	mComponent = theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::EndOfString()
{
	mStringParser->SkipWhitespace();
	return mStringParser->EndOfString();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentConfig::SkipComma()
{
	mStringParser->SkipWhitespace();
	if(mStringParser->GetChar()==',')
		mStringParser->IncrementPos();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentConfig::EnsureComma()
{
	if(!mStringParser->CheckNextChar(','))
		throw ConfigObjectException("Expecting ','");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ComponentConfig::GetIntValue()
{
	int anInt;
	if(mStringParser->ReadValue(anInt))
		return anInt;


	std::string aStr;
//	mStringParser->ReadString(aStr,true);
	while(!mStringParser->EndOfString())
	{
		char aChar = mStringParser->GetChar();
		if(safe_isspace(aChar))
			break;
		
		if(aChar!='@' && aChar!='_' && aChar!='.' && !safe_isalnum(aChar))
			break;

		aStr+=aChar;
		mStringParser->IncrementPos();
	}
	bool isFunc = mStringParser->GetChar()=='(';

	if(aStr[0]=='@')
	{
		if(aStr.length()>3 && toupper(aStr[1]=='I') && toupper(aStr[2]=='D') && aStr[3]=='_')
		{
			std::string aName = aStr.substr(4);
			int aVal;
			if(!mResourceConfigTable->GetControlId(aName,aVal))
				throw ConfigObjectException("Invalid control id: " + aStr);

			return aVal;
		}
 		else
			return GetIntResource(aStr.c_str() + 1);
	}

	ComponentConfig *aConfig = this;
	std::string aValueName;
	int aDotPos = aStr.find_last_of('.');
	if(aDotPos!=string::npos)
	{
		std::string aComponent = aStr.substr(0,aDotPos);
		aValueName = aStr.substr(aDotPos+1);

		aConfig = GetComponentConfig(aComponent);
	}
	else
		aValueName = aStr;

	StringToUpperCaseInPlace(aValueName);
	if(isFunc)
	{

		IntVector aParams;
		mStringParser->IncrementPos();
		while(!mStringParser->CheckNextChar(')'))
		{
			aParams.push_back(ReadInt());
			if(mStringParser->CheckNextChar(')'))
				break;

			EnsureComma();
		}
		
		aConfig->GetNamedIntFunc(aValueName,aParams,anInt);
		return anInt;
	}

	if(aConfig->GetNamedIntValue(aValueName,anInt))
		return anInt;
	else
		throw ConfigObjectException("Invalid named value: " + aValueName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ComponentConfig::ParseExpression(int theLevel)
{
	mStringParser->SkipWhitespace();
	int aVal;
	if(mStringParser->GetChar()=='(')
	{
		mStringParser->IncrementPos();
		aVal = ParseExpression(0);
		mStringParser->SkipWhitespace();
		if(mStringParser->GetChar()!=')')
			throw ConfigObjectException("Expecting ')'");

		mStringParser->IncrementPos();
	}
	else if(mStringParser->GetChar()=='-')
	{
		mStringParser->IncrementPos();
		aVal = -ParseExpression(3);
	}
	else 
		aVal = GetIntValue();
//	else if(!mStringParser->ReadValue(aVal))
//		throw ConfigObjectException("Expecting int");

	while(true)
	{		
		mStringParser->SkipWhitespace();
		char anOp = mStringParser->GetChar();
		mStringParser->IncrementPos();
		if(anOp=='+')
		{
			if(theLevel>=1)
			{
				mStringParser->IncrementPos(-1);
				return aVal;
			}
			else
				aVal += ParseExpression(1);
		}
		else if(anOp=='-')
		{
			if(theLevel>=1)
			{
				mStringParser->IncrementPos(-1);
				return aVal;
			}
			else
				aVal -= ParseExpression(1);
		}
		else if(anOp=='*')
		{
			if(theLevel>=2)
			{
				mStringParser->IncrementPos(-1);
				return aVal;
			}
			else
				aVal *= ParseExpression(2);
		}
		else if(anOp=='/')
		{

			int aDiv;
			if(theLevel>=2)
			{
				mStringParser->IncrementPos(-1);
				return aVal;
			}
			else 	
			{
				aDiv = ParseExpression(2);
				if(aDiv==0)
					throw ConfigObjectException("Division by 0.");
				else
					aVal /= aDiv;
			}
		}
		else
		{
			mStringParser->IncrementPos(-1);
			return aVal;
		}
	}

	return aVal;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ComponentConfig::ReadInt()
{
	return ParseExpression(0);
/*
	int anInt;
	if(mStringParser->ReadValue(anInt))
		return anInt;
	else
		throw ConfigObjectException("Expecting int");*/
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::ReadBool()
{
	bool aBool;
	if(mStringParser->ReadValue(aBool))
		return aBool;
	else
		throw ConfigObjectException("Expecting bool");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ComponentConfig::ReadChar()
{
	mStringParser->SkipWhitespace();
	if(mStringParser->EndOfString())
		throw ConfigObjectException("Expecting char");

	int aChar = mStringParser->GetWChar();
	mStringParser->IncrementPos();

	return aChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ComponentConfig::ReadFlag(bool *on)
{
	mStringParser->SkipWhitespace();
	int aChar = mStringParser->GetChar();
	if(aChar=='+')
	{
		mStringParser->IncrementPos();
		if(on)
			*on = true;
	}
	else if(aChar=='-')
	{
		mStringParser->IncrementPos();
		if(on)
			*on = false;
	}

	return ReadString(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ComponentConfig::ReadRawString(bool stopAtWhitespace)
{
	std::wstring aStr;
	mStringParser->ReadValue(aStr, stopAtWhitespace);
	return aStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ComponentConfig::GetStringResource(const std::string &theKey)
{
	GUIString aStr;
	if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetString(theKey,aStr))
		return aStr;
	else if(mParent!=NULL)
		return ((ComponentConfig*)mParent)->GetStringResource(theKey);
	else
		throw ConfigObjectException("String resource not found: " + theKey);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Image* ComponentConfig::GetImageResource(const std::string &theKey)
{
	Image *anImage = NULL;
	if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetImage(theKey,anImage))
		return anImage;
	else if(mParent!=NULL)
		return ((ComponentConfig*)mParent)->GetImageResource(theKey);
	else
		throw ConfigObjectException("Image resource not found: " + theKey);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Font* ComponentConfig::GetFontResource(const std::string &theKey)
{
	Font *anFont = NULL;
	if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetFont(theKey,anFont))
		return anFont;
	else if(mParent!=NULL)
		return ((ComponentConfig*)mParent)->GetFontResource(theKey);
	else
		throw ConfigObjectException("Font resource not found: " + theKey);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundPtr ComponentConfig::GetSoundResource(const std::string &theKey)
{
	Sound *anSound = NULL;
	if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetSound(theKey,anSound))
		return anSound;
	else if(mParent!=NULL)
		return ((ComponentConfig*)mParent)->GetSoundResource(theKey);
	else
		throw ConfigObjectException("Sound resource not found: " + theKey);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DWORD ComponentConfig::GetColorResource(const std::string &theKey)
{
	DWORD aColor = 0;
	if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetColor(theKey,aColor))
		return aColor;
	else if(mParent!=NULL)
		return ((ComponentConfig*)mParent)->GetColorResource(theKey);
	else
		throw ConfigObjectException("Color resource not found: " + theKey);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentConfig::GetBackgroundResource(const std::string &theKey, Background &theBackground)
{
	if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetBackground(theKey,theBackground))
		return;
	else if(mParent!=NULL)
		((ComponentConfig*)mParent)->GetBackgroundResource(theKey,theBackground);
	else
		throw ConfigObjectException("Background resource not found: " + theKey);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ComponentConfig::GetIntResource(const std::string &theKey)
{
	int anInt;
	if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetInt(theKey,anInt))
		return anInt;
	else if(mParent!=NULL)
		return ((ComponentConfig*)mParent)->GetIntResource(theKey);
	else
		throw ConfigObjectException("Int resource not found: " + theKey);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ComponentConfig::ReadString(bool stopAtWhitespace)
{
	mStringParser->SkipWhitespace();
	if(mStringParser->GetChar()=='@')
	{
		mStringParser->IncrementPos();
		string aKey;
		mStringParser->ReadValue(aKey);
		return GetStringResource(aKey);

/*
		std::string aStr;
		mStringParser->ReadString(aStr,stopAtWhitespace);
		int aPos = aStr.find_first_of('_');
		if(aPos!=string::npos)
		{
			string aType = aStr.substr(1,aPos-1);
			string aVal = aStr.substr(aPos+1);
			if(stricmp(aType.c_str(),"RES")==0)
			{
				const ResourceEntry *anEntry = GetResourceEntry(aVal,ResourceType_String);
				return anEntry->mDesc;
			}
		}
			
		throw ConfigObjectException("Invalid string config: " + aStr);	*/
	}
	else
	{
		std::wstring aStr;
		mStringParser->ReadValue(aStr, stopAtWhitespace);
		return aStr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FontPtr ComponentConfig::ReadFont()
{
	mStringParser->SkipWhitespace();
	if(mStringParser->GetChar()=='@')
	{
		mStringParser->IncrementPos();
		std::string aName;
		mStringParser->ReadString(aName);
		return GetFontResource(aName);
	}
	else if(mStringParser->GetChar()=='(')
	{
		mStringParser->IncrementPos();
		std::string aType;
		mStringParser->ReadString(aType);
		if(stricmp(aType.c_str(),"CFG")==0)
		{
			EnsureComma();
			std::string aConfigName;
			mStringParser->ReadString(aConfigName);

			ComponentConfig *aConfig = this;
			FontConfig *aFontConfig = NULL;
			while(true)
			{
				aFontConfig = dynamic_cast<FontConfig*>(aConfig->GetComponentConfig(aConfigName,false));
				if(aFontConfig!=NULL)
					break;
				else
					aConfig = (ComponentConfig*)aConfig->GetParent();
			}

			if(aFontConfig==NULL)
				throw ConfigObjectException("Invalid FontConfig: " + aConfigName);
			else if(aFontConfig->GetFont()==NULL)
				throw ConfigObjectException("NULL Font: " + aConfigName);
				
			if(!mStringParser->CheckNextChar(')'))
				throw ConfigObjectException("Expecting ')'");
			
			return aFontConfig->GetFont();
		}
		else
			throw ConfigObjectException("Bad Font Type: " + aType);
	}
	else
		return ResourceConfig::SafeReadFont(*mStringParser);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr ComponentConfig::GetImageFromStr(const std::string &theImageSpec, bool getRaw)
{
	const std::string &aStr = theImageSpec;
	if(aStr[0]=='@')
	{
		int aPos = aStr.find_first_of('_');
		if(aPos!=string::npos)
		{
			string aType = aStr.substr(1,aPos-1);
			if(stricmp(aType.c_str(),"CFG")==0)
			{
				std::string aComponent = aStr.substr(aPos+1);
				ComponentConfig *aConfig = this;
				ImageConfig *anImageConfig = NULL;
				while(aConfig!=NULL)
				{
					anImageConfig = dynamic_cast<ImageConfig*>(aConfig->GetComponentConfig(aComponent,false));
					if(anImageConfig!=NULL)
						break;
					else
						aConfig = (ComponentConfig*)aConfig->GetParent();
				}

				if(anImageConfig==NULL)
					throw ConfigObjectException("Invalid ImageConfig: " + aStr);
				else
				{
					ImagePtr anImage;
					if(getRaw)
						anImage = anImageConfig->GetRawImage();
					else
						anImage = anImageConfig->GetImage();

					if(anImage.get()==NULL)
						throw ConfigObjectException("Image Doesn't Exist: " + aStr);

					return anImage;
				}			
			}
		}
		else if(stricmp(aStr.c_str(),"@NULL")==0)
			return NULL;
		else
			return GetImageResource(aStr.c_str()+1);

		throw ConfigObjectException("Bad Image Specification: " + aStr);
	}
	ImagePtr anImage;
	if(getRaw)
		anImage = Window::GetDefaultWindow()->DecodeImageRaw(GetResourceFile(aStr).c_str());
	else
		anImage = WindowManager::GetDefaultWindowManager()->DecodeDelayImage(GetResourceFile(aStr).c_str());

	if(anImage.get()==NULL)
		throw ConfigObjectException("Failed to decode image: " + aStr);

	return anImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr ComponentConfig::GetRawImageFromStr(const std::string &theImageSpec)
{
	return (RawImage*)GetImageFromStr(theImageSpec,true).get();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr ComponentConfig::ReadImage(bool getRaw)
{
	std::string anImageName;
	mStringParser->ReadString(anImageName);
	return GetImageFromStr(anImageName,getRaw);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr ComponentConfig::ReadRawImage()
{
	RawImagePtr aRawImage = (RawImage*)ReadImage(true).get();
	return aRawImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StandardCursor ComponentConfig::ReadStandardCursor()
{
	GUIString aName = ReadString();
	if(aName.compareNoCase("Arrow")==0)
		return StandardCursor_Arrow;
	else if(aName.compareNoCase("Hand")==0)
		return StandardCursor_Hand;
	else if(aName.compareNoCase("IBeam")==0)
		return StandardCursor_IBeam;
	else if(aName.compareNoCase("EastWest")==0)
		return StandardCursor_EastWest;
	else if(aName.compareNoCase("NorthSouth")==0)
		return StandardCursor_NorthSouth;
	else if(aName.compareNoCase("NorthSouthEastWest")==0)
		return StandardCursor_NorthSouthEastWest;
	else
		throw ConfigObjectException("Invalid predefined cursor type: " + (std::string)aName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CursorPtr ComponentConfig::ReadCursor()
{
	GUIString aType = ReadString();
	if(aType.compareNoCase("Predefined")==0)
	{
		EnsureComma();
		return Cursor::GetStandardCursor(ReadStandardCursor());
	}
	else
		throw ConfigObjectException("Invalid cursor type: " + (std::string)aType);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int GetHexDigit(char theChar)
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
int ComponentConfig::ReadColor()
{
	mStringParser->SkipWhitespace();
	int aColor = 0;
	if(mStringParser->GetChar()=='@')
	{
		mStringParser->IncrementPos();
		std::string aName;
		mStringParser->ReadString(aName,true);
		return GetColorResource(aName);			
	}
	else
		return ResourceConfig::SafeReadColor(*mStringParser);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundPtr ComponentConfig::ReadSound()
{
	mStringParser->SkipWhitespace();
	int aColor = 0;
	if(mStringParser->GetChar()=='@')
	{
		mStringParser->IncrementPos();
		std::string aName;
		mStringParser->ReadString(aName,true);
		return GetSoundResource(aName);			
	}
	else
		return ResourceConfig::SafeReadSound(*mStringParser);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Direction ComponentConfig::ReadDirection()
{
	GUIString aStr = ReadString();
	if(aStr.compareNoCase("Left")==0)
		return Direction_Left;
	else if(aStr.compareNoCase("Right")==0)
		return Direction_Right;
	else if(aStr.compareNoCase("Up")==0)
		return Direction_Up;
	else if(aStr.compareNoCase("Down")==0)
		return Direction_Down;
	else
		throw ConfigObjectException("Bad Direction");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HorzAlign ComponentConfig::ReadHorzAlign()
{
	GUIString aStr = ReadString();
	if(aStr.compareNoCase("Left")==0)
		return HorzAlign_Left;
	else if(aStr.compareNoCase("Center")==0)
		return HorzAlign_Center;
	else if(aStr.compareNoCase("Right")==0)
		return HorzAlign_Right;
	else
		throw ConfigObjectException("Bad HorzAlign");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
VertAlign ComponentConfig::ReadVertAlign()
{
	GUIString aStr = ReadString();
	if(aStr.compareNoCase("Top")==0)
		return VertAlign_Top;
	else if(aStr.compareNoCase("Center")==0)
		return VertAlign_Center;
	else if(aStr.compareNoCase("Bottom")==0)
		return VertAlign_Bottom;
	else
		throw ConfigObjectException("Bad VertAlign");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SelectionColor ComponentConfig::ReadSelectionColor()
{
	int aBackColor = ReadColor();
	if(EndOfString())
		return aBackColor;

	EnsureComma();
	GUIString aStr = ReadString();
	if(aStr.compareNoCase("USEFOREGROUND")==0)
	{
		EnsureComma();
		int aForeColor = ReadColor();
		return SelectionColor(aBackColor, aForeColor);
	}
	else if(aStr.compareNoCase("INVERTORIGINAL")==0)
		return SelectionColor(aBackColor,ForegroundSelectionMode_InvertOriginalColor);
	else if(aStr.compareNoCase("USEORIGINAL")==0)
		return SelectionColor(aBackColor,ForegroundSelectionMode_UseOriginalColor);
	else
		throw ConfigObjectException("Invalid SelectionColor");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Background ComponentConfig::ReadBackground()
{
	if(mStringParser->GetChar()=='@')
	{
		mStringParser->IncrementPos();
		std::string aKey;
		mStringParser->ReadString(aKey,true);
		Background aBackground;
		GetBackgroundResource(aKey,aBackground);
		return aBackground;
	}
	else
		return ResourceConfig::SafeReadBackground(*mStringParser);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::PreParse(ConfigParser &theParser)
{
	if(!ConfigObject::PreParse(theParser))
		return false;

	if(!mInheritedName.empty())
		return IncludeGroup(theParser,mInheritedName);
	else
		return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentConfig::SetDesiredSizeIfNecessary()
{
	if(mComponent->Width()>0)
		mHasSetWidth = true;
	if(mComponent->Height()>0)
		mHasSetHeight = true;

	if(mHasSetWidth && mHasSetHeight)
		return;

	int aWidth = 0, aHeight = 0;
	mComponent->GetDesiredSize(aWidth,aHeight);
	if(mHasSetWidth)
	{
		aWidth = mComponent->Width();
		mHasSetWidth = true;
	}
	if(mHasSetHeight)
	{
		aHeight = mComponent->Height();
		mHasSetHeight = true;
	}

	mComponent->SetSize(aWidth,aHeight);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::PostParse(ConfigParser &theParser)
{
	SetDesiredSizeIfNecessary();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::HandleChildGroup(ConfigParser &theParser, const std::string& theGroupType)
{
	if(ConfigObject::HandleChildGroup(theParser,theGroupType))
		return true;

	if(theGroupType=="MODIFY")
		return HandleModifyGroup(theParser);
	else if(theGroupType=="RESOURCE")
		return HandleResourceGroup(theParser);

	ComponentConfigPtr aConfig;
	ComponentPtr aComponent;
	if(!mResourceConfigTable->AllocateConfig(theGroupType,aConfig,aComponent))
		return false;

	aConfig->SetComponent(aComponent);
	bool success = ParseChild(theParser,aConfig,false);

	if(success && !aConfig->GetName().empty())
	{
		aConfig->SetParent(this);
		mConfigMap[aConfig->GetName()] = aConfig;
	}

	return success;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComponentConfig* ComponentConfig::GetComponentConfig(const GUIString &theName, bool throwExceptionOnFailure)
{
	int aDotPos = theName.find('.');
	GUIString aName, aRest;
	if(aDotPos>0)
	{
		aName = theName.substr(0,aDotPos);
		aRest = theName.substr(aDotPos+1);
	}
	else
		aName = theName;
		

	ComponentConfig *aConfig = NULL;
	if(aName.compareNoCase("this")==0)
		aConfig = this;
	else if(aName.compareNoCase("parent")==0)
		aConfig = (ComponentConfig*)GetParent();
	else
	{
		ConfigMap::iterator anItr = mConfigMap.find(aName);
		if(anItr!=mConfigMap.end())
			aConfig = anItr->second;
		else if(aName.at(0)=='_') // special predefined component
		{
			std::string aNameStr = aName.substr(1);
			StringToUpperCaseInPlace(aNameStr);
			Component *aComponent = NULL;
			if(GetPredefinedComponent(aNameStr,aConfig,aComponent) && aConfig!=NULL && aComponent!=NULL)
			if(aConfig!=NULL)
			{
				mConfigMap[aName] = aConfig;
				aConfig->SetComponent(aComponent);
				aConfig->SetParent(this);
			}
		}
	}

	if(aConfig==NULL)
	{
		if(throwExceptionOnFailure)
		{
			string aNameStr = aName;
			throw ConfigObjectException("Component not found: " + aNameStr);
		}
		else
			return NULL;
	}

	if(aRest.empty())
		return aConfig;
	else
		return aConfig->GetComponentConfig(aRest);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* ComponentConfig::GetComponentByName(const GUIString &theName)
{
	ComponentConfig *aConfig = GetComponentConfig(theName,false);
	if(aConfig==NULL)
		return NULL;
	else
		return aConfig->GetComponent();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* ComponentConfig::GetChildComponent(const std::string &theName)
{
	ConfigMap::iterator anItr = mConfigMap.find(theName);
	if(anItr==mConfigMap.end())
		return NULL;

	return anItr->second->GetComponent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComponentConfig* ComponentConfig::GetChildConfig(const std::string &theName)
{
	ConfigMap::iterator anItr = mConfigMap.find(theName);
	if(anItr!=mConfigMap.end())
		return anItr->second;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent)
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComponentConfig* ComponentConfig::ReadComponentConfig()
{
	GUIString aName = ReadString();
	ComponentConfig *aConfig = GetComponentConfig(aName);
	return aConfig;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* ComponentConfig::ReadComponent()
{
	ComponentConfig *aConfig = ReadComponentConfig();
	return aConfig->GetComponent();
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::HandleMapComponent()
{
	ComponentConfig *aConfig = ReadComponentConfig();
	if(!EndOfString())
	{
		EnsureComma();
		GUIString aName = ReadString(); 
		int aDotPos = aName.rFind('.');
		if(aDotPos>=0)
		{
			GUIString aComp = aName.substr(0,aDotPos);
			aName = aName.substr(aDotPos+1);
			ComponentConfig *aMapToConfig = GetComponentConfig(aComp);
			if(aName.empty())
				aName = aConfig->GetName();

			aMapToConfig->mConfigMap[aName] = aConfig;
		}
		mConfigMap[aName] = aConfig;
	}
	else
		aConfig->mConfigMap[GetName()] = this;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentConfig::AddChildToConfigMap(const GUIString &theName, ComponentConfig *theConfig)
{
	if(!theName.empty())
		mConfigMap[theName] = theConfig;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::HandleModifyGroup(ConfigParser &theParser)
{
	std::string aGroupName, anInheritedName;
	if(!ReadGroupStart(theParser,aGroupName,anInheritedName))
		return true;

	ComponentConfig *aConfig = GetComponentConfig(aGroupName);
	aConfig->ParseGroup(theParser,true,false,false,false);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::HandleResourceGroup(ConfigParser &theParser)
{

	std::string aName, anInheritName;
	if(!ReadGroupStart(theParser,aName,anInheritName)) 
		return true;

	bool alreadyExists = false;
	ResourceConfigPtr aResourceConfig = mResourceConfigTable->AddConfig(aName, &alreadyExists);
//	if(alreadyExists)
//		throw ConfigObjectException("Resource already defined: " + aName);
	aResourceConfig->SetAllowOverwrite(alreadyExists);

	return ParseChild(theParser,aResourceConfig,false,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::HandleModify()
{
	ComponentConfig *aConfig = ReadComponentConfig(); EnsureComma();

/*	mStringParser->SkipWhitespace();
	while(!mStringParser->EndOfString())
	{
		int aChar = mStringParser->GetChar();
		if(aChar==':')
			break;

		anInstruction += aChar;
		mStringParser->IncrementPos();
	}

	if(mStringParser->EndOfString())
		throw ConfigObjectException("Expecting ':'");*/


	std::string anInstruction = ReadString(); SkipComma();
	StringToUpperCaseInPlace(anInstruction);

	if(!aConfig->BaseHandleInstruction(*mConfigParser,anInstruction,*mStringParser))
		mConfigParser->AddWarning("Unrecognized instruction: " + anInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::HandleKeyVal(ConfigParser &theParser, const std::string& theKey, StringParser &theVal)
{
	if(ConfigObject::HandleKeyVal(theParser,theKey,theVal))
		return true;

	if(theKey=="SETRESOURCE")
	{
		std::string aName;
		theVal.ReadString(aName);

		mCurResourceConfig = mResourceConfigTable->SafeGetConfig(aName);
		return true;
	}
	else 
		return BaseHandleInstruction(theParser,theKey,theVal);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::BaseHandleInstruction(ConfigParser &theParser, const std::string& theKey, StringParser &theVal)
{
	mConfigParser = &theParser;
	mStringParser = &theVal;

	return HandleInstruction(theKey);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::GetNamedIntValue(const std::string &theName, int &theValue)
{
	if(theName=="LEFT")
		theValue = mComponent->Left();
	else if(theName=="RIGHT")
		theValue = mComponent->Right();
	else if(theName=="TOP")
		theValue = mComponent->Top();
	else if(theName=="BOTTOM")
		theValue = mComponent->Bottom();
	else if(theName=="WIDTH")
		theValue = mComponent->Width();
	else if(theName=="HEIGHT")
		theValue = mComponent->Height();
	else
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::GetNamedIntFunc(const std::string &theName, const IntVector &theParams, int &theResult)
{
	if(theName=="MAX")
	{
		int aMax = 0;
		for(IntVector::const_iterator anItr = theParams.begin(); anItr!=theParams.end(); ++anItr)
		{
			if(*anItr > aMax)
				aMax = *anItr;
		}
		theResult = aMax;
	}
	else if(theName=="MIN")
	{
		int aMin = 0;
		for(IntVector::const_iterator anItr = theParams.begin(); anItr!=theParams.end(); ++anItr)
		{
			if(*anItr < aMin)
				aMin = *anItr;
		}
		theResult = aMin;
	}
	else
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentConfig::HandleInstruction(const std::string &theKey)
{
	if(theKey=="MODIFY")
		return HandleModify();
	else if(theKey=="MAPCOMPONENT")
		return HandleMapComponent();
	else if(theKey=="CONTROLID")
		mComponent->SetControlId(ReadInt());
	else if(theKey=="LEFT")
		mComponent->SetLeft(ReadInt());
	else if(theKey=="TOP")
		mComponent->SetTop(ReadInt());
	else if(theKey=="WIDTH")
	{
		mHasSetWidth = true;
		mComponent->SetWidth(ReadInt());
	}
	else if(theKey=="HEIGHT")
	{
		mHasSetHeight = true;
		mComponent->SetHeight(ReadInt());
	}
	else if(theKey=="POS")
	{
		int x = ReadInt(); EnsureComma();
		int y = ReadInt(); 
		mComponent->SetPos(x,y);
	}
	else if(theKey=="SIZE")
	{
		mHasSetWidth = mHasSetHeight = true;
		int width = ReadInt(); EnsureComma();
		int height = ReadInt(); 
		mComponent->SetSize(width,height);
	}
	else if(theKey=="TRANSLUCENTCOLOR")
	{
		int aColor = ReadColor(); 
		if(EndOfString())
			mComponent->SetTranslucentColor(aColor);
		else
		{
			EnsureComma();
			mComponent->SetTranslucentColor(aColor,ReadInt());
		}
	}
	else if(theKey=="SIZEATLEAST")
	{	
		SetDesiredSizeIfNecessary();

		int width = ReadInt(); EnsureComma();
		int height = ReadInt();
		mHasSetWidth = width>0;
		mHasSetHeight = height>0;
		mComponent->SetSizeAtLeast(width,height);
	}
	else if(theKey=="SETDESIREDSIZE")
	{
		int extraWidth = ReadInt(); EnsureComma(); 
		int extraHeight = ReadInt();
		mComponent->SetDesiredSize(extraWidth, extraHeight);
	}
	else if(theKey=="SETDESIREDWIDTH")
		mComponent->SetDesiredWidth(ReadInt());
	else if(theKey=="SETDESIREDHEIGHT")
		mComponent->SetDesiredHeight(ReadInt());
	else if(theKey=="POSSIZE")
	{
		mHasSetWidth = mHasSetHeight = true;

		int x = ReadInt(); EnsureComma();
		int y = ReadInt(); EnsureComma();
		int width = ReadInt(); EnsureComma();
		int height = ReadInt(); 
		mComponent->SetPosSize(x,y,width,height);
	}
	else if(theKey=="ENABLE")
		mComponent->Enable(ReadBool());
	else if(theKey=="SETVISIBLE")
		mComponent->SetVisible(ReadBool());
	else if(theKey=="REQUESTFOCUS")
	{
		bool doRequest = ReadBool();
		if(doRequest)
			mComponent->RequestFocus();
	}	
	else if(theKey=="DEFAULTCURSOR")
		mComponent->SetDefaultCursor(ReadCursor());
	else if(theKey=="COMPONENTFLAGS")
	{
		while(true)
		{
			bool add = true;
			int aFlag = 0;
			GUIString aStr = ReadFlag(&add);
			if(aStr.empty())
				break;

			if(aStr.compareNoCase("Invisible")==0) aFlag = ComponentFlag_Invisible;
			else if(aStr.compareNoCase("WantFocus")==0) aFlag = ComponentFlag_WantFocus;
			else if(aStr.compareNoCase("ParentDrag")==0) aFlag = ComponentFlag_ParentDrag;
			else if(aStr.compareNoCase("NoParentCursor")==0) aFlag = ComponentFlag_NoParentCursor;
			else if(aStr.compareNoCase("Disabled")==0) aFlag = ComponentFlag_Disabled;
			else if(aStr.compareNoCase("GrabBG")==0) aFlag = ComponentFlag_GrabBG;
			else if(aStr.compareNoCase("Translucent")==0) aFlag = ComponentFlag_Translucent;
			else if(aStr.compareNoCase("NoInput")==0) aFlag = ComponentFlag_NoInput;
			else if(aStr.compareNoCase("AlwaysOnTop")==0) aFlag = ComponentFlag_AlwaysOnTop;
			else if(aStr.compareNoCase("WantTabFocus")==0) aFlag = ComponentFlag_WantTabFocus;
			else if(aStr.compareNoCase("Clip")==0) aFlag = ComponentFlag_Clip;
			else if(aStr.compareNoCase("DebugBG")==0) aFlag = ComponentFlag_DebugBG;
			else 
				throw ConfigObjectException("Unknown component flag: " + (std::string)aStr);

			mComponent->SetComponentFlags(aFlag,add);
		}
	}
	else if(theKey=="DECODEIMAGES")
		Window::GetDefaultWindow()->SetSkipDecodeImages(!ReadBool());
	else if(theKey=="SHOWPARSETIME")
		gShowParseTime = ReadBool();
	else if(theKey=="STARTPARSETIMER")
	{
		ConfigParser::ResetLineCount();
		PerfTimer::GlobalStart();
	}
	else if(theKey=="STOPPARSETIMER")
	{
		PerfTimer::GlobalStop();
		gLinesParsed = ConfigParser::GetLineCount();
	}
	else
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// SetResourceFolder: Set the folder that contains the resource files.  Since
// the working directory can change, it behooves us to get the entire path if
// it isn't supplied.
///////////////////////////////////////////////////////////////////////////////
void ComponentConfig::SetResourceFolder(const std::string &theFolder)
{
	// Make sure we have a trailing slash.
	mResourceFolder = theFolder;
	int nLen = mResourceFolder.length();
	if (nLen>0 && mResourceFolder[nLen - 1] != '/' && mResourceFolder[nLen - 1] != '\\')
		mResourceFolder += '/';
}

///////////////////////////////////////////////////////////////////////////////
// GetResourceFile: Prepend the resource folder to the supplied file name and
// return the resulting file name.
///////////////////////////////////////////////////////////////////////////////
std::string ComponentConfig::GetResourceFile(const std::string &theFileName)
{
	if (theFileName.length() >= 2)	
	{
		if(theFileName[1] == ':') // drive:
			return theFileName;
		else if(theFileName[0] == '.' && (theFileName[1] == '/' || theFileName[1] == '\\'))
			return theFileName.substr(2);
	}
	else if(strnicmp(theFileName.c_str(),"\\\\",2)==0)
	{
		// Network part - We have a complete path.
		return theFileName;
	}
	else if(strnicmp(theFileName.c_str(),"html",4)==0)
	{
		// URL - We have a complete path.
		return theFileName;
	}

	return mResourceFolder + theFileName;
}
