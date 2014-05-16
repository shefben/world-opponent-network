#include "WONAPI.h"

#include "LobbyGame.h"
#include "LobbyStaging.h"
#include "LobbyScreen.h"
#include "ServerListCtrl.h"
#include "WONLobby.h"


#include "WONCommon/StringParser.h"
#include "WONGUI/GUICompat.h"
#include "WONGUI/jpeglib/JPGDecoder.h"
#include "WONGUI/pnglib/PNGDecoder.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/ComboBox.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/MultiListBox.h"
#include "WONGUI/MSSound.h"
#include "WONGUI/WONSound.h"
#include "WONGUI/mp3lib/MP3Decoder.h"
#include "WONSocket/SocketThreadEx.h"


using namespace WONAPI;

PlatformWindowManagerPtr gWindowMgr;
PlatformWindowPtr gWindow;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TestServerDetailsItem : public ServerDetailsItem
{
public:
	virtual void PaintDetailsHook(Graphics &g, MultiListArea *theListArea);
	virtual int GetDetailsHeightHook() { return 170; }

	static ServerDetailsItem* ItemFactory() { return new TestServerDetailsItem; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TestServerDetailsItem::PaintDetailsHook(Graphics &g, MultiListArea *theListArea)
{
	static const char *col1[] = 
	{
		"Game Type:","Map Size:","Map Type:","Resources:","Starting Epoch:","Ending Epoch:",
		"Game Until Limit:", "Game Variant:", "Difficulty Level:", "Game Speed:", 
		"# of Wonders for Victory:", NULL 
	};

	static const char *col2[] = 
	{ 
		"\"Saved Game\"","Tiny", "Continental", "Tournament-Low", "Atomic Age - WWI", 
		"Nano Age", "500", "Tournament", "Easy", "Fast", "3", NULL 
	};


	static const char *col3[] = { "Reveal Map:","Use Custom Civs:","Lock Teams:","Lock Speed:", "PingTime:",NULL };
	static const char *col4[] = { "Yes","Yes","Yes","Yes",NULL,NULL };
	char aBuf[20];
	sprintf(aBuf,"%d",mGame->GetPing());
	col4[4] = aBuf;

	static const char **cols[] = { col1, col2, col3, col4 };
	static const int aNumCols = sizeof(cols)/4;
	static int colgaps[aNumCols] = { 10, 50, 10, 0};
	static int colcolors[aNumCols] = {0x800000, 0, 0x800000, 0};


	Font *aFont = theListArea->GetFont();
	g.SetFont(aFont);

	int x = 0, y = 0;
	int i=0;

	for(i=0; i<aNumCols; i++)
	{
		y = 0;
		int j = 0;
		int aMaxWidth = 0;
		g.SetColor(colcolors[i]);
		while(cols[i][j]!=NULL)
		{
			std::string aStr = cols[i][j];
			int aWidth = aFont->GetStringWidth(aStr);
			if(aWidth>aMaxWidth)
				aMaxWidth = aWidth;

			g.DrawString(aStr,x,y);
			y+=aFont->GetHeight();
			j++;
		}


		x+=aMaxWidth + colgaps[i];		
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitWindow(Window *theWindow, bool isFullScreen)
{
	// Create Window
	int aCreateFlags = CreateWindow_Centered | CreateWindow_SizeSpecClientArea;
	if(isFullScreen)
		aCreateFlags |= CreateWindow_Popup;

	int aWidth = 800;
	int aHeight = 600;

	theWindow->SetTitle("Lobby");
	theWindow->SetCreateFlags(aCreateFlags);
	theWindow->Create(WONRectangle(0,0,aWidth,aHeight));


	RootContainer *aRoot = theWindow->GetRoot();

	//ServerDetailsItem::SetItemFactory(TestServerDetailsItem::ItemFactory);
	// Game Specific
	ServerListCtrl::ColumnInfo *anInfo = ServerListCtrl::GetColumnInfo("Epoch");
	anInfo = ServerListCtrl::GetColumnInfo("MapName");
	anInfo = ServerListCtrl::GetColumnInfo("GameType");

	WONLobby::Init();
	WONLobby::InitAuth(L"China");
	ContainerPtr aLobby = WONLobby::GetLobbyContainer();
	aRoot->AddChildLayout(aLobby,CLI_SameSize,aRoot);	
	aRoot->AddChild(aLobby);
	WONLobby::StartLobby();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONGUI_ENTRY_POINT
{
	const char *aStr = GetCommandLine();
	StringParser aParser(aStr);
	std::string aParam1;
	aParser.ReadString(aParam1,true);
	aParam1.erase();
	aParser.ReadString(aParam1,true);

	bool fullscreen = !aParam1.empty();

	Container::SetMouseWheelGoesToFocusChildDef(false);

	WONSound_InitMS();
	gWindowMgr = new PlatformWindowManager;
	gWindowMgr->SetIsFullScreen(fullscreen,800,600);
	gWindowMgr->SetPopupInOwnWindowDef(false);
	MultiSoundDecoderPtr aDecoder = new MultiSoundDecoder;
	WONMP3Decoder::Add(aDecoder);
	WONWaveDecoder::Add(aDecoder);
	gWindowMgr->SetSoundDecoder(aDecoder);
//	gWindowMgr->SetSoundDecoder(new MSSoundDecoder);

	JPGDecoder::Add(gWindowMgr->GetImageDecoder());
	PNGDecoder::Add(gWindowMgr->GetImageDecoder());

	gWindow = new PlatformWindow;
	gWindow->SetMinimizeOnResolutionRestore(true);


	// Init WONAPI
	WONAPICoreEx anAPI;
	anAPI.SetDoPumpThread(true);
	anAPI.Startup();

	// InitWindow
	InitWindow(gWindow,fullscreen);
	
	while(true)
	{
		LobbyStatus aStatus = WONLobby::RunLobby();
		LobbyScreen::ShowWindow(false);
		WONLobby::SuspendLobby();

		if(aStatus==LobbyStatus_JoinGame)
			MSMessageBox::Msg(NULL,"Playing Game.","You're playing a fun game now.");
		else if(aStatus==LobbyStatus_HostGame)
			MSMessageBox::Msg(NULL,"Hosting Game.","You're hosting a fun game now.");
		else
			break;

		LobbyScreen::ShowWindow(true);
		WONLobby::ResumeLobby();
		LobbyStaging::SetGameInProgress(false);
	}
	

	WONLobby::Destroy();
	gWindowMgr->CloseAllWindows();
	WONSound_DestroyMS();

	return 0;
}