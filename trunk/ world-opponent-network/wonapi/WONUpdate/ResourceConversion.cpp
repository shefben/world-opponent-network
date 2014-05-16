//----------------------------------------------------------------------------------
// ResourceConversion.cpp
//----------------------------------------------------------------------------------
#include "WONGUI/MSNativeImage.h"
#include "ResourceConversion.h"
#include "PatchUtils.h"
#include "StreamBmp.h"
#include "Resources/SierraUpWRS.h"
#include "Resources/Resource.h" // Old resource definitions.
#include "WONGUI/Window.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ResourceMaps::GetString(int theId)
{
	STRING_MAP_CITR anItr = mStringMap.find(theId);
	if(anItr == mStringMap.end())
		return "Invalid String";

	return anItr->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr ResourceMaps::GetImage(int theId)
{
	IMAGE_MAP_CITR anItr = mImageMap.find(theId);
	if(anItr == mImageMap.end())
		return NULL;

	return anItr->second;
}

//----------------------------------------------------------------------------------
// ResourceConversion Constructor.
//----------------------------------------------------------------------------------
ResourceConversion::ResourceConversion(const std::string& sDll, ResourceMapsPtr theResourceMaps) :
	mResourceMaps(theResourceMaps)
{
	m_hDll = NULL;
	m_bAvP2 = false;
	m_bEmpireEarth = false;
	m_bHalfLife = false;
	m_bNolf = false;
	m_bNR2002 = false;
	SetDllFile(sDll);
}

//----------------------------------------------------------------------------------
// ConvertIcon: Load the resource from the DLL, and convert it.
//----------------------------------------------------------------------------------
bool ResourceConversion::ConvertIcon(int nOldID, int nNewID, const std::string& sNewID)
{
	HICON anIconHandle = (HICON)LoadIcon(m_hDll, MAKEINTRESOURCE(nOldID));
	if (anIconHandle)
	{
		MSNativeIconPtr anIcon = new MSNativeIcon(Window::GetDefaultWindow()->GetDisplayContext(), anIconHandle);
		NativeImagePtr anImage = anIcon->GetImage();
		mResourceMaps->mImageMap[nNewID] = anImage;
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------
// ConvertImage: Load the resource from the DLL, and convert it.
//----------------------------------------------------------------------------------
bool ResourceConversion::ConvertImage(int nOldID, int nNewID, const std::string& sNewID)
{
	// Load the bitmap.
	DisplayContextPtr pDC = new DisplayContext();
	HBITMAP hBmp = LoadBitmap(m_hDll, MAKEINTRESOURCE(nOldID));
	if (hBmp)
	{
		MSNativeImagePtr anImage = new MSNativeImage(Window::GetDefaultWindow()->GetDisplayContext(), hBmp);
		mResourceMaps->mImageMap[nNewID] = anImage;
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------
// ConvertString: Load the old string and (if it exists), write it to the new
// resoiurce collection.
//----------------------------------------------------------------------------------
bool ResourceConversion::ConvertString(int nOldID, int nNewID, const std::string& sNewID)
{
	char sRaw[MAX_PATH] = "";

	if (LoadString(m_hDll, nOldID, sRaw, MAX_PATH))
	{
		// Add the string.
		mResourceMaps->mStringMap[nNewID] = sRaw;
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------
// CreateDisabledColor: Construct a gray text color by taking 'half' of the 
// supplied text color.
//----------------------------------------------------------------------------------
bool ResourceConversion::CreateGrayTextColor(int nOldID, int nNewID, const std::string& sNewID)
{
	char sRaw[MAX_PATH] = "";

	if (LoadString(m_hDll, nOldID, sRaw, MAX_PATH))
	{
		// Convert the colors.
		if (IsColorString(sRaw))
		{
			DWORD clSolid = StringToRGB(sRaw);
			int nRed =   (clSolid & 0xFF0000) >> 16;
			int nGreen = (clSolid & 0x00FF00) >> 8;
			int nBlue =  clSolid & 0x0000FF;

			nRed   = 255 - ((255 - nRed) / 2);
			nGreen = 255 - ((255 - nGreen) / 2);
			nBlue  = 255 - ((255 - nBlue) / 2);

			wsprintf(sRaw, "%d, %d, %d", nRed, nGreen, nBlue);
		}

		// Add the string.
		mResourceMaps->mStringMap[nNewID] = sRaw;
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------
// ConvertImages: Convert all of the customized images.
//----------------------------------------------------------------------------------
void ResourceConversion::ConvertImages(void)
{
	// Common Dialog - Custom Background:
	if (! ConvertImage(IDB_CD_LARGE_TEXT_BG, IDB_COMMON_DLG_BACKGROUND, "IDB_COMMON_DLG_BACKGROUND"))
		ConvertImage(IDB_CD_SMALL_TEXT_BG, IDB_COMMON_DLG_BACKGROUND, "IDB_COMMON_DLG_BACKGROUND");

	// No one created dialog-specific backgrounds, so don't bother converting them.

	bool bCommonGameLogo = false;

	// Empire Earth used an 'empty' default image, so we will ignore it and use one of the others.
	if (! m_bEmpireEarth)
	{
		// Common Dialog - Game Logo:
		bCommonGameLogo = ConvertImage(IDB_CD_LARGE_IMAGE, IDB_COMMON_DLG_GAME_LOGO, "IDB_COMMON_DLG_GAME_LOGO");
	}
	// Welcome Dialog - Game Logo:
	if (ConvertImage(IDB_WD_LARGE_IMAGE, IDB_WELCOME_DLG_GAME_LOGO, "IDB_WELCOME_DLG_GAME_LOGO") && ! bCommonGameLogo)
		bCommonGameLogo = ConvertImage(IDB_WD_LARGE_IMAGE, IDB_COMMON_DLG_GAME_LOGO, "IDB_COMMON_DLG_GAME_LOGO");

	// Select Host Dialog - Game Logo:
	if (ConvertImage(IDB_SHD_LARGE_IMAGE, IDB_SELECT_HOST_DLG_GAME_LOGO, "IDB_SELECT_HOST_DLG_GAME_LOGO") && ! bCommonGameLogo)
		bCommonGameLogo = ConvertImage(IDB_SHD_LARGE_IMAGE, IDB_COMMON_DLG_GAME_LOGO, "IDB_COMMON_DLG_GAME_LOGO");

	// Download Dialog - Game Logo:
	if (ConvertImage(IDB_DD_LARGE_IMAGE, IDB_DOWNLOAD_DLG_GAME_LOGO, "IDB_DOWNLOAD_DLG_GAME_LOGO") && ! bCommonGameLogo)
		bCommonGameLogo = ConvertImage(IDB_DD_LARGE_IMAGE, IDB_COMMON_DLG_GAME_LOGO, "IDB_COMMON_DLG_GAME_LOGO");

	// Visit Host Dialog - Game Logo:
	if (ConvertImage(IDB_VHD_LARGE_IMAGE, IDB_VISIT_HOST_DLG_GAME_LOGO, "IDB_VISIT_HOST_DLG_GAME_LOGO") && ! bCommonGameLogo)
		bCommonGameLogo = ConvertImage(IDB_VHD_LARGE_IMAGE, IDB_COMMON_DLG_GAME_LOGO, "IDB_COMMON_DLG_GAME_LOGO");

	// Optional Patch Dialog - Game Logo:
	if (ConvertImage(IDB_OPD_LARGE_IMAGE, IDB_OPTIONAL_PATCH_DLG_GAME_LOGO, "IDB_OPTIONAL_PATCH_DLG_GAME_LOGO") && ! bCommonGameLogo)
		bCommonGameLogo = ConvertImage(IDB_OPD_LARGE_IMAGE, IDB_COMMON_DLG_GAME_LOGO, "IDB_COMMON_DLG_GAME_LOGO");

	// Config Proxy Dialog - Game Logo:
	if (ConvertImage(IDB_CPD_LARGE_IMAGE, IDB_CONFIG_PROXY_DLG_GAME_LOGO, "IDB_CONFIG_PROXY_DLG_GAME_LOGO") && ! bCommonGameLogo)
		bCommonGameLogo = ConvertImage(IDB_CPD_LARGE_IMAGE, IDB_COMMON_DLG_GAME_LOGO, "IDB_COMMON_DLG_GAME_LOGO");

	// MPS Logo:
	ConvertImage(IDB_MAIN_LOGO, IDB_COMMON_DLG_MPS_LOGO, "IDB_COMMON_DLG_MPS_LOGO");
}

//----------------------------------------------------------------------------------
// ConvertStrings: Convert the non-color related strings.
//----------------------------------------------------------------------------------
void ResourceConversion::ConvertStrings(void)
{
	// Common Dialog:
	ConvertString(IDS_CD_YES,                IDS_COMMON_DLG_YES,                    "IDS_COMMON_DLG_YES");
	ConvertString(IDS_CD_NO,                 IDS_COMMON_DLG_NO,                     "IDS_COMMON_DLG_NO");
	ConvertString(IDS_CD_OK,                 IDS_COMMON_DLG_OK,                     "IDS_COMMON_DLG_OK");
	ConvertString(IDS_CD_CANCEL,             IDS_COMMON_DLG_CANCEL,                 "IDS_COMMON_DLG_CANCEL");
	ConvertString(IDS_CD_CLOSE,              IDS_COMMON_DLG_CLOSE,                  "IDS_COMMON_DLG_CLOSE");
	//IDS_CD_CLEAR_SCROLL_BACKGROUND // Depricated

	// Select Host Dialog:
	ConvertString(IDS_SHD_TITLE,             IDS_SELECT_HOST_DLG_TITLE,             "IDS_SELECT_HOST_DLG_TITLE");
	ConvertString(IDS_SHD_INFO_1,            IDS_SELECT_HOST_DLG_INFO_1,            "IDS_SELECT_HOST_DLG_INFO_1");
	ConvertString(IDS_SHD_INFO_2,            IDS_SELECT_HOST_DLG_INFO_2,            "IDS_SELECT_HOST_DLG_INFO_2");
	ConvertString(IDS_SHD_INFO_3,            IDS_SELECT_HOST_DLG_INFO_3,            "IDS_SELECT_HOST_DLG_INFO_3");
	ConvertString(IDS_SHD_INFO_4,            IDS_SELECT_HOST_DLG_INFO_4,            "IDS_SELECT_HOST_DLG_INFO_4");
	ConvertString(IDS_SHD_INFO_5,            IDS_SELECT_HOST_DLG_INFO_5,            "IDS_SELECT_HOST_DLG_INFO_5");
	ConvertString(IDS_SHD_INFO_6,            IDS_SELECT_HOST_DLG_INFO_6,            "IDS_SELECT_HOST_DLG_INFO_6");
	ConvertString(IDS_SHD_CONFIG_PROXY,      IDS_SELECT_HOST_DLG_CONFIG_PROXY,      "IDS_SELECT_HOST_DLG_CONFIG_PROXY");
	ConvertString(IDS_SHD_HELP,              IDS_SELECT_HOST_DLG_HELP,              "IDS_SELECT_HOST_DLG_HELP");
	ConvertString(IDS_SHD_NEXT,              IDS_SELECT_HOST_DLG_NEXT,              "IDS_SELECT_HOST_DLG_NEXT");
	ConvertString(IDS_SHD_HAVE_PATCH,        IDS_SELECT_HOST_DLG_HAVE_PATCH,        "IDS_SELECT_HOST_DLG_HAVE_PATCH");
	ConvertString(IDS_SHD_PATCH_FILTER,      IDS_SELECT_HOST_DLG_PATCH_FILTER,      "IDS_SELECT_HOST_DLG_PATCH_FILTER");
	ConvertString(IDS_SHD_INFO_MANUAL,       IDS_SELECT_HOST_DLG_INFO_MANUAL,       "IDS_SELECT_HOST_DLG_INFO_MANUAL");

	// Config Proxy Dialog:
	ConvertString(IDS_CPD_TITLE,             IDS_CONFIG_PROXY_DLG_TITLE,            "IDS_CONFIG_PROXY_DLG_TITLE");
	ConvertString(IDS_CPD_INFO_1,            IDS_CONFIG_PROXY_DLG_INFO_1,           "IDS_CONFIG_PROXY_DLG_INFO_1");
	ConvertString(IDS_CPD_INFO_2,            IDS_CONFIG_PROXY_DLG_INFO_2,           "IDS_CONFIG_PROXY_DLG_INFO_2");
	ConvertString(IDS_CPD_INFO_3,            IDS_CONFIG_PROXY_DLG_INFO_3,           "IDS_CONFIG_PROXY_DLG_INFO_3");
	ConvertString(IDS_CPD_INFO_4,            IDS_CONFIG_PROXY_DLG_INFO_4,           "IDS_CONFIG_PROXY_DLG_INFO_4");
	ConvertString(IDS_CPD_INFO_5,            IDS_CONFIG_PROXY_DLG_INFO_5,           "IDS_CONFIG_PROXY_DLG_INFO_5");
	ConvertString(IDS_CPD_INFO_6,            IDS_CONFIG_PROXY_DLG_INFO_6,           "IDS_CONFIG_PROXY_DLG_INFO_6");
	ConvertString(IDS_CPD_USE_PROXY,         IDS_CONFIG_PROXY_DLG_USE_PROXY,        "IDS_CONFIG_PROXY_DLG_USE_PROXY");
	ConvertString(IDS_CPD_HOST,              IDS_CONFIG_PROXY_DLG_HOST,             "IDS_CONFIG_PROXY_DLG_HOST");
	ConvertString(IDS_CPD_PORT,              IDS_CONFIG_PROXY_DLG_PORT,             "IDS_CONFIG_PROXY_DLG_PORT");
	ConvertString(IDS_CPD_HELP,              IDS_CONFIG_PROXY_DLG_HELP,             "IDS_CONFIG_PROXY_DLG_HELP");

	// Download Dialog:
	ConvertString(IDS_DD_TITLE,              IDS_DOWNLOAD_DLG_TITLE,                "IDS_DOWNLOAD_DLG_TITLE");
	ConvertString(IDS_DD_INFO_1,             IDS_DOWNLOAD_DLG_INFO_1,               "IDS_DOWNLOAD_DLG_INFO_1");
	ConvertString(IDS_DD_INFO_2,             IDS_DOWNLOAD_DLG_INFO_2,               "IDS_DOWNLOAD_DLG_INFO_2");
	ConvertString(IDS_DD_INFO_3,             IDS_DOWNLOAD_DLG_INFO_3,               "IDS_DOWNLOAD_DLG_INFO_3");
	ConvertString(IDS_DD_INFO_4,             IDS_DOWNLOAD_DLG_INFO_4,               "IDS_DOWNLOAD_DLG_INFO_4");
	ConvertString(IDS_DD_INFO_5,             IDS_DOWNLOAD_DLG_INFO_5,               "IDS_DOWNLOAD_DLG_INFO_5");
	ConvertString(IDS_DD_INFO_6,             IDS_DOWNLOAD_DLG_INFO_6,               "IDS_DOWNLOAD_DLG_INFO_6");
	ConvertString(IDS_DD_HOST,               IDS_DOWNLOAD_DLG_HOST,                 "IDS_DOWNLOAD_DLG_HOST");
	ConvertString(IDS_DD_PROGRESS,           IDS_DOWNLOAD_DLG_PROGRESS,             "IDS_DOWNLOAD_DLG_PROGRESS");
	ConvertString(IDS_DD_PREPARING,          IDS_DOWNLOAD_DLG_PREPARING,            "IDS_DOWNLOAD_DLG_PREPARING");
	ConvertString(IDS_DD_ESTIMATING,         IDS_DOWNLOAD_DLG_ESTIMATING,           "IDS_DOWNLOAD_DLG_ESTIMATING");
	ConvertString(IDS_DD_HELP,               IDS_DOWNLOAD_DLG_HELP,                 "IDS_DOWNLOAD_DLG_HELP");
	ConvertString(IDS_DD_BACK,               IDS_DOWNLOAD_DLG_BACK,                 "IDS_DOWNLOAD_DLG_BACK");
	ConvertString(IDS_DD_VISIT_HOST,         IDS_DOWNLOAD_DLG_VISIT_HOST,           "IDS_DOWNLOAD_DLG_VISIT_HOST");
	ConvertString(IDS_DD_HOSTED_BY,          IDS_DOWNLOAD_DLG_HOSTED_BY,            "IDS_DOWNLOAD_DLG_HOSTED_BY");
	ConvertString(IDS_DD_BYTES_TRANSFERRED,  IDS_DOWNLOAD_DLG_BYTES_TRANSFERRED,           "IDS_DOWNLOAD_DLG_BYTES_TRANSFERRED");
	//IDS_DD_SECONDS        // Obsolete
	//IDS_DD_MINUTE         // Obsolete
	//IDS_DD_MINUTES        // Obsolete
	//IDS_DD_HOUR           // Obsolete
	//IDS_DD_HOURS          // Obsolete
	//IDS_DD_TIME_REMAINING // Obsolete
	ConvertString(IDS_DD_CONNECTION_REFUSED,      IDS_DOWNLOAD_DLG_CONNECTION_REFUSED,      "IDS_DOWNLOAD_DLG_CONNECTION_REFUSED");
	ConvertString(IDS_DD_INVALID_HTTP_HEADER,     IDS_DOWNLOAD_DLG_INVALID_HTTP_HEADER,     "IDS_DOWNLOAD_DLG_INVALID_HTTP_HEADER");
	ConvertString(IDS_DD_INVALID_HTTP_REDIRECT,   IDS_DOWNLOAD_DLG_INVALID_HTTP_REDIRECT,   "IDS_DOWNLOAD_DLG_INVALID_HTTP_REDIRECT");
	ConvertString(IDS_DD_TOO_MANY_HTTP_REDIRECTS, IDS_DOWNLOAD_DLG_TOO_MANY_HTTP_REDIRECTS, "IDS_DOWNLOAD_DLG_TOO_MANY_HTTP_REDIRECTS");
	ConvertString(IDS_DD_DOWNLOAD_ERROR,          IDS_DOWNLOAD_DLG_DOWNLOAD_ERROR,          "IDS_DOWNLOAD_DLG_DOWNLOAD_ERROR");
	ConvertString(IDS_DD_UNKNOWN_HTTP_ERROR,      IDS_DOWNLOAD_DLG_UNKNOWN_HTTP_ERROR,      "IDS_DOWNLOAD_DLG_UNKNOWN_HTTP_ERROR");
	ConvertString(IDS_DD_HTTP_ERROR,              IDS_DOWNLOAD_DLG_HTTP_ERROR,              "IDS_DOWNLOAD_DLG_HTTP_ERROR");
	ConvertString(IDS_DD_HTTP_404_ERROR,          IDS_DOWNLOAD_DLG_HTTP_404_ERROR,          "IDS_DOWNLOAD_DLG_HTTP_404_ERROR");
	ConvertString(IDS_DD_CONFIRM,                 IDS_DOWNLOAD_DLG_CONFIRM,                 "IDS_DOWNLOAD_DLG_CONFIRM");
	ConvertString(IDS_DD_ABANDON_DOWNLOAD,        IDS_DOWNLOAD_DLG_ABANDON_DOWNLOAD,        "IDS_DOWNLOAD_DLG_ABANDON_DOWNLOAD");
	ConvertString(IDS_DD_ABORT_PATCH,             IDS_DOWNLOAD_DLG_ABORT_PATCH,             "IDS_DOWNLOAD_DLG_ABORT_PATCH");
	ConvertString(IDS_DD_UNKNOWN_FTP_ERROR,       IDS_DOWNLOAD_DLG_UNKNOWN_FTP_ERROR,       "IDS_DOWNLOAD_DLG_UNKNOWN_FTP_ERROR");
	ConvertString(IDS_DD_FTP_ERROR,               IDS_DOWNLOAD_DLG_FTP_ERROR,               "IDS_DOWNLOAD_DLG_FTP_ERROR");
	ConvertString(IDS_DD_INVALID_FTP_RESP,        IDS_DOWNLOAD_DLG_INVALID_FTP_RESP,        "IDS_DOWNLOAD_DLG_INVALID_FTP_RESP");
	ConvertString(IDS_DD_INVALID_FTP_PASV_RESP,   IDS_DOWNLOAD_DLG_INVALID_FTP_PASV_RESP,   "IDS_DOWNLOAD_DLG_INVALID_FTP_PASV_RESP");
	ConvertString(IDS_DD_NO_DATA,                 IDS_DOWNLOAD_DLG_NO_DATA,                 "IDS_DOWNLOAD_DLG_NO_DATA");
	ConvertString(IDS_DD_INVALID_FTP_FILE,        IDS_DOWNLOAD_DLG_INVALID_FTP_FILE,        "IDS_DOWNLOAD_DLG_INVALID_FTP_FILE");
	ConvertString(IDS_DD_INVALID_FTP_USER,        IDS_DOWNLOAD_DLG_INVALID_FTP_USER,        "IDS_DOWNLOAD_DLG_INVALID_FTP_USER");
	ConvertString(IDS_DD_INVALID_FTP_PASSWORD,    IDS_DOWNLOAD_DLG_INVALID_FTP_PASSWORD,    "IDS_DOWNLOAD_DLG_INVALID_FTP_PASSWORD");
	ConvertString(IDS_DD_TIME_HOURS_MINUTES,      IDS_DOWNLOAD_DLG_TIME_HOURS_MINUTES,      "IDS_DOWNLOAD_DLG_TIME_HOURS_MINUTES");
	ConvertString(IDS_DD_TIME_HOUR_MINUTES,       IDS_DOWNLOAD_DLG_TIME_HOUR_MINUTES,       "IDS_DOWNLOAD_DLG_TIME_HOUR_MINUTES");
	ConvertString(IDS_DD_TIME_MINUTES_SECONDS,    IDS_DOWNLOAD_DLG_TIME_MINUTES_SECONDS,    "IDS_DOWNLOAD_DLG_TIME_MINUTES_SECONDS");
	ConvertString(IDS_DD_TIME_MINUTE_SECONDS,     IDS_DOWNLOAD_DLG_TIME_MINUTE_SECONDS,     "IDS_DOWNLOAD_DLG_TIME_MINUTE_SECONDS");
	ConvertString(IDS_DD_TIME_SECONDS,            IDS_DOWNLOAD_DLG_TIME_SECONDS,            "IDS_DOWNLOAD_DLG_TIME_SECONDS");
	ConvertString(IDS_DD_HTTP_421_ERROR,          IDS_DOWNLOAD_DLG_HTTP_421_ERROR,          "IDS_DOWNLOAD_DLG_HTTP_421_ERROR");

	// Message Dialog:
//	ConvertString(IDS_MD_BAD_TYPE,                IDS_MESSAGE_DLG_BAD_TYPE,                 "IDS_MESSAGE_DLG_BAD_TYPE");

	// Abort Dialog:
	ConvertString(IDS_AD_ABORT,                   IDS_ABORT_DLG_ABORT,                      "IDS_ABOUT_DLG_ABORT");

	// Welcome (Main) Dialog:
	ConvertString(IDS_WD_CONTINUE,                IDS_WELCOME_DLG_CONTINUE,                 "IDS_WELCOME_DLG_CONTINUE");
	ConvertString(IDS_WD_CANCEL,                  IDS_WELCOME_DLG_CANCEL,                   "IDS_WELCOME_DLG_CANCEL");
	ConvertString(IDS_WD_CONFIG_PROXY,            IDS_WELCOME_DLG_CONFIG_PROXY,             "IDS_WELCOME_DLG_CONFIG_PROXY");
	ConvertString(IDS_WD_HELP,                    IDS_WELCOME_DLG_HELP,                     "IDS_WELCOME_DLG_HELP");
	ConvertString(IDS_WD_BAD_HELP_EXE,            IDS_BAD_HELP_EXE,                         "IDS_BAD_HELP_EXE"); // No longer in the Welcome Dialog
	ConvertString(IDS_WD_INFO_1,                  IDS_WELCOME_DLG_INFO_1,                   "IDS_WELCOME_DLG_INFO_1");
	ConvertString(IDS_WD_INFO_2,                  IDS_WELCOME_DLG_INFO_2,                   "IDS_WELCOME_DLG_INFO_2");
	ConvertString(IDS_WD_INFO_3,                  IDS_WELCOME_DLG_INFO_3,                   "IDS_WELCOME_DLG_INFO_3");
	ConvertString(IDS_WD_INFO_4,                  IDS_WELCOME_DLG_INFO_4,                   "IDS_WELCOME_DLG_INFO_4");
	ConvertString(IDS_WD_INFO_5,                  IDS_WELCOME_DLG_INFO_5,                   "IDS_WELCOME_DLG_INFO_5");
	ConvertString(IDS_WD_INFO_6,                  IDS_WELCOME_DLG_INFO_6,                   "IDS_WELCOME_DLG_INFO_6");

	// About Patcher Dialog:
	ConvertString(IDS_APD_VERSION,                IDS_ABOUT_DLG_VERSION,                    "IDS_ABOUT_DLG_VERSION");
	ConvertString(IDS_APD_COPYRIGHT,              IDS_ABOUT_DLG_COPYRIGHT,                  "IDS_ABOUT_DLG_COPYRIGHT");
	ConvertString(IDS_APD_TITLE,                  IDS_ABOUT_DLG_TITLE,                      "IDS_ABOUT_DLG_TITLE");

	// Message of the Day Dialog:
	ConvertString(IDS_MDD_SYSTEM_TITLE,           IDS_MOTD_DLG_SYSTEM_TITLE,                "IDS_MOTD_DLG_SYSTEM_TITLE");
	ConvertString(IDS_MDD_PRODUCT_TITLE,          IDS_MOTD_DLG_PRODUCT_TITLE,               "IDS_MOTD_DLG_PRODUCT_TITLE");

	// Visit Host Dialog:
	ConvertString(IDS_VHD_TITLE,                  IDS_VISIT_HOST_DLG_TITLE,                 "IDS_VISIT_HOST_DLG_TITLE");
	ConvertString(IDS_VHD_INFO_1,                 IDS_VISIT_HOST_DLG_INFO_1,                "IDS_VISIT_HOST_DLG_INFO_1");
	ConvertString(IDS_VHD_INFO_2,                 IDS_VISIT_HOST_DLG_INFO_2,                "IDS_VISIT_HOST_DLG_INFO_2");
	ConvertString(IDS_VHD_INFO_3,                 IDS_VISIT_HOST_DLG_INFO_3,                "IDS_VISIT_HOST_DLG_INFO_3");
	ConvertString(IDS_VHD_INFO_4,                 IDS_VISIT_HOST_DLG_INFO_4,                "IDS_VISIT_HOST_DLG_INFO_4");
	ConvertString(IDS_VHD_INFO_5,                 IDS_VISIT_HOST_DLG_INFO_5,                "IDS_VISIT_HOST_DLG_INFO_5");
	ConvertString(IDS_VHD_INFO_6,                 IDS_VISIT_HOST_DLG_INFO_6,                "IDS_VISIT_HOST_DLG_INFO_6");
	ConvertString(IDS_VHD_HELP,                   IDS_VISIT_HOST_DLG_HELP,                  "IDS_VISIT_HOST_DLG_HELP");
	ConvertString(IDS_VHD_BACK,                   IDS_VISIT_HOST_DLG_BACK,                  "IDS_VISIT_HOST_DLG_BACK");
	ConvertString(IDS_VHD_NEXT,                   IDS_VISIT_HOST_DLG_NEXT,                  "IDS_VISIT_HOST_DLG_NEXT");

	// Optional Patch Dialog:
	ConvertString(IDS_OPD_TITLE,                  IDS_OPTIONAL_PATCH_DLG_TITLE,             "IDS_OPTIONAL_PATCH_DLG_TITLE");
	ConvertString(IDS_OPD_INFO_1,                 IDS_OPTIONAL_PATCH_DLG_INFO_1,            "IDS_OPTIONAL_PATCH_DLG_INFO_1");
	ConvertString(IDS_OPD_INFO_2,                 IDS_OPTIONAL_PATCH_DLG_INFO_2,            "IDS_OPTIONAL_PATCH_DLG_INFO_2");
	ConvertString(IDS_OPD_INFO_3,                 IDS_OPTIONAL_PATCH_DLG_INFO_3,            "IDS_OPTIONAL_PATCH_DLG_INFO_3");
	ConvertString(IDS_OPD_INFO_4,                 IDS_OPTIONAL_PATCH_DLG_INFO_4,            "IDS_OPTIONAL_PATCH_DLG_INFO_4");
	ConvertString(IDS_OPD_INFO_5,                 IDS_OPTIONAL_PATCH_DLG_INFO_5,            "IDS_OPTIONAL_PATCH_DLG_INFO_5");
	ConvertString(IDS_OPD_INFO_6,                 IDS_OPTIONAL_PATCH_DLG_INFO_6,            "IDS_OPTIONAL_PATCH_DLG_INFO_6");
	ConvertString(IDS_OPD_VIEW_DETAILS,           IDS_OPTIONAL_PATCH_DLG_VIEW_DETAILS,      "IDS_OPTIONAL_PATCH_DLG_VIEW_DETAILS");
	ConvertString(IDS_OPD_HELP,                   IDS_OPTIONAL_PATCH_DLG_HELP,              "IDS_OPTIONAL_PATCH_DLG_HELP");
	ConvertString(IDS_OPD_NEXT,                   IDS_OPTIONAL_PATCH_DLG_NEXT,              "IDS_OPTIONAL_PATCH_DLG_NEXT");
	ConvertString(IDS_OPD_BAD_DESC_EXE,           IDS_OPTIONAL_PATCH_DLG_BAD_DESC_EXE,      "IDS_OPTIONAL_PATCH_DLG_BAD_DESC_EXE");
	ConvertString(IDS_OPD_DESC_TITLE,             IDS_OPTIONAL_PATCH_DLG_DESC_TITLE,        "IDS_OPTIONAL_PATCH_DLG_DESC_TITLE");
	ConvertString(IDS_OPD_FETCHING_DESC,          IDS_OPTIONAL_PATCH_DLG_FETCHING_DESC,     "IDS_OPTIONAL_PATCH_DLG_FETCHING_DESC");
	ConvertString(IDS_OPD_TIMED_OUT,              IDS_OPTIONAL_PATCH_DLG_TIMED_OUT,         "IDS_OPTIONAL_PATCH_DLG_TIMED_OUT");
	ConvertString(IDS_OPD_DOWNLOAD_FAILED,        IDS_OPTIONAL_PATCH_DLG_DOWNLOAD_FAILED,   "IDS_OPTIONAL_PATCH_DLG_DOWNLOAD_FAILED");

	// General (non specific):
	ConvertString(IDS_CHECKING_FOR_PATCH,         IDS_CHECKING_FOR_PATCH,                   "IDS_CHECKING_FOR_PATCH");
	ConvertString(IDS_DOWNLOADING_INFO,           IDS_DOWNLOADING_INFO,                     "IDS_DOWNLOADING_INFO");
	ConvertString(IDS_ABORT_TITLE,                IDS_ABORT_TITLE,                          "IDS_ABORT_TITLE");
	ConvertString(IDS_VISIT,                      IDS_VISIT,                                "IDS_VISIT");
	ConvertString(IDS_WRONG_SIZE,                 IDS_WRONG_SIZE,                           "IDS_WRONG_SIZE");
	ConvertString(IDS_INVALID_CHECKSUM,           IDS_INVALID_CHECKSUM,                     "IDS_INVALID_CHECKSUM");
	ConvertString(IDS_INVALID_PATCH,              IDS_INVALID_PATCH,                        "IDS_INVALID_PATCH");
	ConvertString(IDS_VALIDATION,                 IDS_VALIDATION,                           "IDS_VALIDATION");
	ConvertString(IDS_VALIDATING,                 IDS_VALIDATING,                           "IDS_VALIDATING");
	ConvertString(IDS_DOWNLOAD_FAILURES,          IDS_DOWNLOAD_FAILURES,                    "IDS_DOWNLOAD_FAILURES");
	ConvertString(IDS_DOWNLOAD_ABORTS,            IDS_DOWNLOAD_ABORTS,                      "IDS_DOWNLOAD_ABORTS");
	ConvertString(IDS_NO_STRING_RES,              IDS_NO_STRING_RES,                        "IDS_NO_STRING_RES");
	ConvertString(IDS_MENU_ABOUT,                 IDS_MENU_ABOUT,                           "IDS_MENU_ABOUT");
	ConvertString(IDS_PARAM_ERR_TITLE,            IDS_PARAM_ERR_TITLE,                      "IDS_PARAM_ERR_TITLE");
	ConvertString(IDS_CFG_FILE_TOO_BIG,           IDS_CFG_FILE_TOO_BIG,                     "IDS_CFG_FILE_TOO_BIG");
	ConvertString(IDS_NO_READ_CFG_FILE,           IDS_NO_READ_CFG_FILE,                     "IDS_NO_READ_CFG_FILE");
	ConvertString(IDS_NO_OPEN_CFG_FILE,           IDS_NO_OPEN_CFG_FILE,                     "IDS_NO_OPEN_CFG_FILE");
	ConvertString(IDS_PATCHER_DISP_NAME,          IDS_PATCHER_DISP_NAME,                    "IDS_PATCHER_DISP_NAME");
	ConvertString(IDS_BAD_HELP_FILE,              IDS_BAD_HELP_FILE,                        "IDS_BAD_HELP_FILE");
	ConvertString(IDS_BAD_RES_FILE,               IDS_BAD_RES_FILE,                         "IDS_BAD_RES_FILE");
	ConvertString(IDS_UNKNOWN_PARAM,              IDS_UNKNOWN_PARAM,                        "IDS_UNKNOWN_PARAM");
	ConvertString(IDS_IP_NO_PROD_NAME,            IDS_IP_NO_PROD_NAME,                      "IDS_IP_NO_PROD_NAME");
	ConvertString(IDS_IP_NO_DISP_NAME,            IDS_IP_NO_DISP_NAME,                      "IDS_IP_NO_DISP_NAME");
	ConvertString(IDS_IP_NO_PATCH_FOLDER,         IDS_IP_NO_PATCH_FOLDER,                   "IDS_IP_NO_PATCH_FOLDER");
	ConvertString(IDS_IP_NO_VERSION,              IDS_IP_NO_VERSION,                        "IDS_IP_NO_VERSION");
	ConvertString(IDS_IP_NO_DIR_SERVER,           IDS_IP_NO_DIR_SERVER,                     "IDS_IP_NO_DIR_SERVER");
//	ConvertString(IDS_MAIN_INFO,                  IDS_MAIN_INFO,                            "IDS_MAIN_INFO");
	ConvertString(IDS_PR_UNKNOWN,                 IDS_PR_UNKNOWN,                           "IDS_PR_UNKNOWN");
	ConvertString(IDS_PR_UP_TO_DATE,              IDS_PR_UP_TO_DATE,                        "IDS_PR_UP_TO_DATE");
	ConvertString(IDS_PR_PATCH_FOUND,             IDS_PR_PATCH_FOUND,                       "IDS_PR_PATCH_FOUND");
	ConvertString(IDS_PR_PATCH_FETCHED,           IDS_PR_PATCH_FETCHED,                     "IDS_PR_PATCH_FETCHED");
	ConvertString(IDS_PR_USER_ABORT,              IDS_PR_USER_ABORT,                        "IDS_PR_USER_ABORT");
	ConvertString(IDS_PR_NO_SERVER_CONNECTION,    IDS_PR_NO_SERVER_CONNECTION,              "IDS_PR_NO_SERVER_CONNECTION");
	ConvertString(IDS_PR_NO_HOST_LIST,            IDS_PR_NO_HOST_LIST,                      "IDS_PR_NO_HOST_LIST");
	ConvertString(IDS_PR_NO_DOWNLOAD,             IDS_PR_NO_DOWNLOAD,                       "IDS_PR_NO_DOWNLOAD");
	ConvertString(IDS_NO_MONITOR,                 IDS_NO_MONITOR,                           "IDS_NO_MONITOR");
	ConvertString(IDS_PS_UNDETERMINED,            IDS_PS_UNDETERMINED,                      "IDS_PS_UNDETERMINED");
	ConvertString(IDS_NO_FIND_PATCH,              IDS_NO_FIND_PATCH,                        "IDS_NO_FIND_PATCH");
	ConvertString(IDS_NO_RUN_PATCH,               IDS_NO_RUN_PATCH,                         "IDS_NO_RUN_PATCH");
	ConvertString(IDS_PATCH_FAILED,               IDS_PATCH_FAILED,                         "IDS_PATCH_FAILED");
	ConvertString(IDS_PREP_PATCHER,               IDS_PREP_PATCHER,                         "IDS_PREP_PATCHER");
	ConvertString(IDS_CHECKING_FOR_THE_PATCH,     IDS_CHECKING_FOR_THE_PATCH,               "IDS_CHECKING_FOR_THE_PATCH");
	ConvertString(IDS_VER_CHECK_FAILED,           IDS_VER_CHECK_FAILED,                     "IDS_VER_CHECK_FAILED");
	ConvertString(IDS_PATCH_FOUND,                IDS_PATCH_FOUND,                          "IDS_PATCH_FOUND");
	ConvertString(IDS_NO_RETRIEVE_PATCH,          IDS_NO_RETRIEVE_PATCH,                    "IDS_NO_RETRIEVE_PATCH");
	ConvertString(IDS_APPLYING,                   IDS_APPLYING,                             "IDS_APPLYING");
	ConvertString(IDS_ANOTHER_PATCH,              IDS_ANOTHER_PATCH,                        "IDS_ANOTHER_PATCH");
	ConvertString(IDS_PATCH_APPLIED,              IDS_PATCH_APPLIED,                        "IDS_PATCH_APPLIED");
	ConvertString(IDS_LAUNCHING_EXE,              IDS_LAUNCHING_EXE,                        "IDS_LAUNCHING_EXE");
	ConvertString(IDS_CHECKING_MOTD,              IDS_CHECKING_MOTD,                        "IDS_CHECKING_MOTD");
	ConvertString(IDS_CHECKING_FOR_MOTD,          IDS_CHECKING_FOR_MOTD,                    "IDS_CHECKING_FOR_MOTD");
	ConvertString(IDS_UP_TO_DATE,                 IDS_UP_TO_DATE,                           "IDS_UP_TO_DATE");
	ConvertString(IDS_PATCHER_INFO_TITLE,         IDS_PATCHER_INFO_TITLE,                   "IDS_PATCHER_INFO_TITLE");
	ConvertString(IDS_VERSION_MISSING,            IDS_VERSION_MISSING,                      "IDS_VERSION_MISSING");
	ConvertString(IDS_PATCH_RETURNED_ERROR,       IDS_PATCH_RETURNED_ERROR,                 "IDS_PATCH_RETURNED_ERROR");
	ConvertString(IDS_CHECK_CONNECTION,           IDS_CHECK_CONNECTION,                     "IDS_CHECK_CONNECTION");
	ConvertString(IDS_TIMEOUT,                    IDS_TIMEOUT,                              "IDS_TIMEOUT");
	ConvertString(IDS_SKIP_MOTD,                  IDS_SKIP_MOTD,                            "IDS_SKIP_MOTD");
	ConvertString(IDS_WARNING,                    IDS_WARNING,                              "IDS_WARNING");
	ConvertString(IDS_INCOMPLETE_PATCH,           IDS_INCOMPLETE_PATCH,                     "IDS_INCOMPLETE_PATCH");
	ConvertString(IDS_SELF_UP_TO_DATE,            IDS_SELF_UP_TO_DATE,                      "IDS_SELF_UP_TO_DATE");
	ConvertString(IDS_CHECKING_SELF_FOR_PATCH,    IDS_CHECKING_SELF_FOR_PATCH,              "IDS_CHECKING_SELF_FOR_PATCH");
	ConvertString(IDS_CLOSE_FOR_SELF_PATCH,       IDS_CLOSE_FOR_SELF_PATCH,                 "IDS_CLOSE_FOR_SELF_PATCH");
	ConvertString(IDS_SELF_NO_HOST_LIST,          IDS_SELF_NO_HOST_LIST,                    "IDS_SELF_NO_HOST_LIST");
	ConvertString(IDS_NO_FIND_RESTARTER,          IDS_NO_FIND_RESTARTER,                    "IDS_NO_FIND_RESTARTER");
	ConvertString(IDS_NO_RUN_RESTARTER,           IDS_NO_RUN_RESTARTER,                     "IDS_NO_RUN_RESTARTER");
	ConvertString(IDS_RESTARTER_FAILED,           IDS_RESTARTER_FAILED,                     "IDS_RESTARTER_FAILED");
	ConvertString(IDS_SIERRA_UPDATE,              IDS_SIERRA_UPDATE,                        "IDS_SIERRA_UPDATE");
	ConvertString(IDS_SELF_UPDATE_LANG,           IDS_SELF_UPDATE_LANG,                     "IDS_SELF_UPDATE_LANG");
	ConvertString(IDS_NO_WRITE_PSAPI,             IDS_NO_WRITE_PSAPI,                       "IDS_NO_WRITE_PSAPI");
	ConvertString(IDS_NO_WRITE_RESTARTER,         IDS_NO_WRITE_RESTARTER,                   "IDS_NO_WRITE_RESTARTER");
	ConvertString(IDS_ERROR,                      IDS_ERROR,                                "IDS_ERROR");
	ConvertString(IDS_HOST,                       IDS_HOST,                                 "IDS_HOST");
	ConvertString(IDS_BYTES,                      IDS_BYTES,                                "IDS_BYTES");
	ConvertString(IDS_KILOBYTES,                  IDS_KILOBYTES,                            "IDS_KILOBYTES");
	ConvertString(IDS_MEGABYTES,                  IDS_MEGABYTES,                            "IDS_MEGABYTES");
	ConvertString(IDS_PR_OPTIONAL_PATCH,          IDS_PR_OPTIONAL_PATCH,                    "IDS_PR_OPTIONAL_PATCH");
	ConvertString(IDS_PR_MANUAL_DOWNLOAD,         IDS_PR_MANUAL_DOWNLOAD,                   "IDS_PR_MANUAL_DOWNLOAD");
	ConvertString(IDS_OPTIONAL_PATCH_FOUND,       IDS_OPTIONAL_PATCH_FOUND,                 "IDS_OPTIONAL_PATCH_FOUND");
	ConvertString(IDS_POSSIBLE_PATCH,             IDS_POSSIBLE_PATCH,                       "IDS_POSSIBLE_PATCH");
	ConvertString(IDS_CHECK_FOR_POSSIBLE_PATCH,   IDS_CHECK_FOR_POSSIBLE_PATCH,             "IDS_CHECK_FOR_POSSIBLE_PATCH");
	ConvertString(IDS_NO_PATCH_URL,               IDS_NO_PATCH_URL,                         "IDS_NO_PATCH_URL");

	// New strings
	ConvertString(IDS_OPTION_START_WIDTH,         IDS_OPTION_START_WIDTH,                   "IDS_OPTION_START_WIDTH");
	ConvertString(IDS_OPTION_START_HEIGHT,        IDS_OPTION_START_HEIGHT,                  "IDS_OPTION_START_HEIGHT");
}

//----------------------------------------------------------------------------------
// ConvertColors: Convert the color strings to new colors (there are fewer now than 
// there were).
//----------------------------------------------------------------------------------
void ResourceConversion::ConvertColors(void)
{
	ConvertString(IDS_CD_BUTTON_UP_FACE_CLR,        IDS_COLOR_3D_FACE,        "IDS_COLOR_3D_FACE");
	ConvertString(IDS_CD_BUTTON_UP_TOP_EXT_CLR,     IDS_COLOR_3D_HILIGHT,     "IDS_COLOR_3D_HILIGHT");
	ConvertString(IDS_CD_BUTTON_UP_BOTTOM_INT_CLR,  IDS_COLOR_3D_SHADOW,      "IDS_COLOR_3D_SHADOW");
	ConvertString(IDS_CD_BUTTON_UP_BOTTOM_EXT_CLR,  IDS_COLOR_3D_DARK_SHADOW, "IDS_COLOR_3D_DARK_SHADOW");
	ConvertString(IDS_CD_BUTTON_UP_TEXT_CLR,        IDS_COLOR_BUTTON_TEXT,    "IDS_COLOR_BUTTON_TEXT");
	ConvertString(IDS_CD_SCROLL_FACE_CLR,           IDS_COLOR_SCROLLBAR,      "IDS_COLOR_SCROLLBAR");
	ConvertString(IDS_CD_LIST_BOX_SELECT_FACE_CLR,  IDS_COLOR_HILIGHT,        "IDS_COLOR_HILIGHT");
	ConvertString(IDS_CD_LIST_BOX_SELECT_TEXT_CLR,  IDS_COLOR_HILIGHT_TEXT,   "IDS_COLOR_HILIGHT_TEXT");
	ConvertString(IDS_CD_TEXT_BACKGROUND_CLR,       IDS_COLOR_TOOL_TIP_BACK,  "IDS_COLOR_TOOL_TIP_BACK");
	ConvertString(IDS_CD_TEXT_FOREGROUND_CLR,       IDS_COLOR_TOOL_TIP_TEXT,  "IDS_COLOR_TOOL_TIP_TEXT");
	ConvertString(IDS_CD_TEXT_BACKGROUND_CLR,       IDS_COLOR_MENU_BACK,      "IDS_COLOR_MENU_BACK");
	ConvertString(IDS_CD_TEXT_FOREGROUND_CLR,       IDS_COLOR_MENU_TEXT,      "IDS_COLOR_MENU_TEXT");
	ConvertString(IDS_CD_TEXT_BACKGROUND_CLR,       IDS_COLOR_BACK,           "IDS_COLOR_BACK");
	ConvertString(IDS_CD_TEXT_FOREGROUND_CLR,       IDS_COLOR_TEXT,           "IDS_COLOR_TEXT");

	// There was no disabled color, so create it from 'half' of the normal text color.
	CreateGrayTextColor(IDS_CD_EDIT_FOREGROUND_CLR, IDS_COLOR_GRAY_TEXT,      "IDS_COLOR_GRAY_TEXT");
}

//----------------------------------------------------------------------------------
// ConvertOther: Convert the left over resources.
//----------------------------------------------------------------------------------
void ResourceConversion::ConvertOthers(void)
{
	// Translate the custom icon.
	ConvertIcon(IDI_APP, IDB_MAIN_ICON, "IDB_MAIN_ICON");

	// No one created a unique cursor, lets not bother translating it.

	// NR2002 has image placement issues, so set the options that will work best.
	if (m_bNR2002)
	{
		mResourceMaps->mStringMap[IDS_OPTION_RESIZEABLE] = "0";
		mResourceMaps->mStringMap[IDS_OPTION_GAME_LOGO_POS] = "UL";
	}

	// AvP2 needs a custom background color.
	if (m_bAvP2)
		mResourceMaps->mStringMap[IDS_COLOR_DLG_BACKGROUND] = "0, 0, 0";

	// Half-Life needs a custom background color, and the image needs to be set in the upper left.
	if (m_bHalfLife)
	{
		mResourceMaps->mStringMap[IDS_COLOR_DLG_BACKGROUND] = "214, 211, 206";
		mResourceMaps->mStringMap[IDS_OPTION_GAME_LOGO_POS] = "UL";
	}

	// Nolf needs a custom background color.
	if (m_bNolf)
		mResourceMaps->mStringMap[IDS_COLOR_DLG_BACKGROUND] = "247, 115, 8";
}

//----------------------------------------------------------------------------------
// Convert: Convert all of the old resources to new ones.
//----------------------------------------------------------------------------------
bool ResourceConversion::Convert(void)
{
	m_hDll = LoadLibrary(m_sDllFile.c_str());
	if (! m_hDll)
		return false;

	ConvertImages();
	ConvertStrings();
	ConvertColors();
	ConvertOthers();

	// Wrap up.
	FreeLibrary(m_hDll);
	m_hDll = NULL;
	return true;
}

//----------------------------------------------------------------------------------
// SetDllFile: Set the dll to translate - and check for specific products.
//----------------------------------------------------------------------------------
void ResourceConversion::SetDllFile(const std::string& sFile)
{
	m_sDllFile = sFile;

	GUIString sLowerFile = sFile;
	sLowerFile.toLowerCase();

	// Identify certain custom DLLs so we can tweak them.
	if (sLowerFile.find("avp2up.dll") != -1)
		m_bAvP2 = true;
	if (sLowerFile.find("eeskin.dll") != -1)
		m_bEmpireEarth = true;
	if (sLowerFile.find("sierraupres.dll") != -1) // Yes, Half-life used this.
		m_bHalfLife = true;
	if (sLowerFile.find("nolf.dll") != -1)
		m_bNolf = true;
	if (sLowerFile.find("papyres.dll") != -1)
		m_bNR2002 = true;
}
