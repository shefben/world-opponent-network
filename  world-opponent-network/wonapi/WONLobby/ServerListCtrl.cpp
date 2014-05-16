#include "ServerListCtrl.h"

#include "LobbyEvent.h"
#include "LobbyGame.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"
#include "LobbyMisc.h"
#include "PingPainter.h"

#include "WONCommon/StringUtil.h"
#include "WONGUI/ComboBox.h"
#include "WONGUI/MultiListBox.h"
#include "WONGUI/WindowManager.h"

using namespace WONAPI;

ServerDetailsItem::ItemFactoryFunc ServerDetailsItem::mItemFactoryFunc = ServerDetailsItem::DefaultItemFactory;
ServerListCtrl::ColumnMap ServerListCtrl::mColumnMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FilterListItem : public TextListItem
{
public:
	bool mInsert;
	int mOldItemPos;
	GUIString mOldStr;
	GUIString mColumnName;

	FilterListItem(const GUIString &theColumnName, Font *theFont) : 
		TextListItem(GUIString::EMPTY_STR,theFont), mInsert(false), mOldItemPos(0), mColumnName(theColumnName) { }
};
static int gFilterItemPos = 1;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerDetailsItem::ServerDetailsItem()
{
	mSelectable = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerDetailsItem::~ServerDetailsItem()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::SetGame(LobbyGame *theGame)
{
	mGame = theGame;
	if(mGame->GetGameType()==LobbyGameType_DirectConnect)
		mIPAddrStr = mGame->GetIPAddr().GetHostAndPortString();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::CalcDimensions(ListArea *theListArea)
{
	Font *aFont = theListArea->GetDefaultFont();

	CalcIcon();

	mSummaryHeight = aFont->GetHeight();
	if(ServerListCtrl_GameExpanded_Image->GetHeight() > mSummaryHeight)
		mSummaryHeight = ServerListCtrl_GameExpanded_Image->GetHeight();

	mSummaryHeight+=2;

	mHeight = mSummaryHeight;
	if(mGame->GetDetailsVisible())
	{
		int aDetailsHeight = GetDetailsHeightHook((MultiListArea*)theListArea);
		if(aDetailsHeight < aFont->GetHeight())
			aDetailsHeight = aFont->GetHeight();

		// need to always add details height even if we don't have the details, 
		// otherwise the item won't be assured of being fully visible when the details arrive
		mHeight += aDetailsHeight;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ServerDetailsItem::CheckMouseSel(ListArea *theListArea, int x, int y)
{
	MultiListArea *aListArea = (MultiListArea*)theListArea;
	return mSelectable && x>aListArea->GetColumnWidth(0) && y<=mSummaryHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::MouseDown(ListArea *theListArea, int x, int y, MouseButton theButton) 
{ 
	if(theButton!=MouseButton_Left)
		return;

	// if this is a direct connection, info may not exist yet
	// BILL: Internet games get into weird states where they can be open but the button doesn't draw
	//       if info doesn't exist the game should display empty fields

	//if((GetPing()!=PingValue_Invalid) && (GetPing()!=PingValue_Fail))
	//	return;

	MultiListArea *aListArea = (MultiListArea*)theListArea;
	if(x<=aListArea->GetColumnWidth(0) && y<=mSummaryHeight)
	//x<=mImages[ServerListCtrl_NotExpanded]->GetWidth() && y<=mImages[ServerListCtrl_NotExpanded]->GetHeight())
	{
		mGame->SetDetailsVisible(!mGame->GetDetailsVisible());
		theListArea->UpdateItem(this);
		theListArea->EnsureVisible(this);

		// FireEvent needs to be a send and not a post in this case so that IsPingingDetails() 
		// can be updated before the list draws again.  Fixme: Could this cause any problems?
		if (mGame->GetDetailsVisible())
			theListArea->FireEvent(new PingGameEvent(mGame,true), true); 
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ServerDetailsItem::CompareIcon(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl)
{
	return i1->mIconId - i2->mIconId;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ServerDetailsItem::CompareSkill(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl)
{
	return i1->mGame->GetSkillLevel() - i2->mGame->GetSkillLevel();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ServerDetailsItem::CompareGameName(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl)
{
	return i1->mGame->GetName().compareNoCase(i2->mGame->GetName());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ServerDetailsItem::CompareAddress(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl)
{
	return i1->mGame->GetIPAddr().StringCompare(i2->mGame->GetIPAddr());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ServerDetailsItem::CompareNumPlayers(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl)
{
	int aSortResult = i1->mGame->GetNumPlayers()  - i2->mGame->GetNumPlayers();
	if (aSortResult == 0)
		aSortResult = i2->mGame->GetMaxPlayers() - i1->mGame->GetMaxPlayers();

	return aSortResult;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ServerDetailsItem::ComparePing(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl)
{
	return i1->mGame->GetPing() - i2->mGame->GetPing();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ServerDetailsItem::Compare(const ListItem *theItem, ListArea *theListArea) const
{
	if(this==theItem)
		return 0;

	int aSortCol = ((MultiListArea*)theListArea)->GetSortColumn();
	int aPrevSortCol = ((MultiListArea*)theListArea)->GetSortColumnPrev();

	ServerListCtrl* aServerListCtrl = (ServerListCtrl*)(theListArea->GetParent()->GetParent());

	int aSortResult = 0;

	// Try first column
	CompareFunc aFunc = aServerListCtrl->mColumnInfo[aSortCol].mCompareFunc;
	if(aFunc!=NULL)
		aSortResult = aFunc(this,(ServerDetailsItem*)theItem, aServerListCtrl);
	if(aSortResult!=0)
		return aSortResult;

	// Try second column
	aFunc = aServerListCtrl->mColumnInfo[aPrevSortCol].mCompareFunc;
	if(aFunc!=NULL)
		aSortResult = aFunc(this,(ServerDetailsItem*)theItem, aServerListCtrl);
	if(aSortResult!=0)
		return aSortResult;

	// Break tie using pointer values
	return this-theItem;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ServerDetailsItem::IsStartOfNumber(char c)
{
	return ((safe_isdigit(c)) || (c == '-') || (c == '+'));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ServerDetailsItem::ParseIntFilter(const GUIString &theFilterStr, int &theLowerBound, int &theUpperBound)
{
	char aBuf[512];
	std::string aStr = theFilterStr;
	strncpy(aBuf,aStr.c_str(),510);
	aBuf[511] = '\0';


	bool gotSomething = false;
	const char *aPtr = strtok(aBuf," ");
	while(aPtr!=NULL)
	{
		if(aPtr[0]=='<')
		{
			if(aPtr[1]=='=' && IsStartOfNumber(aPtr[2]))
			{
				theUpperBound = atoi(aPtr+2);
				gotSomething = true;
			}
			else if(IsStartOfNumber(aPtr[1]))
			{
				gotSomething = true;
				theUpperBound = atoi(aPtr+1)-1;
			}
		}
		else if(aPtr[0]=='>')
		{
			if(aPtr[1]=='=' && IsStartOfNumber(aPtr[2]))
			{
				gotSomething = true;
				theLowerBound = atoi(aPtr+2);
			}
			else if(IsStartOfNumber(aPtr[1]))
			{
				gotSomething = true;
				theLowerBound = atoi(aPtr+1)+1;
			}
		}
		else if(aPtr[0]=='=')
		{
			if(IsStartOfNumber(aPtr[1]))
			{
				theLowerBound = theUpperBound = atoi(aPtr+1);
				gotSomething = true;
			}
		}
		else if (IsStartOfNumber(aPtr[0]))
		{
			theLowerBound = theUpperBound = atoi(aPtr);
			gotSomething = true;
		}

		aPtr = strtok(NULL," ");
	}

	return gotSomething;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::FilterSkill(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl)
{
	int aLowerBound = -100000, anUpperBound = 100000;
	if(!ParseIntFilter(theFilterStr,aLowerBound,anUpperBound)) // no arguments
		return;

	ServerDetailsList::iterator anItr = theList.begin();
	while(anItr!=theList.end())
	{
		ServerDetailsItem *anItem = *anItr;
	
		// Does the item pass the filter?
		if ((anItem->mGame->GetSkillLevel() >= aLowerBound) && (anItem->mGame->GetSkillLevel() <= anUpperBound))
			++anItr;
		else
			theList.erase(anItr++);
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::FilterGameName(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl)
{	
	ServerDetailsList::iterator anItr = theList.begin();
	while(anItr!=theList.end())
	{
		ServerDetailsItem *anItem = *anItr;
	
		// Does the item pass the filter?
		if (anItem->mGame->GetName().findNoCase(theFilterStr)>=0)
			++anItr;
		else
			theList.erase(anItr++);
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::FilterAddress(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl)
{
	ServerDetailsList::iterator anItr = theList.begin();
	while(anItr!=theList.end())
	{
		ServerDetailsItem *anItem = *anItr;
	
		// Does the item pass the filter?
		if (anItem->mIPAddrStr.findNoCase(theFilterStr)>=0)
			++anItr;
		else
			theList.erase(anItr++);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::FilterNumPlayers(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl)
{
	int aPos;
	bool full = false;
	bool notfull = false;
	bool open = false;

	aPos = theFilterStr.findNoCase(ServerListCtrl_NotFullFilter_String);
	if(aPos==0 || (aPos>0 && theFilterStr.at(aPos-1)==' '))
		notfull = true;

	aPos = theFilterStr.findNoCase(ServerListCtrl_FullFilter_String);
	if(aPos==0 || (aPos>0 && theFilterStr.at(aPos-1)==' '))
		full = true;

	aPos = theFilterStr.findNoCase(ServerListCtrl_OpenFilter_String);
	if(aPos==0 || (aPos>0 && theFilterStr.at(aPos-1)==' '))
		open = true;

	if(full && notfull)
	{
		theList.clear();
		return;
	}

	int aLowerBound = -100000, anUpperBound = 100000;
	if(!ParseIntFilter(theFilterStr,aLowerBound,anUpperBound) && !full && !notfull && !open) // no arguments
		return;


	// Check each item
	ServerDetailsList::iterator anItr = theList.begin();
	while(anItr!=theList.end())
	{
		ServerDetailsItem *anItem = *anItr;

		int aNumPlayers = anItem->mGame->GetNumPlayers();
		int aMaxPlayers = anItem->mGame->GetMaxPlayers();
		if(aMaxPlayers==0) // unlimited
			aMaxPlayers = 0x7ffffff;

		if(aNumPlayers<aLowerBound || aNumPlayers>anUpperBound)
			theList.erase(anItr++);
		else if((full && aNumPlayers<aMaxPlayers) || (notfull && aNumPlayers>=aMaxPlayers))
			theList.erase(anItr++);
		else if(open && !anItem->mGame->IsOkToJoin())
			theList.erase(anItr++);
		else
			++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::FilterPing(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl)
{
	int aLowerBound = -100000, anUpperBound = 100000;
	if(!ParseIntFilter(theFilterStr,aLowerBound,anUpperBound))
		return;

	// Check each item
	ServerDetailsList::iterator anItr = theList.begin();
	while(anItr!=theList.end())
	{
		ServerDetailsItem *anItem = *anItr;

		int aNumBars = 10 - anItem->mGame->GetPing()/100;

		// Does this item pass the filter?
		if(aNumBars>=aLowerBound && aNumBars<=anUpperBound)
			++anItr;
		else
			theList.erase(anItr++);
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::CalcIcon()
{
	// Draw icon based on precedence
	if (mGame->InProgress())	// only check for game which don't allow join in progress
		mIconId = ServerListCtrl_GameInProgress_Image_Id;
	else if (mGame->IsInviteOnly())
	{
		if (mGame->IAmInvited())
			mIconId = ServerListCtrl_GameAreInvited_Image_Id;
		else 
			mIconId = ServerListCtrl_GameNotInvited_Image_Id;
	}
	else if (mGame->IsAskToJoin())
		mIconId = ServerListCtrl_GameAskToJoin_Image_Id;
	else if (mGame->HasPassword())
		mIconId = ServerListCtrl_GameHasPassword_Image_Id;
	else
		mIconId = ServerListCtrl_GameNone_Image_Id;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void PaintImageVertCenter(Graphics &g, Image *theImage, ServerDetailsItem *theItem, int dx =  0)
{
	g.DrawImage(theImage,dx,(theItem->GetSummaryHeight() - theImage->GetHeight())/2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::PaintIcon(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl)
{
	if(theItem->mGame->GetDetailsVisible())
		PaintImageVertCenter(g,ServerListCtrl_GameExpanded_Image,theItem,15);
	else
		PaintImageVertCenter(g,ServerListCtrl_GameNotExpanded_Image,theItem,15);

	PaintImageVertCenter(g,LobbyResource_GetImage(theItem->mIconId),theItem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::PaintSkill(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl)
{
	unsigned char aSkillLevel = theItem->mGame->GetSkillLevel();
	if(aSkillLevel>=0 && aSkillLevel<LobbySkillLevel_Max)
		PaintImageVertCenter(g,LobbyResource_GetImage((LobbyResourceId)(ServerListCtrl_SkillOpen_Image_Id+aSkillLevel)),theItem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::PaintGameName(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl)
{
	g.SetFont(theListCtrl->GetBoldFont());
	g.DrawString(theItem->mGame->GetName(),5,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::PaintAddress(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl)
{
	g.DrawString(theItem->mIPAddrStr,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::PaintNumPlayers(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl)
{
	char aBuf[50];
	if(theItem->mGame->GetMaxPlayers()>0)
		sprintf(aBuf,"%d/%d",theItem->mGame->GetNumPlayers(),theItem->mGame->GetMaxPlayers());
	else
		sprintf(aBuf,"%d",theItem->mGame->GetNumPlayers());

	GUIString aStr = aBuf;

	int aColWidth = theListCtrl->GetListArea()->GetPaintColumnWidth();
	int aStrWidth = g.GetFont()->GetStringWidth(aStr);
	g.DrawString(aStr,(aColWidth-aStrWidth)/2,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::PaintPing(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl)
{
	LobbyPaintPingBars(g,theItem->mGame->GetPing(),theItem->mSummaryHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerDetailsItem::Paint(Graphics &g, ListArea *theListArea)
{
	MultiListArea *aListArea = (MultiListArea*)theListArea;

	ServerListCtrl* aServerListCtrl = (ServerListCtrl*)(aListArea->GetParent()->GetParent());

	int aPaintCol = aListArea->GetColumnPaintContext();
	int aColor = mGame->InProgress()?g.GetColorSchemeColor(StandardColor_GrayText):g.GetColorSchemeColor(StandardColor_Text);
	if(mSelected)
		aColor = aListArea->GetSelColor().GetForegroundColor(g,aColor);

	g.SetColor(aColor);
	g.SetFont(theListArea->GetFont());

	if(aPaintCol==-1)
	{
		if(mSelected)
		{
			g.SetColor(aListArea->GetSelColor().GetBackgroundColor(g));
			g.FillRect(0,0,aListArea->GetPaintColumnWidth(),mSummaryHeight);
		}

		if(mGame->GetDetailsVisible())
		{
			int anIndent = aListArea->GetColumnWidth(0);
			g.Translate(anIndent,mSummaryHeight);

			if(mGame->GetHasDetails())
				PaintDetailsHook(g,aListArea);
			else
			{
				g.SetFont(theListArea->GetFont());
				g.ApplyColorSchemeColor(StandardColor_Text);
				if(mGame->IsPingingDetails())
					g.DrawString(ServerListCtrl_Querying_String,0,0);
				else
					g.DrawString(ServerListCtrl_QueryFailed_String,0,0);
			}

			g.Translate(-anIndent,-mSummaryHeight);
		}
			
		return;
	}

	PaintFunc aFunc = aServerListCtrl->mColumnInfo[aPaintCol].mPaintFunc;
	if(aFunc!=NULL)
		aFunc(g,this,aServerListCtrl);
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerListCtrl::ServerListCtrl()
{
	mGameType = LobbyGameType_None;
	mNumColumns = 4;

	mFilterTimeAccum = 0;
	mNeedFilter = false;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerListCtrl::~ServerListCtrl()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerListCtrl::ColumnInfo::~ColumnInfo()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerListCtrl::ColumnInfo::ColumnInfo() 
{
	mPaintFunc = NULL;
	mCompareFunc = NULL;
	mFilterFunc = NULL;
	mGameTypeFlags = 0xff;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerListCtrl::ColumnInfo* ServerListCtrl::GetColumnInfo(const GUIString &theColumnName)
{
	ColumnInfo &anInfo = mColumnMap[theColumnName];
	anInfo.mColumnName = theColumnName;
	return &anInfo;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::InitColumnMap()
{
	ColumnInfo *anInfo;

	// Icon
	anInfo = GetColumnInfo("Icon");
	anInfo->mPaintFunc = ServerDetailsItem::PaintIcon;
	anInfo->mCompareFunc = ServerDetailsItem::CompareIcon;

	// Skill
	anInfo = GetColumnInfo("Skill");
	anInfo->mPaintFunc = ServerDetailsItem::PaintSkill;
	anInfo->mCompareFunc = ServerDetailsItem::CompareSkill;
	anInfo->mFilterFunc = ServerDetailsItem::FilterSkill;

	// GameName
	anInfo = GetColumnInfo("GameName");
	anInfo->mPaintFunc = ServerDetailsItem::PaintGameName;
	anInfo->mCompareFunc = ServerDetailsItem::CompareGameName;
	anInfo->mFilterFunc = ServerDetailsItem::FilterGameName;

	// IPAddr
	anInfo = GetColumnInfo("Address");
	anInfo->mPaintFunc = ServerDetailsItem::PaintAddress;
	anInfo->mCompareFunc = ServerDetailsItem::CompareAddress;
	anInfo->mFilterFunc = ServerDetailsItem::FilterAddress;
	anInfo->mGameTypeFlags = GameTypeFlag_DirectConnect;

	// Num Players
	anInfo = GetColumnInfo("NumPlayers");
	anInfo->mPaintFunc = ServerDetailsItem::PaintNumPlayers;
	anInfo->mCompareFunc = ServerDetailsItem::CompareNumPlayers;
	anInfo->mFilterFunc = ServerDetailsItem::FilterNumPlayers;

	// Ping
	anInfo = GetColumnInfo("Ping");
	anInfo->mPaintFunc = ServerDetailsItem::PaintPing;
	anInfo->mCompareFunc = ServerDetailsItem::ComparePing;
	anInfo->mFilterFunc = ServerDetailsItem::FilterPing;
	anInfo->mGameTypeFlags &= ~GameTypeFlag_Lan;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::AddDefaultFilters(const GUIString &theColumn, const GUIString &theStrings, bool addIfColumnExists)
{
	LobbyPersistentData::AddDefaultServerListFilters(theColumn,theStrings,addIfColumnExists);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;
	
		WONComponentConfig_Get(aConfig,mList,"ServerList");
		mBoldFont = mList->GetDefaultFontMod(FontStyle_Bold);

		WONComponentConfig_Get(aConfig,mJoinButton,"JoinGameButton");
		WONComponentConfig_Get(aConfig,mRefreshButton,"RefreshListButton");
		WONComponentConfig_Get(aConfig,mSummaryLabel,"ServerListSummary");
		mRemoveButton = aConfig->GetChildComponent("RemoveButton");

		UpdateJoinButton();
		UpdateSummaryLabel();
		SubscribeToBroadcast(true);		
	}
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ServerListCtrl::TimerEvent(int theDelta)
{
	if(mNeedFilter)
	{
		mFilterTimeAccum += theDelta;
		if(mFilterTimeAccum >= 100) // if less than 100ms then the user is still typing
		{
			// wait time is proportional to the size of the list but never greater than 500 ms
			if(mFilterTimeAccum >= 500 || mFilterTimeAccum>=mServerDetailsMap.size()/4) 
			{
				AddFilteredEntries_All(true);
				mNeedFilter = false;
			}
		}
	}

	if(!mNeedFilter)
		mTimerFlags &= ~TimerFlag_IWant;

	return Container::TimerEvent(theDelta);
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::LoadFilterCombos()
{
	for(ColumnInfoVector::iterator anItr = mColumnInfo.begin(); anItr!=mColumnInfo.end(); ++anItr)
	{
		ComboBox *aCombo = anItr->mFilterCombo;
		if(aCombo==NULL)
			continue;

		aCombo->Clear();
		aCombo->InsertItem(new TextListItem(ServerListCtrl_NoFilter_String,mBoldFont)); 
		aCombo->InsertItem(new FilterListItem(anItr->mColumnName,mBoldFont));

		const ServerListFilterSet &aSet = LobbyPersistentData::GetServerListFilterSet(anItr->mColumnName);
		ServerListFilterSet::const_iterator anItr = aSet.begin();
		while(anItr!=aSet.end())
		{
			aCombo->InsertString(*anItr);
			++anItr;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::FilterInProgressGames(ServerDetailsList &theList)
{
	ServerDetailsList::iterator anItr = theList.begin();
	while(anItr!=theList.end())
	{
		ServerDetailsItem *anItem = *anItr;
	
		// Does the item pass the filter?
		if (anItem->GetGame()->InProgress())
			theList.erase(anItr++);
		else
			++anItr;
	}		
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::DoFilter(ServerDetailsList &theList)
{
	// Remember the selection before applying filter
	LobbyGamePtr aSelGame = GetSelGame();

	for(int i=0; i<mColumnInfo.size(); i++)
	{
		ColumnInfo &anInfo = mColumnInfo[i];
		if(anInfo.mFilterFunc!=NULL && anInfo.mFilterCombo.get()!=NULL && !anInfo.mFilterCombo->GetText().empty())
			anInfo.mFilterFunc(anInfo.mFilterCombo->GetText(), theList, this);
	}

	// Filter in 
	if(mGameType!=LobbyGameType_DirectConnect && !LobbyPersistentData::GetShowGamesInProgress())
		FilterInProgressGames(theList);
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ServerListCtrl::FitsFilter(ServerDetailsItem *theItem)
{
	ServerDetailsList aList;
	aList.push_back(theItem);
	DoFilter(aList);
	return !aList.empty();
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::UpdateJoinButton()
{
	bool enable = false;
	LobbyGame *aGame = GetSelGame();
	if(aGame!=NULL)
		enable = aGame->IsOkToJoin();

	mJoinButton->Enable(enable);

	if (mRemoveButton.get())
		mRemoveButton->Enable(aGame!=NULL);
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::UpdateSummaryLabel()
{
	if(mSummaryLabel.get()==NULL)
		return;

	char aTotalCtBuf[33], aSubCtBuf[33];
	sprintf(aTotalCtBuf, "%d", mServerDetailsMap.size());

	if (LobbyMisc::GetNumPingsToDo() > 0)
	{
		sprintf(aSubCtBuf, "%d", LobbyMisc::GetNumPingsToDo());
		mSummaryLabel->SetText(StringLocalize::GetStr(ServerListCtrl_ListPingSummary_String,aSubCtBuf));
		mRefreshButton->SetText(ServerListCtrl_StopRefresh_String);
	}
	else
	{
		sprintf(aSubCtBuf, "%d", mList->GetNumItems());
		mSummaryLabel->SetText(StringLocalize::GetStr(ServerListCtrl_ListSummary_String,aSubCtBuf,aTotalCtBuf));
		mRefreshButton->SetText(ServerListCtrl_RefreshList_String);
	}

}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::AddEntries(const ServerDetailsList &theList)
{
	LobbyGame *aGame = GetSelGame();
	mList->BeginMultiChange();
	mList->Clear();
	ServerDetailsList::const_iterator anItr = theList.begin();
	while(anItr!=theList.end())
	{
		ServerDetailsItem *anItem = *anItr;
		mList->ListArea::InsertItem(anItem);
		anItem->GetGame()->SetIsFiltered(false);

		if(anItem->GetGame()==aGame)
		{
			int pos = mList->GetItemPos(anItem);
			mList->SetSelItem(pos);
		}

		++anItr;
	}

	mList->EndMultiChange();
	UpdateJoinButton();
	UpdateSummaryLabel();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::AddFilteredEntries(bool preserveListPos, ServerDetailsList &theList)
{
	mNeedFilter = false;

	// Get visible items
	typedef std::pair<ListItem*,int> ListItemPos;
	typedef std::list<ListItemPos> ListItemPosList;
	ListItemPosList aVisibleList;
	if(preserveListPos) 
	{
		int aTop = mList->GetVertOffset();
		int aBottom = mList->GetVertOffset() + mList->GetScrollArea().Height();
		int aPos = mList->GetItemPosAtYPos(aTop);
		ListItem *aFirstItem = mList->GetItem(aPos);
		if(aFirstItem!=NULL && (aFirstItem->my + aFirstItem->mHeight)/2 < aTop) // make sure item is at least halfway visible
			aPos++;

		while(true)
		{
			ListItem *aVisibleItem = mList->GetItem(aPos++);
			if(aVisibleItem==NULL)
				break;

			if(aVisibleItem->my > aBottom)
				break;

			aVisibleList.push_back(ListItemPos(aVisibleItem,aVisibleItem->my - aTop));
		}
	}


	DoFilter(theList);
	AddEntries(theList);

	// make sure if one of the visible items is still in the list then it is  still showing
	if(preserveListPos) 
	{
		for(ListItemPosList ::iterator anItr = aVisibleList.begin(); anItr!=aVisibleList.end(); ++anItr)
		{
			ListItem *anItem = anItr->first;
			if(mList->GetItemPos(anItem)>=0)
			{
				mList->SetVertOffset(anItem->my - anItr->second); // keep at relatively same position
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::AddFilteredEntries_All(bool preserveListPos)
{
	// Filter entries
	ServerDetailsList aList;
	for(ServerDetailsMap::iterator anItr = mServerDetailsMap.begin(); anItr!=mServerDetailsMap.end(); ++anItr)
	{
		anItr->second->GetGame()->SetIsFiltered(true); // will set to fale in addentries
		aList.push_back(anItr->second);
	}

	AddFilteredEntries(preserveListPos,aList);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::AddFilteredEntries_List(bool preserveListPos)
{
	// Filter entries
	ServerDetailsList aList;
	int aNumItems = mList->GetNumItems();
	for(int i=0; i<aNumItems; i++)
		aList.push_back((ServerDetailsItem*)mList->GetItem(i));

	AddFilteredEntries(preserveListPos,aList);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame* ServerListCtrl::GetSelGame()
{
	ServerDetailsItem *anItem = (ServerDetailsItem*)mList->GetSelItem();
	if(anItem==NULL)
		return NULL;
	else
		return anItem->GetGame();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::HandleGameChanged(GameChangedEvent* theEvent)
{
	if(theEvent->mGame->GetGameType()!=mGameType)
		return;

	LobbyGame *aGame = theEvent->mGame;
	if(theEvent->IsAdd())
	{
		ServerDetailsItemPtr anItem = ServerDetailsItem::CreateItem();
		anItem->SetGame(aGame);
		mServerDetailsMap[aGame] = anItem;

		bool fitsFilter = FitsFilter(anItem);
		aGame->SetIsFiltered(!fitsFilter);
		if(fitsFilter)
			mList->ListArea::InsertItem(anItem);
	}
	else if(theEvent->IsDelete())
	{
		ServerDetailsMap::iterator anItr = mServerDetailsMap.find(aGame);
		if(anItr!=mServerDetailsMap.end())
		{
			mList->DeleteItem(anItr->second);
			mServerDetailsMap.erase(anItr);
		}
	}
	else	// modify
	{
		ServerDetailsMap::iterator anItr = mServerDetailsMap.find(aGame);
		if(anItr==mServerDetailsMap.end())
			return;

		ServerDetailsItem *anItem = anItr->second;
		bool inList = mList->GetItemPos(anItem)>=0;
		bool fitsFilter = FitsFilter(anItem);
		anItem->GetGame()->SetIsFiltered(!fitsFilter);

		if(inList)
		{
			if(fitsFilter)
				mList->ListArea::UpdateItem(anItem);
			else
				mList->ListArea::DeleteItem(anItem);
		}
		else // not in list
		{
			if(fitsFilter)
				mList->ListArea::InsertItem(anItem);
		}
	}

	UpdateJoinButton();
	UpdateSummaryLabel();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::HandleSyncGameList(SyncGameListEvent* theEvent)
{
	if(theEvent->mGameList->GetGameType()!=mGameType)
		return;

	LobbyGameList *aGameList = theEvent->mGameList;
	aGameList->Rewind();

	mServerDetailsMap.clear();
	while(aGameList->HasMoreGames())
	{
		LobbyGame *aGame = aGameList->NextGame();
		ServerDetailsItemPtr anItem = ServerDetailsItem::CreateItem();
		anItem->SetGame(aGame);
		mServerDetailsMap[aGame] = anItem;
	}

	AddFilteredEntries_All(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::HandleListItemClicked(ListItemClickedEvent* theEvent)
{
	if(!theEvent->IsSelectedItem())
		return;

	ServerDetailsItem* anItem = (ServerDetailsItem*)mList->GetSelItem();
	if (anItem == NULL)
		return;

	
	LobbyGame *aGame = anItem->GetGame();
	if(theEvent->mButton==MouseButton_Right)
	{
		FireEvent(new GameActionEvent(LobbyGameAction_ShowMenu, aGame));
		return;
	}
	else if(theEvent->mWasDoubleClick && theEvent->mButton==MouseButton_Left)
	{
		if (theEvent->mItemX<mList->GetColumnWidth(0)) // clicking the open/close arrow
			return;

		if(aGame->IsOkToJoin())
			FireEvent(new JoinGameEvent(aGame));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::HandleComboTextChanged(Component *theCombo)
{
	ComboBox *aCombo = dynamic_cast<ComboBox*>(theCombo);
	if(aCombo==NULL)
		return;

	// Don't filter immediately.  
	mNeedFilter = true;
	mFilterTimeAccum = 0;
	RequestTimer(true);

	const GUIString &aText = aCombo->GetText();
	FilterListItem *anItem = (FilterListItem*)aCombo->GetItem(gFilterItemPos);
	if(anItem==NULL)
		return;

	if(aText.empty())
	{
		anItem->mText = ServerListCtrl_AddRemove_String; // "(Add/Remove)"; 
		anItem->mInsert = true;
		anItem->mOldStr = "";
		anItem->mOldItemPos = 0;
		aCombo->UpdateItem(anItem);
		return;
	}

		
	int aNumItems = aCombo->GetNumItems();
	for(int i=0; i<aNumItems; i++)
	{
		if(aCombo->GetItem(i)->mText==aText)
		{
			if(i<=gFilterItemPos)
				return;
			else
				break;

			break;
		}
	}

	anItem->mInsert = i>=aNumItems;
	anItem->mOldItemPos = i;
	anItem->mOldStr = aText;

	GUIString aStr;
	if(anItem->mInsert)
		aStr = ServerListCtrl_Add_String; // "(Add \""; 
	else
		aStr = ServerListCtrl_Remove_String; //"(Remove \"";

	aStr.append(aText);
	aStr.append("\")");
	anItem->mText = aStr;
	aCombo->UpdateItem(anItem);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::HandleComboSelChanged(ComboSelChangedEvent *theEvent)
{
	ComboBox *aCombo = dynamic_cast<ComboBox*>(theEvent->mComponent);
	if(aCombo==NULL)
		return;


	int anItemPos = aCombo->GetItemPos(theEvent->mItem);
	if(anItemPos>gFilterItemPos) 
		return;
	
	if(anItemPos==0 && theEvent->mActiveSel) // clear filter
	{
		aCombo->SetText("");
		return;
	}
	
	FilterListItem *anItem = (FilterListItem*)aCombo->GetItem(gFilterItemPos);
	if(theEvent->mActiveSel) // add/remove
	{
		if(anItem->mInsert)
		{
			if(!anItem->mOldStr.empty())
			{
				ListItem *aNewItem = aCombo->InsertString(anItem->mOldStr);
				aCombo->SetSelItem(aCombo->GetItemPos(aNewItem));
				LobbyPersistentData::AddServerListFilter(anItem->mColumnName,anItem->mOldStr);
			}
			else
				aCombo->SetText("");
		}
		else
		{
			if(anItem->mOldItemPos>gFilterItemPos)
			{
				aCombo->DeleteItem(anItem->mOldItemPos);
				aCombo->SetText("");
				LobbyPersistentData::RemoveServerListFilter(anItem->mColumnName,anItem->mOldStr);
			}
		}
	}
	else
	{
		aCombo->SetText(anItem->mOldStr); // set text back so it doesn't say insert '...' or remove '...'
		aCombo->GetInputBox()->SetSel(-1,0);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ServerListCtrl::HandleRefreshButton()
{
	if(LobbyMisc::GetNumPingsToDo() > 0) // stop refresh
	{
		LobbyMisc::KillPings(true);
		AddFilteredEntries_List(true);
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_GameChanged: HandleGameChanged((GameChangedEvent*)theEvent); return;
		case LobbyEvent_SyncGameList: HandleSyncGameList((SyncGameListEvent*)theEvent); return;
		case LobbyEvent_EnteredScreen: 
		case LobbyEvent_LoadFilterCombos:
			LoadFilterCombos(); 
			return;

		case LobbyEvent_ResyncGameList:
			AddFilteredEntries_All(true);
			return;

		case LobbyEvent_SyncFilteredGameList:
			AddFilteredEntries_List(true);
			return;

		case ComponentEvent_ButtonPressed:
			if(theEvent->mComponent==mJoinButton.get())
			{
				LobbyGame *aGame = GetSelGame();
				if(aGame!=NULL)
					FireEvent(new JoinGameEvent(aGame));

				return;
			}
			else if(theEvent->mComponent==mRefreshButton.get())
			{
				if(HandleRefreshButton())
					return;
				else
					break;
			}
			break;
			
		case ComponentEvent_ListSelChanged: UpdateJoinButton(); break;
		case ComponentEvent_InputTextChanged: HandleComboTextChanged(theEvent->mComponent); return;		
		case ComponentEvent_ComboSelChanged: HandleComboSelChanged((ComboSelChangedEvent*)theEvent); return;		
		case ComponentEvent_ListItemClicked: HandleListItemClicked((ListItemClickedEvent*)theEvent); return;
	}

	Container::HandleComponentEvent(theEvent);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrl::SetNumColumns(int numColumns)
{
	mNumColumns = numColumns;
	mColumnInfo.resize(numColumns);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerListCtrlConfig::ServerListCtrlConfig()
{	
	mHeader = NULL;
	mNumColumns = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ServerListCtrlConfig::GetNamedIntValue(const std::string &theName, int &theValue)
{
	if(theName=="NUMCOLUMNS")
		theValue = mServerList->mColumnInfo.size();
	else
		return ComponentConfig::GetNamedIntValue(theName, theValue);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListCtrlConfig::HandleAddColumn()
{
	MultiListArea *aList = mServerList->GetListArea();
	if(aList==NULL)
	{
		Component *aListComp = GetChildComponent("ServerList");
		if(aListComp==NULL)
			throw ConfigObjectException("Need ServerList before calling AddColumn");

		aList = dynamic_cast<MultiListArea*>(aListComp);
		if(aList==NULL)
			throw ConfigObjectException("ServerList needs to be a MultiListArea");

		mServerList->mList = aList;

		Component *aHeaderComp = GetChildComponent("ServerListHeader");
		if(aHeaderComp==NULL)
			throw ConfigObjectException("Need ServerList header before calling AddColumn");

		mHeader = dynamic_cast<MultiListHeader*>(aHeaderComp);
		if(mHeader==NULL)
			throw ConfigObjectException("ServerListHeader needs to be a MultiListHeader");
	}
	

	GUIString aColName = ReadString(); EnsureComma();
	Component *aComponent = GetChildComponent(aColName); 
	if(aComponent==NULL)
		throw ConfigObjectException("Component not found: " + (std::string)aColName);

	int aColumnWidth = ReadInt(); 
	ComboBox *aCombo = NULL;
	if(!EndOfString())
	{
		EnsureComma();
		aCombo = SafeConfigGetComponent<ComboBox*>(ReadComponent(),"ComboBox");
	}	

	ServerListCtrl::ColumnMap::iterator anItr = mServerList->mColumnMap.find(aColName);
	if(anItr==mServerList->mColumnMap.end())
		throw ConfigObjectException("Column not defined: " + (std::string)aColName);

	ServerListCtrl::ColumnInfo &anInfo = anItr->second;
	unsigned char aGameFlags = anInfo.mGameTypeFlags;
	switch(mServerList->mGameType)
	{
		case LobbyGameType_Lan: if(aGameFlags&ServerListCtrl::GameTypeFlag_Lan) break; else return;
		case LobbyGameType_Internet: if(aGameFlags&ServerListCtrl::GameTypeFlag_Internet) break; else return;
		case LobbyGameType_DirectConnect: if(aGameFlags&ServerListCtrl::GameTypeFlag_DirectConnect) break; else return;
		default: return;
	}

	mNumColumns++;
	aList->SetNumColumns(mNumColumns);
	
	if(aColumnWidth==0)
		aList->SetDynamicSizeColumn(mNumColumns-1);
	else
		aList->SetColumnWidth(mNumColumns-1,aColumnWidth);

	mHeader->SetColumnItem(mNumColumns-1,aComponent);
 
	mServerList->mColumnInfo.push_back(anItr->second);
	mServerList->mColumnInfo.back().mFilterCombo = aCombo;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ServerListCtrlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(ContainerConfig::HandleInstruction(theInstruction))
		return true;

	if(theInstruction=="LISTID")
	{
		GUIString aStr = ReadString();
		if(aStr.compareNoCase("net")==0)
			mServerList->SetGameType(LobbyGameType_Internet);
		else if(aStr.compareNoCase("lan")==0)
			mServerList->SetGameType(LobbyGameType_Lan);
		else if(aStr.compareNoCase("direct")==0)
			mServerList->SetGameType(LobbyGameType_DirectConnect);
		else
			throw ConfigObjectException("Invalid server list id: " + (std::string)aStr);
	}
	else if(theInstruction=="ADDCOLUMN")
	{
		HandleAddColumn();
	}	
	else if (theInstruction=="DEFAULTFILTER")
	{
		GUIString aColumnName = ReadString(); EnsureComma();
		GUIString aFilterList = ReadString();
		ServerListCtrl::AddDefaultFilters(aColumnName,aFilterList,false);
	}
	else if (theInstruction=="NUMCOLUMNS")
	{
		mServerList->SetNumColumns(ReadInt());
	}
	else if (theInstruction=="SETCOLUMNFILTERTYPE")
	{
		int aColumnNum = ReadInt();
		EnsureComma();

		ServerListHeaderType aType = ServerListHeaderType_String;

		GUIString aStr = ReadString();
		if (aStr.compareNoCase("int")==0)
			aType = ServerListHeaderType_Int;
		else if (aStr.compareNoCase("string")==0)
			aType = ServerListHeaderType_String;
		else if (aStr.compareNoCase("player")==0)
			aType = ServerListHeaderType_Player;
		else 
			throw ConfigObjectException("Unknown filter type: " + (std::string)aStr);

/*		if (aColumnNum < mServerList->GetNumColumns())
			mServerList->mHeaderTypeVector[aColumnNum] = aType;
		else
			throw ConfigObjectException("Type specified for column number above NumColumns: " + (std::string)aStr);*/
	}
	else
		return false;
		
	return true;
}
