//----------------------------------------------------------------------------------
// PatchCore.h
//----------------------------------------------------------------------------------
#ifndef __PatchCore_H__
#define __PatchCore_H__

#pragma warning(disable : 4786)

#ifndef _STRING_
#include <string>
#endif
#ifndef __WON_MSCONTROLS_H__
#include "WONGUI/MSControls.h"
#endif
#ifndef __WON_GETMOTDOP_H__
#include "WONAPI/WONMisc/GetMOTDOp.h"
#endif
#ifndef  __WON_DNSTestOP_H__
#include "WONAPI/WONMisc/DNSTestOp.h"
#endif
#ifndef  __WON_GETMULTIDIROP_H__
#include "WONAPI/WONDir/GetMultiDirOp.h"
#endif
#ifndef __CheckValidVersionOp_H__
#include "WONAPI/WONMisc/CheckValidVersionOp.h"
#endif

namespace WONAPI
{

class OperationInfo;

//----------------------------------------------------------------------------------
// PatchCoreCallback.
//----------------------------------------------------------------------------------
class PatchCoreCallback
{
public:
//	virtual void Complete(AsyncOp* pOp) = 0;
	virtual void Complete(OperationInfo* pOpInfo) = 0;
};
typedef std::list<PatchCoreCallback*> PatchCoreCallbackList;


//----------------------------------------------------------------------------------
// OperationInfo.
//----------------------------------------------------------------------------------
class OperationInfo
{
protected:
	PatchCoreCallbackList m_Callbacks; // Callbacks to invoke when the state changes.

public:
	AsyncOpPtr            m_pOp;       // The operation itself.
	GUIString             m_sResult;   // User readable (or so we hope) summary of the results.

	OperationInfo(void)
	{
		Reset();
	}

	void Reset(void)
	{
		m_pOp     = NULL;
		m_sResult = "";
	}

	bool Started(void)
	{
		return m_pOp.get() != NULL;
	}

	bool Completed(void)
	{
		if (! m_pOp.get())
			return false;
		return ! m_pOp->AwaitingCompletion();
	}

	bool Succeeded(void)
	{
		if (! m_pOp.get())
			return false;

		// The extras won't hurt, since they can not happen for some ops.
		return m_pOp->GetStatus() == WS_Success || 
			   m_pOp->GetStatus() == WS_DBProxyServ_ValidNotLatest ||
			   m_pOp->GetStatus() == WS_DBProxyServ_OutOfDate ||
			   m_pOp->GetStatus() == WS_DBProxyServ_OutOfDateNoUpdate;
	}

	void AddCallback(PatchCoreCallback* pCallback)
	{
		m_Callbacks.push_back(pCallback);
	}

	void InvokeCallbacks(void)
	{
		for (PatchCoreCallbackList::iterator Itr = m_Callbacks.begin(); Itr != m_Callbacks.end(); Itr++)
		{
			PatchCoreCallback* pCallback = *Itr;

			if (pCallback)
				pCallback->Complete(this);
		}
	}
};

//----------------------------------------------------------------------------------
// PatchCore.
//----------------------------------------------------------------------------------
class PatchCore
{
	//------------------------------------------------------------------------------
	// 	PatchCoreCompletion.
	//------------------------------------------------------------------------------
	class PatchCoreCompletion : public WONAPI::CompletionBase<WONAPI::AsyncOpPtr>
	{
	private:
		typedef void(PatchCore::*Callback)(WONAPI::AsyncOp *theOp);
		Callback mCallback;

	public:
		PatchCoreCompletion(Callback theCallback) : mCallback(theCallback) {}
		virtual void Complete(WONAPI::AsyncOpPtr theOp) 
		{
			if (PatchCore::m_pInstance)
				(PatchCore::m_pInstance->*mCallback)(theOp);
		}
	};

protected:
	OperationInfo    m_DnsTestOp;           // DNS test operation.
	OperationInfo    m_GetMotdOp;           // MotD fetch operation.
	OperationInfo    m_ServerLookupOp;      // Server Lookup (get servers from dir server).
	OperationInfo    m_SelfVerCheckOp;      // SierraUp version check (self check).
	OperationInfo    m_GameVerCheckOp;      // Application game) version check.

	int              m_nSelfGetVerOpResult; // Result of the self (SierraUp) version check.
	int              m_nGameGetVerOpResult; // Result of the Game's version check.
	ServerContextPtr m_pSelfPatchServers;   // Servers to pass to the version check operation when checking our version.
	ServerContextPtr m_pGamePatchServers;   // Servers to pass to the version check operation when checking the game's version.
	bool             m_bNoMotdFound;        // We were unable to download the MotD - it may not exist.
	bool             m_bStoping;            // Are we in the process of shutting down?
//	bool             m_bMixedPatches;       // Are some patch direct download and do some require host visitation?
	bool             m_bSkipValidation;     // Should we skip the validation of the file (the user can do so if the use a previously downloaded patch).


	// DNS test.
	void DnsTestComplete(DNSTestOp* pDnsTestOp);
	void DnsTestCallback(AsyncOp* pOp);
	void StartDnsTest(void);
	void StopDnsTest(void);

	// MOTD download operation.
	void MotdComplete(GetMOTDOp* pGetMotdOp);
	void MotdCallback(AsyncOp* pOp);
	void StartMotdDownload(void);
	void StopMotdDownload(void);

	// Server lookup (from directery server entries) operation.
	void ServerLookupComplete(GetMultiDirOp* pServerLookupOp);
	void ServerLookupCallback(AsyncOp* pOp);
	void StartServerLookup(void);
	void StopServerLookup(void);

	// SierraUp (self) version check.
	void PrepareSelfForUpdate(void);
	void SelfVersionCheckComplete(CheckValidVersionOp* pSelfVerCheckOp);
	void SelfVersionCheckCallback(AsyncOp* pOp);
	void StartSelfVersionCheck(void);
	void StopSelfVersionCheck(void);

	// Application (game) version check.
	void GameVersionCheckComplete(CheckValidVersionOp* pGameVerCheckOp);
	void GameVersionCheckCallback(AsyncOp* pOp);
	void StartGameVersionCheck(void);
	void StopGameVersionCheck(void);

public:
	static PatchCore* m_pInstance; // Global instance of this object.

	PatchCore(void);
	~PatchCore(void);

	// DNS test.
	void DoDnsTest(void);
	void ResetDnsTest(void)                                        { m_DnsTestOp.Reset(); }
	void SetDnsTestCallback(PatchCoreCallback* pCallback)          { m_DnsTestOp.AddCallback(pCallback); }
	OperationInfo* GetDnsTestOp(void)                              { return &m_DnsTestOp; }

	// MOTD download operation.
	void DoMotdDownload(void);
	void ResetMotdDownload(void)                                   { m_GetMotdOp.Reset(); }
	void SetMotdCallback(PatchCoreCallback* pCallback)             { m_GetMotdOp.AddCallback(pCallback); }
	OperationInfo* GetMotdDownloadOp(void)                         { return &m_GetMotdOp; }

	// Server lookup (from directery server entries) operation.
	void DoServerLookup(void);
	void ResetServerLookup(void)                                   { m_ServerLookupOp.Reset(); }
	void SetServerLookupCallback(PatchCoreCallback* pCallback)     { m_ServerLookupOp.AddCallback(pCallback); }
	OperationInfo* GetServerLookupOp(void)                         { return &m_ServerLookupOp; }

	// SierraUp (self) version check.
	void DoSelfVersionCheck(void);
	void ResetSelfVersionCheck(void)                               { m_SelfVerCheckOp.Reset(); }
	void SetSelfVersionCheckCallback(PatchCoreCallback* pCallback) { m_SelfVerCheckOp.AddCallback(pCallback); }
	OperationInfo* GetSelfVersionCheckOp(void)                     { return &m_SelfVerCheckOp; }

	// Application (game) version check.
	void DoGameVersionCheck(void);
	void ResetGameVersionCheck(void)                               { m_GameVerCheckOp.Reset(); }
	void SetGameVersionCheckCallback(PatchCoreCallback* pCallback) { m_GameVerCheckOp.AddCallback(pCallback); }
	OperationInfo* GetGameVersionCheckOp(void)                     { return &m_GameVerCheckOp; }


	void DoNextOperation(void);
	void StopOperations(void);

	std::wstring ConstructDirForGameUpdateServer(void);
	void ConvertPatchTypes(const std::string& sTypes, CheckValidVersionOp* pOp);
	int  GetSelfVersionCheckResult(void) const                     { return m_nSelfGetVerOpResult; }
	int  GetGameVersionCheckResult(void) const                     { return m_nGameGetVerOpResult; }
//	bool GetGameVersionOk(void)                                    { return m_nGameGetVerOpResult == SUR_UP_TO_DATE; }
//	void SetMixedPatches(bool bSet = true)                         { m_bMixedPatches = bSet; }
//	bool GetMixedPatches(void)                                     { return m_bMixedPatches; }
	void SetSkipValidation(bool bSet = true)                       { m_bSkipValidation = bSet; }
	bool GetSkipValidation(void) const                             { return m_bSkipValidation; }

	ServerContext* GetGamePatchServers(void) { return m_pGamePatchServers; }

	void TransferPatchList(const PatchDataList& PatchList);

	bool ValidatePatchedState(Window* pParent, GUIString& sExpVer);
	bool RunPatch(Window* pParent, std::string& sPatchExe);
//	bool ApplySelectedPatch(Window *pParent);
	int  ApplySelectedPatch(Window *pParent);
	int  ValidateSelectedPatch(Window *pParent);
	bool MatchLocalPatch(Window *pParent, const std::string& sFile);
//	void UpdatePatchList(PatchList& Patches, VersionDescriptionMap& VerMap);
	void RemoveMotdFiles(void);
};

} // namespace

#endif