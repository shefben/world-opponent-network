#include "BadUserDialog.h"
#include "LobbyResource.h"

#include "WONGUI/GUIConfig.h"
#include "WONGUI/MultiListBox.h"
#include "WONRouting/RoutingTypes.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BadUserItem : public ListItem
{
public:
	FontPtr mFont;
	GUIString mColStr[6];
	int mColInt[6];
	bool mIsSimple;

	BadUserItem(const RoutingBadUser &theUser, bool isSimple);

	virtual void Paint(Graphics &g, ListArea *theListArea);
	virtual void CalcDimensions(ListArea *theListArea);
	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BadUserItem::BadUserItem(const RoutingBadUser &theUser, bool isSimple)
{
	char aBuf[100];

	mIsSimple = isSimple;
	mColStr[0] = theUser.mName;

	sprintf(aBuf,"%d",theUser.mWONId);
	mColStr[1] = aBuf;
	mColInt[1] = theUser.mWONId;

	unsigned long aTime = theUser.mExpirationDiff;
	GUIString aTimeStr;
	GUIString aStringId;

	if(aTime>3600*24)
	{
		aTime/=3600*24;
		aStringId = aTime>1?BadUserDialog_XDays_String:BadUserDialog_OneDay_String;
	}
	else if(aTime>3600)
	{
		aTime/=3600;
		aStringId = aTime>1?BadUserDialog_XHours_String:BadUserDialog_OneHour_String;
	}
	else if(aTime>60)
	{
		aTime/=60;
		aStringId = aTime>1?BadUserDialog_XMinutes_String:BadUserDialog_OneMinute_String;
	}
	else if(aTime>0)
		aStringId = aTime>1?BadUserDialog_XSeconds_String:BadUserDialog_OneSecond_String;
	else
		aStringId = BadUserDialog_Infinite_String;

	sprintf(aBuf,"%d",aTime);

	mColInt[2] = theUser.mExpirationDiff;
	mColStr[2] = StringLocalize::GetStr(aStringId,aBuf);

	mColStr[3] = theUser.mModeratorName;

	
	sprintf(aBuf,"%d",theUser.mModeratorWONId);
	mColStr[4] = aBuf;
	mColInt[4] = theUser.mModeratorWONId;

	mColStr[5] = theUser.mModeratorComment;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BadUserItem::CalcDimensions(ListArea *theListArea)
{
	mFont = theListArea->GetFont();
	mHeight = mFont->GetHeight();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BadUserItem::Paint(Graphics &g, ListArea *theListArea)
{
	int aColor = theListArea->GetTextColor();
	if(mSelected)
		aColor = theListArea->GetSelColor().GetForegroundColor(g,aColor);

	int aPaintCol = 0;
	bool drawSelection = mSelected;
	if(!mIsSimple)
	{
		aPaintCol = ((MultiListArea*)theListArea)->GetColumnPaintContext();
		if(aPaintCol!=-1)
			drawSelection = false;
	}
	
	
	if(drawSelection)
	{
		g.SetColor(theListArea->GetSelColor().GetBackgroundColor(g));
		g.FillRect(0,0,theListArea->GetPaintColumnWidth(),mFont->GetHeight());
		if(!mIsSimple)
			return;
	}
	if(aPaintCol<0 || aPaintCol>5)
		return;

	g.SetColor(aColor);
	g.SetFont(mFont);
	g.DrawString(mColStr[aPaintCol],0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int BadUserItem::Compare(const ListItem *theItem, ListArea *theListArea) const
{
	int aSortCol = ((MultiListArea*)theListArea)->GetSortColumn();
	if(aSortCol<0 || aSortCol>5)
		return this<theItem;

	switch(aSortCol)
	{	
		case 1:
		case 2: 
		case 4: 
			return mColInt[aSortCol] - ((BadUserItem*)theItem)->mColInt[aSortCol];

		default:
			return mColStr[aSortCol].compare(((BadUserItem*)theItem)->mColStr[aSortCol]);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BadUserDialog::BadUserDialog()
{
	mIsSimple = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BadUserDialog::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mUserList,"UserList");
		WONComponentConfig_Get(aConfig,mSimpleUserList,"SimpleUserList");
		WONComponentConfig_Get(aConfig,mUserListBox,"UserListBox");
		WONComponentConfig_Get(aConfig,mSimpleUserListBox,"SimpleUserListBox");
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BadUserDialog::SetUserList(const RoutingBadUserList &theList, bool isSimple)
{
	mIsSimple = isSimple;

	mUserListBox->SetVisible(!isSimple);
	mSimpleUserListBox->SetVisible(isSimple);

	ListArea *aList = mIsSimple?mSimpleUserList.get():mUserList.get();
	aList->SetVisible(true);

	aList->BeginMultiChange();

	aList->Clear();

	RoutingBadUserList::const_iterator anItr = theList.begin();
	while(anItr!=theList.end())
	{
		aList->InsertItem(new BadUserItem(*anItr,isSimple));
		++anItr;
	}

	aList->EndMultiChange();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BadUserDialog::GetIds(IdList &theList)
{
	ListArea *aList = mIsSimple?mSimpleUserList.get():mUserList.get();

	theList.clear();
	aList->RewindSelections();
	while(aList->HasMoreSelections())
	{
		BadUserItem *anItem = (BadUserItem *)aList->GetNextSelection();
		theList.push_back(anItem->mColInt[1]);
	}
}

