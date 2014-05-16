//----------------------------------------------------------------------------------
// PatchCore.cpp
//----------------------------------------------------------------------------------
#include "WONAPI/WONMisc/GetMOTDOp.h"
#include "PatchCore.h"
#include "CustomInfo.h"
#include "PatchUtils.h"
#include "CRC32.h"
#include "MessageDlg.h"
#include "DebugLog.h"
#include "WonUpdateCtrl.h"


using namespace WONAPI;


//----------------------------------------------------------------------------------
// Constants.
//----------------------------------------------------------------------------------
LPSTR        sHostBmpKey           = "hostbmp";   // Titan Data Object ID (not a wide string).
LPSTR        sHostNameKey          = "hostname";  // Titan Data Object ID (not a wide string).
LPSTR        sHostUrlKey           = "hosturl";   // Titan Data Object ID (not a wide string).
LPSTR        sHostTxtKey           = "hosttxt";   // Titan Data Object ID (not a wide string).
LPSTR        sMustVisitKey         = "mustvisit"; // Titan Data Object ID (not a wide string).

LPSTR        sDescUrlKey           = "urlds ";    // Version data Key (include the space) (not a wide string).
//LPSTR      NO_VERSION            = "0.0.0.0";   // Default version string corresponding to 'no version'.
std::wstring wsPatchServerDispName = L"Patch Server";
std::wstring wsSierraUpPatchDir    = L"/SierraUp/Patch";
std::string  sSierraUpProdId       = "SierraUp";
std::string  sSierraUpExtraConfig  = "EN";        // SierraUp is always in 'English'.


//----------------------------------------------------------------------------------
// Static member variables.
//----------------------------------------------------------------------------------
PatchCore* PatchCore::m_pInstance = NULL;


//----------------------------------------------------------------------------------
// PatchCore Constructor.
//----------------------------------------------------------------------------------
PatchCore::PatchCore(void)
{
	m_pInstance           = this;
	m_bNoMotdFound        = false;
	m_bStoping            = false;
	m_nSelfGetVerOpResult = SUR_UNKNOWN;
	m_nGameGetVerOpResult = SUR_UNKNOWN;
	m_bSkipValidation     = false;

	m_pSelfPatchServers = new ServerContext();
	m_pGamePatchServers = new ServerContext();
}

//----------------------------------------------------------------------------------
// PatchCore Destructor.
//----------------------------------------------------------------------------------
PatchCore::~PatchCore(void)
{
	StopOperations();
	m_pInstance = NULL;
}


//==================================================================================
// DNS Test.
//==================================================================================

//----------------------------------------------------------------------------------
// DnsTestCallback: Callback invoked when the operation completes.
//----------------------------------------------------------------------------------
void PatchCore::DnsTestCallback(AsyncOp* pOp)
{
	if (pOp->Killed())
		return;

	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	// Process the results.
	if (pOp->GetStatus() == WS_Success)
		m_DnsTestOp.m_sResult = pResMgr->GetFinishedString(Global_Success_String);
	else
		m_DnsTestOp.m_sResult = pResMgr->GetFinishedString(Global_CheckConection_String);

	// Inform anyone who cares that we are finished.
	m_DnsTestOp.InvokeCallbacks();

	// Start the next operation in the chain.
	if (pOp->GetStatus() == WS_Success)
		DoNextOperation();
}

//----------------------------------------------------------------------------------
// DoDnsTest: Kick off the operation if it has not already finished.
//----------------------------------------------------------------------------------
void PatchCore::StartDnsTest(void)
{
	DebugLog("Starting DNS Test...\n");

	// Prepare the operation.
	m_DnsTestOp.m_pOp = new DNSTestOp("www.sierra.com");
	m_DnsTestOp.m_pOp->SetCompletion(new PatchCoreCompletion(DnsTestCallback));

	// Start the operation.
	m_DnsTestOp.m_pOp->Run(OP_MODE_ASYNC, OP_TIMEOUT_INFINITE);
}

//----------------------------------------------------------------------------------
// StopDnsTest: Shut down the operation if it has not completed.
//----------------------------------------------------------------------------------
void PatchCore::StopDnsTest(void)
{
//	if (m_DnsTestOp.m_pOp.get() && m_DnsTestOp.m_pOp->AwaitingCompletion())
	if (m_DnsTestOp.m_pOp.get())
		m_DnsTestOp.m_pOp->Kill();
}

//----------------------------------------------------------------------------------
// DoDnsTest: Kick off the operation if it has not already finished.
//----------------------------------------------------------------------------------
void PatchCore::DoDnsTest(void)
{
	// Make sure we are not shutting down.
	if (m_bStoping)
		return;

	// If the op has never been created, we need to run it.
	if (! m_DnsTestOp.m_pOp.get())
	{
		StartDnsTest();
		return;
	}

	// If the operation completed previously, directly invoke the callback.
	if (m_DnsTestOp.m_pOp->GetStatus() == WS_Success)
	{
		m_DnsTestOp.InvokeCallbacks();
		return;
	}

	// If the operation is not already running, start it.
	if (! m_DnsTestOp.m_pOp->AwaitingCompletion())
		StartDnsTest();
}


//==================================================================================
// MOTD Download.
//==================================================================================

//----------------------------------------------------------------------------------
// MotdCallback: Callback invoked when the operation completes.
//----------------------------------------------------------------------------------
void PatchCore::MotdCallback(AsyncOp* pOp)
{
	if (pOp->Killed())
		return;

	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	// Process the results.
	if (pOp->GetStatus() == WS_Success)
		m_GetMotdOp.m_sResult = pResMgr->GetFinishedString(Global_Success_String);
	else if (pOp->GetStatus() == WS_GetMOTD_SysNotFound)
	{
		m_GetMotdOp.m_sResult = pResMgr->GetFinishedString(Global_MotdNotFound_String);
		m_bNoMotdFound = true; // If the MotD failed, it may be because it does not exist.
	}
	else if (pOp->GetStatus() == WS_GetMOTD_GameNotFound)
	{
		m_GetMotdOp.m_sResult = pResMgr->GetFinishedString(Global_GameMotdNotFound_String);
		m_bNoMotdFound = true; // If the MotD failed, it may be because it does not exist.
	}
	else
	{
		m_GetMotdOp.m_sResult = pResMgr->GetFinishedString(Global_MotdError_String);
		ReplaceSubString(m_GetMotdOp.m_sResult , "%ERROR%", WONStatusToString(pOp->GetStatus()));
		m_bNoMotdFound = true; // If the MotD failed, it may be because it does not exist.
	}

	// Inform anyone who cares that we are finished.
	m_GetMotdOp.InvokeCallbacks();

	// Start the next operation in the chain.
	DoNextOperation(); // Always do this - we don't care if this operation failed.
}

//----------------------------------------------------------------------------------
// StartMotdDownload: Kick off the operation.
//----------------------------------------------------------------------------------
void PatchCore::StartMotdDownload(void)
{
	DebugLog("Starting MotD Download...\n");

	CustomInfo* pCI = GetCustomInfo();

//	m_bUserSkippedMotd = false;

	// Prepare the download.
	m_GetMotdOp.m_pOp = new GetMOTDOp(pCI->GetProductName());
	m_GetMotdOp.m_pOp->SetCompletion(new PatchCoreCompletion(MotdCallback));
	if (pCI->GetExtraConfig().length())
		dynamic_cast<GetMOTDOp*>(m_GetMotdOp.m_pOp.get())->SetExtraConfig(pCI->GetExtraConfig());

	// Start the operation.
	m_GetMotdOp.m_pOp->Run(OP_MODE_ASYNC, pCI->GetMotdTimeout());
}

//----------------------------------------------------------------------------------
// StopMotdDownload: Shut down the operation if it has not completed.
//----------------------------------------------------------------------------------
void PatchCore::StopMotdDownload(void)
{
//	if (m_GetMotdOp.m_pOp.get() && m_GetMotdOp.m_pOp->AwaitingCompletion())
	if (m_GetMotdOp.m_pOp.get())
		m_GetMotdOp.m_pOp->Kill();
}

//----------------------------------------------------------------------------------
// DoMotdDownload: Kick off the operation if it has not already finished.
//----------------------------------------------------------------------------------
void PatchCore::DoMotdDownload(void)
{
	// Make sure we are not shutting down.
	if (m_bStoping)
		return;

	// If the op has never been created, we need to run it.
	if (! m_GetMotdOp.m_pOp.get())
	{
		StartMotdDownload();
		return;
	}

	// If the operation completed previously, directly invoke the callback.
	if (m_GetMotdOp.m_pOp->GetStatus() == WS_Success)
	{
		m_GetMotdOp.InvokeCallbacks();
		return;
	}

	// If the operation is not already running, start it.
	if (! m_GetMotdOp.m_pOp->AwaitingCompletion())
		StartMotdDownload();
}


//==================================================================================
// Server Lookup (from directery server entries) operation.
//==================================================================================

//----------------------------------------------------------------------------------
// ServerLookupCallback: Callback invoked when the operation completes.
//----------------------------------------------------------------------------------
void PatchCore::ServerLookupCallback(AsyncOp* pRawOp)
{
	if (pRawOp->Killed())
		return;

	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();
	GetMultiDirOp* pOp = dynamic_cast<GetMultiDirOp*>(pRawOp);

	std::wstring wsGamePatchDir = ConstructDirForGameUpdateServer();

	// Process the results.
	if (pOp->GetStatus() == WS_DirServ_MultiGetPartialFailure)
		m_ServerLookupOp.m_sResult = pResMgr->GetFinishedString(Global_PartialDirServerFailure_String);
	else if (pOp->GetStatus() != WS_Success)
	{
		m_ServerLookupOp.m_sResult = pResMgr->GetFinishedString(Global_DirServerFailure_String);
		ReplaceSubString(m_ServerLookupOp.m_sResult, "%ERROR%", WONStatusToString(pOp->GetStatus()));
	}
	else
	{
		m_ServerLookupOp.m_sResult = pResMgr->GetFinishedString(Global_Success_String);

		m_pSelfPatchServers->Clear();
		m_pGamePatchServers->Clear();
//#define BRENT_TEST
#ifdef BRENT_TEST
m_pSelfPatchServers->AddAddress("brentl:15800");
m_pGamePatchServers->AddAddress("brentl:15800");
#else
		GetEntityRequestList::const_iterator anItr = pOp->GetRequestList().begin();
		for (; anItr != pOp->GetRequestList().end(); ++anItr)
		{
			if ((*anItr)->GetStatus() == WS_Success)
			{
				// Parse each request.
				DirEntityList::const_iterator anEntityItr = (*anItr)->GetDirEntityList().begin();
				for (; anEntityItr != (*anItr)->GetDirEntityList().end(); ++anEntityItr)
				{
					// Parse each entity.
					if ((*anEntityItr)->mDisplayName == wsPatchServerDispName)
					{
						if ((*anItr)->GetPath() == wsSierraUpPatchDir)
						{
							// These are the SierraUp servers.
							m_pSelfPatchServers->AddAddress((*anEntityItr)->GetNetAddrAsIP());
						}
						else if ((*anItr)->GetPath() == wsGamePatchDir)
						{
							// These are the game patch servers.
							m_pGamePatchServers->AddAddress((*anEntityItr)->GetNetAddrAsIP());
						}
					}
				}
			}
		}
#endif
	}

	// Inform anyone who cares that we are finished.
	m_ServerLookupOp.InvokeCallbacks();

	// Start the next operation in the chain.
	if (pOp->GetStatus() == WS_Success)
		DoNextOperation();
}

//----------------------------------------------------------------------------------
// StartServerLookup: Kick off the operation.
//----------------------------------------------------------------------------------
void PatchCore::StartServerLookup(void)
{
	DebugLog("Starting Server Lookup Operation...\n");

	CustomInfo* pCI = GetCustomInfo();

	// Prepare the download.
	m_ServerLookupOp.m_pOp = new GetMultiDirOp(pCI->GetDirectoryServers());
	GetMultiDirOp* pOp = dynamic_cast<GetMultiDirOp*>(m_ServerLookupOp.m_pOp.get());
	pOp->SetCompletion(new PatchCoreCompletion(ServerLookupCallback));
	pOp->SetFlags(DIR_GF_DECOMPROOT | DIR_GF_DECOMPSERVICES | DIR_GF_SERVADDNETADDR |
				  DIR_GF_ADDDISPLAYNAME | DIR_GF_SERVADDPATH | DIR_GF_ADDTYPE);

	pOp->AddPath(wsSierraUpPatchDir);
	pOp->AddPath(ConstructDirForGameUpdateServer());

	// Start the operation.
	m_ServerLookupOp.m_pOp->Run(OP_MODE_ASYNC, pCI->GetVersionTimeout());
}

//----------------------------------------------------------------------------------
// StopServerLookup: Shut down the operation if it has not completed.
//----------------------------------------------------------------------------------
void PatchCore::StopServerLookup(void)
{
//	if (m_ServerLookupOp.m_pOp.get() && m_ServerLookupOp.m_pOp->AwaitingCompletion())
	if (m_ServerLookupOp.m_pOp.get())
		m_ServerLookupOp.m_pOp->Kill();
}

//----------------------------------------------------------------------------------
// DoServerLookup: Kick off the operation if it has not already finished.
//----------------------------------------------------------------------------------
void PatchCore::DoServerLookup(void)
{
	// Make sure we are not shutting down.
	if (m_bStoping)
		return;

	// If the op has never been created, we need to run it.
	if (! m_ServerLookupOp.m_pOp.get())
	{
		StartServerLookup();
		return;
	}

	// If the operation completed previously, directly invoke the callback.
	if (m_ServerLookupOp.m_pOp->GetStatus() == WS_Success)
	{
		m_ServerLookupOp.InvokeCallbacks();
		return;
	}

	// If the operation is not already running, start it.
	if (! m_ServerLookupOp.m_pOp->AwaitingCompletion())
		StartServerLookup();
}


//==================================================================================
// SierraUp (Self) Version Check.
//==================================================================================

//----------------------------------------------------------------------------------
// PrepareSelfForUpdate: Over-write the product settings with SierraUp settings so 
// we can proceed to patch ourself, and present the correct info to the user.
//----------------------------------------------------------------------------------
void PatchCore::PrepareSelfForUpdate(void)
{
	// Change custom info to refer to SierraUp (it will have to be restarted, so 
	// these changes will not affect the calling application
	CustomInfo* pCustInfo = GetCustomInfo();
	pCustInfo->SetDisplayName("SierraUpdate");
	pCustInfo->SetProductName(sSierraUpProdId);
	pCustInfo->SetCurVersion(pCustInfo->GetSierraUpVersion());
	pCustInfo->SetExtraConfig(sSierraUpExtraConfig);
	pCustInfo->SetLaunchExe("");
	pCustInfo->SetMonitorPatch(false);
//	pCustInfo->SetNoPatchURL(???);
	pCustInfo->SetPatchTypes("");
	pCustInfo->SetValidateVersionFile("");
}

//----------------------------------------------------------------------------------
// SelfVersionCheckCallback: Callback invoked when the operation completes.
//----------------------------------------------------------------------------------
void PatchCore::SelfVersionCheckCallback(AsyncOp* pRawOp)
{
	if (pRawOp->Killed())
		return;

	// Process the results.
	CheckValidVersionOp* pOp = dynamic_cast<CheckValidVersionOp*>(pRawOp);
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();
	PatchList RawPatches;
	pCustInfo->SetNewVersion(pOp->GetToVersion());

	switch (pOp->GetStatus())
	{
	case WS_Success:
		// SierraUp is up to date, keep going.
		m_nSelfGetVerOpResult = SUR_UP_TO_DATE;
		m_SelfVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_Success_String);
		break;

	case WS_DBProxyServ_ValidNotLatest:
		// An optional version of SierraUp is availible.
		// Normally, we don't care, just keep going.
		m_nSelfGetVerOpResult = SUR_UP_TO_DATE;
		m_SelfVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_Success_String);

		// However we have a back door (pretend the version is mandatory).
//		// if (???) - This one can wait
//		{
//			PrepareSelfForUpdate();
//			m_nSelfGetVerOpResult = SUR_PATCH_FOUND;
//			m_SelfVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_PatchRequired_String);
//			TransferPatchList(pOp->GetPatchDataList());
//		}
		break;
	
	case WS_DBProxyServ_OutOfDate:
		// An manadatory patch is availible.
		PrepareSelfForUpdate();
		m_nSelfGetVerOpResult = SUR_PATCH_FOUND;
		m_SelfVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_PatchRequired_String);
		TransferPatchList(pOp->GetPatchDataList());
		break;

	case WS_DBProxyServ_OutOfDateNoUpdate:
		// No patch is availible - the user is screwed.
		DebugLog("SierraUp's version is out of date, but no updates (from this version) found.\n");
		m_nSelfGetVerOpResult = SUR_NO_PATCHES;
		m_SelfVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_SierraUpBadVersionNoUpdate_String);
		break;

	case WS_TimedOut:
		// Operation timed out.
		DebugLog("Version request timed out.\n");
		m_nSelfGetVerOpResult = SUR_NO_CONNECT;
		m_SelfVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_UpdateServerTimedOut_String);
		break;

	default:
		// Something unexpected happened.
		DebugLog("SierraUp version info request failed.\n");
		m_nSelfGetVerOpResult = SUR_NO_CONNECT;
		m_SelfVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_PatchServerError_String);
		ReplaceSubString(m_SelfVerCheckOp.m_sResult, "%ERROR%", WONStatusToString(pOp->GetStatus()));
	}

	// Inform anyone who cares that we are finished.
	m_SelfVerCheckOp.InvokeCallbacks();

	// Start the next operation in the chain.
	if (pOp->GetStatus() == WS_Success || pOp->GetStatus() == WS_DBProxyServ_ValidNotLatest)
		DoNextOperation();
}

//----------------------------------------------------------------------------------
// StartSelfVersionCheck: Kick off the operation.
//----------------------------------------------------------------------------------
void PatchCore::StartSelfVersionCheck(void)
{
	DebugLog("Starting SierraUp (Self) Version Check ...\n");

	CustomInfo* pCI = GetCustomInfo();

	// Prepare the download.
	m_SelfVerCheckOp.m_pOp = new CheckValidVersionOp(sSierraUpProdId, m_pSelfPatchServers);
	CheckValidVersionOp* pOp = dynamic_cast<CheckValidVersionOp*>(m_SelfVerCheckOp.m_pOp.get());
	pOp->SetCompletion(new PatchCoreCompletion(SelfVersionCheckCallback));

	pOp->SetConfigName(sSierraUpExtraConfig);
	pOp->SetVersion(pCI->GetSierraUpVersion());
//pOp->SetVersion("1.9.9.4"); // Passed - Invalid - upgrade via 'any previous' version.
//pOp->SetVersion("1.9.9.5"); // Passed - Invalid - upgrade via a 'specific' version.
//pOp->SetVersion("1.9.9.6"); // Passed - Invalid - upgrade via 'any previous' version.
//pOp->SetVersion("1.9.9.7"); // Passed - Valid   - latest (no upgrade path).
//pOp->SetVersion("1.9.9.8"); // Passed - Invalid - no path to upgrade.
//pOp->SetVersion("1.9.9.9"); // Passed - Valid   - latest.
//pOp->SetVersion("2.0.0.0"); // Passed - Invalid - too high.

	// Start the operation.
	m_SelfVerCheckOp.m_pOp->Run(OP_MODE_ASYNC, pCI->GetVersionTimeout());
}

//----------------------------------------------------------------------------------
// StopSelfVersionCheck: Shut down the operation if it has not completed.
//----------------------------------------------------------------------------------
void PatchCore::StopSelfVersionCheck(void)
{
//	if (m_SelfVerCheckOp.m_pOp.get() && m_SelfVerCheckOp.m_pOp->AwaitingCompletion())
	if (m_SelfVerCheckOp.m_pOp.get())
		m_SelfVerCheckOp.m_pOp->Kill();
}

//----------------------------------------------------------------------------------
// DoSelfVersionCheck: Kick off the operation if it has not already finished.
//----------------------------------------------------------------------------------
void PatchCore::DoSelfVersionCheck(void)
{
	// Make sure we are not shutting down.
	if (m_bStoping)
		return;

	// If the op has never been created, we need to run it.
	if (! m_SelfVerCheckOp.m_pOp.get())
	{
		StartSelfVersionCheck();
		return;
	}

	// If the operation completed previously, directly invoke the callback.
	if (m_SelfVerCheckOp.m_pOp->GetStatus() == WS_Success)
	{
		m_SelfVerCheckOp.InvokeCallbacks();
		return;
	}

	// If the operation is not already running, start it.
	if (! m_SelfVerCheckOp.m_pOp->AwaitingCompletion())
		StartSelfVersionCheck();
}


//==================================================================================
// Applicaton (Game) Version Check.
//==================================================================================

//----------------------------------------------------------------------------------
// GameVersionCheckCallback: Callback invoked when the operation completes.
//----------------------------------------------------------------------------------
void PatchCore::GameVersionCheckCallback(AsyncOp* pRawOp)
{
	if (pRawOp->Killed())
		return;

	// Process the results.
	CheckValidVersionOp* pOp = dynamic_cast<CheckValidVersionOp*>(pRawOp);
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();
	PatchList RawPatches;
	pCustInfo->SetPatchDescriptionUrl(pOp->GetToVersionDescriptionURL());
	pCustInfo->SetNewVersion(pOp->GetToVersion());

	switch (pOp->GetStatus())
	{
	case WS_Success:
		// The application is up to date.
		pCustInfo->SetOptionalUpgrade(false);
		m_nGameGetVerOpResult = SUR_UP_TO_DATE;
		m_GameVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_Success_String);
		break;

	case WS_DBProxyServ_ValidNotLatest:
		// An optional patch is availible.
		pCustInfo->SetOptionalUpgrade(true);
		m_nGameGetVerOpResult = SUR_OPTIONAL_PATCH;
		m_GameVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_OptionalPatch_String);
		TransferPatchList(pOp->GetPatchDataList());
		break;

	case WS_DBProxyServ_OutOfDate:
		// An manadatory patch is availible.
		pCustInfo->SetOptionalUpgrade(false);
		m_nGameGetVerOpResult = SUR_PATCH_FOUND;
		m_GameVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_PatchRequired_String);
		TransferPatchList(pOp->GetPatchDataList());
		break;

	case WS_DBProxyServ_OutOfDateNoUpdate:
		// No patch is availible - the user is screwed.
		DebugLog("Version is out of date, but no updates (from this version) found.\n");
		m_nGameGetVerOpResult = SUR_NO_PATCHES;
		m_GameVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_BadVersionNoUpdate_String);
		break;

	case WS_TimedOut:
		// Operation timed out.
		DebugLog("Version request timed out.\n");
		m_nGameGetVerOpResult = SUR_NO_CONNECT;
		m_GameVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_UpdateServerTimedOut_String);
		break;

	default:
		// Something unexpected happened.
		DebugLog("Game version info request failed.\n");
		m_nGameGetVerOpResult = SUR_NO_CONNECT;
		m_GameVerCheckOp.m_sResult = pResMgr->GetFinishedString(Global_PatchServerError_String);
		ReplaceSubString(m_GameVerCheckOp.m_sResult, "%ERROR%", WONStatusToString(pOp->GetStatus()));
	}

	// Inform anyone who cares that we are finished.
	m_GameVerCheckOp.InvokeCallbacks();

	// Start the next operation in the chain.
	if (pOp->GetStatus() == WS_Success)
		DoNextOperation();
}

//----------------------------------------------------------------------------------
// StartGameVersionCheck: Kick off the operation.
//----------------------------------------------------------------------------------
void PatchCore::StartGameVersionCheck(void)
{
	DebugLog("Starting SierraUp (Game) Version Check ...\n");

	CustomInfo* pCI = GetCustomInfo();

	// Prepare the download.
	m_GameVerCheckOp.m_pOp = new CheckValidVersionOp(pCI->GetProductName(), m_pGamePatchServers);
	CheckValidVersionOp* pOp = dynamic_cast<CheckValidVersionOp*>(m_GameVerCheckOp.m_pOp.get());
	pOp->SetCompletion(new PatchCoreCompletion(GameVersionCheckCallback));

	pOp->SetConfigName(pCI->GetExtraConfig());
	pOp->SetVersion(pCI->GetCurVersion());
	ConvertPatchTypes(pCI->GetPatchTypes(), pOp);

	// Start the operation.
	m_GameVerCheckOp.m_pOp->Run(OP_MODE_ASYNC, pCI->GetVersionTimeout());
}

//----------------------------------------------------------------------------------
// StopGameVersionCheck: Shut down the operation if it has not completed.
//----------------------------------------------------------------------------------
void PatchCore::StopGameVersionCheck(void)
{
//	if (m_GameVerCheckOp.m_pOp.get() && m_GameVerCheckOp.m_pOp->AwaitingCompletion())
	if (m_GameVerCheckOp.m_pOp.get())
		m_GameVerCheckOp.m_pOp->Kill();
}

//----------------------------------------------------------------------------------
// DoGameVersionCheck: Kick off the operation if it has not already finished.
//----------------------------------------------------------------------------------
void PatchCore::DoGameVersionCheck(void)
{
	// Make sure we are not shutting down.
	if (m_bStoping)
		return;

	// If the op has never been created, we need to run it.
	if (! m_GameVerCheckOp.m_pOp.get())
	{
		StartGameVersionCheck();
		return;
	}

	// If the operation completed previously, directly invoke the callback.
	if (m_GameVerCheckOp.m_pOp->GetStatus() == WS_Success)
	{
		m_GameVerCheckOp.InvokeCallbacks();
		return;
	}

	// If the operation is not already running, start it.
	if (! m_GameVerCheckOp.m_pOp->AwaitingCompletion())
		StartGameVersionCheck();
}


//==================================================================================
// Non Operation-Specific methods.
//==================================================================================

//----------------------------------------------------------------------------------
// DoNextOperation: Start the next operation that has not completed.
//----------------------------------------------------------------------------------
void PatchCore::DoNextOperation(void)
{
	// Make sure we are not shutting down.
	if (m_bStoping)
		return;

	// We must have received a successful DNS test before anything else.
	if (! m_DnsTestOp.Succeeded())
	{
		DoDnsTest();
		return;
	}

	// The MotD always comes next, although the user can 'skip' past it in the dialog, it still wants to complete.
	if (! m_GetMotdOp.Completed())
	{
		DoMotdDownload();
		return;
	}

	// Get the server lists from the main directory server.
	if (! m_ServerLookupOp.Succeeded())
	{
		DoServerLookup();
		return;
	}

	// Check to see if SierraUp needs to update itself.
	if (! m_SelfVerCheckOp.Succeeded())
	{
		DoSelfVersionCheck();
		return;
	}

	// If SierraUp need does not need updating, continue.
	if (m_nSelfGetVerOpResult == SUR_UP_TO_DATE)
	{
		// Check to see if the application (game) needs to be updated.
		if (! m_GameVerCheckOp.Succeeded())
		{
			DoGameVersionCheck();
			return;
		}
	}
}

//----------------------------------------------------------------------------------
// StopOperations: Shut down any operations that have not completed.
//----------------------------------------------------------------------------------
void PatchCore::StopOperations(void)
{
	m_bStoping = true;

	StopDnsTest();
	StopMotdDownload();
	StopServerLookup();
	StopSelfVersionCheck();
	StopGameVersionCheck();
}

//----------------------------------------------------------------------------------
// ConstructDirForGameUpdateServer: Assemble the path to the directory that 
// contains the update servers for the game.
//----------------------------------------------------------------------------------
std::wstring PatchCore::ConstructDirForGameUpdateServer(void)
{
	std::wstring wsGamePatchDir = L"/" + StringToWString(GetCustomInfo()->GetProductName()) + L"/Patch";
	return wsGamePatchDir;
}

//----------------------------------------------------------------------------------
// ConvertPatchTypes: Convert the comma delimited string to the operations 
// individual strings.
//----------------------------------------------------------------------------------
void PatchCore::ConvertPatchTypes(const std::string& sTypes, CheckValidVersionOp* pOp)
{
	std::string sList = sTypes;

	while (sList != "")
	{
		int nPos = sList.find(',');
		if (nPos == -1)
		{
			// Last one.
			pOp->AddVersionType(sList);
			return;
		}

		std::string sType = sList;
		sList.erase(0, nPos + 1);
		sType.erase(nPos, sList.length() + 1);
		if (sType != "")
			pOp->AddVersionType(sType);
	}
/*
	std::string sList = sTypes;

	int nPos = sList.find(',');
	while (nPos != -1)
	{
		std::string sType = sList;
		sList.erase(0, nPos + 1);
		sType.erase(nPos, sList.length() + 1);
		if (sType != "")
			pOp->AddVersionType(sType);
		nPos = sList.find(',');
	}
*/
}

//----------------------------------------------------------------------------------
// ValidatePatchedState: Make sure the patch was completely applied (if we are 
// suppose to watch this).
//----------------------------------------------------------------------------------
bool PatchCore::ValidatePatchedState(Window* pParent, GUIString& sExpVer)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	// Handle the case where previous patch validation needs to be done by us.
	std::string sFile = pCustInfo->GetValidateVersionFile();
	if (sFile == "")
		return true;

	char sValVer[MAX_PATH];
	GetPrivateProfileString("VersionInfo", "CurrentVersion", sNoVersion.c_str(), sValVer, MAX_PATH, sFile.c_str());
	sExpVer = sValVer;

	// If the file contains a valid version, test against it.
	if (IsVersion(sValVer))
	{
		if (CompareVersions(sValVer, pCustInfo->GetCurVersion()) < 0)
		{
			// It appears that a user has aborted a patch, so set the current version to validate version so they can try again.
			MessageBox(pParent, pResMgr->GetFinishedString(Global_IncompletePatch_String), pResMgr->GetFinishedString(Global_Warning_String), MD_OK);
			pCustInfo->SetCurVersion(sValVer);
			return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------
// RunPatch: Run the patch, and wait for it to complete (if appropriate).
//----------------------------------------------------------------------------------
bool PatchCore::RunPatch(Window* pParent, std::string& sPatchExe)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	std::string sCmdLine = "\"";
	sCmdLine += sPatchExe;
	sCmdLine += "\"";

	STARTUPINFO SI;
	ZeroMemory(&SI, sizeof(SI));
	SI.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION PI;
	BOOL bRes = CreateProcess(NULL, (LPSTR)(sCmdLine.c_str()), NULL, NULL, FALSE, NULL, NULL, NULL, &SI, &PI);

	if (bRes)
	{
		BOOL bOK = false;
		DWORD dwExitCode = 0;
		if (pCustInfo->GetMonitorPatch())
		{
			// Wait for the process to get going.
			if (WaitForInputIdle(PI.hProcess, INFINITE) != 0)
			{
				MessageBox(pParent, pResMgr->GetFinishedString(Global_NoMonitor_String), pResMgr->GetFinishedString(Global_PsUndetermined_String), MD_OK);
				CloseHandle(PI.hThread);
				CloseHandle(PI.hProcess);
				return false;
			}

			// Wait for the process to finish.
			bOK = GetExitCodeProcess(PI.hProcess, &dwExitCode);
			while (bOK && dwExitCode == STILL_ACTIVE) //lint !e1924
			{
				ReleaseControl();
				Sleep(500);
				bOK = GetExitCodeProcess(PI.hProcess, &dwExitCode);
			}
		}

		// Clean up.
		CloseHandle(PI.hThread);
		CloseHandle(PI.hProcess);

		if (pCustInfo->GetMonitorPatch())
		{
			if (! bOK)
			{
				MessageBox(pParent, pResMgr->GetFinishedString(Global_NoMonitor_String), pResMgr->GetFinishedString(Global_PsUndetermined_String), MD_OK);
				return false;
			}

			if (pCustInfo->GetHavePatchSuccessValue() && dwExitCode != pCustInfo->GetPatchSuccessValue())
			{
				DebugLog("Patch Failed: Return code (%d) from patch did not match 'success' value (%d)\n", dwExitCode, pCustInfo->GetPatchSuccessValue());
				MessageBox(pParent, pResMgr->GetFinishedString(Global_PatchReturnedError_String), pResMgr->GetFinishedString(Global_PatchFailed_String), MD_OK);
//				AddInfoText(pResMgr->GetFinishedString(Global_PatchReturnedError_String));
				return false;
			}
		}

		return true;
	}
	else
	{
		GUIString sMsg;

		if (GetLastError() == 2)
			sMsg = pResMgr->GetFinishedString(Global_NoFindPatch_String);
		else
			sMsg = pResMgr->GetFinishedString(Global_NoRunPatch_String);

		MessageBox(pParent, sMsg, pResMgr->GetFinishedString(Global_PatchFailed_String), MD_OK);
		return false;
	}
}

//----------------------------------------------------------------------------------
// ApplySelectedPatch: Run the patch (apply it).
//----------------------------------------------------------------------------------
int PatchCore::ApplySelectedPatch(Window *pParent)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	// Force the user to look at the MotDs when we upgrade them.
	RemoveMotdFiles();

	// If we are patching ourself, there is special handing elsewhere.
	if (m_nGameGetVerOpResult == SUR_UNKNOWN)
		return SUR_PATCHING_SELF;

	std::string sPatchExe = pCustInfo->GetPatchFile();
	DebugLog("Applying update file: %s\n", sPatchExe.c_str());

	bool bRun = RunPatch(pParent, sPatchExe);

	int nRet = bRun ? SUR_PATCH_APPLIED : SUR_PATCH_FAILED;

	if (pCustInfo->GetMonitorPatch() && bRun && pCustInfo->GetValidateVersionFile().length())
	{
		GUIString sExpVer;

		// Handle the case where previous patch validation needs to be done by us.
		if (ValidatePatchedState(pParent, sExpVer))
		{
			pCustInfo->SetCurVersion(std::string(sExpVer).c_str()); // Cast to narrow string.

			// Update the current version in the information file.
			pCustInfo->SaveLastMonitoredVersion(pCustInfo->GetCurVersion());

			// Tell the user that we are going to look for further updates.
			//MessageBox(pParent, pResMgr->GetFinishedString(IDS_MAY_BE_ANOTHER_PATCH), pResMgr->GetFinishedString(Global_PatchApplied_String), MD_OK); //??? IDS_MAY_BE_ANOTHER_PATCH does not exist yet

			// Tell the controller that we have to effectively start over
			nRet = SUR_START_OVER;
		}
		else
		{
			// Cound not extract a valid version.
			GUIString sError = pResMgr->GetFinishedString(Global_VersionFileCheckFailed_String);
			ReplaceSubString(sError, "%FILE%", pCustInfo->GetValidateVersionFile());
			ReplaceSubString(sError, "%EXP_VERSION%", sExpVer);
			MessageBox(pParent, sError, pResMgr->GetFinishedString(Global_PatchApplied_String), MD_OK);
			nRet = SUR_PATCH_FAILED;
		}
	}

	return nRet;
}

//----------------------------------------------------------------------------------
// CRCThreadData.
//----------------------------------------------------------------------------------
class CRCThreadData : public RefCount
{
public:
	std::string m_sFile;
	bool        m_bAborted;
	int         m_nCRC;

	CRCThreadData(const std::string& sFile)
	{
		m_sFile = sFile;
		m_bAborted = false;
		m_nCRC = 0;
	}

};
typedef SmartPtr<CRCThreadData> CRCThreadDataPtr;

//----------------------------------------------------------------------------------
// CRCEndEvent.
//----------------------------------------------------------------------------------
class CRCEndEvent : public WindowEventBase
{
public:
	CRCThreadDataPtr mCRCData;

	virtual void Deliver()
	{
		if(mCRCData->m_bAborted)
			return;

		WindowManager::GetDefaultWindowManager()->EndDialog(MDR_COMPLETE);
	}

	CRCEndEvent(CRCThreadData *theData) : mCRCData(theData) { }
};

//----------------------------------------------------------------------------------
// StartCrcCheckThread: Begin the thread that will actually do the CRC calculation.
//----------------------------------------------------------------------------------
unsigned int __stdcall StartCrcCheckThread(void* pRawData)
{
	CRCThreadDataPtr pData = reinterpret_cast<CRCThreadData*>(pRawData);
	pData->Release(); // release extra 

	pData->m_nCRC = AbortableCalcFileCRC(pData->m_sFile.c_str(), pData->m_bAborted);

#ifdef _DEBUG
	// Artificially slow down the test (so we can see it.
	for (int i = 0; i < 100; i++)
		if (! (GetKeyState(VK_SHIFT) & 0x8000))
			Sleep(50);
#endif

	if (! pData->m_bAborted)
	{
		WindowManagerPtr aWindowMgr = WindowManager::GetDefaultWindowManager();
		aWindowMgr->PostEvent(new CRCEndEvent(pData));
	}

	return ! pData->m_bAborted;
}

//----------------------------------------------------------------------------------
// ValidateSelectedPatch: Check the patch size and checksum to see if they match.
//----------------------------------------------------------------------------------
int PatchCore::ValidateSelectedPatch(Window *pParent)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	CPatchData* pPatch = pCustInfo->GetSelectedPatch();
	if (pPatch)
	{
		std::string sFile = pCustInfo->GetPatchFile();

		DWORD tStart = GetTickCount();
		DebugLog("Validating update file: %s\n", sFile.c_str());

		// Create the thread that will actually do the operation.
		CRCThreadDataPtr pCRCData = new CRCThreadData(sFile);
		unsigned int nThreadID = 0;
		pCRCData->CreateRef(); // for thread
		HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, StartCrcCheckThread, pCRCData.get(), 0, &nThreadID)); //lint !e1055 !e746

		// This can take a bit, so give the user an abort dialog.
		int aResult = MessageBox(pParent, pResMgr->GetFinishedString(Global_Validating_String), pResMgr->GetFinishedString(Global_Validation_String), MD_ABORT);
		if (aResult != MDR_COMPLETE)
			pCRCData->m_bAborted = true;

		CloseHandle(hThread);

		if (pCRCData->m_bAborted)
			return SUR_USER_CANCEL;

		long nExpSize = pPatch->GetPatchSize();
		long nExpChecksum = pPatch->GetChecksum();

		long nSize = GetFileSize(sFile);
		long nChecksum = pCRCData->m_nCRC;

		if (nExpSize != nSize) // Compare sizes.
		{
			DebugLog("Validation failed, expected %d bytes, file is %d\n", nExpSize, nSize);
			pPatch->SetDownloadFailures(pPatch->GetDownloadFailures() + 1);

			switch (MessageBox(pParent, pResMgr->GetFinishedString(Global_WrongSize_String), pResMgr->GetFinishedString(Global_InvalidPatch_String), MD_YESNO))
			{
				case MDR_YES:
					DeleteFile(sFile.c_str());
					return SUR_NO_DOWNLOAD;
				case MDR_NO:
					return SUR_USER_CANCEL;
			}
		}
		else if (nExpChecksum != 0 && nExpChecksum != nChecksum) // Compare checksums.
		{
			DebugLog("Validation failed, incorrect checksum\n");

			pPatch->SetDownloadFailures(pPatch->GetDownloadFailures() + 1);

			switch (MessageBox(pParent, pResMgr->GetFinishedString(Global_InvalidChecksum_String), pResMgr->GetFinishedString(Global_InvalidPatch_String), MD_YESNO))
			{
				case MDR_YES:
					DeleteFile(sFile.c_str());
					return SUR_NO_DOWNLOAD;
				case MDR_NO:
					return SUR_USER_CANCEL;
			}
		}
		else
		{
			DWORD tElapsed = GetTickCount() - tStart;
			DebugLog("Validation succeeded, validation time = %d ms.\n", tElapsed);
			return SUR_PATCH_READY;
		}
	}

	// Just in case.
	return SUR_NO_DOWNLOAD;
}


//----------------------------------------------------------------------------------
// MatchLocalPatch: Look through all of the valid patches (from this version), and 
// see if the one on disk matches one of them.  If so, 'select' it, and pass it on 
// for validation and application.
//----------------------------------------------------------------------------------
bool PatchCore::MatchLocalPatch(Window *pParent, const std::string& sFile)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();
	pCustInfo->SetSelectedPatch(NULL);

//	// Create the thread that will actually do the operation.
//	CRCThreadDataPtr pCRCData = new CRCThreadData(sFile);
//	unsigned int nThreadID = 0;
//	pCRCData->CreateRef(); // for thread
//	HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, StartCrcCheckThread, pCRCData.get(), 0, &nThreadID)); //lint !e1055 !e746
//
//	// This can take a bit, so give the user an abort dialog.
//	int aResult = MessageBox(pParent, pResMgr->GetFinishedString(Global_Validating_String), pResMgr->GetFinishedString(Global_Validation_String), MD_ABORT);
//	if (aResult != MDR_COMPLETE)
//		pCRCData->m_bAborted = true;
//
//	CloseHandle(hThread);
//
//	if (pCRCData->m_bAborted)
//		return false;

	long nFileSize = GetFileSize(sFile);
//	long nChecksum = pCRCData->m_nCRC;

	// See if the file size and checksum match an entry in the list.
	CPatchDataList* pPatches = pCustInfo->GetPatchList();
	CPatchDataList::iterator Itr = pPatches->begin();

	while (Itr != pPatches->end())
	{
		CPatchData* pData = *Itr;

//		if (pData && pData->GetChecksum() == nChecksum && pData->GetPatchSize() == nFileSize)
		if (pData && pData->GetPatchSize() == nFileSize)
		{
			pCustInfo->SetSelectedPatch(pData);
			pCustInfo->SetPatchFile(sFile);
			break;
		}

		++Itr;
	}

	// If we found a match, proceed with it.
	if (pCustInfo->GetSelectedPatch())
//	{
//		m_nFileCRC = nChecksum;
		return true;
//	}

	// We did not find a patch.  Warn the user that the patch may be bad and ask if they realy want to use it.
	GUIString sWarning = pResMgr->GetFinishedString(Global_LocalPatchDoesNotMatch_String);

	if (MessageBox(pParent, sWarning, pResMgr->GetFinishedString(Global_Warning_String), MD_YESNO) == MDR_YES)
	{
		SetSkipValidation(true);
		pCustInfo->SetPatchFile(sFile);
		return true;
	}

	SetSkipValidation(false);
	pCustInfo->SetPatchFile("");
	return false;
}

//----------------------------------------------------------------------------------
// TransferPatchList: Go through the patches and add all of the items to our patch 
// info list (it has additional information).
//----------------------------------------------------------------------------------
void PatchCore::TransferPatchList(const PatchDataList& PatchList)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	// Clear out any pervious patches (this should not make a difference, but just in case)...
	pCustInfo->ClearPatchList();

	// Now go through the patches and add all of the items to the patch info list.
	PatchDataList::iterator Itr = PatchList.begin();

	while (Itr != PatchList.end())
	{
		PatchData* pInfo = *Itr;
		CPatchData* pPatchData = new CPatchData();

		// Patch Name.
		pPatchData->SetPatchName(pInfo->GetDisplayName());

		// Host Bitmap.
		pPatchData->SetHostBmp(pInfo->GetHostImageURL());

		// Host Name.
		pPatchData->SetHostName(pInfo->GetHostName());

		// Host URL.
		pPatchData->SetHostUrl(pInfo->GetHostURL());

		// Versions.
		pPatchData->SetFromVersion(pInfo->GetFromVersion());

		// Must Visit Host flag.
		pPatchData->SetMustVisitHost(pInfo->GetMustVisitHost());

		// Host Text.
		if (pInfo->GetHostText() != "")
			pPatchData->SetHostTxt(pInfo->GetHostText());
		else
		{
			GUIString sVisit = pResMgr->GetFinishedString(VersionCheck_Visit_String);
			sVisit.append(pPatchData->GetHostName());
			pPatchData->SetHostTxt(sVisit);
		}

		// Patch URL.
		pPatchData->SetPatchUrl(pInfo->GetNetAddress());

		// Patch Size.
		pPatchData->SetPatchSize(pInfo->GetPatchSize());

		// Patch Checksum.
		pPatchData->SetChecksum(pInfo->GetCheckSum());

		#ifdef _DEBUG
		///pPatchData->MessageBox(GetWindow());
		#endif

		// Add the item to the list.
		pCustInfo->AddPatch(pPatchData);

		// Advance to the next item.
		++Itr;
	}
}

//----------------------------------------------------------------------------------
// RemoveMotdFiles: Nuke the MotD Files.
//----------------------------------------------------------------------------------
void PatchCore::RemoveMotdFiles(void)
{
	CustomInfo* pCustInfo = GetCustomInfo();

	GetMOTDOpPtr pGetMotdOp = new GetMOTDOp(pCustInfo->GetProductName());
	if (pCustInfo->GetExtraConfig().length())
		pGetMotdOp->SetExtraConfig(pCustInfo->GetExtraConfig());

	DeleteFile(pGetMotdOp->GetSysMOTDPath().c_str());
	DeleteFile(pGetMotdOp->GetGameMOTDPath().c_str());
}
