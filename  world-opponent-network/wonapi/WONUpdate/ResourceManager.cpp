//----------------------------------------------------------------------------------
// ResourceManager.cpp
//----------------------------------------------------------------------------------
#include "WONGUI/SimpleComponent.h"
#include "ResourceManager.h"
#include "PatchUtils.h"
#include "CustomInfo.h"
#include "Resources/SierraUpWRS.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// Constants.
//----------------------------------------------------------------------------------
const GUIString sDispNameKey       = "%DISPLAY_NAME%";
const GUIString sHostNameKey       = "%HOST_NAME%";
const GUIString sPatchNameKey      = "%PATCH_NAME%";
const GUIString sHostTxtKey        = "%HOST_TEXT%";
const GUIString sPatchSizeKey      = "%PATCH_SIZE%";
const GUIString sPatchSizeUnitsKey = "%PATCH_SIZE_UNITS%";
const GUIString sTcpPortKey        = "%TCP_PORT%";
const GUIString sCurVersionKey     = "%CUR_VERSION%";
const GUIString sNewVersionKey     = "%NEW_VERSION%";
const GUIString sSelfVersionKey    = "%SIERRAUP_VERSION%";


//----------------------------------------------------------------------------------
// ResourceManager Constructor.
//----------------------------------------------------------------------------------
ResourceManager::ResourceManager(void)
{
}

//----------------------------------------------------------------------------------
// ResourceManager Destructor.
//----------------------------------------------------------------------------------
ResourceManager::~ResourceManager(void)
{
}

//----------------------------------------------------------------------------------
// GuiGetSysColor: Translate a Windows Color to a GUI color.
//----------------------------------------------------------------------------------
DWORD GuiGetSysColor(int nIndex)
{
	DWORD aResult = GetSysColor(nIndex);
	return ((aResult&0xff0000)>>16) | (aResult&0xff00)| ((aResult&0xff)<<16);
}

//----------------------------------------------------------------------------------
// GetFinishedString: Translate any key/value pairs in the string.
//----------------------------------------------------------------------------------
GUIString ResourceManager::GetFinishedString(const GUIString& sInfo)
{
	GUIString sString = sInfo;

	CustomInfo* pCustInfo = GetCustomInfo();

	// Replace Key/Value pairs.
	ReplaceSubString(sString, sDispNameKey, pCustInfo->GetDisplayName());
	ReplaceSubString(sString, sCurVersionKey, pCustInfo->GetCurVersion());
	ReplaceSubString(sString, sNewVersionKey, pCustInfo->GetNewVersion());
	ReplaceSubString(sString, sSelfVersionKey, pCustInfo->GetSierraUpVersion());

	if (sString.find(sTcpPortKey) != -1)
	{
		// Find all of the ports.
		TCHAR sPort[32];
		TCHAR sPorts[1024] = "21, 15101"; // 21 is the Generic FTP outbound port, 15101 is used by SierraUp to update itself.
		std::list<std::string>* pServers = pCustInfo->GetDirectoryServerNames();
		if (pServers)
		{
			UINT nAddresses = pServers->size();
			USHORT* pPorts = new USHORT[nAddresses + 2];
			memset(pPorts, 0, sizeof(USHORT) * (nAddresses + 2));
			std::list<std::string>::iterator Itr = pServers->begin();
			int nPorts = 0;
			pPorts[nPorts++] = 21;
			pPorts[nPorts++] = 15101;

			while (Itr != pServers->end())
			{
				GUIString sAddr = *Itr;
				int nPortOffset = sAddr.find(':');
				if (nPortOffset != -1)
				{
					sAddr.erase(0, nPortOffset + 1);
					USHORT nPort = static_cast<USHORT>(atoi(std::string(sAddr).c_str()));
					bool bFoundPort = false;
					for (int i = 0; i < nPorts; i++)
						if (pPorts[i] == nPort)
							bFoundPort = true;
					if (! bFoundPort)
					{
						pPorts[nPorts++] = nPort;
						if (nPorts == 1)
							wsprintf(sPort, "%hd", nPort);
						else
							wsprintf(sPort, ", %hd", nPort);
						strcat(sPorts, sPort);
					}
				}
				Itr++;
			}
			ReplaceSubString(sString, sTcpPortKey, sPorts);
			delete[] pPorts;
		}
		else
			ReplaceSubString(sString, sTcpPortKey, "<Unknown>"); //$$$
	}

	CPatchData* pSelectedPatch = pCustInfo->GetSelectedPatch();
	if (pSelectedPatch)
	{
		ReplaceSubString(sString, sPatchNameKey, pSelectedPatch->GetPatchName());
		ReplaceSubString(sString, sHostNameKey, pSelectedPatch->GetHostName());
		ReplaceSubString(sString, sHostTxtKey, pSelectedPatch->GetHostTxt());

		if (sString.find(sPatchSizeKey) != -1)
		{
			GUIString sUnits = GetFinishedString(Global_Bytes_String);
			long nPatchSize = pSelectedPatch->GetPatchSize();
			if (nPatchSize > 1024 * 10)
			{
				sUnits = GetFinishedString(Global_KiloBytes_String);
				nPatchSize = (nPatchSize + 1023) / 1024;

				if (nPatchSize > 1024 * 10)
				{
					sUnits = GetFinishedString(Global_MegaBytes_String);
					nPatchSize = (nPatchSize + 1023) / 1024;
				}
			}
			GUIString sSize = NumToStrWithCommas(nPatchSize);
			ReplaceSubString(sString, sPatchSizeKey, sSize);
			ReplaceSubString(sString, sPatchSizeUnitsKey, sUnits);
		}
	}

	return sString;
}

//----------------------------------------------------------------------------------
// If the supplied string id contains a valid string, append it to the end of the
// supplied info string.
//----------------------------------------------------------------------------------
void ResourceManager::AppendInfoString(GUIString& sInfo, UpdateResourceId nId)
{
	GUIString sSep = "\n\n";
	GUIString sLine = GetFinishedString(UpdateResource_GetString(nId));

	if (sLine.length() && sLine != " ")
	{
		if (sInfo.length())
			sInfo.append(sSep);
		sInfo.append(sLine);
	}
}

//----------------------------------------------------------------------------------
// Assemble an information string.
//----------------------------------------------------------------------------------
GUIString ResourceManager::BuildInfoString(UpdateResourceId nFirstId)
{
	GUIString sInfo = "";

	AppendInfoString(sInfo, UpdateResourceId(nFirstId + 0));
	AppendInfoString(sInfo, UpdateResourceId(nFirstId + 1));
	AppendInfoString(sInfo, UpdateResourceId(nFirstId + 2));
	AppendInfoString(sInfo, UpdateResourceId(nFirstId + 3));
	AppendInfoString(sInfo, UpdateResourceId(nFirstId + 4));
	AppendInfoString(sInfo, UpdateResourceId(nFirstId + 5));
	
	return sInfo;
}

//----------------------------------------------------------------------------------
// Init: Prepare the initial settings.
//----------------------------------------------------------------------------------
void ResourceManager::Init(void)
{
	// Start by fetching the default colors.
//	WindowManager::GetDefaultWindowManager()->InitSystemColorScheme();
}

