//----------------------------------------------------------------------------------
// WONUpdateCtrl.h
//----------------------------------------------------------------------------------
#ifndef __WONUpdateCtrl_H__
#define __WONUpdateCtrl_H__

#include "WizardCtrl.h"
#include "WelcomeCtrl.h"
#include "ConfigProxyCtrl.h"
#include "MotdCtrl.h"
#include "VersionCheckCtrl.h"
#include "OptionalPatchCtrl.h"
#include "PatchDetailsCtrl.h"
#include "SelectHostCtrl.h"
#include "DownloadCtrl.h"
//@@@#include "TukatiDownloadCtrl.h"
#include "WebLaunchCtrl.h"
#include "AboutCtrl.h"
#include "LoginCtrl.h"
#include "PatchCore.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// Sierra Update Result codes (SUR_...).
//----------------------------------------------------------------------------------
const int SUR_UP_TO_DATE      =  0; // The application is up to date - no patch required.
const int SUR_PATCH_FOUND     =  1; // A patch is availible.
const int SUR_PATCH_READY     =  2; // The patch has been downloaded and must be run,  
                                    // call GetPatchFile(...) for the executable.
const int SUR_OPTIONAL_PATCH  =  3; // An optional patch is available, but was not applied.
const int SUR_PATCH_APPLIED   =  4; // A patch was found and applied.
const int SUR_PATCHING_SELF   =  5; // A SierraUp patch was found and is being applied.
const int SUR_START_OVER      =  6; // A patch was applied but we are going to look for another.

const int SUR_UNKNOWN         = -1; // Unknown error.
const int SUR_USER_CANCEL     = -2; // The user elected not to patch (there might or might not be a patch).
const int SUR_NO_CONNECT      = -3; // Unable to connect to server (don't know if patch needed.)
const int SUR_NO_HOST_LIST    = -4; // Unable to connect to server (don't know if patch needed.)
const int SUR_NO_DOWNLOAD     = -5; // Unable to download the patch (there is a patch).
const int SUR_MANUAL_DOWNLOAD = -6; // Manual Patch being downloaded and installed.
const int SUR_NO_PATCHES      = -7; // The version is out of date, but no patches were found.
const int SUR_PATCH_FAILED    = -8; // The patch could not be run, or failed to run properly.


//----------------------------------------------------------------------------------
// SierraUpFinishedCallbackProc.
//----------------------------------------------------------------------------------
extern "C" 
{
	typedef void (*SierraUpFinishedCallbackProc)(int nReturnCode); //lint !e761
	typedef void (*SierraUpTitleChangedCallbackProc)(const GUIString& sTittle); //lint !e761
}

WON_PTR_FORWARD(ConfigObject);

//----------------------------------------------------------------------------------
// WONUpdateCtrl.
//----------------------------------------------------------------------------------
class WONUpdateCtrl : public Container
{
public:
	typedef enum
	{
		WS_None,
		WS_Welcome,
		WS_ConfigProxy,
		WS_Motd,
		WS_VersionCheck,
		WS_OptionalPatch,
		WS_PatchDetails,
		WS_SelectHost,
		WS_Download,
//@@@		WS_TukatiDownload,
		WS_WebLaunch,

	} WIZARD_SCREEN;

protected:
	WIZARD_SCREEN                    m_CurrentScreen;      // What screen is being displayed?
	WIZARD_SCREEN                    m_PreviousScreen;     // What screen is displayed last?
	WelcomeCtrlPtr                   m_pWelcomeCtrl;       // Welcome Screen.
	ConfigProxyCtrlPtr               m_pConfigProxyCtrl;   // Configure Proxy Screen.
	MotdCtrlPtr                      m_pMotdCtrl;          // MotD Screen.
	VersionCheckCtrlPtr              m_pVersionCheckCtrl;  // Version Check Status Screen.
	OptionalPatchCtrlPtr             m_pOptionalPatchCtrl; // Optional Patch Screen.
	PatchDetailsCtrlPtr              m_pPatchDetailsCtrl;  // Patch Details Screen.
	SelectHostCtrlPtr                m_pSelectHostCtrl;    // Select Host Screen.
	DownloadCtrlPtr                  m_pDownloadCtrl;      // Download Screen.
	WebLaunchCtrlPtr                 m_pWebLaunchCtrl;     // Web Launch Screen.
	SierraUpFinishedCallbackProc     m_pFinishedProc;      // Callback to invoke when SierraUp is done.
	SierraUpTitleChangedCallbackProc m_pTitleChangedProc;  // Callback to invoke when the title should change.
	int                              m_nReturnResult;      // Result of the update operation (to return to the caller).
	PatchCore                        m_PatchCore;          // True patching threads and operations.
	AboutCtrlPtr                     m_pAboutCtrl;         // About dialog control
	LoginCtrlPtr                     m_pLoginCtrl;         // Login dialog control
//@@@	TukatiDownloadCtrlPtr            m_pTukatiDownloadCtrl;// Tukati download Screen.
//@@@	TukatiSpeedPassInstallCtrlPtr    m_pTukatiSpeedPassInstallCtrl; // Tukati speed pass install dialog.

	ConfigObjectPtr mConfigObject;

	void Finish(int nReturnCode);

public:
	virtual void HandleComponentEvent(ComponentEvent* pEvent);
	virtual void SizeChanged(void);

public:
	WONUpdateCtrl(void);
	~WONUpdateCtrl(void);

	void SetReturnResult(int nResult) { m_nReturnResult = nResult; }
	int GetReturnResult(void) const   { return m_nReturnResult; }
	PatchCore* GetPatchCore(void)     { return &m_PatchCore; }

	inline void SetFinishedCallback(SierraUpFinishedCallbackProc pProc) { m_pFinishedProc = pProc; }
	void SetTitleChangedCallback(SierraUpTitleChangedCallbackProc pProc);

	ResourceConfigTablePtr InitScreens(void);
	ResourceConfigTablePtr GetResourceTable(void);

	virtual bool KeyDown(int nKey);

	void UpdateTitle(WIZARD_SCREEN Screen);
	void ShowScreen(WIZARD_SCREEN WS);
	void ShowNextScreen(void);
	void ShowPreviousScreen(void);

	inline WIZARD_SCREEN GetScreen(void) const { return m_CurrentScreen; }
	WizardCtrl* GetScreenCtrl(void);

	AboutCtrlPtr GetAboutCtrl(void) { return m_pAboutCtrl; }
//@@@	TukatiSpeedPassInstallCtrlPtr GetTukatiSpeedPassInstallCtrl(void) { return m_pTukatiSpeedPassInstallCtrl; }

	void ShowHelp(void);

	SelectHostCtrl* GetSelectHostCtrl(void) { return m_pSelectHostCtrl.get(); }
};
typedef SmartPtr<WONUpdateCtrl> WONUpdateCtrlPtr;


//----------------------------------------------------------------------------------
// Prototypes.
//----------------------------------------------------------------------------------
extern "C" WONUpdateCtrl* GetMainControl(void);

};

#endif
