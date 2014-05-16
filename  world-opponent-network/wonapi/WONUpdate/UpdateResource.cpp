#include "WONGUI/WONGUIConfig/ComponentConfig.h"
#include "UpdateResource.h"

using namespace WONAPI;

GUIString Global_Yes_String;
GUIString Global_No_String;
GUIString Global_Ok_String;
GUIString Global_Cancel_String;
GUIString Global_Close_String;
GUIString Global_BadHelpExe_String;
GUIString Global_WrongSize_String;
GUIString Global_InvalidChecksum_String;
GUIString Global_InvalidPatch_String;
GUIString Global_Validation_String;
GUIString Global_Validating_String;
GUIString Global_DownloadFailures_String;
GUIString Global_DownloadAborts_String;
GUIString Global_NoStringRes_String;
GUIString Global_MenuAbout_String;
GUIString Global_ParamErrTitle_String;
GUIString Global_CfgFileTooBig_String;
GUIString Global_NoReadCfgFile_String;
GUIString Global_NoOpenCfgFile_String;
GUIString Global_PatcherDispName_String;
GUIString Global_BadHelpFile_String;
GUIString Global_BadResFile_String;
GUIString Global_UnknownParam_String;
GUIString Global_PrUnknown_String;
GUIString Global_PrUpToDate_String;
GUIString Global_PrPatchFound_String;
GUIString Global_PrPatchFetched_String;
GUIString Global_PrUserAbort_String;
GUIString Global_PrNoServerConnection_String;
GUIString Global_PrNoHostList_String;
GUIString Global_PrNoDownload_String;
GUIString Global_NoMonitor_String;
GUIString Global_PsUndetermined_String;
GUIString Global_NoFindPatch_String;
GUIString Global_NoRunPatch_String;
GUIString Global_PatchFailed_String;
GUIString Global_PrepPatcher_String;
GUIString Global_CheckingForThePatch_String;
GUIString Global_VerCheckFailed_String;
GUIString Global_PatchFound_String;
GUIString Global_NoRetrievePatch_String;
GUIString Global_Applying_String;
GUIString Global_AnotherPatch_String;
GUIString Global_PatchApplied_String;
GUIString Global_LaunchingExe_String;
GUIString Global_CheckingMotd_String;
GUIString Global_CheckingForMotd_String;
GUIString Global_UpToDate_String;
GUIString Global_PatcherInfoTitle_String;
GUIString Global_VersionFileCheckFailed_String;
GUIString Global_PatchReturnedError_String;
GUIString Global_CheckConnection_String;
GUIString Global_TimeOut_String;
GUIString Global_SkipMotd_String;
GUIString Global_Warning_String;
GUIString Global_IncompletePatch_String;
GUIString Global_SelfUpToDate_String;
GUIString Global_CheckingSelfForPatch_String;
GUIString Global_CloseForSelfPatch_String;
GUIString Global_SelfNoHostList_String;
GUIString Global_NoFindRestarter_String;
GUIString Global_NoRunRestarter_String;
GUIString Global_RestarterFailed_String;
GUIString Global_SierraUpdate_String;
GUIString Global_SelfUpdateLang_String;
GUIString Global_NoWritePsapi_String;
GUIString Global_NoWriteRestarter_String;
GUIString Global_Error_String;
GUIString Global_Host_String;
GUIString Global_Bytes_String;
GUIString Global_KiloBytes_String;
GUIString Global_MegaBytes_String;
GUIString Global_PrOptionalPatch_String;
GUIString Global_PrManualDownload_String;
GUIString Global_OptionalPatchFound_String;
GUIString Global_PossiblePatch_String;
GUIString Global_CheckForPossiblePatch_String;
GUIString Global_NoPatchUrl_String;
GUIString Global_LocalPatchDoesNotMatch_String;
GUIString Global_CheckConection_String;
GUIString Global_CheckConectionTitle_String;
GUIString Global_Success_String;
GUIString Global_MotdSkipped_String;
GUIString Global_MotdNotFound_String;
GUIString Global_GameMotdNotFound_String;
GUIString Global_MotdError_String;
GUIString Global_NoNewMotd_String;
GUIString Global_PatchRequired_String;
GUIString Global_OptionalPatch_String;
GUIString Global_PartialDirServerFailure_String;
GUIString Global_DirServerFailure_String;
GUIString Global_BadVersionNoUpdate_String;
GUIString Global_SierraUpBadVersionNoUpdate_String;
GUIString Global_PatchServerError_String;
GUIString Global_UpdateServerTimedOut_String;
GUIString Global_NoConfigFile_String;
ImagePtr Global_GameLogo_Image;
ImagePtr Global_MpsLogo_Image;
int Global_HOST_AD_WIDTH_Int;
int Global_HOST_AD_HEIGHT_Int;

GUIString SelectHost_Title_String;
GUIString SelectHost_Info1_String;
GUIString SelectHost_Info2_String;
GUIString SelectHost_Info3_String;
GUIString SelectHost_Info4_String;
GUIString SelectHost_Info5_String;
GUIString SelectHost_Info6_String;
GUIString SelectHost_ConfigProxy_String;
GUIString SelectHost_Help_String;
GUIString SelectHost_Next_String;
GUIString SelectHost_HavePatch_String;
GUIString SelectHost_PatchFilter_String;
GUIString SelectHost_InfoManual_String;
GUIString SelectHost_Back_String;
GUIString SelectHost_FileOpenDlgTitle_String;
GUIString SelectHost_HostTitle_String;
GUIString SelectHost_Results_String;
GUIString SelectHost_WaitInfo1_String;
GUIString SelectHost_WaitInfo2_String;
GUIString SelectHost_WaitInfo3_String;
GUIString SelectHost_WaitInfo4_String;
GUIString SelectHost_WaitInfo5_String;
GUIString SelectHost_WaitInfo6_String;

GUIString ConfigProxy_Title_String;
GUIString ConfigProxy_Info1_String;
GUIString ConfigProxy_Info2_String;
GUIString ConfigProxy_Info3_String;
GUIString ConfigProxy_Info4_String;
GUIString ConfigProxy_Info5_String;
GUIString ConfigProxy_Info6_String;
GUIString ConfigProxy_UseProxy_String;
GUIString ConfigProxy_Host_String;
GUIString ConfigProxy_Port_String;
GUIString ConfigProxy_Help_String;

GUIString Download_Title_String;
GUIString Download_Info1_String;
GUIString Download_Info2_String;
GUIString Download_Info3_String;
GUIString Download_Info4_String;
GUIString Download_Info5_String;
GUIString Download_Info6_String;
GUIString Download_Host_String;
GUIString Download_Progress_String;
GUIString Download_Preparing_String;
GUIString Download_Estimating_String;
GUIString Download_Help_String;
GUIString Download_Back_String;
GUIString Download_VisitHost_String;
GUIString Download_HostedBy_String;
GUIString Download_BytesTransferred_String;
GUIString Download_ConnectionRefused_String;
GUIString Download_InvalidHttpHeader_String;
GUIString Download_InvalidHttpRedirect_String;
GUIString Download_TooManyHttpRedirects_String;
GUIString Download_DownloadError_String;
GUIString Download_UnknownHttpError_String;
GUIString Download_HttpError_String;
GUIString Download_Http404Error_String;
GUIString Download_Confirm_String;
GUIString Download_AbandonDownload_String;
GUIString Download_AbortPatch_String;
GUIString Download_UnknownFtpError_String;
GUIString Download_FtpError_String;
GUIString Download_InvalidFtpResp_String;
GUIString Download_InvalidFtpPasvResp_String;
GUIString Download_NoData_String;
GUIString Download_InvalidFtpFile_String;
GUIString Download_InvalidFtpUser_String;
GUIString Download_InvalidFtpPassword_String;
GUIString Download_TimeHoursMinutes_String;
GUIString Download_TimeHourMinutes_String;
GUIString Download_TimeMinutesSeconds_String;
GUIString Download_TimeMinuteSeconds_String;
GUIString Download_TimeSeconds_String;
GUIString Download_Http421Error_String;
GUIString Download_Next_String;
GUIString Download_UnableToFindHost_String;
GUIString Download_NoHostUrl_String;

GUIString Dialog_AbortDlgAbort_String;
Background Dialog_BaseBackground_Background;

GUIString Welcome_Continue_String;
GUIString Welcome_Cancel_String;
GUIString Welcome_ConfigProxy_String;
GUIString Welcome_Help_String;
GUIString Welcome_Info1_String;
GUIString Welcome_Info2_String;
GUIString Welcome_Info3_String;
GUIString Welcome_Info4_String;
GUIString Welcome_Info5_String;
GUIString Welcome_Info6_String;
GUIString Welcome_Title_String;

GUIString About_Version_String;
GUIString About_Copyright_String;
GUIString About_Title_String;

GUIString Motd_SystemTitle_String;
GUIString Motd_ProductTitle_String;
GUIString Motd_Info1_String;
GUIString Motd_Info2_String;
GUIString Motd_Info3_String;
GUIString Motd_Info4_String;
GUIString Motd_Info5_String;
GUIString Motd_Info6_String;
GUIString Motd_Help_String;
GUIString Motd_Back_String;
GUIString Motd_Next_String;
GUIString Motd_Skip_String;
GUIString Motd_Title_String;

GUIString VisitHost_Title_String;
GUIString VisitHost_Info1_String;
GUIString VisitHost_Info2_String;
GUIString VisitHost_Info3_String;
GUIString VisitHost_Info4_String;
GUIString VisitHost_Info5_String;
GUIString VisitHost_Info6_String;
GUIString VisitHost_Help_String;
GUIString VisitHost_Back_String;
GUIString VisitHost_Next_String;

GUIString OptionalPatch_Title_String;
GUIString OptionalPatch_Info1_String;
GUIString OptionalPatch_Info2_String;
GUIString OptionalPatch_Info3_String;
GUIString OptionalPatch_Info4_String;
GUIString OptionalPatch_Info5_String;
GUIString OptionalPatch_Info6_String;
GUIString OptionalPatch_ViewDetails_String;
GUIString OptionalPatch_Help_String;
GUIString OptionalPatch_Next_String;
GUIString OptionalPatch_BadDescExe_String;
GUIString OptionalPatch_DescTitle_String;
GUIString OptionalPatch_FetchingDesc_String;
GUIString OptionalPatch_TimedOut_String;
GUIString OptionalPatch_DownloadFailed_String;
GUIString OptionalPatch_Back_String;

GUIString PatchDetails_Title_String;
GUIString PatchDetails_Info1_String;
GUIString PatchDetails_Info2_String;
GUIString PatchDetails_Info3_String;
GUIString PatchDetails_Info4_String;
GUIString PatchDetails_Info5_String;
GUIString PatchDetails_Info6_String;
GUIString PatchDetails_Help_String;
GUIString PatchDetails_Back_String;
GUIString PatchDetails_Next_String;
GUIString PatchDetails_FileTooBig_String;
GUIString PatchDetails_FileTooBigTtl_String;

GUIString VersionCheck_Title_String;
GUIString VersionCheck_Info1_String;
GUIString VersionCheck_Info2_String;
GUIString VersionCheck_Info3_String;
GUIString VersionCheck_Info4_String;
GUIString VersionCheck_Info5_String;
GUIString VersionCheck_Info6_String;
GUIString VersionCheck_Help_String;
GUIString VersionCheck_Next_String;
GUIString VersionCheck_Back_String;
GUIString VersionCheck_OkToGoBack_String;
GUIString VersionCheck_StatusPleaseWait_String;
GUIString VersionCheck_StatusErrorFound_String;
GUIString VersionCheck_StatusFinished_String;
GUIString VersionCheck_DNSTestStarted_String;
GUIString VersionCheck_MotdLookupStarted_String;
GUIString VersionCheck_ServerLookupStarted_String;
GUIString VersionCheck_SelfVersionCheckStarted_String;
GUIString VersionCheck_GameVersionCheckStarted_String;
GUIString VersionCheck_UpToDate_String;
GUIString VersionCheck_Finish_String;
GUIString VersionCheck_CheckingForPatch_String;
GUIString VersionCheck_AbortTitle_String;
GUIString VersionCheck_Visit_String;

GUIString Login_Title_String;
GUIString Login_Info_String;
GUIString Login_Name_String;
GUIString Login_Password_String;
GUIString Login_Ok_String;

ResourceConfigTablePtr UpdateResource_Init(WONAPI::ResourceConfigTable *theTable)
{
	ResourceConfigTablePtr aTable = theTable;
	if(theTable==NULL)
		aTable = theTable = new ResourceConfigTable;

	theTable->RegisterGlueResourceFunc(UpdateResource_SafeGlueResources);

	return aTable;
}

static void *gResources[] =
{
	&Global_Yes_String,
	&Global_No_String,
	&Global_Ok_String,
	&Global_Cancel_String,
	&Global_Close_String,
	&Global_BadHelpExe_String,
	&Global_WrongSize_String,
	&Global_InvalidChecksum_String,
	&Global_InvalidPatch_String,
	&Global_Validation_String,
	&Global_Validating_String,
	&Global_DownloadFailures_String,
	&Global_DownloadAborts_String,
	&Global_NoStringRes_String,
	&Global_MenuAbout_String,
	&Global_ParamErrTitle_String,
	&Global_CfgFileTooBig_String,
	&Global_NoReadCfgFile_String,
	&Global_NoOpenCfgFile_String,
	&Global_PatcherDispName_String,
	&Global_BadHelpFile_String,
	&Global_BadResFile_String,
	&Global_UnknownParam_String,
	&Global_PrUnknown_String,
	&Global_PrUpToDate_String,
	&Global_PrPatchFound_String,
	&Global_PrPatchFetched_String,
	&Global_PrUserAbort_String,
	&Global_PrNoServerConnection_String,
	&Global_PrNoHostList_String,
	&Global_PrNoDownload_String,
	&Global_NoMonitor_String,
	&Global_PsUndetermined_String,
	&Global_NoFindPatch_String,
	&Global_NoRunPatch_String,
	&Global_PatchFailed_String,
	&Global_PrepPatcher_String,
	&Global_CheckingForThePatch_String,
	&Global_VerCheckFailed_String,
	&Global_PatchFound_String,
	&Global_NoRetrievePatch_String,
	&Global_Applying_String,
	&Global_AnotherPatch_String,
	&Global_PatchApplied_String,
	&Global_LaunchingExe_String,
	&Global_CheckingMotd_String,
	&Global_CheckingForMotd_String,
	&Global_UpToDate_String,
	&Global_PatcherInfoTitle_String,
	&Global_VersionFileCheckFailed_String,
	&Global_PatchReturnedError_String,
	&Global_CheckConnection_String,
	&Global_TimeOut_String,
	&Global_SkipMotd_String,
	&Global_Warning_String,
	&Global_IncompletePatch_String,
	&Global_SelfUpToDate_String,
	&Global_CheckingSelfForPatch_String,
	&Global_CloseForSelfPatch_String,
	&Global_SelfNoHostList_String,
	&Global_NoFindRestarter_String,
	&Global_NoRunRestarter_String,
	&Global_RestarterFailed_String,
	&Global_SierraUpdate_String,
	&Global_SelfUpdateLang_String,
	&Global_NoWritePsapi_String,
	&Global_NoWriteRestarter_String,
	&Global_Error_String,
	&Global_Host_String,
	&Global_Bytes_String,
	&Global_KiloBytes_String,
	&Global_MegaBytes_String,
	&Global_PrOptionalPatch_String,
	&Global_PrManualDownload_String,
	&Global_OptionalPatchFound_String,
	&Global_PossiblePatch_String,
	&Global_CheckForPossiblePatch_String,
	&Global_NoPatchUrl_String,
	&Global_LocalPatchDoesNotMatch_String,
	&Global_CheckConection_String,
	&Global_CheckConectionTitle_String,
	&Global_Success_String,
	&Global_MotdSkipped_String,
	&Global_MotdNotFound_String,
	&Global_GameMotdNotFound_String,
	&Global_MotdError_String,
	&Global_NoNewMotd_String,
	&Global_PatchRequired_String,
	&Global_OptionalPatch_String,
	&Global_PartialDirServerFailure_String,
	&Global_DirServerFailure_String,
	&Global_BadVersionNoUpdate_String,
	&Global_SierraUpBadVersionNoUpdate_String,
	&Global_PatchServerError_String,
	&Global_UpdateServerTimedOut_String,
	&Global_NoConfigFile_String,
	&Global_GameLogo_Image,
	&Global_MpsLogo_Image,
	&Global_HOST_AD_WIDTH_Int,
	&Global_HOST_AD_HEIGHT_Int,

	&SelectHost_Title_String,
	&SelectHost_Info1_String,
	&SelectHost_Info2_String,
	&SelectHost_Info3_String,
	&SelectHost_Info4_String,
	&SelectHost_Info5_String,
	&SelectHost_Info6_String,
	&SelectHost_ConfigProxy_String,
	&SelectHost_Help_String,
	&SelectHost_Next_String,
	&SelectHost_HavePatch_String,
	&SelectHost_PatchFilter_String,
	&SelectHost_InfoManual_String,
	&SelectHost_Back_String,
	&SelectHost_FileOpenDlgTitle_String,
	&SelectHost_HostTitle_String,
	&SelectHost_Results_String,
	&SelectHost_WaitInfo1_String,
	&SelectHost_WaitInfo2_String,
	&SelectHost_WaitInfo3_String,
	&SelectHost_WaitInfo4_String,
	&SelectHost_WaitInfo5_String,
	&SelectHost_WaitInfo6_String,

	&ConfigProxy_Title_String,
	&ConfigProxy_Info1_String,
	&ConfigProxy_Info2_String,
	&ConfigProxy_Info3_String,
	&ConfigProxy_Info4_String,
	&ConfigProxy_Info5_String,
	&ConfigProxy_Info6_String,
	&ConfigProxy_UseProxy_String,
	&ConfigProxy_Host_String,
	&ConfigProxy_Port_String,
	&ConfigProxy_Help_String,

	&Download_Title_String,
	&Download_Info1_String,
	&Download_Info2_String,
	&Download_Info3_String,
	&Download_Info4_String,
	&Download_Info5_String,
	&Download_Info6_String,
	&Download_Host_String,
	&Download_Progress_String,
	&Download_Preparing_String,
	&Download_Estimating_String,
	&Download_Help_String,
	&Download_Back_String,
	&Download_VisitHost_String,
	&Download_HostedBy_String,
	&Download_BytesTransferred_String,
	&Download_ConnectionRefused_String,
	&Download_InvalidHttpHeader_String,
	&Download_InvalidHttpRedirect_String,
	&Download_TooManyHttpRedirects_String,
	&Download_DownloadError_String,
	&Download_UnknownHttpError_String,
	&Download_HttpError_String,
	&Download_Http404Error_String,
	&Download_Confirm_String,
	&Download_AbandonDownload_String,
	&Download_AbortPatch_String,
	&Download_UnknownFtpError_String,
	&Download_FtpError_String,
	&Download_InvalidFtpResp_String,
	&Download_InvalidFtpPasvResp_String,
	&Download_NoData_String,
	&Download_InvalidFtpFile_String,
	&Download_InvalidFtpUser_String,
	&Download_InvalidFtpPassword_String,
	&Download_TimeHoursMinutes_String,
	&Download_TimeHourMinutes_String,
	&Download_TimeMinutesSeconds_String,
	&Download_TimeMinuteSeconds_String,
	&Download_TimeSeconds_String,
	&Download_Http421Error_String,
	&Download_Next_String,
	&Download_UnableToFindHost_String,
	&Download_NoHostUrl_String,

	&Dialog_AbortDlgAbort_String,
	&Dialog_BaseBackground_Background,

	&Welcome_Continue_String,
	&Welcome_Cancel_String,
	&Welcome_ConfigProxy_String,
	&Welcome_Help_String,
	&Welcome_Info1_String,
	&Welcome_Info2_String,
	&Welcome_Info3_String,
	&Welcome_Info4_String,
	&Welcome_Info5_String,
	&Welcome_Info6_String,
	&Welcome_Title_String,

	&About_Version_String,
	&About_Copyright_String,
	&About_Title_String,

	&Motd_SystemTitle_String,
	&Motd_ProductTitle_String,
	&Motd_Info1_String,
	&Motd_Info2_String,
	&Motd_Info3_String,
	&Motd_Info4_String,
	&Motd_Info5_String,
	&Motd_Info6_String,
	&Motd_Help_String,
	&Motd_Back_String,
	&Motd_Next_String,
	&Motd_Skip_String,
	&Motd_Title_String,

	&VisitHost_Title_String,
	&VisitHost_Info1_String,
	&VisitHost_Info2_String,
	&VisitHost_Info3_String,
	&VisitHost_Info4_String,
	&VisitHost_Info5_String,
	&VisitHost_Info6_String,
	&VisitHost_Help_String,
	&VisitHost_Back_String,
	&VisitHost_Next_String,

	&OptionalPatch_Title_String,
	&OptionalPatch_Info1_String,
	&OptionalPatch_Info2_String,
	&OptionalPatch_Info3_String,
	&OptionalPatch_Info4_String,
	&OptionalPatch_Info5_String,
	&OptionalPatch_Info6_String,
	&OptionalPatch_ViewDetails_String,
	&OptionalPatch_Help_String,
	&OptionalPatch_Next_String,
	&OptionalPatch_BadDescExe_String,
	&OptionalPatch_DescTitle_String,
	&OptionalPatch_FetchingDesc_String,
	&OptionalPatch_TimedOut_String,
	&OptionalPatch_DownloadFailed_String,
	&OptionalPatch_Back_String,

	&PatchDetails_Title_String,
	&PatchDetails_Info1_String,
	&PatchDetails_Info2_String,
	&PatchDetails_Info3_String,
	&PatchDetails_Info4_String,
	&PatchDetails_Info5_String,
	&PatchDetails_Info6_String,
	&PatchDetails_Help_String,
	&PatchDetails_Back_String,
	&PatchDetails_Next_String,
	&PatchDetails_FileTooBig_String,
	&PatchDetails_FileTooBigTtl_String,

	&VersionCheck_Title_String,
	&VersionCheck_Info1_String,
	&VersionCheck_Info2_String,
	&VersionCheck_Info3_String,
	&VersionCheck_Info4_String,
	&VersionCheck_Info5_String,
	&VersionCheck_Info6_String,
	&VersionCheck_Help_String,
	&VersionCheck_Next_String,
	&VersionCheck_Back_String,
	&VersionCheck_OkToGoBack_String,
	&VersionCheck_StatusPleaseWait_String,
	&VersionCheck_StatusErrorFound_String,
	&VersionCheck_StatusFinished_String,
	&VersionCheck_DNSTestStarted_String,
	&VersionCheck_MotdLookupStarted_String,
	&VersionCheck_ServerLookupStarted_String,
	&VersionCheck_SelfVersionCheckStarted_String,
	&VersionCheck_GameVersionCheckStarted_String,
	&VersionCheck_UpToDate_String,
	&VersionCheck_Finish_String,
	&VersionCheck_CheckingForPatch_String,
	&VersionCheck_AbortTitle_String,
	&VersionCheck_Visit_String,

	&Login_Title_String,
	&Login_Info_String,
	&Login_Name_String,
	&Login_Password_String,
	&Login_Ok_String,


	NULL
};
const GUIString& UpdateResource_GetString(UpdateResourceId theId) { return *(GUIString*)gResources[theId]; }
Image* UpdateResource_GetImage(UpdateResourceId theId) { return *(ImagePtr*)gResources[theId]; }
Sound* UpdateResource_GetSound(UpdateResourceId theId) { return *(SoundPtr*)gResources[theId]; }
Font* UpdateResource_GetFont(UpdateResourceId theId) { return *(FontPtr*)gResources[theId]; }
const Background& UpdateResource_GetBackground(UpdateResourceId theId) { return *(Background*)gResources[theId]; }
DWORD UpdateResource_GetColor(UpdateResourceId theId) { return *(DWORD*)gResources[theId]; }
int UpdateResource_GetInt(UpdateResourceId theId) { return *(int*)gResources[theId]; }
void UpdateResource_SafeGlueResources(ResourceConfigTable *theTable)
{
	ResourceConfig *aConfig;
	aConfig=theTable->SafeGetConfig("Global");
	Global_Yes_String = aConfig->SafeGetString("Yes");
	Global_No_String = aConfig->SafeGetString("No");
	Global_Ok_String = aConfig->SafeGetString("Ok");
	Global_Cancel_String = aConfig->SafeGetString("Cancel");
	Global_Close_String = aConfig->SafeGetString("Close");
	Global_BadHelpExe_String = aConfig->SafeGetString("BadHelpExe");
	Global_WrongSize_String = aConfig->SafeGetString("WrongSize");
	Global_InvalidChecksum_String = aConfig->SafeGetString("InvalidChecksum");
	Global_InvalidPatch_String = aConfig->SafeGetString("InvalidPatch");
	Global_Validation_String = aConfig->SafeGetString("Validation");
	Global_Validating_String = aConfig->SafeGetString("Validating");
	Global_DownloadFailures_String = aConfig->SafeGetString("DownloadFailures");
	Global_DownloadAborts_String = aConfig->SafeGetString("DownloadAborts");
	Global_NoStringRes_String = aConfig->SafeGetString("NoStringRes");
	Global_MenuAbout_String = aConfig->SafeGetString("MenuAbout");
	Global_ParamErrTitle_String = aConfig->SafeGetString("ParamErrTitle");
	Global_CfgFileTooBig_String = aConfig->SafeGetString("CfgFileTooBig");
	Global_NoReadCfgFile_String = aConfig->SafeGetString("NoReadCfgFile");
	Global_NoOpenCfgFile_String = aConfig->SafeGetString("NoOpenCfgFile");
	Global_PatcherDispName_String = aConfig->SafeGetString("PatcherDispName");
	Global_BadHelpFile_String = aConfig->SafeGetString("BadHelpFile");
	Global_BadResFile_String = aConfig->SafeGetString("BadResFile");
	Global_UnknownParam_String = aConfig->SafeGetString("UnknownParam");
	Global_PrUnknown_String = aConfig->SafeGetString("PrUnknown");
	Global_PrUpToDate_String = aConfig->SafeGetString("PrUpToDate");
	Global_PrPatchFound_String = aConfig->SafeGetString("PrPatchFound");
	Global_PrPatchFetched_String = aConfig->SafeGetString("PrPatchFetched");
	Global_PrUserAbort_String = aConfig->SafeGetString("PrUserAbort");
	Global_PrNoServerConnection_String = aConfig->SafeGetString("PrNoServerConnection");
	Global_PrNoHostList_String = aConfig->SafeGetString("PrNoHostList");
	Global_PrNoDownload_String = aConfig->SafeGetString("PrNoDownload");
	Global_NoMonitor_String = aConfig->SafeGetString("NoMonitor");
	Global_PsUndetermined_String = aConfig->SafeGetString("PsUndetermined");
	Global_NoFindPatch_String = aConfig->SafeGetString("NoFindPatch");
	Global_NoRunPatch_String = aConfig->SafeGetString("NoRunPatch");
	Global_PatchFailed_String = aConfig->SafeGetString("PatchFailed");
	Global_PrepPatcher_String = aConfig->SafeGetString("PrepPatcher");
	Global_CheckingForThePatch_String = aConfig->SafeGetString("CheckingForThePatch");
	Global_VerCheckFailed_String = aConfig->SafeGetString("VerCheckFailed");
	Global_PatchFound_String = aConfig->SafeGetString("PatchFound");
	Global_NoRetrievePatch_String = aConfig->SafeGetString("NoRetrievePatch");
	Global_Applying_String = aConfig->SafeGetString("Applying");
	Global_AnotherPatch_String = aConfig->SafeGetString("AnotherPatch");
	Global_PatchApplied_String = aConfig->SafeGetString("PatchApplied");
	Global_LaunchingExe_String = aConfig->SafeGetString("LaunchingExe");
	Global_CheckingMotd_String = aConfig->SafeGetString("CheckingMotd");
	Global_CheckingForMotd_String = aConfig->SafeGetString("CheckingForMotd");
	Global_UpToDate_String = aConfig->SafeGetString("UpToDate");
	Global_PatcherInfoTitle_String = aConfig->SafeGetString("PatcherInfoTitle");
	Global_VersionFileCheckFailed_String = aConfig->SafeGetString("VersionFileCheckFailed");
	Global_PatchReturnedError_String = aConfig->SafeGetString("PatchReturnedError");
	Global_CheckConnection_String = aConfig->SafeGetString("CheckConnection");
	Global_TimeOut_String = aConfig->SafeGetString("TimeOut");
	Global_SkipMotd_String = aConfig->SafeGetString("SkipMotd");
	Global_Warning_String = aConfig->SafeGetString("Warning");
	Global_IncompletePatch_String = aConfig->SafeGetString("IncompletePatch");
	Global_SelfUpToDate_String = aConfig->SafeGetString("SelfUpToDate");
	Global_CheckingSelfForPatch_String = aConfig->SafeGetString("CheckingSelfForPatch");
	Global_CloseForSelfPatch_String = aConfig->SafeGetString("CloseForSelfPatch");
	Global_SelfNoHostList_String = aConfig->SafeGetString("SelfNoHostList");
	Global_NoFindRestarter_String = aConfig->SafeGetString("NoFindRestarter");
	Global_NoRunRestarter_String = aConfig->SafeGetString("NoRunRestarter");
	Global_RestarterFailed_String = aConfig->SafeGetString("RestarterFailed");
	Global_SierraUpdate_String = aConfig->SafeGetString("SierraUpdate");
	Global_SelfUpdateLang_String = aConfig->SafeGetString("SelfUpdateLang");
	Global_NoWritePsapi_String = aConfig->SafeGetString("NoWritePsapi");
	Global_NoWriteRestarter_String = aConfig->SafeGetString("NoWriteRestarter");
	Global_Error_String = aConfig->SafeGetString("Error");
	Global_Host_String = aConfig->SafeGetString("Host");
	Global_Bytes_String = aConfig->SafeGetString("Bytes");
	Global_KiloBytes_String = aConfig->SafeGetString("KiloBytes");
	Global_MegaBytes_String = aConfig->SafeGetString("MegaBytes");
	Global_PrOptionalPatch_String = aConfig->SafeGetString("PrOptionalPatch");
	Global_PrManualDownload_String = aConfig->SafeGetString("PrManualDownload");
	Global_OptionalPatchFound_String = aConfig->SafeGetString("OptionalPatchFound");
	Global_PossiblePatch_String = aConfig->SafeGetString("PossiblePatch");
	Global_CheckForPossiblePatch_String = aConfig->SafeGetString("CheckForPossiblePatch");
	Global_NoPatchUrl_String = aConfig->SafeGetString("NoPatchUrl");
	Global_LocalPatchDoesNotMatch_String = aConfig->SafeGetString("LocalPatchDoesNotMatch");
	Global_CheckConection_String = aConfig->SafeGetString("CheckConection");
	Global_CheckConectionTitle_String = aConfig->SafeGetString("CheckConectionTitle");
	Global_Success_String = aConfig->SafeGetString("Success");
	Global_MotdSkipped_String = aConfig->SafeGetString("MotdSkipped");
	Global_MotdNotFound_String = aConfig->SafeGetString("MotdNotFound");
	Global_GameMotdNotFound_String = aConfig->SafeGetString("GameMotdNotFound");
	Global_MotdError_String = aConfig->SafeGetString("MotdError");
	Global_NoNewMotd_String = aConfig->SafeGetString("NoNewMotd");
	Global_PatchRequired_String = aConfig->SafeGetString("PatchRequired");
	Global_OptionalPatch_String = aConfig->SafeGetString("OptionalPatch");
	Global_PartialDirServerFailure_String = aConfig->SafeGetString("PartialDirServerFailure");
	Global_DirServerFailure_String = aConfig->SafeGetString("DirServerFailure");
	Global_BadVersionNoUpdate_String = aConfig->SafeGetString("BadVersionNoUpdate");
	Global_SierraUpBadVersionNoUpdate_String = aConfig->SafeGetString("SierraUpBadVersionNoUpdate");
	Global_PatchServerError_String = aConfig->SafeGetString("PatchServerError");
	Global_UpdateServerTimedOut_String = aConfig->SafeGetString("UpdateServerTimedOut");
	Global_NoConfigFile_String = aConfig->SafeGetString("NoConfigFile");
	Global_GameLogo_Image = aConfig->SafeGetImage("GameLogo");
	Global_MpsLogo_Image = aConfig->SafeGetImage("MpsLogo");
	Global_HOST_AD_WIDTH_Int = aConfig->SafeGetInt("HOST_AD_WIDTH");
	Global_HOST_AD_HEIGHT_Int = aConfig->SafeGetInt("HOST_AD_HEIGHT");

	aConfig=theTable->SafeGetConfig("SelectHost");
	SelectHost_Title_String = aConfig->SafeGetString("Title");
	SelectHost_Info1_String = aConfig->SafeGetString("Info1");
	SelectHost_Info2_String = aConfig->SafeGetString("Info2");
	SelectHost_Info3_String = aConfig->SafeGetString("Info3");
	SelectHost_Info4_String = aConfig->SafeGetString("Info4");
	SelectHost_Info5_String = aConfig->SafeGetString("Info5");
	SelectHost_Info6_String = aConfig->SafeGetString("Info6");
	SelectHost_ConfigProxy_String = aConfig->SafeGetString("ConfigProxy");
	SelectHost_Help_String = aConfig->SafeGetString("Help");
	SelectHost_Next_String = aConfig->SafeGetString("Next");
	SelectHost_HavePatch_String = aConfig->SafeGetString("HavePatch");
	SelectHost_PatchFilter_String = aConfig->SafeGetString("PatchFilter");
	SelectHost_InfoManual_String = aConfig->SafeGetString("InfoManual");
	SelectHost_Back_String = aConfig->SafeGetString("Back");
	SelectHost_FileOpenDlgTitle_String = aConfig->SafeGetString("FileOpenDlgTitle");
	SelectHost_HostTitle_String = aConfig->SafeGetString("HostTitle");
	SelectHost_Results_String = aConfig->SafeGetString("Results");
	SelectHost_WaitInfo1_String = aConfig->SafeGetString("WaitInfo1");
	SelectHost_WaitInfo2_String = aConfig->SafeGetString("WaitInfo2");
	SelectHost_WaitInfo3_String = aConfig->SafeGetString("WaitInfo3");
	SelectHost_WaitInfo4_String = aConfig->SafeGetString("WaitInfo4");
	SelectHost_WaitInfo5_String = aConfig->SafeGetString("WaitInfo5");
	SelectHost_WaitInfo6_String = aConfig->SafeGetString("WaitInfo6");

	aConfig=theTable->SafeGetConfig("ConfigProxy");
	ConfigProxy_Title_String = aConfig->SafeGetString("Title");
	ConfigProxy_Info1_String = aConfig->SafeGetString("Info1");
	ConfigProxy_Info2_String = aConfig->SafeGetString("Info2");
	ConfigProxy_Info3_String = aConfig->SafeGetString("Info3");
	ConfigProxy_Info4_String = aConfig->SafeGetString("Info4");
	ConfigProxy_Info5_String = aConfig->SafeGetString("Info5");
	ConfigProxy_Info6_String = aConfig->SafeGetString("Info6");
	ConfigProxy_UseProxy_String = aConfig->SafeGetString("UseProxy");
	ConfigProxy_Host_String = aConfig->SafeGetString("Host");
	ConfigProxy_Port_String = aConfig->SafeGetString("Port");
	ConfigProxy_Help_String = aConfig->SafeGetString("Help");

	aConfig=theTable->SafeGetConfig("Download");
	Download_Title_String = aConfig->SafeGetString("Title");
	Download_Info1_String = aConfig->SafeGetString("Info1");
	Download_Info2_String = aConfig->SafeGetString("Info2");
	Download_Info3_String = aConfig->SafeGetString("Info3");
	Download_Info4_String = aConfig->SafeGetString("Info4");
	Download_Info5_String = aConfig->SafeGetString("Info5");
	Download_Info6_String = aConfig->SafeGetString("Info6");
	Download_Host_String = aConfig->SafeGetString("Host");
	Download_Progress_String = aConfig->SafeGetString("Progress");
	Download_Preparing_String = aConfig->SafeGetString("Preparing");
	Download_Estimating_String = aConfig->SafeGetString("Estimating");
	Download_Help_String = aConfig->SafeGetString("Help");
	Download_Back_String = aConfig->SafeGetString("Back");
	Download_VisitHost_String = aConfig->SafeGetString("VisitHost");
	Download_HostedBy_String = aConfig->SafeGetString("HostedBy");
	Download_BytesTransferred_String = aConfig->SafeGetString("BytesTransferred");
	Download_ConnectionRefused_String = aConfig->SafeGetString("ConnectionRefused");
	Download_InvalidHttpHeader_String = aConfig->SafeGetString("InvalidHttpHeader");
	Download_InvalidHttpRedirect_String = aConfig->SafeGetString("InvalidHttpRedirect");
	Download_TooManyHttpRedirects_String = aConfig->SafeGetString("TooManyHttpRedirects");
	Download_DownloadError_String = aConfig->SafeGetString("DownloadError");
	Download_UnknownHttpError_String = aConfig->SafeGetString("UnknownHttpError");
	Download_HttpError_String = aConfig->SafeGetString("HttpError");
	Download_Http404Error_String = aConfig->SafeGetString("Http404Error");
	Download_Confirm_String = aConfig->SafeGetString("Confirm");
	Download_AbandonDownload_String = aConfig->SafeGetString("AbandonDownload");
	Download_AbortPatch_String = aConfig->SafeGetString("AbortPatch");
	Download_UnknownFtpError_String = aConfig->SafeGetString("UnknownFtpError");
	Download_FtpError_String = aConfig->SafeGetString("FtpError");
	Download_InvalidFtpResp_String = aConfig->SafeGetString("InvalidFtpResp");
	Download_InvalidFtpPasvResp_String = aConfig->SafeGetString("InvalidFtpPasvResp");
	Download_NoData_String = aConfig->SafeGetString("NoData");
	Download_InvalidFtpFile_String = aConfig->SafeGetString("InvalidFtpFile");
	Download_InvalidFtpUser_String = aConfig->SafeGetString("InvalidFtpUser");
	Download_InvalidFtpPassword_String = aConfig->SafeGetString("InvalidFtpPassword");
	Download_TimeHoursMinutes_String = aConfig->SafeGetString("TimeHoursMinutes");
	Download_TimeHourMinutes_String = aConfig->SafeGetString("TimeHourMinutes");
	Download_TimeMinutesSeconds_String = aConfig->SafeGetString("TimeMinutesSeconds");
	Download_TimeMinuteSeconds_String = aConfig->SafeGetString("TimeMinuteSeconds");
	Download_TimeSeconds_String = aConfig->SafeGetString("TimeSeconds");
	Download_Http421Error_String = aConfig->SafeGetString("Http421Error");
	Download_Next_String = aConfig->SafeGetString("Next");
	Download_UnableToFindHost_String = aConfig->SafeGetString("UnableToFindHost");
	Download_NoHostUrl_String = aConfig->SafeGetString("NoHostUrl");

	aConfig=theTable->SafeGetConfig("Dialog");
	Dialog_AbortDlgAbort_String = aConfig->SafeGetString("AbortDlgAbort");
	Dialog_BaseBackground_Background = aConfig->SafeGetBackground("BaseBackground");

	aConfig=theTable->SafeGetConfig("Welcome");
	Welcome_Continue_String = aConfig->SafeGetString("Continue");
	Welcome_Cancel_String = aConfig->SafeGetString("Cancel");
	Welcome_ConfigProxy_String = aConfig->SafeGetString("ConfigProxy");
	Welcome_Help_String = aConfig->SafeGetString("Help");
	Welcome_Info1_String = aConfig->SafeGetString("Info1");
	Welcome_Info2_String = aConfig->SafeGetString("Info2");
	Welcome_Info3_String = aConfig->SafeGetString("Info3");
	Welcome_Info4_String = aConfig->SafeGetString("Info4");
	Welcome_Info5_String = aConfig->SafeGetString("Info5");
	Welcome_Info6_String = aConfig->SafeGetString("Info6");
	Welcome_Title_String = aConfig->SafeGetString("Title");

	aConfig=theTable->SafeGetConfig("About");
	About_Version_String = aConfig->SafeGetString("Version");
	About_Copyright_String = aConfig->SafeGetString("Copyright");
	About_Title_String = aConfig->SafeGetString("Title");

	aConfig=theTable->SafeGetConfig("Motd");
	Motd_SystemTitle_String = aConfig->SafeGetString("SystemTitle");
	Motd_ProductTitle_String = aConfig->SafeGetString("ProductTitle");
	Motd_Info1_String = aConfig->SafeGetString("Info1");
	Motd_Info2_String = aConfig->SafeGetString("Info2");
	Motd_Info3_String = aConfig->SafeGetString("Info3");
	Motd_Info4_String = aConfig->SafeGetString("Info4");
	Motd_Info5_String = aConfig->SafeGetString("Info5");
	Motd_Info6_String = aConfig->SafeGetString("Info6");
	Motd_Help_String = aConfig->SafeGetString("Help");
	Motd_Back_String = aConfig->SafeGetString("Back");
	Motd_Next_String = aConfig->SafeGetString("Next");
	Motd_Skip_String = aConfig->SafeGetString("Skip");
	Motd_Title_String = aConfig->SafeGetString("Title");

	aConfig=theTable->SafeGetConfig("VisitHost");
	VisitHost_Title_String = aConfig->SafeGetString("Title");
	VisitHost_Info1_String = aConfig->SafeGetString("Info1");
	VisitHost_Info2_String = aConfig->SafeGetString("Info2");
	VisitHost_Info3_String = aConfig->SafeGetString("Info3");
	VisitHost_Info4_String = aConfig->SafeGetString("Info4");
	VisitHost_Info5_String = aConfig->SafeGetString("Info5");
	VisitHost_Info6_String = aConfig->SafeGetString("Info6");
	VisitHost_Help_String = aConfig->SafeGetString("Help");
	VisitHost_Back_String = aConfig->SafeGetString("Back");
	VisitHost_Next_String = aConfig->SafeGetString("Next");

	aConfig=theTable->SafeGetConfig("OptionalPatch");
	OptionalPatch_Title_String = aConfig->SafeGetString("Title");
	OptionalPatch_Info1_String = aConfig->SafeGetString("Info1");
	OptionalPatch_Info2_String = aConfig->SafeGetString("Info2");
	OptionalPatch_Info3_String = aConfig->SafeGetString("Info3");
	OptionalPatch_Info4_String = aConfig->SafeGetString("Info4");
	OptionalPatch_Info5_String = aConfig->SafeGetString("Info5");
	OptionalPatch_Info6_String = aConfig->SafeGetString("Info6");
	OptionalPatch_ViewDetails_String = aConfig->SafeGetString("ViewDetails");
	OptionalPatch_Help_String = aConfig->SafeGetString("Help");
	OptionalPatch_Next_String = aConfig->SafeGetString("Next");
	OptionalPatch_BadDescExe_String = aConfig->SafeGetString("BadDescExe");
	OptionalPatch_DescTitle_String = aConfig->SafeGetString("DescTitle");
	OptionalPatch_FetchingDesc_String = aConfig->SafeGetString("FetchingDesc");
	OptionalPatch_TimedOut_String = aConfig->SafeGetString("TimedOut");
	OptionalPatch_DownloadFailed_String = aConfig->SafeGetString("DownloadFailed");
	OptionalPatch_Back_String = aConfig->SafeGetString("Back");

	aConfig=theTable->SafeGetConfig("PatchDetails");
	PatchDetails_Title_String = aConfig->SafeGetString("Title");
	PatchDetails_Info1_String = aConfig->SafeGetString("Info1");
	PatchDetails_Info2_String = aConfig->SafeGetString("Info2");
	PatchDetails_Info3_String = aConfig->SafeGetString("Info3");
	PatchDetails_Info4_String = aConfig->SafeGetString("Info4");
	PatchDetails_Info5_String = aConfig->SafeGetString("Info5");
	PatchDetails_Info6_String = aConfig->SafeGetString("Info6");
	PatchDetails_Help_String = aConfig->SafeGetString("Help");
	PatchDetails_Back_String = aConfig->SafeGetString("Back");
	PatchDetails_Next_String = aConfig->SafeGetString("Next");
	PatchDetails_FileTooBig_String = aConfig->SafeGetString("FileTooBig");
	PatchDetails_FileTooBigTtl_String = aConfig->SafeGetString("FileTooBigTtl");

	aConfig=theTable->SafeGetConfig("VersionCheck");
	VersionCheck_Title_String = aConfig->SafeGetString("Title");
	VersionCheck_Info1_String = aConfig->SafeGetString("Info1");
	VersionCheck_Info2_String = aConfig->SafeGetString("Info2");
	VersionCheck_Info3_String = aConfig->SafeGetString("Info3");
	VersionCheck_Info4_String = aConfig->SafeGetString("Info4");
	VersionCheck_Info5_String = aConfig->SafeGetString("Info5");
	VersionCheck_Info6_String = aConfig->SafeGetString("Info6");
	VersionCheck_Help_String = aConfig->SafeGetString("Help");
	VersionCheck_Next_String = aConfig->SafeGetString("Next");
	VersionCheck_Back_String = aConfig->SafeGetString("Back");
	VersionCheck_OkToGoBack_String = aConfig->SafeGetString("OkToGoBack");
	VersionCheck_StatusPleaseWait_String = aConfig->SafeGetString("StatusPleaseWait");
	VersionCheck_StatusErrorFound_String = aConfig->SafeGetString("StatusErrorFound");
	VersionCheck_StatusFinished_String = aConfig->SafeGetString("StatusFinished");
	VersionCheck_DNSTestStarted_String = aConfig->SafeGetString("DNSTestStarted");
	VersionCheck_MotdLookupStarted_String = aConfig->SafeGetString("MotdLookupStarted");
	VersionCheck_ServerLookupStarted_String = aConfig->SafeGetString("ServerLookupStarted");
	VersionCheck_SelfVersionCheckStarted_String = aConfig->SafeGetString("SelfVersionCheckStarted");
	VersionCheck_GameVersionCheckStarted_String = aConfig->SafeGetString("GameVersionCheckStarted");
	VersionCheck_UpToDate_String = aConfig->SafeGetString("UpToDate");
	VersionCheck_Finish_String = aConfig->SafeGetString("Finish");
	VersionCheck_CheckingForPatch_String = aConfig->SafeGetString("CheckingForPatch");
	VersionCheck_AbortTitle_String = aConfig->SafeGetString("AbortTitle");
	VersionCheck_Visit_String = aConfig->SafeGetString("Visit");

	aConfig=theTable->SafeGetConfig("Login");
	Login_Title_String = aConfig->SafeGetString("Title");
	Login_Info_String = aConfig->SafeGetString("Info");
	Login_Name_String = aConfig->SafeGetString("Name");
	Login_Password_String = aConfig->SafeGetString("Password");
	Login_Ok_String = aConfig->SafeGetString("Ok");

}

void UpdateResource_UnloadResources()
{
	Global_Yes_String.erase();
	Global_No_String.erase();
	Global_Ok_String.erase();
	Global_Cancel_String.erase();
	Global_Close_String.erase();
	Global_BadHelpExe_String.erase();
	Global_WrongSize_String.erase();
	Global_InvalidChecksum_String.erase();
	Global_InvalidPatch_String.erase();
	Global_Validation_String.erase();
	Global_Validating_String.erase();
	Global_DownloadFailures_String.erase();
	Global_DownloadAborts_String.erase();
	Global_NoStringRes_String.erase();
	Global_MenuAbout_String.erase();
	Global_ParamErrTitle_String.erase();
	Global_CfgFileTooBig_String.erase();
	Global_NoReadCfgFile_String.erase();
	Global_NoOpenCfgFile_String.erase();
	Global_PatcherDispName_String.erase();
	Global_BadHelpFile_String.erase();
	Global_BadResFile_String.erase();
	Global_UnknownParam_String.erase();
	Global_PrUnknown_String.erase();
	Global_PrUpToDate_String.erase();
	Global_PrPatchFound_String.erase();
	Global_PrPatchFetched_String.erase();
	Global_PrUserAbort_String.erase();
	Global_PrNoServerConnection_String.erase();
	Global_PrNoHostList_String.erase();
	Global_PrNoDownload_String.erase();
	Global_NoMonitor_String.erase();
	Global_PsUndetermined_String.erase();
	Global_NoFindPatch_String.erase();
	Global_NoRunPatch_String.erase();
	Global_PatchFailed_String.erase();
	Global_PrepPatcher_String.erase();
	Global_CheckingForThePatch_String.erase();
	Global_VerCheckFailed_String.erase();
	Global_PatchFound_String.erase();
	Global_NoRetrievePatch_String.erase();
	Global_Applying_String.erase();
	Global_AnotherPatch_String.erase();
	Global_PatchApplied_String.erase();
	Global_LaunchingExe_String.erase();
	Global_CheckingMotd_String.erase();
	Global_CheckingForMotd_String.erase();
	Global_UpToDate_String.erase();
	Global_PatcherInfoTitle_String.erase();
	Global_VersionFileCheckFailed_String.erase();
	Global_PatchReturnedError_String.erase();
	Global_CheckConnection_String.erase();
	Global_TimeOut_String.erase();
	Global_SkipMotd_String.erase();
	Global_Warning_String.erase();
	Global_IncompletePatch_String.erase();
	Global_SelfUpToDate_String.erase();
	Global_CheckingSelfForPatch_String.erase();
	Global_CloseForSelfPatch_String.erase();
	Global_SelfNoHostList_String.erase();
	Global_NoFindRestarter_String.erase();
	Global_NoRunRestarter_String.erase();
	Global_RestarterFailed_String.erase();
	Global_SierraUpdate_String.erase();
	Global_SelfUpdateLang_String.erase();
	Global_NoWritePsapi_String.erase();
	Global_NoWriteRestarter_String.erase();
	Global_Error_String.erase();
	Global_Host_String.erase();
	Global_Bytes_String.erase();
	Global_KiloBytes_String.erase();
	Global_MegaBytes_String.erase();
	Global_PrOptionalPatch_String.erase();
	Global_PrManualDownload_String.erase();
	Global_OptionalPatchFound_String.erase();
	Global_PossiblePatch_String.erase();
	Global_CheckForPossiblePatch_String.erase();
	Global_NoPatchUrl_String.erase();
	Global_LocalPatchDoesNotMatch_String.erase();
	Global_CheckConection_String.erase();
	Global_CheckConectionTitle_String.erase();
	Global_Success_String.erase();
	Global_MotdSkipped_String.erase();
	Global_MotdNotFound_String.erase();
	Global_GameMotdNotFound_String.erase();
	Global_MotdError_String.erase();
	Global_NoNewMotd_String.erase();
	Global_PatchRequired_String.erase();
	Global_OptionalPatch_String.erase();
	Global_PartialDirServerFailure_String.erase();
	Global_DirServerFailure_String.erase();
	Global_BadVersionNoUpdate_String.erase();
	Global_SierraUpBadVersionNoUpdate_String.erase();
	Global_PatchServerError_String.erase();
	Global_UpdateServerTimedOut_String.erase();
	Global_NoConfigFile_String.erase();
	Global_GameLogo_Image = NULL;
	Global_MpsLogo_Image = NULL;
	SelectHost_Title_String.erase();
	SelectHost_Info1_String.erase();
	SelectHost_Info2_String.erase();
	SelectHost_Info3_String.erase();
	SelectHost_Info4_String.erase();
	SelectHost_Info5_String.erase();
	SelectHost_Info6_String.erase();
	SelectHost_ConfigProxy_String.erase();
	SelectHost_Help_String.erase();
	SelectHost_Next_String.erase();
	SelectHost_HavePatch_String.erase();
	SelectHost_PatchFilter_String.erase();
	SelectHost_InfoManual_String.erase();
	SelectHost_Back_String.erase();
	SelectHost_FileOpenDlgTitle_String.erase();
	SelectHost_HostTitle_String.erase();
	SelectHost_Results_String.erase();
	SelectHost_WaitInfo1_String.erase();
	SelectHost_WaitInfo2_String.erase();
	SelectHost_WaitInfo3_String.erase();
	SelectHost_WaitInfo4_String.erase();
	SelectHost_WaitInfo5_String.erase();
	SelectHost_WaitInfo6_String.erase();
	ConfigProxy_Title_String.erase();
	ConfigProxy_Info1_String.erase();
	ConfigProxy_Info2_String.erase();
	ConfigProxy_Info3_String.erase();
	ConfigProxy_Info4_String.erase();
	ConfigProxy_Info5_String.erase();
	ConfigProxy_Info6_String.erase();
	ConfigProxy_UseProxy_String.erase();
	ConfigProxy_Host_String.erase();
	ConfigProxy_Port_String.erase();
	ConfigProxy_Help_String.erase();
	Download_Title_String.erase();
	Download_Info1_String.erase();
	Download_Info2_String.erase();
	Download_Info3_String.erase();
	Download_Info4_String.erase();
	Download_Info5_String.erase();
	Download_Info6_String.erase();
	Download_Host_String.erase();
	Download_Progress_String.erase();
	Download_Preparing_String.erase();
	Download_Estimating_String.erase();
	Download_Help_String.erase();
	Download_Back_String.erase();
	Download_VisitHost_String.erase();
	Download_HostedBy_String.erase();
	Download_BytesTransferred_String.erase();
	Download_ConnectionRefused_String.erase();
	Download_InvalidHttpHeader_String.erase();
	Download_InvalidHttpRedirect_String.erase();
	Download_TooManyHttpRedirects_String.erase();
	Download_DownloadError_String.erase();
	Download_UnknownHttpError_String.erase();
	Download_HttpError_String.erase();
	Download_Http404Error_String.erase();
	Download_Confirm_String.erase();
	Download_AbandonDownload_String.erase();
	Download_AbortPatch_String.erase();
	Download_UnknownFtpError_String.erase();
	Download_FtpError_String.erase();
	Download_InvalidFtpResp_String.erase();
	Download_InvalidFtpPasvResp_String.erase();
	Download_NoData_String.erase();
	Download_InvalidFtpFile_String.erase();
	Download_InvalidFtpUser_String.erase();
	Download_InvalidFtpPassword_String.erase();
	Download_TimeHoursMinutes_String.erase();
	Download_TimeHourMinutes_String.erase();
	Download_TimeMinutesSeconds_String.erase();
	Download_TimeMinuteSeconds_String.erase();
	Download_TimeSeconds_String.erase();
	Download_Http421Error_String.erase();
	Download_Next_String.erase();
	Download_UnableToFindHost_String.erase();
	Download_NoHostUrl_String.erase();
	Dialog_AbortDlgAbort_String.erase();
	Dialog_BaseBackground_Background = Background();
	Welcome_Continue_String.erase();
	Welcome_Cancel_String.erase();
	Welcome_ConfigProxy_String.erase();
	Welcome_Help_String.erase();
	Welcome_Info1_String.erase();
	Welcome_Info2_String.erase();
	Welcome_Info3_String.erase();
	Welcome_Info4_String.erase();
	Welcome_Info5_String.erase();
	Welcome_Info6_String.erase();
	Welcome_Title_String.erase();
	About_Version_String.erase();
	About_Copyright_String.erase();
	About_Title_String.erase();
	Motd_SystemTitle_String.erase();
	Motd_ProductTitle_String.erase();
	Motd_Info1_String.erase();
	Motd_Info2_String.erase();
	Motd_Info3_String.erase();
	Motd_Info4_String.erase();
	Motd_Info5_String.erase();
	Motd_Info6_String.erase();
	Motd_Help_String.erase();
	Motd_Back_String.erase();
	Motd_Next_String.erase();
	Motd_Skip_String.erase();
	Motd_Title_String.erase();
	VisitHost_Title_String.erase();
	VisitHost_Info1_String.erase();
	VisitHost_Info2_String.erase();
	VisitHost_Info3_String.erase();
	VisitHost_Info4_String.erase();
	VisitHost_Info5_String.erase();
	VisitHost_Info6_String.erase();
	VisitHost_Help_String.erase();
	VisitHost_Back_String.erase();
	VisitHost_Next_String.erase();
	OptionalPatch_Title_String.erase();
	OptionalPatch_Info1_String.erase();
	OptionalPatch_Info2_String.erase();
	OptionalPatch_Info3_String.erase();
	OptionalPatch_Info4_String.erase();
	OptionalPatch_Info5_String.erase();
	OptionalPatch_Info6_String.erase();
	OptionalPatch_ViewDetails_String.erase();
	OptionalPatch_Help_String.erase();
	OptionalPatch_Next_String.erase();
	OptionalPatch_BadDescExe_String.erase();
	OptionalPatch_DescTitle_String.erase();
	OptionalPatch_FetchingDesc_String.erase();
	OptionalPatch_TimedOut_String.erase();
	OptionalPatch_DownloadFailed_String.erase();
	OptionalPatch_Back_String.erase();
	PatchDetails_Title_String.erase();
	PatchDetails_Info1_String.erase();
	PatchDetails_Info2_String.erase();
	PatchDetails_Info3_String.erase();
	PatchDetails_Info4_String.erase();
	PatchDetails_Info5_String.erase();
	PatchDetails_Info6_String.erase();
	PatchDetails_Help_String.erase();
	PatchDetails_Back_String.erase();
	PatchDetails_Next_String.erase();
	PatchDetails_FileTooBig_String.erase();
	PatchDetails_FileTooBigTtl_String.erase();
	VersionCheck_Title_String.erase();
	VersionCheck_Info1_String.erase();
	VersionCheck_Info2_String.erase();
	VersionCheck_Info3_String.erase();
	VersionCheck_Info4_String.erase();
	VersionCheck_Info5_String.erase();
	VersionCheck_Info6_String.erase();
	VersionCheck_Help_String.erase();
	VersionCheck_Next_String.erase();
	VersionCheck_Back_String.erase();
	VersionCheck_OkToGoBack_String.erase();
	VersionCheck_StatusPleaseWait_String.erase();
	VersionCheck_StatusErrorFound_String.erase();
	VersionCheck_StatusFinished_String.erase();
	VersionCheck_DNSTestStarted_String.erase();
	VersionCheck_MotdLookupStarted_String.erase();
	VersionCheck_ServerLookupStarted_String.erase();
	VersionCheck_SelfVersionCheckStarted_String.erase();
	VersionCheck_GameVersionCheckStarted_String.erase();
	VersionCheck_UpToDate_String.erase();
	VersionCheck_Finish_String.erase();
	VersionCheck_CheckingForPatch_String.erase();
	VersionCheck_AbortTitle_String.erase();
	VersionCheck_Visit_String.erase();
	Login_Title_String.erase();
	Login_Info_String.erase();
	Login_Name_String.erase();
	Login_Password_String.erase();
	Login_Ok_String.erase();
}

