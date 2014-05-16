#include "WONGUI/WONGUI.h"
#include "WONCommon/FileReader.h"
#include "WONCommon/StringUtil.h"
#include "ResourceHeader.h"

using namespace WONAPI;
using namespace std;

PlatformWindowManagerPtr gWindowMgr;
PlatformWindowPtr gWindow;
PlatformWindowPtr gConfigWindow;

MultiListAreaPtr gList;
ButtonPtr gAddButton;
ButtonPtr gEditButton;
ButtonPtr gRemoveButton;
ButtonPtr gGenerateButton;

DialogPtr gConfigDialog;
InputBoxPtr gProjectInput;
InputBoxPtr gConfigInput;
InputBoxPtr gOutputInput;
ButtonPtr gBrowseConfig;
ButtonPtr gBrowseOutput;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct ProjectEntry
{
	std::string mConfig;
	std::string mOutput;
};
typedef std::map<std::string,ProjectEntry,StringLessNoCase> ProjectMap;
ProjectMap gProjectMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ReadProjectMap()
{
	gProjectMap.clear();
	FILE *aFile = fopen("projects.txt","r");
	if(aFile==NULL)
		return;

	AutoFile aCloseFile = aFile;

	char aBuf[4096];
	while(true)
	{
		std::string strings[3];

		for(int i=0; i<3; i++)
		{
			if(fgets(aBuf,4096,aFile)==NULL)
				return;

			char *aPtr = strrchr(aBuf,'\n');
			if(aPtr!=NULL)
				*aPtr = '\0';

			strings[i] = aBuf;
		}
		fgets(aBuf,4096,aFile);

		ProjectEntry &anEntry = gProjectMap[strings[0]];
		anEntry.mConfig = strings[1];
		anEntry.mOutput = strings[2];
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteProjectMap()
{
	FILE *aFile = fopen("projects.txt","w");
	if(aFile==NULL)
		return;

	for(ProjectMap::iterator anItr = gProjectMap.begin(); anItr!=gProjectMap.end(); ++anItr)
		fprintf(aFile,"%s\n%s\n%s\n\n",anItr->first.c_str(),anItr->second.mConfig.c_str(),anItr->second.mOutput.c_str());

	fclose(aFile);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InsertProjectItem(const GUIString &theName, const GUIString &theConfig, const GUIString &theOutput)
{
	MultiListItemPtr anItem;
	for(int i=0; i<gList->GetNumItems(); i++)
	{
		TextListItem *aTextItem = (TextListItem*)gList->GetItem(i)->GetItem(0);
		if(aTextItem->mText.compareNoCase(theName)==0)
		{
			anItem = gList->GetItem(i);
			break;
		}
	}

	bool isInsert = false;
	if(anItem.get()==NULL)
	{
		anItem = new MultiListItem;
		isInsert = true;
	}

	anItem->SetItem(new TextListItem(theName), 0);
	anItem->SetItem(new TextListItem(theConfig), 1);
	anItem->SetItem(new TextListItem(theOutput), 2);
	
	if(isInsert)
		gList->InsertItem(anItem);
	else
		gList->UpdateItem(anItem);

	gList->EnsureVisible(anItem);
	gList->SetSelItem(gList->GetItemPos(anItem));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SyncListWithProjectMap()
{
	gList->BeginMultiChange();
	gList->Clear();
	for(ProjectMap::iterator anItr = gProjectMap.begin(); anItr!=gProjectMap.end(); ++anItr)
	{
		gList->InsertRow();
		int aRow = gList->GetNumItems()-1;
		gList->SetString(aRow,0,anItr->first);
		gList->SetString(aRow,1,anItr->second.mConfig);
		gList->SetString(aRow,2,anItr->second.mOutput);
	}

	gList->EndMultiChange();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DoConfigDialog(bool isEdit)
{
	if(isEdit)
	{
		MultiListItem *anItem = gList->GetSelItem();
		if(anItem==NULL)
			return;

		gProjectInput->SetText(((TextListItem*)anItem->GetItem(0))->mText);
		gConfigInput->SetText(((TextListItem*)anItem->GetItem(1))->mText);
		gOutputInput->SetText(((TextListItem*)anItem->GetItem(2))->mText);
		gProjectInput->Enable(false);
		gConfigInput->RequestFocus();
	}
	else
	{
		gProjectInput->Enable(true);
		gProjectInput->RequestFocus();
	}

	gConfigWindow->CenterWindow();
	gConfigWindow->Show(true);
	int aResult = gConfigWindow->DoDialog(gConfigDialog);
	gConfigWindow->Show(false);

	if(aResult!=ControlId_Ok)
		return;

	ProjectEntry &anEntry = gProjectMap[gProjectInput->GetText()];
	anEntry.mConfig = gConfigInput->GetText();
	anEntry.mOutput = gOutputInput->GetText();
	WriteProjectMap();

	InsertProjectItem(gProjectInput->GetText(),anEntry.mConfig,anEntry.mOutput);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RemoveProject()
{
	MultiListItem *anItem = gList->GetSelItem();
	if(anItem==NULL)
		return;

	GUIString aProject = ((TextListItem*)anItem->GetItem(0))->mText;
	gProjectMap.erase(aProject);
	WriteProjectMap();

	gList->DeleteItem(anItem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShowError(const GUIString &theError)
{
	MSMessageBox::Msg(gWindow,"Error",theError);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GenerateProject()
{
	MultiListItem *anItem = gList->GetSelItem();
	if(anItem==NULL)
		return;

	std::string aProject = ((TextListItem*)anItem->GetItem(0))->mText;
	std::string aConfig = ((TextListItem*)anItem->GetItem(1))->mText;
	std::string anOutput = ((TextListItem*)anItem->GetItem(2))->mText;

	ConfigParser aParser;
	aParser.SetWarningsAsErrors(true);
	if(!aParser.OpenFile(aConfig))
	{
		ShowError("Failed to open resource file.");
		return;
	}

	ResourceHeaderPtr aResource = new ResourceHeader;
	aResource->SetFileName(anOutput);

	if(aResource->IsFileReadOnly())
	{
		ShowError("Source file is readonly.");
		return;
	}

	if(!aResource->ParseTopLevel(aParser))
	{
		ShowError("Parse Error: " + aParser.GetAbortReason());
		return;
	}
	
	if(!aResource->WriteSourceFiles())
	{
		ShowError("Error writing source files.");
		return;
	}

	MSMessageBox::Msg(gWindow,"Success!","The resource files were written successfully.");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowsePath(InputBox *theInputBox)
{
	MSFileDialog aDialog(true);
	aDialog.SetWarnIfExists(false);
	aDialog.SetNoChangeDir(true);

	if(!aDialog.DoDialog(gConfigWindow))
		return;

	theInputBox->SetText(aDialog.GetFile());
	theInputBox->SetSel();
	theInputBox->RequestFocus();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CompListener(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_ButtonPressed)
	{
		if(theEvent->mComponent==gAddButton)
			DoConfigDialog(false);
		else if(theEvent->mComponent==gEditButton)
			DoConfigDialog(true);
		else if(theEvent->mComponent==gRemoveButton)
			RemoveProject();
		else if(theEvent->mComponent==gGenerateButton)
			GenerateProject();
		else if(theEvent->mComponent==gBrowseConfig)
			BrowsePath(gConfigInput);
		else if(theEvent->mComponent==gBrowseOutput)
			BrowsePath(gOutputInput);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitMainWindow()
{
	// Window
	gWindowMgr = new PlatformWindowManager;
	gWindowMgr->InitSystemColorScheme();

	gWindow = new PlatformWindow;
	gWindow->SetCreateFlags(CreateWindow_Centered | CreateWindow_SizeSpecClientArea);
	gWindow->SetTitle("Resource Header");
	gWindow->Create(WONRectangle(0,0,640,480));

	// Background
	Container *aContainer = gWindow->GetRoot();
	ComponentPtr aBack = new MSDlgBack;
	aContainer->AddChildLayout(aBack,CLI_SameSize,aContainer);
	aContainer->AddChild(aBack);

	// Buttons
	gAddButton = new MSButton("Add");
	gEditButton = new MSButton("Edit");
	gRemoveButton = new MSButton("Remove");
	gGenerateButton = new MSButton("Generate");
	gAddButton->SetSizeAtLeast(100,0);

	// List
	MSListCtrlPtr aList = new MSListCtrl;
	gList = aList->GetListArea();
	aList->SetScrollbarConditions(ScrollbarCondition_Conditional,ScrollbarCondition_Conditional);
	MSHeaderControl *aHeader = aList->GetHeader();
	aHeader->SetColumn(0,"Project");
	aHeader->SetColumn(1,"Config");
	aHeader->SetColumn(2,"Output");
	aHeader->SetColumn(3,"");
	gList->SetNumColumns(3);
	gList->SetColumnWidth(0,100);
	gList->SetColumnWidth(2,300);
	gList->SetDynamicSizeColumn(1);

	// Layout
	LayoutPointPtr lp1 = new LayoutPoint, lp2 = new LayoutPoint;
	aContainer->AddLayoutPoint(lp1); aContainer->AddLayoutPoint(lp2);
	aContainer->AddChildLayout((new SameSizeLayout(gAddButton,true,true))->Add(gEditButton)->Add(gRemoveButton)->Add(gGenerateButton));
	aContainer->AddChildLayout(gAddButton,CLI_SameBottom,aContainer,0,-10);
	aContainer->AddChildLayout(gEditButton,CLI_SameTop,gAddButton);
	aContainer->AddChildLayout(gRemoveButton,CLI_SameTop,gEditButton);
	aContainer->AddChildLayout(gGenerateButton,CLI_SameTop,gRemoveButton);
	aContainer->AddChildLayout((new SpaceEvenlyLayout(aContainer))->Add(lp1)->Add(gAddButton)->Add(gEditButton)->Add(gRemoveButton)->Add(gGenerateButton)->Add(lp2));
	aContainer->AddChildLayout(aList,CLI_SameWidth | CLI_SameLeft | CLI_SameTop,aContainer,10,10,-20,-20);
	aContainer->AddChildLayout(aList,CLI_GrowToTop,gAddButton,0,0,0,-10);

	// Add controls
	aContainer->AddChild(aList);
	aContainer->AddChild(gAddButton);
	aContainer->AddChild(gEditButton);
	aContainer->AddChild(gRemoveButton);
	aContainer->AddChild(gGenerateButton);

	aContainer->SetListener(new ComponentListenerFunc(CompListener));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitConfigWindow()
{
	// Window
	gConfigWindow = new PlatformWindow;
	gConfigWindow->SetParent(gWindow);
	gConfigWindow->SetCreateFlags(CreateWindow_Centered | CreateWindow_SizeSpecClientArea | CreateWindow_HideAtFirst);
	gConfigWindow->SetTitle("Config Project");
	gConfigWindow->Create(WONRectangle(0,0,320,200));

	// Background
	gConfigDialog = new Dialog;
	gConfigDialog->SetEndOnEscape(ControlId_Cancel);
	Container *aRoot = gConfigWindow->GetRoot();
	aRoot->AddChildLayout(gConfigDialog,CLI_SameSize,aRoot);
	aRoot->AddChild(gConfigDialog);

	Container *aContainer = gConfigDialog; 
	ComponentPtr aBack = new MSDlgBack;
	aContainer->AddChildLayout(aBack,CLI_SameSize,aContainer);
	aContainer->AddChild(aBack);

	// Labels
	MSLabelPtr aLabel1 = new MSLabel("Project:");
	MSLabelPtr aLabel2 = new MSLabel("Config:");
	MSLabelPtr aLabel3 = new MSLabel("Output:");
	aLabel1->SetHeight(22);
	aContainer->AddChildLayout((new SameSizeLayout(aLabel1,true,true))->Add(aLabel2)->Add(aLabel3));

	// Edits
	gProjectInput = new MSInputBox;
	gConfigInput = new MSInputBox;
	gOutputInput = new MSInputBox;
	gBrowseConfig = new MSButton("...");
	gBrowseOutput = new MSButton("...");

	// Buttons
	MSButtonPtr anOkButton = new MSButton("Ok");
	MSButtonPtr aCancelButton = new MSButton("Cancel");
	anOkButton->SetSizeAtLeast(100,0);
	anOkButton->SetControlId(ControlId_Ok);
	aCancelButton->SetControlId(ControlId_Cancel);
	anOkButton->SetButtonFlags(ButtonFlag_EndDialog,true);
	aCancelButton->SetButtonFlags(ButtonFlag_EndDialog,true);
	gConfigDialog->SetDefaultButton(anOkButton);

	// Layout
	aContainer->AddChildLayout(aLabel1,CLI_SameLeft | CLI_SameTop,aContainer,10,10);
	aContainer->AddChildLayout(aLabel2,CLI_SameLeft | CLI_Below,aLabel1,0,10);
	aContainer->AddChildLayout(aLabel3,CLI_SameLeft | CLI_Below,aLabel2,0,10);
	aContainer->AddChildLayout(gProjectInput,CLI_Right | CLI_SameTop | CLI_SameHeight,aLabel1,10);
	aContainer->AddChildLayout(gProjectInput,CLI_GrowToRight,aContainer,0,0,-20-gBrowseConfig->Width());
	aContainer->AddChildLayout(gConfigInput,CLI_SameLeft | CLI_Below | CLI_SameSize,gProjectInput,0,10);
	aContainer->AddChildLayout(gOutputInput,CLI_SameLeft | CLI_Below | CLI_SameSize,gConfigInput,0,10);
	aContainer->AddChildLayout(gBrowseConfig,CLI_Right | CLI_SameHeight | CLI_SameTop,gConfigInput,10);
	aContainer->AddChildLayout(gBrowseOutput,CLI_Right | CLI_SameHeight | CLI_SameTop,gOutputInput,10);
	aContainer->AddChildLayout(anOkButton,CLI_Below,gOutputInput,0,10);
	aContainer->AddChildLayout(aCancelButton,CLI_SameWidth | CLI_SameTop | CLI_Right,anOkButton,10);
	aContainer->AddChildLayout((new CenterLayout(aContainer,anOkButton,true,false))->Add(aCancelButton));

	// Add controls
	aContainer->AddChild(aLabel1);
	aContainer->AddChild(aLabel2);
	aContainer->AddChild(aLabel3);
	aContainer->AddChild(gProjectInput);
	aContainer->AddChild(gConfigInput);
	aContainer->AddChild(gOutputInput);
	aContainer->AddChild(anOkButton);
	aContainer->AddChild(aCancelButton);
	aContainer->AddChild(gBrowseConfig);
	aContainer->AddChild(gBrowseOutput);

	gConfigWindow->Resize(800,200 - (aContainer->Height() - anOkButton->Bottom()) + 10);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONGUI_ENTRY_POINT
{
	InitMainWindow();
	InitConfigWindow();
	ReadProjectMap();
	SyncListWithProjectMap();

	return gWindowMgr->MessageLoop();
}
