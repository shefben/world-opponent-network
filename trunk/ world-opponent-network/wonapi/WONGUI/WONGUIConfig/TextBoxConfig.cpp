#include "TextBoxConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextAreaConfig::SetComponent(Component *theComponent)
{
	ScrollAreaConfig::SetComponent(theComponent);
	mTextArea = (TextArea*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextAreaConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="TEXTFLAGS")
	{
		while(true)
		{
			bool add = true;
			int aFlag = 0;
			GUIString aStr = ReadFlag(&add);
			if(aStr.empty())
				break;

			if(aStr.compareNoCase("WordWrap")==0) aFlag = TextFlag_WordWrap;
			else if(aStr.compareNoCase("AllowSelection")==0) aFlag = TextFlag_AllowSelection;
			else if(aStr.compareNoCase("ScrollOnBottom")==0) aFlag = TextFlag_ScrollOnBottom;
			else if(aStr.compareNoCase("ShowPartialLines")==0) aFlag = TextFlag_ShowPartialLines;
			else if(aStr.compareNoCase("ScrollFullLine")==0) aFlag = TextFlag_ScrollFullLine;
			else if(aStr.compareNoCase("DelayWordWrap")==0) aFlag = TextFlag_DelayWordWrap;
			else 
				throw ConfigObjectException("Unknown text flag: " + (std::string)aStr);

			mTextArea->SetTextFlags(aFlag,add);
		}
	}
	else if(theInstruction=="TEXT")
		mTextArea->AddSegment(ReadString());
	else if(theInstruction=="FORMATTEDTEXT")
		mTextArea->AddFormatedText(ReadString());
	else if(theInstruction=="FONT")
		mTextArea->SetFont(ReadFont());
	else if(theInstruction=="TEXTCOLOR")
		mTextArea->SetTextColor(ReadColor());
	else if(theInstruction=="SELCOLOR")
		mTextArea->SetSelectionColor(ReadSelectionColor());
	else if(theInstruction=="LINKFONT")
		mTextArea->SetLinkFont(ReadFont());
	else if(theInstruction=="MAXCHARS")
		mTextArea->SetMaxChars(ReadInt());
	else if(theInstruction=="MAXLINES")
		mTextArea->SetMaxLines(ReadInt());
	else if(theInstruction=="WRAPINDENTSIZE")
		mTextArea->SetWrapIndentSize(ReadInt());
	else if(theInstruction=="HORZALIGN")
		mTextArea->SetLineAlignment(ReadHorzAlign());
	else
		return ScrollAreaConfig::HandleInstruction(theInstruction);

	return true;
}

