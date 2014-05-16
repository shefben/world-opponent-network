#ifndef __UpdateResource_H__
#define __UpdateResource_H__
#include "WONGUI/WONGUIConfig/ResourceConfig.h"
#include "WONGUI/StringLocalize.h"
#include "WONGUI/EventTypes.h"

extern WONAPI::GUIString Global_Yes_String;
extern WONAPI::GUIString Global_No_String;
extern WONAPI::GUIString Global_Ok_String;
extern WONAPI::GUIString Global_Cancel_String;
extern WONAPI::GUIString Global_Close_String;
extern WONAPI::GUIString Global_BadHelpExe_String;
extern WONAPI::GUIString Global_WrongSize_String;
extern WONAPI::GUIString Global_InvalidChecksum_String;
extern WONAPI::GUIString Global_InvalidPatch_String;
extern WONAPI::GUIString Global_Validation_String;
extern WONAPI::GUIString Global_Validating_String;
extern WONAPI::GUIString Global_DownloadFailures_String;
extern WONAPI::GUIString Global_DownloadAborts_String;
extern WONAPI::GUIString Global_NoStringRes_String;
extern WONAPI::GUIString Global_MenuAbout_String;
extern WONAPI::GUIString Global_ParamErrTitle_String;
extern WONAPI::GUIString Global_CfgFileTooBig_String;
extern WONAPI::GUIString Global_NoReadCfgFile_String;
extern WONAPI::GUIString Global_NoOpenCfgFile_String;
extern WONAPI::GUIString Global_PatcherDispName_String;
extern WONAPI::GUIString Global_BadHelpFile_String;
extern WONAPI::GUIString Global_BadResFile_String;
extern WONAPI::GUIString Global_UnknownParam_String;
extern WONAPI::GUIString Global_PrUnknown_String;
extern WONAPI::GUIString Global_PrUpToDate_String;
extern WONAPI::GUIString Global_PrPatchFound_String;
extern WONAPI::GUIString Global_PrPatchFetched_String;
extern WONAPI::GUIString Global_PrUserAbort_String;
extern WONAPI::GUIString Global_PrNoServerConnection_String;
extern WONAPI::GUIString Global_PrNoHostList_String;
extern WONAPI::GUIString Global_PrNoDownload_String;
extern WONAPI::GUIString Global_NoMonitor_String;
extern WONAPI::GUIString Global_PsUndetermined_String;
extern WONAPI::GUIString Global_NoFindPatch_String;
extern WONAPI::GUIString Global_NoRunPatch_String;
extern WONAPI::GUIString Global_PatchFailed_String;
extern WONAPI::GUIString Global_PrepPatcher_String;
extern WONAPI::GUIString Global_CheckingForThePatch_String;
extern WONAPI::GUIString Global_VerCheckFailed_String;
extern WONAPI::GUIString Global_PatchFound_String;
extern WONAPI::GUIString Global_NoRetrievePatch_String;
extern WONAPI::GUIString Global_Applying_String;
extern WONAPI::GUIString Global_AnotherPatch_String;
extern WONAPI::GUIString Global_PatchApplied_String;
extern WONAPI::GUIString Global_LaunchingExe_String;
extern WONAPI::GUIString Global_CheckingMotd_String;
extern WONAPI::GUIString Global_CheckingForMotd_String;
extern WONAPI::GUIString Global_UpToDate_String;
extern WONAPI::GUIString Global_PatcherInfoTitle_String;
extern WONAPI::GUIString Global_VersionFileCheckFailed_String;
extern WONAPI::GUIString Global_PatchReturnedError_String;
extern WONAPI::GUIString Global_CheckConnection_String;
extern WONAPI::GUIString Global_TimeOut_String;
extern WONAPI::GUIString Global_SkipMotd_String;
extern WONAPI::GUIString Global_Warning_String;
extern WONAPI::GUIString Global_IncompletePatch_String;
extern WONAPI::GUIString Global_SelfUpToDate_String;
extern WONAPI::GUIString Global_CheckingSelfForPatch_String;
extern WONAPI::GUIString Global_CloseForSelfPatch_String;
extern WONAPI::GUIString Global_SelfNoHostList_String;
extern WONAPI::GUIString Global_NoFindRestarter_String;
extern WONAPI::GUIString Global_NoRunRestarter_String;
extern WONAPI::GUIString Global_RestarterFailed_String;
extern WONAPI::GUIString Global_SierraUpdate_String;
extern WONAPI::GUIString Global_SelfUpdateLang_String;
extern WONAPI::GUIString Global_NoWritePsapi_String;
extern WONAPI::GUIString Global_NoWriteRestarter_String;
extern WONAPI::GUIString Global_Error_String;
extern WONAPI::GUIString Global_Host_String;
extern WONAPI::GUIString Global_Bytes_String;
extern WONAPI::GUIString Global_KiloBytes_String;
extern WONAPI::GUIString Global_MegaBytes_String;
extern WONAPI::GUIString Global_PrOptionalPatch_String;
extern WONAPI::GUIString Global_PrManualDownload_String;
extern WONAPI::GUIString Global_OptionalPatchFound_String;
extern WONAPI::GUIString Global_PossiblePatch_String;
extern WONAPI::GUIString Global_CheckForPossiblePatch_String;
extern WONAPI::GUIString Global_NoPatchUrl_String;
extern WONAPI::GUIString Global_LocalPatchDoesNotMatch_String;
extern WONAPI::GUIString Global_CheckConection_String;
extern WONAPI::GUIString Global_CheckConectionTitle_String;
extern WONAPI::GUIString Global_Success_String;
extern WONAPI::GUIString Global_MotdSkipped_String;
extern WONAPI::GUIString Global_MotdNotFound_String;
extern WONAPI::GUIString Global_GameMotdNotFound_String;
extern WONAPI::GUIString Global_MotdError_String;
extern WONAPI::GUIString Global_NoNewMotd_String;
extern WONAPI::GUIString Global_PatchRequired_String;
extern WONAPI::GUIString Global_OptionalPatch_String;
extern WONAPI::GUIString Global_PartialDirServerFailure_String;
extern WONAPI::GUIString Global_DirServerFailure_String;
extern WONAPI::GUIString Global_BadVersionNoUpdate_String;
extern WONAPI::GUIString Global_SierraUpBadVersionNoUpdate_String;
extern WONAPI::GUIString Global_PatchServerError_String;
extern WONAPI::GUIString Global_UpdateServerTimedOut_String;
extern WONAPI::GUIString Global_NoConfigFile_String;
extern WONAPI::ImagePtr Global_GameLogo_Image;
extern WONAPI::ImagePtr Global_MpsLogo_Image;
extern int Global_HOST_AD_WIDTH_Int;
extern int Global_HOST_AD_HEIGHT_Int;

extern WONAPI::GUIString SelectHost_Title_String;
extern WONAPI::GUIString SelectHost_Info1_String;
extern WONAPI::GUIString SelectHost_Info2_String;
extern WONAPI::GUIString SelectHost_Info3_String;
extern WONAPI::GUIString SelectHost_Info4_String;
extern WONAPI::GUIString SelectHost_Info5_String;
extern WONAPI::GUIString SelectHost_Info6_String;
extern WONAPI::GUIString SelectHost_ConfigProxy_String;
extern WONAPI::GUIString SelectHost_Help_String;
extern WONAPI::GUIString SelectHost_Next_String;
extern WONAPI::GUIString SelectHost_HavePatch_String;
extern WONAPI::GUIString SelectHost_PatchFilter_String;
extern WONAPI::GUIString SelectHost_InfoManual_String;
extern WONAPI::GUIString SelectHost_Back_String;
extern WONAPI::GUIString SelectHost_FileOpenDlgTitle_String;
extern WONAPI::GUIString SelectHost_HostTitle_String;
extern WONAPI::GUIString SelectHost_Results_String;
extern WONAPI::GUIString SelectHost_WaitInfo1_String;
extern WONAPI::GUIString SelectHost_WaitInfo2_String;
extern WONAPI::GUIString SelectHost_WaitInfo3_String;
extern WONAPI::GUIString SelectHost_WaitInfo4_String;
extern WONAPI::GUIString SelectHost_WaitInfo5_String;
extern WONAPI::GUIString SelectHost_WaitInfo6_String;

extern WONAPI::GUIString ConfigProxy_Title_String;
extern WONAPI::GUIString ConfigProxy_Info1_String;
extern WONAPI::GUIString ConfigProxy_Info2_String;
extern WONAPI::GUIString ConfigProxy_Info3_String;
extern WONAPI::GUIString ConfigProxy_Info4_String;
extern WONAPI::GUIString ConfigProxy_Info5_String;
extern WONAPI::GUIString ConfigProxy_Info6_String;
extern WONAPI::GUIString ConfigProxy_UseProxy_String;
extern WONAPI::GUIString ConfigProxy_Host_String;
extern WONAPI::GUIString ConfigProxy_Port_String;
extern WONAPI::GUIString ConfigProxy_Help_String;

extern WONAPI::GUIString Download_Title_String;
extern WONAPI::GUIString Download_Info1_String;
extern WONAPI::GUIString Download_Info2_String;
extern WONAPI::GUIString Download_Info3_String;
extern WONAPI::GUIString Download_Info4_String;
extern WONAPI::GUIString Download_Info5_String;
extern WONAPI::GUIString Download_Info6_String;
extern WONAPI::GUIString Download_Host_String;
extern WONAPI::GUIString Download_Progress_String;
extern WONAPI::GUIString Download_Preparing_String;
extern WONAPI::GUIString Download_Estimating_String;
extern WONAPI::GUIString Download_Help_String;
extern WONAPI::GUIString Download_Back_String;
extern WONAPI::GUIString Download_VisitHost_String;
extern WONAPI::GUIString Download_HostedBy_String;
extern WONAPI::GUIString Download_BytesTransferred_String;
extern WONAPI::GUIString Download_ConnectionRefused_String;
extern WONAPI::GUIString Download_InvalidHttpHeader_String;
extern WONAPI::GUIString Download_InvalidHttpRedirect_String;
extern WONAPI::GUIString Download_TooManyHttpRedirects_String;
extern WONAPI::GUIString Download_DownloadError_String;
extern WONAPI::GUIString Download_UnknownHttpError_String;
extern WONAPI::GUIString Download_HttpError_String;
extern WONAPI::GUIString Download_Http404Error_String;
extern WONAPI::GUIString Download_Confirm_String;
extern WONAPI::GUIString Download_AbandonDownload_String;
extern WONAPI::GUIString Download_AbortPatch_String;
extern WONAPI::GUIString Download_UnknownFtpError_String;
extern WONAPI::GUIString Download_FtpError_String;
extern WONAPI::GUIString Download_InvalidFtpResp_String;
extern WONAPI::GUIString Download_InvalidFtpPasvResp_String;
extern WONAPI::GUIString Download_NoData_String;
extern WONAPI::GUIString Download_InvalidFtpFile_String;
extern WONAPI::GUIString Download_InvalidFtpUser_String;
extern WONAPI::GUIString Download_InvalidFtpPassword_String;
extern WONAPI::GUIString Download_TimeHoursMinutes_String;
extern WONAPI::GUIString Download_TimeHourMinutes_String;
extern WONAPI::GUIString Download_TimeMinutesSeconds_String;
extern WONAPI::GUIString Download_TimeMinuteSeconds_String;
extern WONAPI::GUIString Download_TimeSeconds_String;
extern WONAPI::GUIString Download_Http421Error_String;
extern WONAPI::GUIString Download_Next_String;
extern WONAPI::GUIString Download_UnableToFindHost_String;
extern WONAPI::GUIString Download_NoHostUrl_String;

extern WONAPI::GUIString Dialog_AbortDlgAbort_String;
extern WONAPI::Background Dialog_BaseBackground_Background;

extern WONAPI::GUIString Welcome_Continue_String;
extern WONAPI::GUIString Welcome_Cancel_String;
extern WONAPI::GUIString Welcome_ConfigProxy_String;
extern WONAPI::GUIString Welcome_Help_String;
extern WONAPI::GUIString Welcome_Info1_String;
extern WONAPI::GUIString Welcome_Info2_String;
extern WONAPI::GUIString Welcome_Info3_String;
extern WONAPI::GUIString Welcome_Info4_String;
extern WONAPI::GUIString Welcome_Info5_String;
extern WONAPI::GUIString Welcome_Info6_String;
extern WONAPI::GUIString Welcome_Title_String;

extern WONAPI::GUIString About_Version_String;
extern WONAPI::GUIString About_Copyright_String;
extern WONAPI::GUIString About_Title_String;

extern WONAPI::GUIString Motd_SystemTitle_String;
extern WONAPI::GUIString Motd_ProductTitle_String;
extern WONAPI::GUIString Motd_Info1_String;
extern WONAPI::GUIString Motd_Info2_String;
extern WONAPI::GUIString Motd_Info3_String;
extern WONAPI::GUIString Motd_Info4_String;
extern WONAPI::GUIString Motd_Info5_String;
extern WONAPI::GUIString Motd_Info6_String;
extern WONAPI::GUIString Motd_Help_String;
extern WONAPI::GUIString Motd_Back_String;
extern WONAPI::GUIString Motd_Next_String;
extern WONAPI::GUIString Motd_Skip_String;
extern WONAPI::GUIString Motd_Title_String;

extern WONAPI::GUIString VisitHost_Title_String;
extern WONAPI::GUIString VisitHost_Info1_String;
extern WONAPI::GUIString VisitHost_Info2_String;
extern WONAPI::GUIString VisitHost_Info3_String;
extern WONAPI::GUIString VisitHost_Info4_String;
extern WONAPI::GUIString VisitHost_Info5_String;
extern WONAPI::GUIString VisitHost_Info6_String;
extern WONAPI::GUIString VisitHost_Help_String;
extern WONAPI::GUIString VisitHost_Back_String;
extern WONAPI::GUIString VisitHost_Next_String;

extern WONAPI::GUIString OptionalPatch_Title_String;
extern WONAPI::GUIString OptionalPatch_Info1_String;
extern WONAPI::GUIString OptionalPatch_Info2_String;
extern WONAPI::GUIString OptionalPatch_Info3_String;
extern WONAPI::GUIString OptionalPatch_Info4_String;
extern WONAPI::GUIString OptionalPatch_Info5_String;
extern WONAPI::GUIString OptionalPatch_Info6_String;
extern WONAPI::GUIString OptionalPatch_ViewDetails_String;
extern WONAPI::GUIString OptionalPatch_Help_String;
extern WONAPI::GUIString OptionalPatch_Next_String;
extern WONAPI::GUIString OptionalPatch_BadDescExe_String;
extern WONAPI::GUIString OptionalPatch_DescTitle_String;
extern WONAPI::GUIString OptionalPatch_FetchingDesc_String;
extern WONAPI::GUIString OptionalPatch_TimedOut_String;
extern WONAPI::GUIString OptionalPatch_DownloadFailed_String;
extern WONAPI::GUIString OptionalPatch_Back_String;

extern WONAPI::GUIString PatchDetails_Title_String;
extern WONAPI::GUIString PatchDetails_Info1_String;
extern WONAPI::GUIString PatchDetails_Info2_String;
extern WONAPI::GUIString PatchDetails_Info3_String;
extern WONAPI::GUIString PatchDetails_Info4_String;
extern WONAPI::GUIString PatchDetails_Info5_String;
extern WONAPI::GUIString PatchDetails_Info6_String;
extern WONAPI::GUIString PatchDetails_Help_String;
extern WONAPI::GUIString PatchDetails_Back_String;
extern WONAPI::GUIString PatchDetails_Next_String;
extern WONAPI::GUIString PatchDetails_FileTooBig_String;
extern WONAPI::GUIString PatchDetails_FileTooBigTtl_String;

extern WONAPI::GUIString VersionCheck_Title_String;
extern WONAPI::GUIString VersionCheck_Info1_String;
extern WONAPI::GUIString VersionCheck_Info2_String;
extern WONAPI::GUIString VersionCheck_Info3_String;
extern WONAPI::GUIString VersionCheck_Info4_String;
extern WONAPI::GUIString VersionCheck_Info5_String;
extern WONAPI::GUIString VersionCheck_Info6_String;
extern WONAPI::GUIString VersionCheck_Help_String;
extern WONAPI::GUIString VersionCheck_Next_String;
extern WONAPI::GUIString VersionCheck_Back_String;
extern WONAPI::GUIString VersionCheck_OkToGoBack_String;
extern WONAPI::GUIString VersionCheck_StatusPleaseWait_String;
extern WONAPI::GUIString VersionCheck_StatusErrorFound_String;
extern WONAPI::GUIString VersionCheck_StatusFinished_String;
extern WONAPI::GUIString VersionCheck_DNSTestStarted_String;
extern WONAPI::GUIString VersionCheck_MotdLookupStarted_String;
extern WONAPI::GUIString VersionCheck_ServerLookupStarted_String;
extern WONAPI::GUIString VersionCheck_SelfVersionCheckStarted_String;
extern WONAPI::GUIString VersionCheck_GameVersionCheckStarted_String;
extern WONAPI::GUIString VersionCheck_UpToDate_String;
extern WONAPI::GUIString VersionCheck_Finish_String;
extern WONAPI::GUIString VersionCheck_CheckingForPatch_String;
extern WONAPI::GUIString VersionCheck_AbortTitle_String;
extern WONAPI::GUIString VersionCheck_Visit_String;

extern WONAPI::GUIString Login_Title_String;
extern WONAPI::GUIString Login_Info_String;
extern WONAPI::GUIString Login_Name_String;
extern WONAPI::GUIString Login_Password_String;
extern WONAPI::GUIString Login_Ok_String;

WONAPI::ResourceConfigTablePtr UpdateResource_Init(WONAPI::ResourceConfigTable *theTable = NULL);
enum UpdateResourceCtrlId
{
	UpdateResourceCtrlId_Min = WONAPI::ControlId_UserMin,
};
enum UpdateResourceId
{
	Global_Yes_String_Id,
	Global_No_String_Id,
	Global_Ok_String_Id,
	Global_Cancel_String_Id,
	Global_Close_String_Id,
	Global_BadHelpExe_String_Id,
	Global_WrongSize_String_Id,
	Global_InvalidChecksum_String_Id,
	Global_InvalidPatch_String_Id,
	Global_Validation_String_Id,
	Global_Validating_String_Id,
	Global_DownloadFailures_String_Id,
	Global_DownloadAborts_String_Id,
	Global_NoStringRes_String_Id,
	Global_MenuAbout_String_Id,
	Global_ParamErrTitle_String_Id,
	Global_CfgFileTooBig_String_Id,
	Global_NoReadCfgFile_String_Id,
	Global_NoOpenCfgFile_String_Id,
	Global_PatcherDispName_String_Id,
	Global_BadHelpFile_String_Id,
	Global_BadResFile_String_Id,
	Global_UnknownParam_String_Id,
	Global_PrUnknown_String_Id,
	Global_PrUpToDate_String_Id,
	Global_PrPatchFound_String_Id,
	Global_PrPatchFetched_String_Id,
	Global_PrUserAbort_String_Id,
	Global_PrNoServerConnection_String_Id,
	Global_PrNoHostList_String_Id,
	Global_PrNoDownload_String_Id,
	Global_NoMonitor_String_Id,
	Global_PsUndetermined_String_Id,
	Global_NoFindPatch_String_Id,
	Global_NoRunPatch_String_Id,
	Global_PatchFailed_String_Id,
	Global_PrepPatcher_String_Id,
	Global_CheckingForThePatch_String_Id,
	Global_VerCheckFailed_String_Id,
	Global_PatchFound_String_Id,
	Global_NoRetrievePatch_String_Id,
	Global_Applying_String_Id,
	Global_AnotherPatch_String_Id,
	Global_PatchApplied_String_Id,
	Global_LaunchingExe_String_Id,
	Global_CheckingMotd_String_Id,
	Global_CheckingForMotd_String_Id,
	Global_UpToDate_String_Id,
	Global_PatcherInfoTitle_String_Id,
	Global_VersionFileCheckFailed_String_Id,
	Global_PatchReturnedError_String_Id,
	Global_CheckConnection_String_Id,
	Global_TimeOut_String_Id,
	Global_SkipMotd_String_Id,
	Global_Warning_String_Id,
	Global_IncompletePatch_String_Id,
	Global_SelfUpToDate_String_Id,
	Global_CheckingSelfForPatch_String_Id,
	Global_CloseForSelfPatch_String_Id,
	Global_SelfNoHostList_String_Id,
	Global_NoFindRestarter_String_Id,
	Global_NoRunRestarter_String_Id,
	Global_RestarterFailed_String_Id,
	Global_SierraUpdate_String_Id,
	Global_SelfUpdateLang_String_Id,
	Global_NoWritePsapi_String_Id,
	Global_NoWriteRestarter_String_Id,
	Global_Error_String_Id,
	Global_Host_String_Id,
	Global_Bytes_String_Id,
	Global_KiloBytes_String_Id,
	Global_MegaBytes_String_Id,
	Global_PrOptionalPatch_String_Id,
	Global_PrManualDownload_String_Id,
	Global_OptionalPatchFound_String_Id,
	Global_PossiblePatch_String_Id,
	Global_CheckForPossiblePatch_String_Id,
	Global_NoPatchUrl_String_Id,
	Global_LocalPatchDoesNotMatch_String_Id,
	Global_CheckConection_String_Id,
	Global_CheckConectionTitle_String_Id,
	Global_Success_String_Id,
	Global_MotdSkipped_String_Id,
	Global_MotdNotFound_String_Id,
	Global_GameMotdNotFound_String_Id,
	Global_MotdError_String_Id,
	Global_NoNewMotd_String_Id,
	Global_PatchRequired_String_Id,
	Global_OptionalPatch_String_Id,
	Global_PartialDirServerFailure_String_Id,
	Global_DirServerFailure_String_Id,
	Global_BadVersionNoUpdate_String_Id,
	Global_SierraUpBadVersionNoUpdate_String_Id,
	Global_PatchServerError_String_Id,
	Global_UpdateServerTimedOut_String_Id,
	Global_NoConfigFile_String_Id,
	Global_GameLogo_Image_Id,
	Global_MpsLogo_Image_Id,
	Global_HOST_AD_WIDTH_Int_Id,
	Global_HOST_AD_HEIGHT_Int_Id,

	SelectHost_Title_String_Id,
	SelectHost_Info1_String_Id,
	SelectHost_Info2_String_Id,
	SelectHost_Info3_String_Id,
	SelectHost_Info4_String_Id,
	SelectHost_Info5_String_Id,
	SelectHost_Info6_String_Id,
	SelectHost_ConfigProxy_String_Id,
	SelectHost_Help_String_Id,
	SelectHost_Next_String_Id,
	SelectHost_HavePatch_String_Id,
	SelectHost_PatchFilter_String_Id,
	SelectHost_InfoManual_String_Id,
	SelectHost_Back_String_Id,
	SelectHost_FileOpenDlgTitle_String_Id,
	SelectHost_HostTitle_String_Id,
	SelectHost_Results_String_Id,
	SelectHost_WaitInfo1_String_Id,
	SelectHost_WaitInfo2_String_Id,
	SelectHost_WaitInfo3_String_Id,
	SelectHost_WaitInfo4_String_Id,
	SelectHost_WaitInfo5_String_Id,
	SelectHost_WaitInfo6_String_Id,

	ConfigProxy_Title_String_Id,
	ConfigProxy_Info1_String_Id,
	ConfigProxy_Info2_String_Id,
	ConfigProxy_Info3_String_Id,
	ConfigProxy_Info4_String_Id,
	ConfigProxy_Info5_String_Id,
	ConfigProxy_Info6_String_Id,
	ConfigProxy_UseProxy_String_Id,
	ConfigProxy_Host_String_Id,
	ConfigProxy_Port_String_Id,
	ConfigProxy_Help_String_Id,

	Download_Title_String_Id,
	Download_Info1_String_Id,
	Download_Info2_String_Id,
	Download_Info3_String_Id,
	Download_Info4_String_Id,
	Download_Info5_String_Id,
	Download_Info6_String_Id,
	Download_Host_String_Id,
	Download_Progress_String_Id,
	Download_Preparing_String_Id,
	Download_Estimating_String_Id,
	Download_Help_String_Id,
	Download_Back_String_Id,
	Download_VisitHost_String_Id,
	Download_HostedBy_String_Id,
	Download_BytesTransferred_String_Id,
	Download_ConnectionRefused_String_Id,
	Download_InvalidHttpHeader_String_Id,
	Download_InvalidHttpRedirect_String_Id,
	Download_TooManyHttpRedirects_String_Id,
	Download_DownloadError_String_Id,
	Download_UnknownHttpError_String_Id,
	Download_HttpError_String_Id,
	Download_Http404Error_String_Id,
	Download_Confirm_String_Id,
	Download_AbandonDownload_String_Id,
	Download_AbortPatch_String_Id,
	Download_UnknownFtpError_String_Id,
	Download_FtpError_String_Id,
	Download_InvalidFtpResp_String_Id,
	Download_InvalidFtpPasvResp_String_Id,
	Download_NoData_String_Id,
	Download_InvalidFtpFile_String_Id,
	Download_InvalidFtpUser_String_Id,
	Download_InvalidFtpPassword_String_Id,
	Download_TimeHoursMinutes_String_Id,
	Download_TimeHourMinutes_String_Id,
	Download_TimeMinutesSeconds_String_Id,
	Download_TimeMinuteSeconds_String_Id,
	Download_TimeSeconds_String_Id,
	Download_Http421Error_String_Id,
	Download_Next_String_Id,
	Download_UnableToFindHost_String_Id,
	Download_NoHostUrl_String_Id,

	Dialog_AbortDlgAbort_String_Id,
	Dialog_BaseBackground_Background_Id,

	Welcome_Continue_String_Id,
	Welcome_Cancel_String_Id,
	Welcome_ConfigProxy_String_Id,
	Welcome_Help_String_Id,
	Welcome_Info1_String_Id,
	Welcome_Info2_String_Id,
	Welcome_Info3_String_Id,
	Welcome_Info4_String_Id,
	Welcome_Info5_String_Id,
	Welcome_Info6_String_Id,
	Welcome_Title_String_Id,

	About_Version_String_Id,
	About_Copyright_String_Id,
	About_Title_String_Id,

	Motd_SystemTitle_String_Id,
	Motd_ProductTitle_String_Id,
	Motd_Info1_String_Id,
	Motd_Info2_String_Id,
	Motd_Info3_String_Id,
	Motd_Info4_String_Id,
	Motd_Info5_String_Id,
	Motd_Info6_String_Id,
	Motd_Help_String_Id,
	Motd_Back_String_Id,
	Motd_Next_String_Id,
	Motd_Skip_String_Id,
	Motd_Title_String_Id,

	VisitHost_Title_String_Id,
	VisitHost_Info1_String_Id,
	VisitHost_Info2_String_Id,
	VisitHost_Info3_String_Id,
	VisitHost_Info4_String_Id,
	VisitHost_Info5_String_Id,
	VisitHost_Info6_String_Id,
	VisitHost_Help_String_Id,
	VisitHost_Back_String_Id,
	VisitHost_Next_String_Id,

	OptionalPatch_Title_String_Id,
	OptionalPatch_Info1_String_Id,
	OptionalPatch_Info2_String_Id,
	OptionalPatch_Info3_String_Id,
	OptionalPatch_Info4_String_Id,
	OptionalPatch_Info5_String_Id,
	OptionalPatch_Info6_String_Id,
	OptionalPatch_ViewDetails_String_Id,
	OptionalPatch_Help_String_Id,
	OptionalPatch_Next_String_Id,
	OptionalPatch_BadDescExe_String_Id,
	OptionalPatch_DescTitle_String_Id,
	OptionalPatch_FetchingDesc_String_Id,
	OptionalPatch_TimedOut_String_Id,
	OptionalPatch_DownloadFailed_String_Id,
	OptionalPatch_Back_String_Id,

	PatchDetails_Title_String_Id,
	PatchDetails_Info1_String_Id,
	PatchDetails_Info2_String_Id,
	PatchDetails_Info3_String_Id,
	PatchDetails_Info4_String_Id,
	PatchDetails_Info5_String_Id,
	PatchDetails_Info6_String_Id,
	PatchDetails_Help_String_Id,
	PatchDetails_Back_String_Id,
	PatchDetails_Next_String_Id,
	PatchDetails_FileTooBig_String_Id,
	PatchDetails_FileTooBigTtl_String_Id,

	VersionCheck_Title_String_Id,
	VersionCheck_Info1_String_Id,
	VersionCheck_Info2_String_Id,
	VersionCheck_Info3_String_Id,
	VersionCheck_Info4_String_Id,
	VersionCheck_Info5_String_Id,
	VersionCheck_Info6_String_Id,
	VersionCheck_Help_String_Id,
	VersionCheck_Next_String_Id,
	VersionCheck_Back_String_Id,
	VersionCheck_OkToGoBack_String_Id,
	VersionCheck_StatusPleaseWait_String_Id,
	VersionCheck_StatusErrorFound_String_Id,
	VersionCheck_StatusFinished_String_Id,
	VersionCheck_DNSTestStarted_String_Id,
	VersionCheck_MotdLookupStarted_String_Id,
	VersionCheck_ServerLookupStarted_String_Id,
	VersionCheck_SelfVersionCheckStarted_String_Id,
	VersionCheck_GameVersionCheckStarted_String_Id,
	VersionCheck_UpToDate_String_Id,
	VersionCheck_Finish_String_Id,
	VersionCheck_CheckingForPatch_String_Id,
	VersionCheck_AbortTitle_String_Id,
	VersionCheck_Visit_String_Id,

	Login_Title_String_Id,
	Login_Info_String_Id,
	Login_Name_String_Id,
	Login_Password_String_Id,
	Login_Ok_String_Id,

};
const WONAPI::GUIString& UpdateResource_GetString(UpdateResourceId theId);
WONAPI::Image* UpdateResource_GetImage(UpdateResourceId theId);
WONAPI::Sound* UpdateResource_GetSound(UpdateResourceId theId);
WONAPI::Font* UpdateResource_GetFont(UpdateResourceId theId);
const WONAPI::Background& UpdateResource_GetBackground(UpdateResourceId theId);
DWORD UpdateResource_GetColor(UpdateResourceId theId);
int UpdateResource_GetInt(UpdateResourceId theId);
void UpdateResource_SafeGlueResources(WONAPI::ResourceConfigTable *theTable);
void UpdateResource_UnloadResources();

#endif
