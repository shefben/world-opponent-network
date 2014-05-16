#include "FontConfig.h"
#include "ResourceConfig.h"
#include "WONGUI/Window.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::FontConfig_Init(ResourceConfigTable *theTable)
{
	theTable->AddAllocator("Font",FontConfig::CfgFactory, FontConfig::CompFactory);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FontConfig::FontConfig()
{
	mHaveInitedComponents = true; // prevent ComponentConfig::InitComponents from crashing
	mNeedRecalcFont = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Font* FontConfig::GetFont()
{
	if(mNeedRecalcFont)
	{
		if(mFont.get()==NULL || mFont->GetDescriptor()!=mDescriptor)
			mFont = Window::GetDefaultWindow()->GetFont(mDescriptor);
	}
	
	return mFont;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FontConfig::SetFont(Font *theFont)
{
	mFont = theFont;
	if(theFont!=NULL)
		mDescriptor = theFont->GetDescriptor();

	mNeedRecalcFont = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FontConfig::GetNamedIntValue(const std::string &theName, int &theValue)
{
	if(mFont.get()==NULL)
		return false;

	if(theName=="SIZE")
		theValue = mDescriptor.mSize;
	else if(theName=="HEIGHT")
		theValue = mFont->GetHeight();
	else
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FontConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SRC")
		SetFont(ReadFont());
	else if(theInstruction=="SIZE")
	{
		mNeedRecalcFont = true;
		mDescriptor.mSize = ReadInt();
	}
	else if(theInstruction=="STYLE")
	{
		while(true)
		{
			mNeedRecalcFont = true;
			bool add = true;
			GUIString aStr = ReadFlag(&add);
			if(aStr.empty())
				break;

			int aStyle = 0;
			if(aStr.compareNoCase("Bold")==0)
				aStyle = FontStyle_Bold;
			else if(aStr.compareNoCase("Italic")==0)
				aStyle = FontStyle_Italic;
			else if(aStr.compareNoCase("Plain")==0)
				aStyle = FontStyle_Plain;
			else if(aStr.compareNoCase("Underline")==0)
				aStyle = FontStyle_Underline;
			else 
				throw ConfigObjectException("Unknown font style: " + (std::string)aStr);
			
			if(add)
				mDescriptor.mStyle |= aStyle;
			else
				mDescriptor.mStyle &= ~aStyle;
		}
	}
	else if(theInstruction=="SETDEFAULTFONT")
		WindowManager::GetDefaultWindowManager()->SetDefaultFont(ReadFont());
	else if(theInstruction=="SETNAMEDFONT")
	{
		GUIString aName = ReadString(); EnsureComma();
		FontPtr aFont = ReadFont();
		WindowManager::GetDefaultWindowManager()->SetNamedFont(aName,aFont);
	}
	else
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FontConfig::PostParse(ConfigParser &theParser)
{
	return true;	// prevent ComponentConfig::PostParse from crashing
}

