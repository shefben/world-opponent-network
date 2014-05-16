//----------------------------------------------------------------------------------
// PatchData.h
//----------------------------------------------------------------------------------
#ifndef __PatchData_H__
#define __PatchData_H__

#pragma once

#ifndef _LIST_
 #include <list>
#endif
#ifndef __WON_MSCONTROLS_H__
 #include "WONGUI/MSControls.h"
#endif
#ifndef __MessageDlg_H__
 #include "MessageDlg.h"
#endif


namespace WONAPI
{

//----------------------------------------------------------------------------------
// CPatchData
//----------------------------------------------------------------------------------
class CPatchData
{
private:
	std::string m_sPatchName;        // Name of the patch (including Host).
	std::string m_sHostName;         // Name of Host (who is hosting this patch).
	std::string m_sHostBmp;          // URL to a mini banner for the host.
	std::string m_sHostTxt;          // Text to put on the button if no bitmap.
	std::string m_sHostUrl;          // URL to go to if the user clicks on the host banner.
	std::string m_sPatchUrl;         // URL to the actual patch.
//	std::string m_sToVersion;        // What Version will the patch upgrade to.
	std::string m_sFromVersion;      // What Version will the patch upgrade from.
	bool        m_bMustVisitHost;    // Force the user to visit the host rather than download directly.
	long        m_nPatchSize;        // Size of the patch in bytes.
	long        m_nChecksum;         // Checksum of the patch (safety net).
	long        m_nDownloadFailures; // How many times has the download failed.
	long        m_nDownloadAborts;   // How many times has the user aborted the download.

public:
	CPatchData(void)
		: m_sPatchName(TEXT("")), m_sHostName(TEXT("")), m_sHostBmp(TEXT("")), 
		  m_sHostTxt(TEXT("")), m_sHostUrl(TEXT("")), m_sPatchUrl(TEXT("")),
//		  m_sToVersion(TEXT("")), m_sFromVersion(TEXT("")), m_bMustVisitHost(false), m_nPatchSize(0), m_nChecksum(0), 
		  m_sFromVersion(TEXT("")), m_bMustVisitHost(false), m_nPatchSize(0), m_nChecksum(0), 
		  m_nDownloadFailures(0), m_nDownloadAborts(0)
	{}

	inline void SetPatchName(const std::string& sPatchName)  { m_sPatchName = sPatchName; }
	inline const std::string& GetPatchName(void) const       { return m_sPatchName; }

	inline void SetHostName(const std::string& sHostName)    { m_sHostName = sHostName; }
	inline const std::string& GetHostName(void) const        { return m_sHostName; }

	inline void SetHostBmp(const std::string& sHostBmp)      { m_sHostBmp = sHostBmp; }
	inline const std::string& GetHostBmp(void) const         { return m_sHostBmp; }

	inline void SetHostTxt(const std::string& sHostTxt)      { m_sHostTxt = sHostTxt; }
	inline const std::string& GetHostTxt(void) const         { return m_sHostTxt; }

	inline void SetHostUrl(const std::string& sHostUrl)      { m_sHostUrl = sHostUrl; }
	inline const std::string& GetHostUrl(void) const         { return m_sHostUrl; }

	inline void SetPatchUrl(const std::string& sPatchUrl)    { m_sPatchUrl = sPatchUrl; }
	inline const std::string& GetPatchUrl(void) const        { return m_sPatchUrl; }

//	inline void SetToVersion(const std::string& sVer)        { m_sToVersion = sVer; }
//	inline const std::string& GetToVersion(void) const       { return m_sToVersion; }

	inline void SetFromVersion(const std::string& sVer)      { m_sFromVersion = sVer; }
	inline const std::string& GetFromVersion(void) const     { return m_sFromVersion; }

	inline void SetMustVisitHost(bool bMustVisit = true)     { m_bMustVisitHost = bMustVisit; }
	inline long GetMustVisitHost(void) const                 { return m_bMustVisitHost; }

	inline void SetPatchSize(long nPatchSize)                { m_nPatchSize = nPatchSize; }
	inline long GetPatchSize(void) const                     { return m_nPatchSize; }

	inline void SetChecksum(long nChecksum)                  { m_nChecksum = nChecksum; }
	inline long GetChecksum(void) const                      { return m_nChecksum; }

	inline void SetDownloadFailures(long nFailures)          { m_nDownloadFailures = nFailures; }
	inline long GetDownloadFailures(void) const              { return m_nDownloadFailures; }

	inline void SetDownloadAborts(long nAborts)              { m_nDownloadAborts = nAborts; }
	inline long GetDownloadAborts(void) const                { return m_nDownloadAborts; }

///@@@	inline bool IsTukati(void) { return (StringCompareNoCase(m_sPatchUrl.substr(0, strlen("TUKATI://")), "TUKATI://") == 0); }

#ifdef _DEBUG
	void DebugBox(Window* pWindow)
	{
		TCHAR sTemp[1024];
		wsprintf(sTemp, "Host Name: %s\n"
						"Host Bitmap: %s\n"
						"Host Text: %s\n"
						"Host URL: %s\n"
						"Patch URL: %s\n"
						"From Version: %s\n"
//						"To Version: %s\n"
						"Must Visit Host: %s\n"
						"Patch Size: %d\n"
						"Check Sum: %d", 
						m_sHostName.c_str(), 
						m_sHostBmp.c_str(), 
						m_sHostTxt.c_str(), 
						m_sHostUrl.c_str(), 
						m_sPatchUrl.c_str(), 
						m_sFromVersion.c_str(), 
//						m_sToVersion.c_str(), 
						m_bMustVisitHost ? TEXT("Yes") : TEXT("No"), 
						m_nPatchSize, 
						m_nChecksum);
		MessageBox(pWindow, sTemp, "Results", MD_OK);
	}

#endif

}; // CPatchData

typedef std::list<CPatchData*> CPatchDataList;

} //namespace

#endif
