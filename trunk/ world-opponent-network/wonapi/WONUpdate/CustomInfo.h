//----------------------------------------------------------------------------------
// CustomInfo.h
//----------------------------------------------------------------------------------
#ifndef __CustomInfo_H__
#define __CustomInfo_H__

#pragma warning(disable : 4786)

#include <string>
#include <list>
#include "WONAPI/WONServer/ServerContext.h"
#include "WONGUI/GUIString.h"
#include "WONGUI/ResourceCollection.h"
#include "Resources/SierraUpWRS.h"
#include "PatchData.h"
#include "ResourceManager.h"
#include "WizardCtrl.h"  // Not sure why this is needed, but stops a link error.


namespace WONAPI
{

//----------------------------------------------------------------------------------
// SierraUpHelpCallbackProc.
//----------------------------------------------------------------------------------
extern "C" 
{
	typedef void (*SierraUpHelpCallbackProc)(void); //lint !e761
}


//----------------------------------------------------------------------------------
// CustomInfo.
//----------------------------------------------------------------------------------
class CustomInfo
{
private:
	// Primary application information.
	GUIString                       m_sDisplayName;           // Name to present to the user.
	std::string                     m_sProductName;           // Name to identify the application.
	std::string                     m_sExtraConfig;           // Extra Config for the patch.
	std::string                     m_sPatchFolder;           // Folder to save the patch to. 
	std::string                     m_sPatchTypes;            // Tells us which types of patches we want.
///	std::string                     m_sPatchDescExe;          // Exe used to show patch info files.
	std::string                     m_sNoPatchURL;            // URL to go to if there are no patches found yet the product has an invalid version.

	// Version infomation.
	std::string                     m_sCurVersion;            // Present version of the application.
	std::string                     m_sNewVersion;            // Patch (to be) version of the application.
	std::string                     m_sUpdateVersion;         // Version to write into the config file (for updating the config file).
	std::string                     m_sSierraUpVersion;       // Present version of SierraUp.

	// Directory Servers.
	std::list<std::string>*         m_pDirSrvrNames;          // List or Directory Server Names (queue names separately for Win95 socket bug).
	WONAPI::ServerContextPtr        m_pDirSrvrs;              // List of Directory Servers.

	// Custom Resource Access.
	ResourceManager                 m_ResourceManager;        // Resource Management.

	// Returned Patch Information.
	CPatchDataList                  m_PatchDataList;          // List of availible patches.
	CPatchData*                     m_pSelectedPatch;         // Patch that the user selected.
	bool                            m_bMixedPatches;          // Are some patches manual and some automatic?
	bool                            m_bOptionalUpgrade;       // Is there an optional upgrade avilible for this version?
	std::string                     m_sPatchDescriptionUrl;   // Url to download if the user wants info on the patch.

	// User Selected, previously downloaded, patch. 
	std::string                     m_sPatchFile;             // File (with folder) of the pending patch.

	// Help button callbacks.
	static SierraUpHelpCallbackProc m_pWelcomeHelpProc;       // Callback to invoke if the help button is pushed.
	static SierraUpHelpCallbackProc m_pConfigProxyHelpProc;   // Callback to invoke if the help button is pushed.
	static SierraUpHelpCallbackProc m_pMotdHelpProc;          // Callback to invoke if the help button is pushed.
	static SierraUpHelpCallbackProc m_pVersionCheckHelpProc;  // Callback to invoke if the help button is pushed.
	static SierraUpHelpCallbackProc m_pOptionalPatchHelpProc; // Callback to invoke if the help button is pushed.
	static SierraUpHelpCallbackProc m_pPatchDetailsHelpProc;  // Callback to invoke if the help button is pushed.
	static SierraUpHelpCallbackProc m_pSelectHostHelpProc;    // Callback to invoke if the help button is pushed.
	static SierraUpHelpCallbackProc m_pDownloadHelpProc;      // Callback to invoke if the help button is pushed.
	static SierraUpHelpCallbackProc m_pWebLaunchHelpProc;     // Callback to invoke if the help button is pushed.

	// Help button files (setting these override directly set callbacks).
	static std::string              m_sWelcomeHelpFile;       // File to show if the help Button is pushed.
	static std::string              m_sConfigProxyHelpFile;   // File to show if the help Button is pushed.
	static std::string              m_sMotdHelpFile;          // File to show if the help Button is pushed.
	static std::string              m_sVersionCheckHelpFile;  // File to show if the help Button is pushed.
	static std::string              m_sOptionalPatchHelpFile; // File to show if the help Button is pushed.
	static std::string              m_sPatchDetailsHelpFile;  // File to show if the help Button is pushed.
	static std::string              m_sSelectHostHelpFile;    // File to show if the help Button is pushed.
	static std::string              m_sDownloadHelpFile;      // File to show if the help Button is pushed.
	static std::string              m_sWebLaunchHelpFile;     // File to show if the help Button is pushed.

	static std::string              m_sShowHelpFileExe;       // Executable used to display the help file (if a help button is pushed).

	// Timeouts.
  	DWORD                           m_tMotdTimeout;           // How long to wait before giving up on the MotD.
	DWORD                           m_tVersionTimeout;        // How long to wait for the Current Version.
	DWORD                           m_tPatchTimeout;          // How long to wait for the Patch.
	DWORD                           m_tPatchDescTimeout;      // How long to wait for the Patch Descriptions.

	// Non-standard Control Flow (for patch monitoring).
	bool                            m_bAutoStart;             // Should we skip the Welcome dialog (go straight to the MotD)?
	bool                            m_bMonitorPatch;          // Should we watch the patch and attempt to validate it?
	std::string                     m_sVersionExe;            // Executable to look the version up in after a patch (must be a standard windows version resource).
	std::string                     m_sLaunchExe;             // Executable to start after the patch has been applied (or if no patch is needed).
	std::string                     m_sLaunchParams;          // Parameters to send to the m_sLaunchExe executable on startup.
	bool                            m_bHavePatchSuccessValue; // Do we have a m_nPatchSuccessValue value?
	int                             m_nPatchSuccessValue;     // Value returned from patch if successful.
	std::string                     m_sValidateVersionFile;   // File to check the version number in post patch (if we are monitoring the patch).
	std::string                     m_sResourceDirectory;	  // Directory for GUI resource files.
	bool                            m_bResizable;
	int                             m_iStartWidth;
	int                             m_iStartHeight;

	// Idea for the future - not used yet
	bool                            m_bAutomaticMode;         // Are we in automatic mode?

	// Remote Debugging.
	bool                            m_bDebug;                 // Do we want to enable debug tracing?

public:
	CustomInfo(void);
	~CustomInfo(void);

	inline void SetDisplayName(const GUIString& sName)
		{ m_sDisplayName = sName; }
	inline GUIString GetDisplayName(void) const
		{ return m_sDisplayName; }

	inline void SetProductName(const std::string& sName)
		{ m_sProductName = sName; }
	inline std::string GetProductName(void) const
		{ return m_sProductName; }

	inline void SetExtraConfig(const std::string& sExtra)
		{ m_sExtraConfig = sExtra; }
	inline std::string GetExtraConfig(void) const
	      { return m_sExtraConfig; }

	void SetPatchFolder(const std::string& sDir);
	std::string GetPatchFolder(void);

	inline void SetVersionExe(const std::string& sExe)
		{ m_sVersionExe = sExe; }
	inline std::string GetVersionExe(void) const
		{ return m_sVersionExe; }

	inline void SetLaunchExe(const std::string& sExe)
		{ m_sLaunchExe = sExe; }
	inline std::string GetLaunchExe(void) const
		{ return m_sLaunchExe; }

	void AppendLaunchParameters(const std::string& sParams);
	inline std::string GetLaunchParameterss(void) const
		{ return m_sLaunchParams; }

	inline void SetPatchSuccessValue(const int nValue, bool bSet = true)
		{ m_nPatchSuccessValue = nValue; m_bHavePatchSuccessValue = bSet; }
	inline int GetPatchSuccessValue(void) const
		{ return m_nPatchSuccessValue; }
	inline bool GetHavePatchSuccessValue(void) const
		{ return m_bHavePatchSuccessValue; }

	inline void SetValidateVersionFile(const std::string& sFile)
		{ m_sValidateVersionFile = sFile; }
	inline std::string GetValidateVersionFile(void) const
		{ return m_sValidateVersionFile; }

	inline void SetPatchTypes(const std::string& sTypes)
		{ m_sPatchTypes = sTypes; }
	inline std::string GetPatchTypes(void) const
		{ return m_sPatchTypes; }

	inline void SetNoPatchURL(const std::string& sURL)
		{ m_sNoPatchURL = sURL; }
	inline std::string GetNoPatchURL(void) const
		{ return m_sNoPatchURL; }

	inline void SetCurVersion(const std::string& sVersion)
		{ m_sCurVersion = sVersion; }
	inline std::string GetCurVersion(void) const
		{ return m_sCurVersion; }

	inline void SetNewVersion(const std::string& sVersion)
		{ m_sNewVersion = sVersion; }
	inline std::string GetNewVersion(void) const
		{ return m_sNewVersion; }

	inline void SetUpdateVersion(const std::string& sVersion)
		{ m_sUpdateVersion = sVersion; }
	inline std::string GetUpdateVersion(void) const
		{ return m_sUpdateVersion; }

	inline void SetSierraUpVersion(const std::string& sVersion)
		{ m_sSierraUpVersion = sVersion; }
	inline std::string GetSierraUpVersion(void) const
		{ return m_sSierraUpVersion; }

	inline void ClearDirectoryServers(void)
		{ m_pDirSrvrNames->clear(); m_pDirSrvrs->Clear(); }
	inline void AddDirectoryServer(const std::string& sDirSrvr)
		{ m_pDirSrvrNames->push_back(sDirSrvr); }
	inline std::list<std::string>* GetDirectoryServerNames(void) const
		{ return m_pDirSrvrNames; }
	WONAPI::ServerContextPtr GetDirectoryServers(void);

	inline ResourceManager* GetResourceManager(void)
		{ return &m_ResourceManager; }

	void ClearPatchList(void);
	inline void AddPatch(CPatchData* pPatch)
		{ m_PatchDataList.push_back(pPatch); }
	inline CPatchDataList* GetPatchList(void)
		{ return &m_PatchDataList; } //lint !e1536

	inline void SetSelectedPatch(CPatchData* pPatch)
		{ m_pSelectedPatch = pPatch; }
	inline CPatchData* GetSelectedPatch(void) const
		{ return m_pSelectedPatch; }

	inline void SetMixedPatches(bool bMixed)
		{ m_bMixedPatches = bMixed; }
	inline bool GetMixedPatches(void) const
		{ return m_bMixedPatches; }

	inline void SetOptionalUpgrade(bool bOptional)
		{ m_bOptionalUpgrade = bOptional; }
	inline bool GetOptionalUpgrade(void) const
		{ return m_bOptionalUpgrade; }

	inline void SetPatchDescriptionUrl(const std::string& sUrl)
		{ m_sPatchDescriptionUrl = sUrl; }
	inline std::string GetPatchDescriptionUrl(void) const
		{ return m_sPatchDescriptionUrl; }

	inline void SetPatchFile(std::string sFile)
		{ m_sPatchFile = sFile; }
	inline std::string GetPatchFile(void) const
		{ return m_sPatchFile; }

	static void SetWelcomeHelpFile(const std::string& sHelpFile);
	static inline std::string GetWelcomeHelpFile(void)
		{ return m_sWelcomeHelpFile; }
	static inline void SetWelcomeHelpCallback(SierraUpHelpCallbackProc pProc)
		{ m_pWelcomeHelpProc = pProc; }
	static inline SierraUpHelpCallbackProc GetWelcomeHelpCallback(void)
		{ return m_pWelcomeHelpProc; }
	static inline void InvokeWelcomeHelpCallback(void)
		{ if (m_pWelcomeHelpProc) m_pWelcomeHelpProc(); }

	static void SetConfigProxyHelpFile(const std::string& sHelpFile);
	static inline std::string GetConfigProxyHelpFile(void)
		{ return m_sConfigProxyHelpFile; }
	static inline void SetConfigProxyHelpCallback(SierraUpHelpCallbackProc pProc)
		{ m_pConfigProxyHelpProc = pProc; }
	static inline SierraUpHelpCallbackProc GetConfigProxyHelpCallback(void)
		{ return m_pConfigProxyHelpProc; }
	static inline void InvokeConfigProxyHelpCallback(void)
		{ if (m_pConfigProxyHelpProc) m_pConfigProxyHelpProc(); }

	static void SetMotdHelpFile(const std::string& sHelpFile);
	static inline std::string GetMotdHelpFile(void)                          
		{ return m_sMotdHelpFile; }
	static inline void SetMotdHelpCallback(SierraUpHelpCallbackProc pProc)
		{ m_pMotdHelpProc = pProc; }
	static inline SierraUpHelpCallbackProc GetMotdHelpCallback(void)
		{ return m_pMotdHelpProc; }
	static inline void InvokeMotdHelpCallback(void)
		{ if (m_pMotdHelpProc) m_pMotdHelpProc(); }

	static void SetVersionCheckHelpFile(const std::string& sHelpFile);
	static inline std::string GetVersionCheckHelpFile(void)
		{ return m_sVersionCheckHelpFile; }
	static inline void SetVersionCheckHelpCallback(SierraUpHelpCallbackProc pProc)
		{ m_pVersionCheckHelpProc = pProc; }
	static inline SierraUpHelpCallbackProc GetVersionCheckHelpCallback(void)
		{ return m_pVersionCheckHelpProc; }
	static inline void InvokeVersionCheckHelpCallback(void)
		{ if (m_pVersionCheckHelpProc) m_pVersionCheckHelpProc(); }

	static void SetOptionalPatchHelpFile(const std::string& sHelpFile);
	static inline std::string GetOptionalPatchHelpFile(void)
		{ return m_sOptionalPatchHelpFile; }
	static inline void SetOptionalPatchHelpCallback(SierraUpHelpCallbackProc pProc)
		{ m_pOptionalPatchHelpProc = pProc; }
	static inline SierraUpHelpCallbackProc GetOptionalPatchHelpCallback(void)
		{ return m_pOptionalPatchHelpProc; }
	static inline void InvokeOptionalPatchHelpCallback(void)
		{ if (m_pOptionalPatchHelpProc) m_pOptionalPatchHelpProc(); }

	static void SetPatchDetailsHelpFile(const std::string& sHelpFile);
	static inline std::string GetPatchDetailsHelpFile(void)
		{ return m_sPatchDetailsHelpFile; }
	static inline void SetPatchDetailsHelpCallback(SierraUpHelpCallbackProc pProc)
		{ m_pPatchDetailsHelpProc = pProc; }
	static inline SierraUpHelpCallbackProc GetPatchDetailsHelpCallback(void)
		{ return m_pPatchDetailsHelpProc; }
	static inline void InvokePatchDetailsHelpCallback(void)
		{ if (m_pPatchDetailsHelpProc) m_pPatchDetailsHelpProc(); }

	static void SetSelectHostHelpFile(const std::string& sHelpFile);
	static inline std::string GetSelectHostHelpFile(void)
		{ return m_sSelectHostHelpFile; }
	static inline void SetSelectHostHelpCallback(SierraUpHelpCallbackProc pProc)
		{ m_pSelectHostHelpProc = pProc; }
	static inline SierraUpHelpCallbackProc GetSelectHostHelpCallback(void)
		{ return m_pSelectHostHelpProc; }
	static inline void InvokeSelectHostHelpCallback(void)
		{ if (m_pSelectHostHelpProc) m_pSelectHostHelpProc(); }

	static void SetDownloadHelpFile(const std::string& sHelpFile);
	static inline std::string GetDownloadHelpFile(void)
		{ return m_sDownloadHelpFile; }
	static inline void SetDownloadHelpCallback(SierraUpHelpCallbackProc pProc)
		{ m_pDownloadHelpProc = pProc; }
	static inline SierraUpHelpCallbackProc GetDownloadHelpCallback(void)
		{ return m_pDownloadHelpProc; }
	static inline void InvokeDownloadHelpCallback(void)
		{ if (m_pDownloadHelpProc) m_pDownloadHelpProc(); }

	static void SetWebLaunchHelpFile(const std::string& sHelpFile);
	static inline std::string GetWebLaunchHelpFile(void)
		{ return m_sWebLaunchHelpFile; }
	static inline void SetWebLaunchHelpCallback(SierraUpHelpCallbackProc pProc)
		{ m_pWebLaunchHelpProc = pProc; }
	static inline SierraUpHelpCallbackProc GetWebLaunchHelpCallback(void)
		{ return m_pWebLaunchHelpProc; }
	static inline void InvokeWebLaunchHelpCallback(void)
		{ if (m_pWebLaunchHelpProc) m_pWebLaunchHelpProc(); }

	static void SetShowHelpFileExe(const std::string& sExe)
		{ m_sShowHelpFileExe = sExe; }
	static inline std::string GetShowHelpFileExe(void)
		{ return m_sShowHelpFileExe; }

	inline void SetMotdTimeout(DWORD tTime)
		{ m_tMotdTimeout = tTime; }
	inline DWORD GetMotdTimeout(void) const
		{ return m_tMotdTimeout; }

	inline void SetVersionTimeout(DWORD tTime)
		{ m_tVersionTimeout = tTime; }
	inline DWORD GetVersionTimeout(void) const
		{ return m_tVersionTimeout; }

	inline void SetPatchTimeout(DWORD tTime)
		{ m_tPatchTimeout = tTime; }
	inline DWORD GetPatchTimeout(void) const
		{ return m_tPatchTimeout; }

	inline void SetPatchDescTimeout(DWORD tTime)
		{ m_tPatchDescTimeout = tTime; }
	inline DWORD GetPatchDescTimeout(void) const
		{ return m_tPatchDescTimeout; }

	inline void SetAutoStart(bool bAuto)
		{ m_bAutoStart = bAuto; }
	inline bool GetAutoStart(void) const
		{ return m_bAutoStart; }

	inline void SetMonitorPatch(bool bMonitor)
		{ m_bMonitorPatch = bMonitor; }
	inline bool GetMonitorPatch(void) const
		{ return m_bMonitorPatch; }

	inline void SetAutomaticMode(bool bAuto = true)
		{ m_bAutomaticMode = bAuto; }
	inline bool GetAutomaticMode(void) const
		{ return m_bAutomaticMode; }

	inline void SetDebug(bool bEnable = true)
		{ m_bDebug = bEnable; }
	inline bool GetDebug(void) const
		{ return m_bDebug; }

	inline void SetResourceDirectory(const std::string& theResourceDirectory)
		{ m_sResourceDirectory = theResourceDirectory; }
	inline const std::string& GetResourceDirectory(void) const
		{ return m_sResourceDirectory; }

	inline void SetResizable(bool theResizable)
		{ m_bResizable = theResizable; }
	inline bool GetResizable(void) const
		{ return m_bResizable; }

	inline void SetStartWidth(int theStartWidth)
		{ m_iStartWidth = theStartWidth; }
	inline int GetStartWidth(void) const
		{ return m_iStartWidth; }

	inline void SetStartHeight(int theStartHeight)
		{ m_iStartHeight = theStartHeight; }
	inline int GetStartHeight(void) const
		{ return m_iStartHeight; }

	void SaveDefaultSelection(const GUIString& sName);
	GUIString LoadDefaultSelection(void);
	void TagFtpDownload(const std::string& sPatchFile, const std::string& sVersion, long nSize, long nCheckSum);
	bool MatchFtpDownloadTag(const std::string& sPatchFile, const std::string& sVersion, long nSize, long nCheckSum);
	void SaveLastMonitoredVersion(const std::string& sVersion);
	std::string GetLastMonitoredVersion(void);
};


//----------------------------------------------------------------------------------
// Prototypes.
//----------------------------------------------------------------------------------
extern "C" CustomInfo* GetCustomInfo(void);

} // namespace

#endif

