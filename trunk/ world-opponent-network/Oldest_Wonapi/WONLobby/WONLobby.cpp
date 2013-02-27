// WONLobby.cpp : Defines the entry point for the DLL application.
// 
#pragma warning(disable : 4786)

#include "WONLobby.h"
#include "WONCommon/Platform.h"					// Platform specfic headers
#include "WONAPI.h"	

#include <shellapi.h>
#include <time.h>
#include "Component.h"
#include "MSWindow.h"
#include "ResourceCollection.h"

using namespace std;
using namespace WONAPI;

/*
#ifdef _USRDLL
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			gLobbyDllHandle = (HINSTANCE)hModule;
		break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif
*/

///////////////////////////////////////////////////////////////////////////////
WONLobby::WONLobby() :
	mWindowP(NULL),
	mResourceCollectionP(NULL),
	mLobbyName(),
	mLobbyDoneCallback(NULL),
	mLobbyDoneCallbackData(NULL)
	//mCrit()
{
}

///////////////////////////////////////////////////////////////////////////////
WONLobby::~WONLobby()
{
	Shutdown();
}

///////////////////////////////////////////////////////////////////////////////
void
WONLobby::LobbyWindowClosed()
{
	mWindowP->GetRoot()->RemoveAllChildren();

	// Call callback
	if (mLobbyDoneCallback)
	{
		mLobbyDoneCallback(LobbyResult_Success, mLobbyDoneCallbackData);
	} // Set event to unblock
	else
	{

	}
}

///////////////////////////////////////////////////////////////////////////////
WONLobby::LobbyResult
WONLobby::Init(const LobbyDefinition& theLobbyDefinitionR)
{
	mResourceCollectionP = new WONResourceCollection();
	mLobbyName = theLobbyDefinitionR.GetLobbyName();
	string aResource;
	if (mLobbyName.length() > 0)
	{
		aResource = mLobbyName + ".lob";
	}
	else // Default to Resource.lob as the name of the Resource file
	{
		aResource = "Resource.lob";
	}
	mResourceCollectionP->OpenCollection(aResource.c_str());

	MSWindow::Init(GetModuleHandle(NULL));

	mWindowP = new MSWindow();

	return LobbyResult_Success;
}

///////////////////////////////////////////////////////////////////////////////
WONLobby::LobbyResult
WONLobby::LaunchLobby(const RECT& theWindowRectR)
{
	mLobbyDoneCallback = NULL;
	mLobbyDoneCallbackData = NULL;

	WONRectangle aWonRect(theWindowRectR.left, theWindowRectR.top, theWindowRectR.right - theWindowRectR.left, theWindowRectR.bottom - theWindowRectR.top);
	mWindowP->Create(aWonRect);

	mWindowP->SetTitle(mLobbyName + " Lobby"); 
	mWindowP->SetCloseEvent(new UserEvent(LobbyWindowClosedStatic, this));

	DefineLobby();

	// Block until Lobby exits
	// Right now this does not make sense that we would want to do this so removed for now
	//HANDLE aEvent = CreateEvent(NULL, false, false, NULL);
	//WaitForSingleObject(aEvent,10000);

	return LobbyResult_Success;
}

///////////////////////////////////////////////////////////////////////////////
WONLobby::LobbyResult
WONLobby::LaunchLobby(const RECT& theWindowRectR, LobbyDoneCallback theLobbyDoneCallback, void* theUserData)
{
	mLobbyDoneCallback = theLobbyDoneCallback;
	mLobbyDoneCallbackData = theUserData;

	WONRectangle aWonRect(theWindowRectR.left, theWindowRectR.top, theWindowRectR.right - theWindowRectR.left, theWindowRectR.bottom - theWindowRectR.top);
	mWindowP->Create(aWonRect);

	mWindowP->SetTitle(mLobbyName + " Lobby"); 
	mWindowP->SetCloseEvent(new UserEvent(LobbyWindowClosedStatic, this));

	DefineLobby();

	return LobbyResult_Success;
}

///////////////////////////////////////////////////////////////////////////////
WONLobby::LobbyResult
WONLobby::Shutdown()
{
	//AutoCrit aCrit(mCrit);

	delete mWindowP;
	mWindowP = NULL;

	delete mResourceCollectionP;
	mResourceCollectionP = NULL;

	return LobbyResult_Success;
}

///////////////////////////////////////////////////////////////////////////////
void 
WONLobby::SetResizeInfo(Component *theComponent, int dx, int dy, int dw, int dh)
{
	LayoutManagerPtr aLayout = new LayoutManager;
	aLayout->SetResizeInfo(dx,dy,dw,dh);
	theComponent->SetLayout(aLayout);
}

///////////////////////////////////////////////////////////////////////////////
void
WONLobby::LobbyWindowClosedStatic(void* theData)
{
	WONLobby* This = (WONLobby*)theData;
	This->LobbyWindowClosed();
}




