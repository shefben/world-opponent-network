#include "WONGUI/GUICompat.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/ResourceCollection.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/jpeglib/JPGDecoder.h"
#include "WONGUI/pnglib/PNGDecoder.h"
#include "WONGUI/MenuBar.h"
#include "WONGUI/TextBox.h"
#include "WONConfig/ConfigParser.h"
#include "WONCommon/WONFile.h"
#include "ResConfig.h"
#include "HexView.h"
#include "ResourcePackerWRS.h"
#include "resource.h"

#include <commdlg.h>


using namespace WONAPI;
using namespace std;

PlatformWindowManagerPtr gWindowMgr;
PlatformWindowPtr gWindow;
PlatformWindowPtr gPreviewWindow;
WONFile gCurResourceFile;
PopupMenuPtr gEditMenu;

const int NUM_LISTS = 5;
MultiListArea *gLists[NUM_LISTS];
Container* gPreviewContainers[NUM_LISTS];
const char *gTypePrefixes[NUM_LISTS] = {"IMG_","STR_","BIN_","CLR_","FNT_"};
TabCtrl* gTabCtrl;
ResourceCollectionWriter gResourceCollection;
NativeImagePtr gListImages[3];

ResourceCollection gResources;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString GetStr(int theId)
{
	return gResources.GetString(theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString GetStr(int theId, GUIString theParam1)
{
	GUIString aStr = gResources.GetString(theId);
	GUIString aNewStr(aStr.length());

	int anOldPos = 0, aNewPos = 0;
	while(anOldPos<aStr.length())
	{
		aNewPos = aStr.find('%',anOldPos);
		if(aNewPos<0)
			break;

		aNewPos++;
		int aNextChar = aStr.at(aNewPos);
		if(aNextChar=='%') // literal %
		{
			aNewStr.append(aStr,anOldPos,aNewPos-anOldPos);
			aNewPos++; // slip second percent
		}
		else if(aNextChar=='s' || aNextChar=='S') // string substitution
		{
			aNewStr.append(aStr,anOldPos,aNewPos-anOldPos-1);
			aNewPos++;
			int anId = 0;
			while(aNewPos<aStr.length())
			{
				int aChar = aStr.at(aNewPos);
				if(aChar>='0' && aChar<='9')
				{
					anId = anId*10 + aChar-'0';			
					aNewPos++;
				}
				else
					break;
			}

			if(anId==0 || anId==1)
				aNewStr.append(theParam1);
		}
		else
			aNewPos++;

		anOldPos = aNewPos;
	}

	if(anOldPos<aStr.length())
		aNewStr.append(aStr,anOldPos);

	return aNewStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ValidateMenuSelection(ComponentEvent *theEvent)
{
	return theEvent==NULL || theEvent->mType==ComponentEvent_ButtonPressed || theEvent->mType==ComponentEvent_MenuItemSelected;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool UpperInput(InputBox *theInput, GUIString &theText)
{
	theText.toUpperCase();
	return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SetAppName(const GUIString &theStr)
{
	GUIString aName = GetStr(STR_APP_NAME_DASH);
	aName.append(theStr);
	gWindow->SetTitle(aName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CheckModified()
{
	GUIString aName = gCurResourceFile.GetFileName();
	if(gResourceCollection.IsModified())
		aName.append(" *");

	SetAppName(aName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString GetTempName(int theListId)
{
	if(theListId<0 || theListId>=NUM_LISTS)
		return "";

	const char *aPre = gTypePrefixes[theListId];	
	int aNum = gLists[theListId]->GetNumItems() + 1;
	while(true)
	{
		char aBuf[50];
		sprintf(aBuf,"%s%d",aPre,aNum);
		if(gResourceCollection.GetResource(aBuf,false)==NULL)
			return aBuf;
		else
			aNum++;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SetResourceFile(const GUIString &thePath)
{
	gCurResourceFile.SetFilePath(thePath);
	gCurResourceFile.SetWorkingDirectory();
	SetAppName(gCurResourceFile.GetFileName());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ClearLists()
{
	for(int i=0; i<NUM_LISTS; i++)
		gLists[i]->Clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnSave(ComponentEvent *theEvent);
bool CheckModifyPrompt()
{
	if(gResourceCollection.IsModified())
	{	
		// Prompt
		int aResult = MSMessageBox::Msg(gWindow,GetStr(STR_SAVE_PROMPT_TITLE),GetStr(STR_SAVE_PROMPT_MSG),
			GetStr(STR_YES),GetStr(STR_NO),GetStr(STR_CANCEL));

		if(aResult==1)
			OnSave(NULL);
		else if(aResult==3)
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CloseResourceFile()
{
	gResourceCollection.Close();
	ClearLists();
	SetResourceFile("");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AllowCloseFunc(Window *theWindow)
{
	return CheckModifyPrompt();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString GetSizeString(const ResourceEntry* theEntry)
{
	int aSize = theEntry->mFileSize;
	if(theEntry->mType==ResourceType_String)
		aSize = theEntry->mDesc.length()*2;

	char aBuf[1024];
	if(aSize < 1024)
		sprintf(aBuf,"%d B",aSize);
	else if(aSize < 1024*1024)
		sprintf(aBuf,"%d K",aSize/1024);
	else
		sprintf(aBuf,"%d M",aSize/(1024*1024));

	return aBuf;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MyListItem : public ListItem
{
public:
	ConstResourceEntryPtr mEntry;
	GUIString mDesc;
	FontPtr mFont;
	int mColor;

	MyListItem(const ResourceEntry *theEntry) : mEntry(theEntry) 
	{ 
		mFont = gWindowMgr->GetNamedFont("ListArea"); 
	}

	int GetIconId() const;
	virtual void Paint(Graphics &g, ListArea *theListArea);
	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const;
	virtual void CalcDimensions(ListArea *theListArea);
};
typedef SmartPtr<MyListItem> MyListItemPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MyListItem::CalcDimensions(ListArea *theListArea)
{
	if(mEntry->mType==ResourceType_String)
		mDesc = ResourceCollection::OutStringFilter(mEntry->mDesc);	
	else
		mDesc = mEntry->mDesc;

	mHeight = mFont->GetHeight();
	if(mHeight<gListImages[0]->GetHeight()+2)
		mHeight = gListImages[0]->GetHeight()+2;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MyListItem::GetIconId() const
{
	int anImageId = 0;
	if(mEntry->mUpdateType==ResourceUpdateType_Update)
		anImageId = 1;
	else if(mEntry->mUpdateType==ResourceUpdateType_Remove)
		anImageId = 2;
	else if((mEntry->mType!=ResourceType_String && mEntry->mType!=ResourceType_Color && mEntry->mType!=ResourceType_Font) && mEntry->mFileSize==0)
		anImageId = 1;

	return anImageId;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MyListItem::Paint(Graphics &g, ListArea *theListArea)
{
	MultiListArea *aListArea = (MultiListArea*)theListArea;
	int aColumnId = aListArea->GetColumnPaintContext();
	g.SetFont(mFont);
	int aColor = aListArea->GetTextColor();
	if(mSelected)
		g.SetColor(aListArea->GetSelColor().GetForegroundColor(g,aColor));
	else
		g.SetColor(aColor);
	
	int aYPos = (mHeight - mFont->GetHeight())/2;
	switch(aColumnId)
	{
		case -1:
			if(mSelected)
			{
				g.SetColor(aListArea->GetSelColor().GetBackgroundColor(g)); 
				g.FillRect(0,0,aListArea->GetPaintColumnWidth(),mHeight);
			}
			break;

		case 0:
		{
			int anImageId = GetIconId();
			aYPos = (mHeight-gListImages[anImageId]->GetHeight())/2;
			g.DrawImage(gListImages[anImageId],0,aYPos);
			break;
		}

		case 1: g.DrawString(mEntry->mName,0,aYPos); break;
		case 2: 
		{
			char aBuf[50];
			sprintf(aBuf,"%d",mEntry->mId);
			g.DrawString(aBuf,0,aYPos); 
		}
		break;
		
		case 3: g.DrawString(GetSizeString(mEntry),0,aYPos); break;
		case 4: g.DrawString(mDesc,0,aYPos); break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MyListItem::Compare(const ListItem *theItem, ListArea *theListArea) const 
{
	MultiListArea *aListArea = (MultiListArea*)theListArea;
	MyListItem *anItem = (MyListItem*)theItem;
	int aColumnId = aListArea->GetSortColumn();
	switch(aColumnId)
	{
		case 0: return GetIconId() - anItem->GetIconId(); break;
		case 1: return mEntry->mName.compareNoCase(anItem->mEntry->mName); break;
		case 2: return mEntry->mId - anItem->mEntry->mId; break;
		case 3: return mEntry->mFileSize - anItem->mEntry->mFileSize; break;
		case 4: return mDesc.compareNoCase(anItem->mDesc); break;	
		default: return this-theItem;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GetCurListId()
{
	return gTabCtrl->GetCurTabId();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiListArea* GetList(int theId)
{
	if(theId<0 || theId>=NUM_LISTS)
		return NULL;

	return gLists[theId];
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiListArea* GetCurList()
{
	return GetList(GetCurListId());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InsertListItem(MultiListArea *theList, const ResourceEntry *theEntry, bool isPopulate = false)
{
	if(theList==NULL)
		return;

	MyListItemPtr anItem = new MyListItem(theEntry);
	theList->ListArea::InsertItem(anItem);
	if(isPopulate)
	{
		int aWidth = anItem->mFont->GetStringWidth(anItem->mEntry->mName) + 10;
		if(aWidth > theList->GetColumnWidth(1))
			theList->SetColumnWidth(1,aWidth);
	}
	else
		theList->EnsureVisible(anItem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateListItem(MultiListArea *theList, MyListItem *theItem)
{
	if(theList==NULL)
		return;

	MyListItemPtr anItem = theItem;
	theList->ListArea::UpdateItem(anItem);
	theList->EnsureVisible(anItem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnNew(ComponentEvent *theEvent)
{
	if(!CheckModifyPrompt())
		return;

	MSFileDialogPtr aDlg = new MSFileDialog(true);
	aDlg->SetTitle(GetStr(STR_NEW_RESOURCE_TITLE));
	aDlg->SetWarnIfExists(true);
	aDlg->SetOkTitle(GetStr(STR_NEW_RESOURCE_OK_BUTTON));
	aDlg->AddFilter("*.wrs",GetStr(STR_RESOURCE_FILES));
	aDlg->AddFilter("*.*",GetStr(STR_ALL_FILES));
	if(!aDlg->DoDialog(gWindow))
		return;

	CloseResourceFile();

	GUIString aFileName = aDlg->GetFile();
	WONFile aFile(aDlg->GetFile());
	if(aFile.GetFileExtension().empty())
		aFileName.append(".wrs");

	gResourceCollection.OpenNew(aFileName);
	SetResourceFile(aFileName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopulateLists()
{
	ClearLists();
	const ResourceIdMap &aMap = gResourceCollection.GetResourceIdMap();
	ResourceIdMap::const_iterator anItr = aMap.begin();
	while(anItr!=aMap.end())
	{
		const ResourceEntry *anEntry = anItr->second;
		int aListId = anEntry->mType;
		InsertListItem(GetList(aListId),anEntry,true);

		++anItr;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnOpen(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	if(!CheckModifyPrompt())
		return;

	MSFileDialogPtr aDlg = new MSFileDialog;
	aDlg->AddFilter("*.wrs",GetStr(STR_RESOURCE_FILES));
	aDlg->AddFilter("*.*",GetStr(STR_ALL_FILES));
	if(aDlg->DoDialog(gWindow))
	{
		GUIString aFileName = aDlg->GetFile();
		CloseResourceFile();
	
		if(!gResourceCollection.Open(aFileName))
			return;

		SetResourceFile(aFileName);
		PopulateLists();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int CheckReadOnly(const GUIString &theFileName)
{
	WONFile aFile(theFileName);
	if(aFile.Exists() && aFile.IsReadOnly())
	{
		int aResult = MSMessageBox::Msg(gWindow,GetStr(STR_READONLY_FILE_TITLE),GetStr(STR_READONLY_FILE_MSG),GetStr(STR_MENU_SAVEAS),GetStr(STR_OVERWRITE),GetStr(STR_CANCEL));
		return aResult;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnImportCFG(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	if(!CheckModifyPrompt())
		return;

	MSFileDialogPtr anInDlg = new MSFileDialog;
	anInDlg->SetTitle("Pick CFG File.");
	anInDlg->SetNoChangeDir(false);

	anInDlg->AddFilter("*.cfg",GetStr(STR_RESOURCE_FILES));
	anInDlg->AddFilter("*.*",GetStr(STR_ALL_FILES));
	if(!anInDlg->DoDialog(gWindow))
		return;

	GUIString anInName = anInDlg->GetFile();

	CloseResourceFile();
	ConfigParser aParser;
	if(!aParser.OpenFile(anInName))
	{
		MSMessageBox::Msg(gWindow,"Error","Failed to open config file.");
		return;
	}

	ResConfigPtr aConfig = new ResConfig(gResourceCollection);
	if(!aConfig->ParseTopLevel(aParser))
	{
		GUIString anError = "Error parsing config file: ";
		anError.append(aParser.GetAbortReason());
		MSMessageBox::Msg(gWindow,"Error",anError);
		return;
	}

	PopulateLists();
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnSaveAs(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	MSFileDialogPtr aDlg = new MSFileDialog(true);
	aDlg->SetNoChangeDir(true);
	aDlg->SetWarnIfExists(true);
	aDlg->AddFilter("*.wrs",GetStr(STR_RESOURCE_FILES));
	aDlg->AddFilter("*.*",GetStr(STR_ALL_FILES));
	if(!aDlg->DoDialog(gWindow))
		return;

	GUIString aFileName = aDlg->GetFile();
	WONFile aFile(aDlg->GetFile());
	if(aFile.GetFileExtension().empty())
		aFileName.append(".wrs");

	int aCheck = CheckReadOnly(aFileName);
	if(aCheck==3)
		return;

	if(aCheck==1)
	{
		OnSaveAs(NULL);
		return;
	}

	gResourceCollection.SaveAs(aFileName);
	SetResourceFile(aFileName);
	PopulateLists();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnSave(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	if(gCurResourceFile.GetFileName().empty())
	{
		OnSaveAs(NULL);
		return;
	}
	
	int aCheck = CheckReadOnly(gCurResourceFile.GetFileName());
	if(aCheck==3)
		return;

	if(aCheck==1)
	{
		OnSaveAs(NULL);
		return;
	}

	gResourceCollection.Save();
	PopulateLists();
	CheckModified();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnExportCFG(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	MSFileDialogPtr aDlg = new MSFileDialog(true);
	aDlg->SetNoChangeDir(true);
	aDlg->SetWarnIfExists(true);
	aDlg->AddFilter("*.cfg","Config Files");
	aDlg->AddFilter("*.*",GetStr(STR_ALL_FILES));
	if(!aDlg->DoDialog(gWindow))
		return;

	GUIString aFileName = aDlg->GetFile();
	WONFile aFile(aDlg->GetFile());
	if(aFile.GetFileExtension().empty())
		aFileName.append(".cfg");

	int aCheck = CheckReadOnly(aFileName);
	if(aCheck==3)
		return;

	if(aCheck==1)
	{
		OnExportCFG(NULL);
		return;
	}
	
	gResourceCollection.ExportToCFG(aFileName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DoStringAdd()
{
	// Create Controls
	int aListId = GetCurListId();

	MSDialogPtr aDialog = new MSDialog;
	aDialog->SetEndOnEscape(-1);
	aDialog->SetSize(320,120);
	MSLabelPtr aLabel1 = new MSLabel(GetStr(STR_NAME_COLON));
	MSLabelPtr aLabel2 = new MSLabel(GetStr(STR_VALUE_COLON));
	MSInputBoxPtr anInput1 = new MSInputBox;
	MSInputBoxPtr anInput2 = new MSInputBox;
	MSButtonPtr aButton1 = new MSButton(GetStr(STR_ADD));
	MSButtonPtr aButton2 = new MSButton(GetStr(STR_DONE));

	// Control Ids / End Dialog
	aDialog->SetDefaultButton(aButton1);
	aButton1->SetControlId(1);
	aButton2->SetControlId(-1);
	aButton1->SetButtonFlags(ButtonFlag_EndDialog,true);
	aButton2->SetButtonFlags(ButtonFlag_EndDialog,true);

	// Layout controls	
	aLabel1->SetPos(5,5);
	aLabel1->SetHeight(24);
	aLabel1->SetVertAlign(VertAlign_Center);
	aLabel2->SetVertAlign(VertAlign_Center);

	aDialog->AddChildLayout(aLabel2,CLI_Below | CLI_SameLeft,aLabel1,0,10);
	aDialog->AddChildLayout((new ClearRightLayout(anInput1, 5))->Add(aLabel1)->Add(aLabel2));
	aDialog->AddChildLayout(anInput1,CLI_SameTop | CLI_SameHeight,aLabel1);
	aDialog->AddChildLayout(anInput1,CLI_GrowToRight,aDialog,0,0,-10);
	aDialog->AddChildLayout(anInput2,CLI_SameTop,aLabel2);
	aDialog->AddChildLayout(anInput2,CLI_SameLeft | CLI_SameSize,anInput1);

	aButton1->SetWidth(80);
	aDialog->AddChildLayout(aButton1,CLI_Below,anInput2,0,30);
	aDialog->AddChildLayout(aButton2,CLI_SameTop | CLI_Right | CLI_SameSize,aButton1,10);
	aDialog->AddChildLayout((new CenterLayout(aDialog,aButton1,true,false))->Add(aButton2));

	// Add Components
	aDialog->AddChild(aLabel1);
	aDialog->AddChild(aLabel2);
	aDialog->AddChild(anInput1);
	aDialog->AddChild(anInput2);
	aDialog->AddChild(aButton1);
	aDialog->AddChild(aButton2);


	// Create Dialog Window
	string aWindowTitle = GetStr(STR_ADD_IMAGE + aListId);

	PlatformWindowPtr aWindow = new PlatformWindow;
	aWindow->SetParent(gWindow);
	aWindow->SetTitle(aWindowTitle);
	aWindow->SetCreateFlags(CreateWindow_SizeSpecClientArea | CreateWindow_NotSizeable);

	WONRectangle aRect;
	gWindow->GetScreenPos(aRect);
	aWindow->Create(WONRectangle(aRect.Left()+10,aRect.Top()+10,aDialog->Width(),aDialog->Height()));
	aWindow->AddComponent(aDialog);

	while(true)
	{
		anInput1->SetText(GetTempName(aListId));
		anInput1->SetTransform(UpperInput);
		anInput2->SetText("");
		anInput1->RequestFocus();
		anInput1->SetSel(strlen(gTypePrefixes[aListId]));

		int aResult = aWindow->DoDialog(aDialog);
		if(aResult==-1)
			break;

		const ResourceEntry *anEntry = NULL;
		anEntry = gResourceCollection.AddResource((ResourceType)aListId,anInput1->GetText(),anInput2->GetText());
		if(anEntry!=NULL)
			InsertListItem(GetCurList(),anEntry);
		else
		{
			MSMessageBox::Msg(aWindow,GetStr(STR_NAME_EXISTS_TITLE),GetStr(STR_NAME_EXISTS_PROMPT,anInput1->GetText()),GetStr(STR_RETRY));
		}
	} 

	aWindow->Close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSFileDialogPtr GetOpenFileDialog(bool isAdd)
{
	int aListId = GetCurListId();

	MSFileDialogPtr aDialog = new MSFileDialog;
	aDialog->SetOkTitle(isAdd?GetStr(STR_ADD):GetStr(STR_OK));
	switch(aListId)
	{
		case ResourceType_Image:
			aDialog->AddFilter("*.bmp;*.gif;*.jpg;*.png",GetStr(STR_IMAGE_FILES));
			aDialog->AddFilter("*.bmp",GetStr(STR_BMP_FILES));
			aDialog->AddFilter("*.gif",GetStr(STR_GIF_FILES));
			aDialog->AddFilter("*.jpg",GetStr(STR_JPG_FILES));
			aDialog->AddFilter("*.png",GetStr(STR_PNG_FILES));
			break;

		case ResourceType_Binary:
			break;
	}
	aDialog->AddFilter("*.*",GetStr(STR_ALL_FILES));
	aDialog->SetTitle(isAdd?GetStr(STR_ADD_IMAGE+aListId):GetStr(STR_BROWSE_IMAGE+aListId));
	aDialog->SetNoChangeDir(true);

	if(isAdd)
		aDialog->SetAllowMultiple(true);

	return aDialog;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DoFileAdd()
{
	int aListId = GetCurListId();

	MSFileDialogPtr aDialog = GetOpenFileDialog(true);
	if(!aDialog->DoDialog(gWindow))
		return;

	const MSFileDialog::FileList &aList = aDialog->GetFileList();
	MSFileDialog::FileList::const_iterator anItr  = aList.begin();
	while(anItr!=aList.end())
	{
		GUIString aTempName = GetTempName(aListId);
		const ResourceEntry *anEntry = NULL;
		string aFilePath = *anItr;
		string aRelativePath = gCurResourceFile.GetRelativePath(aFilePath);
		anEntry = gResourceCollection.AddResource((ResourceType)aListId,aTempName,aRelativePath);
		if(anEntry!=NULL)
			InsertListItem(GetList(aListId),anEntry);

		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnAdd(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	int aListId = gTabCtrl->GetCurTabId();
	if(aListId==ResourceType_String || aListId==ResourceType_Color || aListId==ResourceType_Font)
		DoStringAdd();
	else
		DoFileAdd();

	CheckModified();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnAddPlaceholder(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	int aListId = gTabCtrl->GetCurTabId();
	GUIString aTempName = GetTempName(aListId);
	const ResourceEntry *anEntry = gResourceCollection.AddResource((ResourceType)aListId,aTempName,"");
	if(anEntry!=NULL)
		InsertListItem(GetList(aListId),anEntry);

	CheckModified();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnBrowse(ComponentEvent *theEvent, InputBox *theInputBox)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	MSFileDialogPtr aDlg = GetOpenFileDialog(false);
	if(aDlg->DoDialog(theEvent->mComponent->GetWindow()))
	{
		string aPath = aDlg->GetFile();
		theInputBox->SetText(gCurResourceFile.GetRelativePath(aPath));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenameOrModify(bool isRename)
{
	int aListId = gTabCtrl->GetCurTabId();
	MultiListArea *aList = gLists[aListId];
	MSDialogPtr aDialog = new MSDialog;	
	aDialog->SetEndOnEscape(-1);

	aList->RewindSelections();
	if(!aList->HasMoreSelections())
		return;

	// Create Controls
	MSLabelPtr aLabel1 = new MSLabel;
	MSLabelPtr aLabel2 = new MSLabel;
	MSInputBoxPtr anInput1 = new MSInputBox;

	MSButtonPtr aButton1 = new MSButton(isRename?GetStr(STR_RENAME):GetStr(STR_MODIFY));
	MSButtonPtr aButton2 = new MSButton(GetStr(STR_SKIP));
	MSButtonPtr aButton3 = new MSButton(GetStr(STR_CANCEL));
	MSButtonPtr aBrowseButton = new MSButton(GetStr(STR_BROWSE));
	aBrowseButton->SetListener(new ComponentListenerFuncParam<InputBox*>(OnBrowse,anInput1));
	if(isRename || aListId==ResourceType_String)
	{
		aBrowseButton->SetWidth(0);
		aBrowseButton->SetVisible(false);
	}

	// Control Ids / End Dialog
	aDialog->SetDefaultButton(aButton1);
	aButton1->SetControlId(1);
	aButton2->SetControlId(2);
	aButton3->SetControlId(-1);
	aButton1->SetButtonFlags(ButtonFlag_EndDialog,true);
	aButton2->SetButtonFlags(ButtonFlag_EndDialog,true);
	aButton3->SetButtonFlags(ButtonFlag_EndDialog,true);

	//Layout Controls
	int aStandardDialogWidth = 400;
	int aDialogWidth = aStandardDialogWidth; 
	aDialog->SetWidth(aStandardDialogWidth);
	aLabel1->SetHeight(24);
	aDialog->AddChildLayout(aLabel1,CLI_SameLeft | CLI_SameTop | CLI_SameWidth,aDialog,5,5,-10);
	aDialog->AddChildLayout(aLabel2,CLI_SameLeft | CLI_SameSize | CLI_Below,aLabel1);
	aBrowseButton->SetHeight(24);
	aDialog->AddChildLayout(aBrowseButton,CLI_SameRight | CLI_Below,aLabel2,0,5);
	aDialog->AddChildLayout(anInput1,CLI_SameLeft,aLabel2);
	aDialog->AddChildLayout(anInput1,CLI_GrowToLeft | CLI_SameTop | CLI_SameHeight,aBrowseButton,0,0,-5);
	aButton1->SetWidth(80);
	aDialog->AddChildLayout(aButton1,CLI_Below,anInput1,0,10);
	aDialog->AddChildLayout(aButton2,CLI_Right | CLI_SameSize | CLI_SameTop,aButton1,10);
	aDialog->AddChildLayout(aButton3,CLI_Right | CLI_SameSize | CLI_SameTop,aButton2,10);
	aDialog->AddChildLayout((new CenterLayout(aDialog,aButton1,true,false))->Add(aButton2)->Add(aButton3));
	aDialog->SetHeight(aButton1->Bottom()+10);

	// Add Components;
	aDialog->AddChild(aLabel1);
	aDialog->AddChild(aLabel2);
	aDialog->AddChild(anInput1);
	aDialog->AddChild(aButton1);
	aDialog->AddChild(aButton2);
	aDialog->AddChild(aButton3);
	aDialog->AddChild(aBrowseButton);

	// Create Window
	PlatformWindowPtr aWindow = new PlatformWindow;
	aWindow->SetParent(gWindow);
	aWindow->SetTitle(isRename?GetStr(STR_RENAME_RESOURCE_TITLE):GetStr(STR_MODIFY_RESOURCE_TITLE));
	aWindow->SetCreateFlags(CreateWindow_SizeSpecClientArea | CreateWindow_NotSizeable);

	WONRectangle aRect;
	gWindow->GetScreenPos(aRect);
	aWindow->Create(WONRectangle(aRect.Left()+10,aRect.Top()+10,aDialog->Width(),aDialog->Height()));
	aWindow->AddComponent(aDialog);

	bool retry = false;
	MyListItem *anItem; 
	while(retry || aList->HasMoreSelections())
	{
		// Init Entry
		if(!retry)
		{
			anItem = (MyListItem*)aList->GetNextSelection();

			GUIString aStr1 = isRename?GetStr(STR_RENAME):GetStr(STR_MODIFY);
			aStr1.append(anItem->mEntry->mName);
			aStr1.append(':');
			aLabel1->SetText(aStr1);

			GUIString aStr2 = GetStr(STR_VALUE_COLON);
			aStr2.append(' ');
			aStr2.append(anItem->mDesc);
			aLabel2->SetText(aStr2);

			anInput1->SetText(isRename?anItem->mEntry->mName:anItem->mDesc);
			if(isRename)
				anInput1->SetSel(strlen(gTypePrefixes[aListId]));
			else
				anInput1->SetSel();
		}
		else
			retry = false;

		anInput1->RequestFocus();
		int aResult = aWindow->DoDialog(aDialog);
		if(aResult==-1)
			break;
		else if(aResult==1)
		{
			ConstResourceEntryPtr anEntry = anItem->mEntry;
			if(isRename && gResourceCollection.RenameResource(anEntry,anInput1->GetText()))
			{
				UpdateListItem(aList,anItem);
			}
			else if(!isRename && gResourceCollection.ModifyResource(anEntry,anInput1->GetText()))
			{
				UpdateListItem(aList,anItem);
			}
			else
			{
				// dup rename or modify problem
				const ResourceEntry* anEntry = gResourceCollection.GetCollisionEntry();
				GUIString aMsg; 
				if(isRename)
				{
					aMsg = GetStr(STR_NAME_EXISTS_PROMPT,anItem->mEntry->mName);
				}
				else 
				{
					aMsg = GetStr(STR_MODIFY_PROBLEM_PROMPT,anItem->mEntry->mName);
				}

				if(MSMessageBox::Msg(aWindow,isRename?GetStr(STR_NAME_EXISTS_TITLE):GetStr(STR_MODIFY_PROBLEM_TITLE),aMsg,GetStr(STR_RETRY),GetStr(STR_SKIP))==1)
					retry = true;				
			}
		}
	}
	aList->Sort();
	aWindow->Close();

	void OnPreview(ComponentEvent *theEvent);
	OnPreview(NULL); // refresh preview
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnRename(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	RenameOrModify(true);
	CheckModified();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnDelete(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	MultiListArea *aList = GetCurList();
	if(aList==NULL)
		return;

	aList->BeginMultiChange();
	aList->RewindSelections();
	while(aList->HasMoreSelections())
	{
		MyListItem *anItem =(MyListItem*)aList->GetNextSelection();
		if(anItem!=NULL && gResourceCollection.DeleteResource(anItem->mEntry))
		{
//			aList->DeleteItem(anItem);
		}
	}
	aList->EndMultiChange();
	CheckModified();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnUpdate(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	MultiListArea *aList = GetCurList();
	if(aList==NULL)
		return;

	aList->BeginMultiChange();
	aList->RewindSelections();
	while(aList->HasMoreSelections())
	{
		MyListItem *anItem =(MyListItem*)aList->GetNextSelection();
		if(anItem!=NULL && gResourceCollection.UpdateResource(anItem->mEntry))
			UpdateListItem(aList,anItem);
	}
	aList->EndMultiChange();
	CheckModified();

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnModify(ComponentEvent *theEvent)
{
	RenameOrModify(false);
	CheckModified();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnChangeId(ComponentEvent *theEvent)
{
	int aListId = gTabCtrl->GetCurTabId();
	MultiListArea *aList = gLists[aListId];
	MSDialogPtr aDialog = new MSDialog;	
	aDialog->SetEndOnEscape(-1);

	aList->RewindSelections();
	if(!aList->HasMoreSelections())
		return;

	// Create Controls
	MSLabelPtr aLabel1 = new MSLabel;
	MSLabelPtr aLabel2 = new MSLabel;
	MSInputBoxPtr anInput1 = new MSInputBox;

	MSButtonPtr aButton1 = new MSButton(GetStr(STR_CHANGE_ID));
	MSButtonPtr aButton2 = new MSButton(GetStr(STR_SKIP));
	MSButtonPtr aButton3 = new MSButton(GetStr(STR_CANCEL));

	// Control Ids / End Dialog
	aDialog->SetDefaultButton(aButton1);
	aButton1->SetControlId(1);
	aButton2->SetControlId(2);
	aButton3->SetControlId(-1);
	aButton1->SetButtonFlags(ButtonFlag_EndDialog,true);
	aButton2->SetButtonFlags(ButtonFlag_EndDialog,true);
	aButton3->SetButtonFlags(ButtonFlag_EndDialog,true);

	//Layout Controls
	int aStandardDialogWidth = 400;
	int aDialogWidth = aStandardDialogWidth; 
	aDialog->SetWidth(aStandardDialogWidth);
	aLabel1->SetHeight(24);
	aDialog->AddChildLayout(aLabel1,CLI_SameLeft | CLI_SameTop | CLI_SameWidth,aDialog,5,5,-10);
	aDialog->AddChildLayout(aLabel2,CLI_SameLeft | CLI_SameSize | CLI_Below,aLabel1);
	aDialog->AddChildLayout(anInput1,CLI_SameLeft | CLI_Below | CLI_SameSize,aLabel2,10);
	aButton1->SetWidth(80);
	aDialog->AddChildLayout(aButton1,CLI_Below,anInput1,0,10);
	aDialog->AddChildLayout(aButton2,CLI_Right | CLI_SameSize | CLI_SameTop,aButton1,10);
	aDialog->AddChildLayout(aButton3,CLI_Right | CLI_SameSize | CLI_SameTop,aButton2,10);
	aDialog->AddChildLayout((new CenterLayout(aDialog,aButton1,true,false))->Add(aButton2)->Add(aButton3));
	aDialog->SetHeight(aButton1->Bottom()+10);

	// Add Components;
	aDialog->AddChild(aLabel1);
	aDialog->AddChild(aLabel2);
	aDialog->AddChild(anInput1);
	aDialog->AddChild(aButton1);
	aDialog->AddChild(aButton2);
	aDialog->AddChild(aButton3);

	// Create Window
	PlatformWindowPtr aWindow = new PlatformWindow;
	aWindow->SetParent(gWindow);
	aWindow->SetTitle(GetStr(STR_CHANGE_ID_TITLE));
	aWindow->SetCreateFlags(CreateWindow_SizeSpecClientArea | CreateWindow_NotSizeable);

	WONRectangle aRect;
	gWindow->GetScreenPos(aRect);
	aWindow->Create(WONRectangle(aRect.Left()+10,aRect.Top()+10,aDialog->Width(),aDialog->Height()));
	aWindow->AddComponent(aDialog);

	bool retry = false;
	MyListItem *anItem; 
	while(retry || aList->HasMoreSelections())
	{
		// Init Entry
		if(!retry)
		{
			anItem = (MyListItem*)aList->GetNextSelection();

			GUIString aStr1 = anItem->mEntry->mName;
			aStr1.append(": ");
			char aBuf[50];
			sprintf(aBuf,"%d",anItem->mEntry->mId);
			aStr1.append(aBuf);
			aLabel1->SetText(aStr1);

			GUIString aStr2 = GetStr(STR_VALUE_COLON);
			aStr2.append(' ');
			aStr2.append(anItem->mDesc);
			aLabel2->SetText(aStr2);

			anInput1->SetText(aBuf);
			anInput1->SetSel();
		}
		else
			retry = false;

		anInput1->RequestFocus();
		int aResult = aWindow->DoDialog(aDialog);
		if(aResult==-1)
			break;
		else if(aResult==1)
		{
			ConstResourceEntryPtr anEntry = anItem->mEntry;
			int anId = anInput1->GetText().atoi();
			if(anId<=0)
			{
				MSMessageBox::Msg(aWindow,GetStr(STR_INVALID_ID_TITLE),GetStr(STR_INVALID_ID_MSG));
			}
			else if(gResourceCollection.ModifyResourceId(anEntry,anId))
			{
				UpdateListItem(aList,anItem);
			}
			else
			{
				// dup id
				if(MSMessageBox::Msg(aWindow,GetStr(STR_ID_IN_USE_TITLE),GetStr(STR_ID_IN_USE_MSG),GetStr(STR_RETRY),GetStr(STR_SKIP))==1)
					retry = true;				
			}
		}
	}
	aList->Sort();
	aWindow->Close();

	CheckModified();

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnQuit(ComponentEvent *theEvent)
{
	if(!ValidateMenuSelection(theEvent))
		return;

	gWindowMgr->CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ContainerPtr CreatePreviewContainer(int theType)
{
	switch(theType)
	{
		case ResourceType_Image:
		{
			MSScrollContainerPtr aContainer = new MSScrollContainer(new ComponentScrollArea(new ImageComponent(NULL)));
			aContainer->GetScrollArea()->SetBackground(StandardColor_3DFace);
			aContainer->SetScrollbarConditions(ScrollbarCondition_Conditional,ScrollbarCondition_Conditional);
			aContainer->SetDraw3DFrame(true);
			return aContainer;
		}
		break;

		case ResourceType_String:
		{
			MSScrollContainerPtr aContainer = new MSScrollContainer(new TextArea);
			((TextArea*)aContainer->GetScrollArea())->SetTextFlags(TextFlag_ScrollOnBottom,false);
			aContainer->SetDraw3DFrame(true);
			aContainer->SetScrollbarConditions(ScrollbarCondition_Conditional,ScrollbarCondition_Conditional);
			return aContainer;
		}
		break;

		case ResourceType_Binary:
		{
			MSScrollContainerPtr aContainer = new MSScrollContainer(new HexView);
			aContainer->SetDraw3DFrame(true);
			aContainer->SetScrollbarConditions(ScrollbarCondition_Conditional,ScrollbarCondition_Conditional);
			return aContainer;
		}
		break;

		case ResourceType_Color:
		{
			ContainerPtr aContainer = new Container;
			RectangleComponentPtr aRect = new RectangleComponent;
			aRect->SetControlId(1);
			aContainer->AddChildLayout(aRect,CLI_SameSize,aContainer);
			aContainer->AddChild(aRect);

			return aContainer;
		}
		break;

		case ResourceType_Font:
		{
			MSScrollContainerPtr aContainer = new MSScrollContainer(new TextArea);
			((TextArea*)aContainer->GetScrollArea())->SetTextFlags(TextFlag_ScrollOnBottom,false);
			aContainer->SetScrollbarConditions(ScrollbarCondition_Conditional,ScrollbarCondition_Conditional);
			return aContainer;
		}
		break;

		default:
			return new Container;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreatePreviewWindow()
{
	gPreviewWindow = new PlatformWindow;
	gPreviewWindow->SetParent(gWindow);
	gPreviewWindow->SetCreateFlags(CreateWindow_HideAtFirst);
	gPreviewWindow->SetTitle(GetStr(STR_PREVIEW_TITLE));
	WONRectangle aWindowRect, aScreenRect;
	gWindow->GetScreenPos(aWindowRect);
	gWindowMgr->GetScreenRect(aScreenRect);
	int aWidth = 320;
	int aHeight = 200;
	int x = aWindowRect.Right();
	if(x + aWidth> aScreenRect.Width())
	{
		x = aWindowRect.Left() - aWidth;
		if(x < 0)
			x = 0;
	}
	
	gPreviewWindow->Create(WONRectangle(x,aWindowRect.Top(),aWidth,aHeight));

	Container *aRoot = gPreviewWindow->GetRoot();

	MSDlgBackPtr aBack = new MSDlgBack;
	aRoot->AddChildLayout(aBack,CLI_SameSize,aRoot);
	gPreviewWindow->AddComponent(aBack);

	for(int i=0; i<NUM_LISTS; i++)
	{
		ContainerPtr aContainer = CreatePreviewContainer(i);
		gPreviewContainers[i] = aContainer;
		if(aContainer.get()!=NULL)
		{
			aContainer->SetVisible(false);
			aRoot->AddChildLayout(aContainer,CLI_SameSize,aRoot);
			gPreviewWindow->AddComponent(aContainer);
		}
		
	}

}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DoPreview(int theType, const ResourceEntry *theEntry)
{
	Container *aContainer = gPreviewContainers[theType];

	GUIString aTitle = GetStr(STR_PREVIEW_DASH_TITLE);
	if(!aContainer->IsVisible())
	{
		for(int i=0; i<NUM_LISTS; i++)
		{
			if(gPreviewContainers[i]!=NULL)
				gPreviewContainers[i]->SetVisible(false);
		}
		if(aContainer!=NULL)
			aContainer->SetVisible(true);

		gPreviewWindow->GetRoot()->Invalidate();
	}
	if(theEntry==NULL)
	{
		aContainer->SetVisible(false);
		gPreviewWindow->GetRoot()->Invalidate();
		return;
	}

	gResourceCollection.DecodeResource(theEntry);
	switch(theType)
	{
		case ResourceType_Image:
		{	
			aTitle.append(theEntry->mDesc);
			NativeImagePtr anImage = (NativeImage*)theEntry->mData.get();

			Container *aRoot = gPreviewWindow->GetRoot();
			Scroller *aScroller = (Scroller*)aContainer;
			ComponentScrollArea *aScrollArea = (ComponentScrollArea*)aScroller->GetScrollArea();
			ImageComponent *anImageComponent = (ImageComponent*)aScrollArea->GetComponent();
			anImageComponent->SetImage(anImage);
			if(anImage.get()==NULL)
				anImageComponent->SetSize(0,0);

			aScrollArea->UpdateComponentSize();
			aRoot->Invalidate();
			aRoot->SizeChanged();
		}
		break;

		case ResourceType_String:
		{	
			aTitle.append(theEntry->mName);
			TextArea *aTextArea = (TextArea*)((Scroller*)aContainer)->GetScrollArea();
			aTextArea->Clear();
			aTextArea->AddSegment(ResourceCollection::OutStringFilter(theEntry->mDesc));
		}
		break;

		case ResourceType_Binary:
		{
			aTitle.append(theEntry->mName);
			HexView* aHexView = (HexView*)(((Scroller*)aContainer)->GetScrollArea());
			aHexView->SetData((ByteBuffer*)theEntry->mData.get());
		}
		break;

		case ResourceType_Color:
		{
			aTitle.append(theEntry->mName);
			RectangleComponent *aComp = (RectangleComponent*)aContainer->GetComponentById(1);
			aComp->SetColor(gResourceCollection.GetColor(theEntry->mId));
		}
		break;

		case ResourceType_Font:
		{
			aTitle.append(theEntry->mName);
			TextArea *aTextArea = (TextArea*)((Scroller*)aContainer)->GetScrollArea();
			Font *aFont = (Font*)theEntry->mData.get();
			if(aFont!=NULL)
				aTextArea->SetFont(aFont);

			aTextArea->Clear();
			aTextArea->AddSegment("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890");
		}
		break;
	}

	gPreviewWindow->SetTitle(aTitle);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OnPreview(ComponentEvent *theEvent)
{
	if(gPreviewWindow.get()==NULL || !gPreviewWindow->IsOpen())
		CreatePreviewWindow();

	if(theEvent!=NULL)
	{
		if(theEvent->mType==ComponentEvent_ListItemUpClicked)
		{
			ListItemClickedEvent *anEvent = (ListItemClickedEvent*)theEvent;
			if(anEvent->mButton==MouseButton_Right)
				gEditMenu->DoPopup(gWindow,NULL);
//				gWindow->DoPopup(PopupParams(gEditMenu,(PopupFlag_StandardPopup | PopupFlag_InOwnWindow) & ~PopupFlag_FireEventOnEnd));
	
			return;
		}
		else if(theEvent->mType==ComponentEvent_ButtonPressed || theEvent->mType==ComponentEvent_MenuItemSelected)
			gPreviewWindow->Show(true);
		else if((theEvent->mType!=ComponentEvent_ListSelChanged && theEvent->mType!=ComponentEvent_TabCtrlTabSelected) || !gPreviewWindow->IsShowing())
			return;
	}

	int aListId = GetCurListId();
	MultiListArea *aList = GetCurList();
	MyListItem *anItem = (MyListItem*)aList->GetSelItem();
	const ResourceEntry *anEntry = NULL;
	if(anItem!=NULL)
		anEntry = anItem->mEntry;

	DoPreview(aListId,anEntry);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ContainerPtr GetListCtrl(int theId)
{
	ContainerPtr aContainer = new Container;

	MSListCtrlPtr aListCtrl = new MSListCtrl;
	MultiListArea *aListArea = aListCtrl->GetListArea();
	aListArea->SetListFlags(ListFlag_MultiSelect,true);
	aListArea->SetNumColumns(5);
	aListArea->SetColumnWidth(0,24);
	aListArea->SetColumnWidth(1,100);
	aListArea->SetColumnWidth(2,50);
	aListArea->SetColumnWidth(3,50);
	aListArea->SetDynamicSizeColumn(4);
	gLists[theId] = aListArea;

	MSHeaderControl *aHeader = aListCtrl->GetHeader();
	aHeader->SetColumn(0,"");
	aHeader->SetColumn(1,GetStr(STR_COLUMN_NAME));
	aHeader->SetColumn(2,GetStr(STR_COLUMN_ID));
	aHeader->SetColumn(3,GetStr(STR_COLUMN_SIZE));
	aHeader->SetColumn(4,GetStr(STR_COLUMN_DESCRIPTION));
	aHeader->SetColumn(5,"");

	aListCtrl->SetDraw3DFrame(true);
	aListCtrl->SetScrollbarConditions(ScrollbarCondition_Conditional,ScrollbarCondition_Conditional);

	// Listeners
	aListArea->SetListener(new ComponentListenerFunc(OnPreview));

	return aListCtrl;
//	aContainer->AddChildLayout(aListCtrl,CLI_SameSize | CLI_SameTop | CLI_SameLeft,aContainer,5,5,-10,-10);
//	aContainer->AddChild(aListCtrl);

//	return aContainer;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MenuBarPtr CreateMainMenu()
{
	MenuBarPtr aMenuBar = new MenuBar;
	PopupMenuPtr aMenu = new PopupMenu;
	aMenu->AddItem(GetStr(STR_MENU_NEW),new ComponentListenerFunc(OnNew));
	aMenu->AddItem(GetStr(STR_MENU_OPEN),new ComponentListenerFunc(OnOpen));
	aMenu->AddItem(GetStr(STR_MENU_SAVE),new ComponentListenerFunc(OnSave));
	aMenu->AddItem(GetStr(STR_MENU_SAVEAS),new ComponentListenerFunc(OnSaveAs));
	aMenu->AddItem("Import CFG...",new ComponentListenerFunc(OnImportCFG));
	aMenu->AddItem("Export CFG...",new ComponentListenerFunc(OnExportCFG));
	aMenu->AddSeperator();
	aMenu->AddItem(GetStr(STR_MENU_QUIT),new ComponentListenerFunc(OnQuit));
	aMenuBar->AddItem(GetStr(STR_MENU_FILE),aMenu);

	aMenu = new PopupMenu;
	aMenu->AddItem(GetStr(STR_ADD),new ComponentListenerFunc(OnAdd));
	aMenu->AddItem(GetStr(STR_ADD_PLACEHOLDER),new ComponentListenerFunc(OnAddPlaceholder));
	aMenu->AddItem(GetStr(STR_RENAME),new ComponentListenerFunc(OnRename));
	aMenu->AddItem(GetStr(STR_CHANGE_ID), new ComponentListenerFunc(OnChangeId));
	aMenu->AddItem(GetStr(STR_MODIFY), new ComponentListenerFunc(OnModify));
	aMenu->AddItem(GetStr(STR_UPDATE),gResources.GetImage(IMG_UPDATE),new ComponentListenerFunc(OnUpdate));
	aMenu->AddSeperator();
	aMenu->AddItem(GetStr(STR_DELETE),gResources.GetImage(IMG_DELETE),new ComponentListenerFunc(OnDelete));
	aMenuBar->AddItem(GetStr(STR_MENU_EDIT),aMenu);
	gEditMenu = aMenu;

	aMenu = new PopupMenu;
	aMenu->AddItem(GetStr(STR_MENU_PREVIEW),new ComponentListenerFunc(OnPreview));
	aMenuBar->AddItem(GetStr(STR_MENU_VIEW),aMenu);


	return aMenuBar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONGUI_ENTRY_POINT
{
	gWindowMgr = new PlatformWindowManager;
	gWindowMgr->InitSystemColorScheme();

	gWindow = new PlatformWindow;
	gWindow->SetCreateFlags(CreateWindow_Centered);
	gWindow->SetTitle("");
	gWindow->SetAllowCloseFunc(AllowCloseFunc);
	gWindow->Create(WONRectangle(0,0,400,500));

	JPGDecoder::Add(gWindowMgr->GetImageDecoder());
	PNGDecoder::Add(gWindowMgr->GetImageDecoder());
 
#ifdef WIN32
	HMODULE aModule = (HMODULE)gWindowMgr->GetHinstance();
	HRSRC aResource = FindResource(aModule,MAKEINTRESOURCE(IDR_WRSFILE),RT_RCDATA);
	HGLOBAL aMem = LoadResource(aModule,aResource);
	LPVOID aMemDataP = LockResource(aMem);
	gResources.Open(aMemDataP,SizeofResource(aModule,aResource));
	gResources.DecodeAll();
	FreeResource(aMem);
#else
	gResources.Open("ResourcePacker.wrs");
	gResources.DecodeAll();
#endif
	gWindow->SetTitle(GetStr(STR_APP_NAME));

	gResourceCollection.SetDoInStringFilter(true);
	
	NativeIconPtr anIcon = gResources.GetImage(IMG_ICON)->GetIcon();
	gWindowMgr->SetDefaultWindowIcon(anIcon);
	gWindow->SetIcon(anIcon);

	gListImages[0] = gResources.GetImage(IMG_OK);
	gListImages[1] = gResources.GetImage(IMG_UPDATE);
	gListImages[2] = gResources.GetImage(IMG_DELETE);

	// Create Controls
	MSDlgBackPtr aBack = new MSDlgBack;
	MSSeperatorPtr aSep = new MSSeperator;
	MenuBarPtr aMenuBar = CreateMainMenu();


	MSTabCtrlPtr aTabCtrl = new MSTabCtrl;
	aTabCtrl->AddTab(new MSTabButton(GetStr(STR_TAB_IMAGES)),GetListCtrl(0));
	aTabCtrl->AddTab(new MSTabButton(GetStr(STR_TAB_STRINGS)),GetListCtrl(1));
	aTabCtrl->AddTab(new MSTabButton(GetStr(STR_TAB_BINARY)),GetListCtrl(2));
	aTabCtrl->AddTab(new MSTabButton(GetStr(STR_TAB_COLORS)),GetListCtrl(3));
	aTabCtrl->AddTab(new MSTabButton(GetStr(STR_TAB_FONTS)),GetListCtrl(4));
	gTabCtrl = aTabCtrl;

	// Listeners
	aTabCtrl->SetListener(new ComponentListenerFunc(OnPreview));

	// Layout controls
	RootContainer *aRoot = gWindow->GetRoot();
	aRoot->AddChildLayout(aBack,CLI_SameSize,aRoot);
	aMenuBar->SetHeight(20);
	aRoot->AddChildLayout(aMenuBar,CLI_SameWidth,aRoot);
	aRoot->AddChildLayout(aSep,CLI_SameWidth | CLI_Below,aMenuBar);
	aRoot->AddChildLayout(aTabCtrl,CLI_Below,aMenuBar,0,10);
	aRoot->AddChildLayout(aTabCtrl,CLI_SameWidth | CLI_GrowToBottom | CLI_SameLeft,aRoot,5,0,-10,-10);

	// Add controls
	gWindow->AddComponent(aBack);
	gWindow->AddComponent(aMenuBar);
	gWindow->AddComponent(aSep);
	gWindow->AddComponent(aTabCtrl);
	
	return gWindowMgr->MessageLoop();
};
