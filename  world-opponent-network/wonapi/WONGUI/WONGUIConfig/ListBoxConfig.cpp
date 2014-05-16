#include "ListBoxConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListAreaConfig::SetComponent(Component *theComponent)
{
	ScrollAreaConfig::SetComponent(theComponent);
	mListArea = (ListArea*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListSortType ListAreaConfig::ReadSortType()
{
	GUIString aStr = ReadString();
	if(aStr.compareNoCase("ASCENDING")==0)
		return ListSortType_Ascending;
	else if(aStr.compareNoCase("DESCENDING")==0)
		return ListSortType_Descending;
	else if(aStr.compareNoCase("NONE")==0)
		return ListSortType_None;
	else
		throw ConfigObjectException("Invalid list sort type.");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListAreaConfig::ReadListFlags(ComponentConfig *theConfig, ListArea *theListArea)
{
	while(true)
	{
		bool add = true;
		int aFlag = 0;
		GUIString aStr = theConfig->ReadFlag(&add);
		if(aStr.empty())
			break;

		if(aStr.compareNoCase("SelOver")==0) aFlag = ListFlag_SelOver;
		else if(aStr.compareNoCase("SelOnlyWhenOver")==0) aFlag = ListFlag_SelOnlyWhenOver;
		else if(aStr.compareNoCase("ToggleSel")==0) aFlag = ListFlag_ToggleSel;
		else if(aStr.compareNoCase("FullRowSelect")==0) aFlag = ListFlag_FullRowSelect;
		else if(aStr.compareNoCase("HilightIcons")==0) aFlag = ListFlag_HilightIcons;
		else if(aStr.compareNoCase("ScrollFullLine")==0) aFlag = ListFlag_ScrollFullLine;
		else if(aStr.compareNoCase("ShowPartialLines")==0) aFlag = ListFlag_ShowPartialLines;
		else if(aStr.compareNoCase("ClickEventOnMouseUp")==0) aFlag = ListFlag_ClickEventOnMouseUp;
		else if(aStr.compareNoCase("ItemWidthCheckOnSel")==0) aFlag = ListFlag_ItemWidthCheckOnSel;
		else if(aStr.compareNoCase("MultiSelect")==0) aFlag = ListFlag_MultiSelect;
		else if(aStr.compareNoCase("KeyMoveWrapAround")==0) aFlag = ListFlag_KeyMoveWrapAround;
		else if(aStr.compareNoCase("NoAutoAdjustScrollPos")==0) aFlag = ListFlag_NoAutoAdjustScrollPos;
		else if(aStr.compareNoCase("DoListItemToolTip")==0) aFlag = ListFlag_DoListItemToolTip;
		else 
			throw ConfigObjectException("Unknown list flag: " + (std::string)aStr);

		theListArea->SetListFlags(aFlag,add);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ListAreaConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="LISTFLAGS")
	{
		ReadListFlags(this,mListArea);
	}
	else if(theInstruction=="TEXTITEM")
	{
		while(!EndOfString())
		{
			mListArea->InsertString(ReadString());
			if(!EndOfString())
				EnsureComma();
		}
	}
	else if(theInstruction=="FONT")
		mListArea->SetFont(ReadFont());
	else if(theInstruction=="TEXTCOLOR")
		mListArea->SetTextColor(ReadColor());
	else if(theInstruction=="SELCOLOR")
		mListArea->SetSelectionColor(ReadSelectionColor());
	else if(theInstruction=="LISTSORTTYPE")
		mListArea->SetSortType(ReadSortType());
	else
		return ScrollAreaConfig::HandleInstruction(theInstruction);

	return true;
}

