#ifndef __SERVERLISTCTRL_H__
#define __SERVERLISTCTRL_H__

#include "WONGUI/ListBox.h"
#include "WONGUI/Button.h"
#include "WONGUI/Label.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"

enum LobbyResourceId;
namespace WONAPI
{

class ComboSelChangedEvent;
class GameChangedEvent;
class MultiListHeader;
class ServerListCtrl;
class SyncGameListEvent;

enum LobbyGameType;

WON_PTR_FORWARD(ComboBox);
WON_PTR_FORWARD(LobbyGame);
WON_PTR_FORWARD(MultiListArea);
WON_PTR_FORWARD(ServerDetailsItem);
typedef std::list<ServerDetailsItem*> ServerDetailsList;

class ServerListFilterSet; 
typedef std::map<GUIString,ServerListFilterSet,GUIStringLessNoCase> ServerListFilterMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ServerDetailsItem : public ListItem
{
protected:
	typedef ServerDetailsItem*(*ItemFactoryFunc)(void);
	static ItemFactoryFunc mItemFactoryFunc;
	static ServerDetailsItem* DefaultItemFactory() { return new ServerDetailsItem; }

	LobbyGamePtr mGame;

	GUIString mIPAddrStr;
	LobbyResourceId mIconId;
	unsigned char mSummaryHeight;

	ServerDetailsItem();
	virtual ~ServerDetailsItem();

public:
	typedef void(*PaintFunc)(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl);
	typedef int(*CompareFunc)(const ServerDetailsItem *i1,const  ServerDetailsItem *i2, ServerListCtrl *theListCtrl);
	typedef void(*FilterFunc)(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl);

public:
	static ServerDetailsItem* CreateItem() { return mItemFactoryFunc(); }

	void SetGame(LobbyGame *theGame);
	LobbyGame* GetGame() const { return mGame; }

	static void SetItemFactory(ItemFactoryFunc theFunc) { mItemFactoryFunc = theFunc; }

	bool HasDetails();

	static void PaintIcon(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl);
	static void PaintSkill(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl);
	static void PaintGameName(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl);
	static void PaintAddress(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl);
	static void PaintNumPlayers(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl);
	static void PaintPing(Graphics &g, ServerDetailsItem *theItem, ServerListCtrl *theListCtrl);

	static int CompareIcon(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl);
	static int CompareSkill(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl);
	static int CompareGameName(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl);
	static int CompareAddress(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl);
	static int CompareNumPlayers(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl);
	static int ComparePing(const ServerDetailsItem *i1, const ServerDetailsItem *i2, ServerListCtrl *theListCtrl);

	static bool IsStartOfNumber(char c);
	static bool ParseIntFilter(const GUIString &theFilterStr, int &theLowerBound, int &theUpperBound);
	static void FilterSkill(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl);
	static void FilterGameName(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl);
	static void FilterAddress(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl);
	static void FilterNumPlayers(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl);
	static void FilterPing(const GUIString &theFilterStr, ServerDetailsList &theList, ServerListCtrl *theListCtrl);

	void CalcIcon();
	virtual void Paint(Graphics &g, ListArea *theListArea);
	virtual void CalcDimensions(ListArea *theListArea);
	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const;
	virtual void MouseDown(ListArea *theListArea, int x, int y, MouseButton theButton);
	virtual bool CheckMouseSel(ListArea *theListArea, int x, int y);

	virtual void PaintDetailsHook(Graphics &/*g*/, MultiListArea * /*theListArea*/) { }
	virtual int GetDetailsHeightHook(MultiListArea * /*theListArea*/) { return 0; }
	int GetSummaryHeight() { return mSummaryHeight; }

};
typedef SmartPtr<ServerDetailsItem> ServerDetailsItemPtr; 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ServerListHeaderType
{
	ServerListHeaderType_String,
	ServerListHeaderType_Int,
	ServerListHeaderType_Player
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ServerListCtrl : public Container
{
public:
	enum GameTypeFlag
	{
		GameTypeFlag_Internet			= 0x01,
		GameTypeFlag_Lan				= 0x02,
		GameTypeFlag_DirectConnect		= 0x04
	};

	struct ColumnInfo
	{
		ServerDetailsItem::PaintFunc mPaintFunc;
		ServerDetailsItem::CompareFunc mCompareFunc;
		ServerDetailsItem::FilterFunc mFilterFunc;
		ComboBoxPtr mFilterCombo;
		unsigned char mGameTypeFlags;
		GUIString mColumnName;

		ColumnInfo();
		~ColumnInfo();
	};

	typedef std::vector<ColumnInfo>	 ColumnInfoVector;
	ColumnInfoVector mColumnInfo;

	typedef std::map<GUIString,ColumnInfo,GUIStringLessNoCase> ColumnMap;

protected:
	LobbyGameType		mGameType;
	bool mNeedFilter;
	int mFilterTimeAccum;
	int mNumColumns;
	FontPtr mBoldFont;

	MultiListAreaPtr	mList;
	typedef std::vector<ComboBoxPtr> ComboBoxVector;
	ButtonPtr			mJoinButton;
	ButtonPtr			mRefreshButton;
	ComponentPtr		mRemoveButton;
	LabelPtr			mSummaryLabel;


	static ColumnMap mColumnMap;

	typedef std::map<LobbyGamePtr, ServerDetailsItemPtr> ServerDetailsMap;
	ServerDetailsMap mServerDetailsMap;

	
	void LoadFilterCombos();
	void AddFilteredEntries(bool preserveListPos, ServerDetailsList &theList); 
	void AddFilteredEntries_All(bool preserveListPos); 
	void AddFilteredEntries_List(bool preserveListPos); 
	void AddEntries(const ServerDetailsList &theList);
	void FilterInProgressGames(ServerDetailsList &theList);
	void DoFilter(ServerDetailsList &theList);
	bool FitsFilter(ServerDetailsItem *theItem);
	void UpdateJoinButton();
	void UpdateSummaryLabel();

	void HandleComboTextChanged(Component *theCombo);
	void HandleComboSelChanged(ComboSelChangedEvent *theEvent);
	void HandleGameChanged(GameChangedEvent *theEvent);
	void HandleListItemClicked(ListItemClickedEvent *theEvent);
	void HandleSyncGameList(SyncGameListEvent *theEvent);
	bool HandleRefreshButton();

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);
	virtual bool TimerEvent(int theDelta);

	~ServerListCtrl();

public:
	ServerListCtrl();

	void SetGameType(LobbyGameType theType) { mGameType = theType; }	
	void SetNumColumns(int numColumns);

	int GetNumColumns() const { return mNumColumns; } 

	LobbyGame* GetSelGame();
	MultiListArea* GetListArea() { return mList.get(); }
	Font* GetBoldFont() { return mBoldFont; }

	static void InitColumnMap();
	static ColumnInfo* GetColumnInfo(const GUIString &theColumnName);
	static void AddDefaultFilters(const GUIString &theColumn, const GUIString &theStrings, bool addIfColumnExists); // comma delimited list

	friend class ServerListCtrlConfig;
};
typedef SmartPtr<ServerListCtrl> ServerListCtrlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ServerListCtrlConfig : public ContainerConfig
{
protected:
	ServerListCtrl *mServerList;
	MultiListHeader *mHeader;
	int mNumColumns;

	virtual bool GetNamedIntValue(const std::string &theName, int &theValue);
	virtual bool HandleInstruction(const std::string &theInstruction);

	void HandleAddColumn();

public:
	ServerListCtrlConfig();
	virtual void SetComponent(Component *theComponent) { ContainerConfig::SetComponent(theComponent); mServerList = (ServerListCtrl*)theComponent; }

	static ComponentConfig* CfgFactory() { return new ServerListCtrlConfig; }
	static Component* CompFactory() { return new ServerListCtrl; }
};


};

#endif