#include "WONGUI/WONGUIConfig/ComponentConfig.h"
#include "LobbyResource.h"

using namespace WONAPI;

GUIString LobbyGlobal_Cancel_String;
GUIString LobbyGlobal_Lan_String;
GUIString LobbyGlobal_No_String;
GUIString LobbyGlobal_OK_String;
GUIString LobbyGlobal_Yes_String;
GUIString LobbyGlobal_QuickPlay_String;
ImagePtr LobbyGlobal_Searching_Image;
ImagePtr LobbyGlobal_Firewall_Image;
SoundPtr LobbyGlobal_ButtonClick_Sound;
SoundPtr LobbyGlobal_LobbyMusic_Sound;
SoundPtr LobbyGlobal_AllReady_Sound;
SoundPtr LobbyGlobal_AllNotReady_Sound;
SoundPtr LobbyGlobal_AskToJoinRecv_Sound;
SoundPtr LobbyGlobal_Error_Sound;
SoundPtr LobbyGlobal_FriendJoined_Sound;
SoundPtr LobbyGlobal_FriendLeft_Sound;
SoundPtr LobbyGlobal_InviteRecv_Sound;
SoundPtr LobbyGlobal_PlayerJoined_Sound;
SoundPtr LobbyGlobal_SierraLogon_Sound;
SoundPtr LobbyGlobal_WhisperRecv_Sound;

GUIString AccountLogic_InvalidCDKeyTitle_String;
GUIString AccountLogic_InvalidCDKeyDetails_String;

GUIString AddFriendDialog_StatusTitle_String;
GUIString AddFriendDialog_StatusDetails_String;

GUIString AdminActionDialog_Warn_String;
GUIString AdminActionDialog_Mute_String;
GUIString AdminActionDialog_UnMute_String;
GUIString AdminActionDialog_Kick_String;
GUIString AdminActionDialog_Ban_String;
GUIString AdminActionDialog_ExplanationOpt_String;
GUIString AdminActionDialog_ExplanationReq_String;

GUIString BadUserDialog_OneDay_String;
GUIString BadUserDialog_OneHour_String;
GUIString BadUserDialog_OneMinute_String;
GUIString BadUserDialog_OneSecond_String;
GUIString BadUserDialog_XDays_String;
GUIString BadUserDialog_XHours_String;
GUIString BadUserDialog_XMinutes_String;
GUIString BadUserDialog_XSeconds_String;
GUIString BadUserDialog_Infinite_String;

GUIString ChatCommandLogic_Whisper_String;
GUIString ChatCommandLogic_Block_String;
GUIString ChatCommandLogic_Emote_String;
GUIString ChatCommandLogic_Ignore_String;
GUIString ChatCommandLogic_Away_String;
GUIString ChatCommandLogic_Invite_String;
GUIString ChatCommandLogic_Uninvite_String;
GUIString ChatCommandLogic_Reply_String;
GUIString ChatCommandLogic_Clear_String;
GUIString ChatCommandLogic_Help_String;
GUIString ChatCommandLogic_ShowTeam_String;
GUIString ChatCommandLogic_Moderator_String;
GUIString ChatCommandLogic_ServerMute_String;
GUIString ChatCommandLogic_ServerBan_String;
GUIString ChatCommandLogic_Mute_String;
GUIString ChatCommandLogic_Ban_String;
GUIString ChatCommandLogic_Unmute_String;
GUIString ChatCommandLogic_ServerUnmute_String;
GUIString ChatCommandLogic_Unban_String;
GUIString ChatCommandLogic_ServerUnban_String;
GUIString ChatCommandLogic_Alert_String;
GUIString ChatCommandLogic_Warn_String;
GUIString ChatCommandLogic_HelpHeader_String;
GUIString ChatCommandLogic_HelpHelp_String;
GUIString ChatCommandLogic_ShowTeamHelp_String;
GUIString ChatCommandLogic_WhisperHelp_String;
GUIString ChatCommandLogic_BlockHelp_String;
GUIString ChatCommandLogic_IgnoreHelp_String;
GUIString ChatCommandLogic_ReplyHelp_String;
GUIString ChatCommandLogic_EmoteHelp_String;
GUIString ChatCommandLogic_AwayHelp_String;
GUIString ChatCommandLogic_ClearHelp_String;
GUIString ChatCommandLogic_InviteHelp_String;
GUIString ChatCommandLogic_UninviteHelp_String;
GUIString ChatCommandLogic_MuteHelp_String;
GUIString ChatCommandLogic_UnmuteHelp_String;
GUIString ChatCommandLogic_BanHelp_String;
GUIString ChatCommandLogic_UnbanHelp_String;
GUIString ChatCommandLogic_ModeratorHelp_String;
GUIString ChatCommandLogic_ServerMuteHelp_String;
GUIString ChatCommandLogic_ServerUnmuteHelp_String;
GUIString ChatCommandLogic_ServerBanHelp_String;
GUIString ChatCommandLogic_ServerUnbanHelp_String;
GUIString ChatCommandLogic_AlertHelp_String;
GUIString ChatCommandLogic_WarnHelp_String;

GUIString ChatCtrl_NormalChat_String;
GUIString ChatCtrl_Emote_String;
GUIString ChatCtrl_TeamChat_String;
GUIString ChatCtrl_TeamEmote_String;
GUIString ChatCtrl_YouWhisperToYourself_String;
GUIString ChatCtrl_YouWhisper_String;
GUIString ChatCtrl_SomeoneWhispersToYou_String;
GUIString ChatCtrl_YouWarnSomeone_String;
GUIString ChatCtrl_YouWarnYourself_String;
GUIString ChatCtrl_SomeoneWarnsYou_String;
GUIString ChatCtrl_SomeoneWarnsSomeone_String;
GUIString ChatCtrl_EnteringRoom_String;
GUIString ChatCtrl_LeavingRoom_String;
GUIString ChatCtrl_Disconnected_String;
GUIString ChatCtrl_YouWereKicked_String;
GUIString ChatCtrl_YouWereBannedMinute_String;
GUIString ChatCtrl_YouWereBannedMinutes_String;
GUIString ChatCtrl_YouWereBannedHour_String;
GUIString ChatCtrl_YouWereBannedHours_String;
GUIString ChatCtrl_YouWereBannedDay_String;
GUIString ChatCtrl_YouWereBannedDays_String;
GUIString ChatCtrl_YouWereBannedIndefinitely_String;
GUIString ChatCtrl_SomeoneWasKicked_String;
GUIString ChatCtrl_SomeoneWasBannedMinute_String;
GUIString ChatCtrl_SomeoneWasBannedMinutes_String;
GUIString ChatCtrl_SomeoneWasBannedHour_String;
GUIString ChatCtrl_SomeoneWasBannedHours_String;
GUIString ChatCtrl_SomeoneWasBannedDay_String;
GUIString ChatCtrl_SomeoneWasBannedDays_String;
GUIString ChatCtrl_SomeoneWasBannedIndefinitely_String;
GUIString ChatCtrl_YouWereUnmuted_String;
GUIString ChatCtrl_YouWereMutedMinute_String;
GUIString ChatCtrl_YouWereMutedMinutes_String;
GUIString ChatCtrl_YouWereMutedHour_String;
GUIString ChatCtrl_YouWereMutedHours_String;
GUIString ChatCtrl_YouWereMutedDay_String;
GUIString ChatCtrl_YouWereMutedDays_String;
GUIString ChatCtrl_YouWereMutedIndefinitely_String;
GUIString ChatCtrl_SomeoneWasUnmuted_String;
GUIString ChatCtrl_SomeoneWasMutedMinute_String;
GUIString ChatCtrl_SomeoneWasMutedMinutes_String;
GUIString ChatCtrl_SomeoneWasMutedHour_String;
GUIString ChatCtrl_SomeoneWasMutedHours_String;
GUIString ChatCtrl_SomeoneWasMutedDay_String;
GUIString ChatCtrl_SomeoneWasMutedDays_String;
GUIString ChatCtrl_SomeoneWasMutedIndefinitely_String;
GUIString ChatCtrl_ModeratorComment_String;
GUIString ChatCtrl_AlertComment_String;
GUIString ChatCtrl_ServerAlert_String;
GUIString ChatCtrl_ServerShutdownStartedMinute_String;
GUIString ChatCtrl_ServerShutdownStartedMinutes_String;
GUIString ChatCtrl_ServerShutdownStartedHour_String;
GUIString ChatCtrl_ServerShutdownStartedHours_String;
GUIString ChatCtrl_ServerShutdownStartedDay_String;
GUIString ChatCtrl_ServerShutdownStartedDays_String;
GUIString ChatCtrl_ServerShutdownAborted_String;
GUIString ChatCtrl_SomeoneInvitesYou_String;
GUIString ChatCtrl_SomeoneUninvitesYou_String;
GUIString ChatCtrl_YouInviteSomeone_String;
GUIString ChatCtrl_YouUninviteSomeone_String;
GUIString ChatCtrl_SomeoneInvitesYouExpl_String;
GUIString ChatCtrl_SomeoneUninvitesYouExpl_String;
GUIString ChatCtrl_YouInviteSomeoneExpl_String;
GUIString ChatCtrl_YouUninviteSomeoneExpl_String;
GUIString ChatCtrl_Accept_String;
GUIString ChatCtrl_Reject_String;
GUIString ChatCtrl_SomeoneAsksToJoin_String;
GUIString ChatCtrl_SomeoneAsksToJoinExpl_String;
GUIString ChatCtrl_ClientNotOnServer_String;
GUIString ChatCtrl_ClientInGame_String;
GUIString ChatCtrl_ClientInChat_String;
GUIString ChatCtrl_ClientInGameAndChat_String;
GUIString ChatCtrl_ClientIsAnonymous_String;
GUIString ChatCtrl_FriendIsChatting_String;
GUIString ChatCtrl_FriendIsInGameStaging_String;
GUIString ChatCtrl_FriendIsPlaying_String;
GUIString ChatCtrl_FriendIsOn_String;
GUIString ChatCtrl_FriendIsNotOnline_String;
GUIString ChatCtrl_PlayerKicked_String;
GUIString ChatCtrl_PlayerBanned_String;
GUIString ChatCtrl_FriendJoined_String;
GUIString ChatCtrl_FriendLeft_String;
GUIString ChatCtrl_TeamMemberJoined_String;
GUIString ChatCtrl_TeamMemberLeft_String;
GUIString ChatCtrl_TeamMembers_String;
GUIString ChatCtrl_IgnoreList_String;

GUIString ChooseNetworkAdapterDialog_Default_String;
GUIString ChooseNetworkAdapterDialog_Unknown_String;

GUIString ClientOptionsCtrl_Whisper_String;
GUIString ClientOptionsCtrl_Invite_String;
GUIString ClientOptionsCtrl_Uninvite_String;
GUIString ClientOptionsCtrl_AddToFriends_String;
GUIString ClientOptionsCtrl_RemoveFromFriends_String;
GUIString ClientOptionsCtrl_ClearAFK_String;
GUIString ClientOptionsCtrl_SetAFK_String;
GUIString ClientOptionsCtrl_Ignore_String;
GUIString ClientOptionsCtrl_Unignore_String;
GUIString ClientOptionsCtrl_Block_String;
GUIString ClientOptionsCtrl_Unblock_String;
GUIString ClientOptionsCtrl_Warn_String;
GUIString ClientOptionsCtrl_Mute_String;
GUIString ClientOptionsCtrl_UnMute_String;
GUIString ClientOptionsCtrl_Kick_String;
GUIString ClientOptionsCtrl_Ban_String;

GUIString DirectConnectionOptionsCtrl_Connect_String;
GUIString DirectConnectionOptionsCtrl_Refresh_String;
GUIString DirectConnectionOptionsCtrl_Remove_String;

GUIString FriendOptionsCtrl_Locate_String;
GUIString FriendOptionsCtrl_RefreshList_String;
GUIString FriendOptionsCtrl_RemoveFromFriends_String;

ImagePtr FriendsListCtrl_Unknown_Image;
ImagePtr FriendsListCtrl_NotFound_Image;
ImagePtr FriendsListCtrl_InChat_Image;
ImagePtr FriendsListCtrl_InGame_Image;

GUIString GameOptionsPopup_Join_String;
GUIString GameOptionsPopup_Ping_String;
GUIString GameOptionsPopup_QueryDetails_String;
GUIString GameOptionsPopup_HideDetails_String;
GUIString GameOptionsPopup_ShowDetails_String;
GUIString GameOptionsPopup_Remove_String;

GUIString GameStagingCtrl_Ready_String;
GUIString GameStagingCtrl_NotReady_String;
GUIString GameStagingCtrl_StartGame_String;
GUIString GameStagingCtrl_LeaveGame_String;
GUIString GameStagingCtrl_DissolveGame_String;

GUIString GameStagingLogic_KickedTitle_String;
GUIString GameStagingLogic_KickedDetails_String;
GUIString GameStagingLogic_BannedTitle_String;
GUIString GameStagingLogic_BannedDetails_String;
GUIString GameStagingLogic_ConnectWarningTitle1_String;
GUIString GameStagingLogic_ConnectWarningDetails1_String;
GUIString GameStagingLogic_ConnectWarningTitle2_String;
GUIString GameStagingLogic_ConnectWarningDetails2_String;
GUIString GameStagingLogic_ConnectWarningTitle3_String;
GUIString GameStagingLogic_ConnectWarningDetails3_String;
GUIString GameStagingLogic_NoPingReplyError_String;
GUIString GameStagingLogic_GettingGameInfoTitle_String;
GUIString GameStagingLogic_GettingGameInfoDetails_String;
GUIString GameStagingLogic_JoinGameErrorTitle_String;
GUIString GameStagingLogic_VerifyingPingTitle_String;
GUIString GameStagingLogic_VerifyingPingDetails_String;
GUIString GameStagingLogic_DissolveGame_String;
GUIString GameStagingLogic_DissolveGameConfirm_String;
GUIString GameStagingLogic_LeaveGame_String;
GUIString GameStagingLogic_LeaveGameConfirm_String;
GUIString GameStagingLogic_NoQuickPlayGameTitle_String;
GUIString GameStagingLogic_NoQuickPlayGameDetails_String;

GUIString GameUserListCtrl_Ban_String;
GUIString GameUserListCtrl_Kick_String;
ImagePtr GameUserListCtrl_Ready_Image;

GUIString InternetQuickPlay_Welcome_String;
GUIString InternetQuickPlay_UserWelcome_String;

GUIString JoinResponse_Joined_String;
GUIString JoinResponse_ServerFull_String;
GUIString JoinResponse_InviteOnly_String;
GUIString JoinResponse_AskToJoin_String;
GUIString JoinResponse_GameInProgress_String;
GUIString JoinResponse_InvalidPassword_String;
GUIString JoinResponse_NotOnLAN_String;
GUIString JoinResponse_Blocked_String;
GUIString JoinResponse_Unknown_String;

GUIString LANLogic_SocketBindError_String;
GUIString LANLogic_SocketListenError_String;
GUIString LANLogic_ConnectToGameTitle_String;
GUIString LANLogic_ConnectToGameDetails_String;
GUIString LANLogic_CheckingAddressTitle_String;
GUIString LANLogic_CheckingAddressDetails_String;
GUIString LANLogic_RegisterTitle_String;
GUIString LANLogic_RegisterDetails_String;
GUIString LANLogic_JoinFailure_String;
GUIString LANLogic_JoinReplyError_String;
GUIString LANLogic_InvalidSubnet_String;

GUIString LobbyContainer_InitStart_String;
GUIString LobbyContainer_InitFailed_String;
GUIString LobbyContainer_CheckingVersion_String;
GUIString LobbyContainer_QueryingServers_String;
GUIString LobbyContainer_NoConnection_String;
GUIString LobbyContainer_DNSLookupFailed_String;
GUIString LobbyContainer_DirLookupFailure_String;
GUIString LobbyContainer_VersionCheckFailure_String;
GUIString LobbyContainer_InitComplete_String;
GUIString LobbyContainer_NoServers_String;
GUIString LobbyContainer_AuthServers_String;
GUIString LobbyContainer_AccountServers_String;
GUIString LobbyContainer_VersionServers_String;
GUIString LobbyContainer_CreateAccountTitle_String;
GUIString LobbyContainer_CreateAccountDetails_String;
GUIString LobbyContainer_RetrievePasswordTitle_String;
GUIString LobbyContainer_RetrievePasswordDetails_String;
GUIString LobbyContainer_RetrieveUsernameTitle_String;
GUIString LobbyContainer_RetrieveUsernameDetails_String;
GUIString LobbyContainer_QueryAccountTitle_String;
GUIString LobbyContainer_QueryAccountDetails_String;
GUIString LobbyContainer_UpdateAccountTitle_String;
GUIString LobbyContainer_UpdateAccountDetails_String;
GUIString LobbyContainer_LoginTitle_String;
GUIString LobbyContainer_LoginDetails_String;
GUIString LobbyContainer_GettingRoomsTitle_String;
GUIString LobbyContainer_GettingRoomsDetails_String;
GUIString LobbyContainer_VersionOutOfDataTitle_String;
GUIString LobbyContainer_VersionOutOfDataDetails_String;

GUIString LobbyGameStatus_Success_String;
GUIString LobbyGameStatus_GameFull_String;
GUIString LobbyGameStatus_GameInProgress_String;
GUIString LobbyGameStatus_UnpackFailure_String;
GUIString LobbyGameStatus_NotInvited_String;
GUIString LobbyGameStatus_CaptainRejectedYou_String;
GUIString LobbyGameStatus_InvalidPassword_String;
GUIString LobbyGameStatus_DuplicateName_String;

GUIString LobbyOptions_DefaultName_String;
GUIString LobbyOptions_CheckingAddress_String;
GUIString LobbyOptions_AddressUnavailable_String;

GUIString RoomCtrl_QueryingServer_String;
ImagePtr RoomCtrl_Password_Image;

GUIString RoomLabelCtrl_LabelPrefix_String;
GUIString RoomLabelCtrl_LabelServer_String;
GUIString RoomLabelCtrl_LabelUser_String;
GUIString RoomLabelCtrl_LabelUsers_String;
GUIString RoomLabelCtrl_LabelDisconnected_String;

GUIString RoutingLogic_GetClientListTitle_String;
GUIString RoutingLogic_GetClientListDetails_String;
GUIString RoutingLogic_GetGroupListTitle_String;
GUIString RoutingLogic_GetGroupListDetails_String;
GUIString RoutingLogic_GameDissolvedTitle_String;
GUIString RoutingLogic_GameDissolvedDetails_String;
GUIString RoutingLogic_NoServersTitle_String;
GUIString RoutingLogic_NoServersDetails_String;
GUIString RoutingLogic_TooManyFriendsTitle_String;
GUIString RoutingLogic_TooManyFriendsDetails_String;
GUIString RoutingLogic_AlreadyJoinedTitle_String;
GUIString RoutingLogic_AlreadyInGame_String;
GUIString RoutingLogic_AlreadyInRoom_String;
GUIString RoutingLogic_JoinGroupTitle_String;
GUIString RoutingLogic_JoinGroupDetails_String;
GUIString RoutingLogic_ConnectTitle_String;
GUIString RoutingLogic_ConnectDetails_String;
GUIString RoutingLogic_RegisterTitle_String;
GUIString RoutingLogic_RegisterDetails_String;
GUIString RoutingLogic_QueryGroupsTitle_String;
GUIString RoutingLogic_QueryGroupsDetails_String;
GUIString RoutingLogic_CreateGroupTitle_String;
GUIString RoutingLogic_CreateGroupDetails_String;
GUIString RoutingLogic_GetBadUsersTitle_String;
GUIString RoutingLogic_GetBadUsersDetails_String;
GUIString RoutingLogic_DisconnectedTitle_String;
GUIString RoutingLogic_DisconnectedDetails_String;
GUIString RoutingLogic_AskingCaptainTitle_String;
GUIString RoutingLogic_AskingCaptainDetails_String;
GUIString RoutingLogic_CollectingPingsTitle_String;
GUIString RoutingLogic_CollectingPingsDetails_String;
GUIString RoutingLogic_GettingGameListTitle_String;
GUIString RoutingLogic_GettingGameListDetails_String;
GUIString RoutingLogic_MuteClientTitle_String;
GUIString RoutingLogic_UnmuteClientTitle_String;
GUIString RoutingLogic_BanClientTitle_String;
GUIString RoutingLogic_KickClientTitle_String;
GUIString RoutingLogic_ModerateDetails_String;
GUIString RoutingLogic_FriendNotFoundTitle_String;
GUIString RoutingLogic_FriendNotFoundDetails_String;
GUIString RoutingLogic_ClientNotFound_String;
GUIString RoutingLogic_ClientAmbiguous_String;
GUIString RoutingLogic_InvalidCommand_String;
GUIString RoutingLogic_RequireName_String;
GUIString RoutingLogic_BadTimeSpec_String;
GUIString RoutingLogic_Error_String;
GUIString RoutingLogic_NotCaptain_String;

GUIString ServerListCtrl_Pinging_String;
GUIString ServerListCtrl_PingFailed_String;
GUIString ServerListCtrl_Querying_String;
GUIString ServerListCtrl_QueryFailed_String;
GUIString ServerListCtrl_ListSummary_String;
GUIString ServerListCtrl_ListPingSummary_String;
GUIString ServerListCtrl_RefreshList_String;
GUIString ServerListCtrl_StopRefresh_String;
GUIString ServerListCtrl_NoFilter_String;
GUIString ServerListCtrl_AddRemove_String;
GUIString ServerListCtrl_Add_String;
GUIString ServerListCtrl_Remove_String;
GUIString ServerListCtrl_FullFilter_String;
GUIString ServerListCtrl_NotFullFilter_String;
GUIString ServerListCtrl_OpenFilter_String;
ImagePtr ServerListCtrl_PingFailed_Image;
ImagePtr ServerListCtrl_GameExpanded_Image;
ImagePtr ServerListCtrl_GameNotExpanded_Image;
ImagePtr ServerListCtrl_GameAreInvited_Image;
ImagePtr ServerListCtrl_GameNone_Image;
ImagePtr ServerListCtrl_GameAskToJoin_Image;
ImagePtr ServerListCtrl_GameHasPassword_Image;
ImagePtr ServerListCtrl_GameNotInvited_Image;
ImagePtr ServerListCtrl_GameInProgress_Image;
ImagePtr ServerListCtrl_SkillOpen_Image;
ImagePtr ServerListCtrl_SkillNovice_Image;
ImagePtr ServerListCtrl_SkillIntermediate_Image;
ImagePtr ServerListCtrl_SkillAdvanced_Image;
ImagePtr ServerListCtrl_SkillExpert_Image;

GUIString ServerOptionsCtrl_Connect_String;
GUIString ServerOptionsCtrl_RefreshServer_String;
GUIString ServerOptionsCtrl_HideDetails_String;
GUIString ServerOptionsCtrl_ShowDetails_String;
GUIString ServerOptionsCtrl_ResfreshList_String;

GUIString StagingLogic_DisconnectedTitle_String;
GUIString StagingLogic_DisconnectedDetails_String;
GUIString StagingLogic_BannedTitle_String;
GUIString StagingLogic_BannedDetails_String;
GUIString StagingLogic_KickedTitle_String;
GUIString StagingLogic_KickedDetails_String;

GUIString UserListCtrl_Ignore_String;
GUIString UserListCtrl_Unignore_String;
ImagePtr UserListCtrl_Normal_Image;
ImagePtr UserListCtrl_Away_Image;
ImagePtr UserListCtrl_Admin_Image;
ImagePtr UserListCtrl_Moderator_Image;
ImagePtr UserListCtrl_Captain_Image;
ImagePtr UserListCtrl_Muted_Image;
ImagePtr UserListCtrl_Ignored_Image;
ImagePtr UserListCtrl_Blocked_Image;

GUIString WONStatus_Success_String;
GUIString WONStatus_Killed_String;
GUIString WONStatus_TimedOut_String;
GUIString WONStatus_InvalidAddress_String;
GUIString WONStatus_GeneralFailure_String;
GUIString WONStatus_AuthFailure_String;
GUIString WONStatus_FTPFailure_String;
GUIString WONStatus_HTTPFailure_String;
GUIString WONStatus_MOTDFailure_String;
GUIString WONStatus_RoutingFailure_String;
GUIString WONStatus_GameSpyFailure_String;
GUIString WONStatus_DirFailure_String;
GUIString WONStatus_ChatFailure_String;
GUIString WONStatus_DatabaseFailure_String;
GUIString WONStatus_NotAllowed_String;
GUIString WONStatus_NotAuthorized_String;
GUIString WONStatus_BehindFirewall_String;
GUIString WONStatus_ExpiredCertificate_String;
GUIString WONStatus_InvalidCDKey_String;
GUIString WONStatus_CDKeyBanned_String;
GUIString WONStatus_CDKeyInUse_String;
GUIString WONStatus_CRCFailed_String;
GUIString WONStatus_UserExists_String;
GUIString WONStatus_UserNotFound_String;
GUIString WONStatus_BadPassword_String;
GUIString WONStatus_InvalidUserName_String;
GUIString WONStatus_BadCommunity_String;
GUIString WONStatus_NotInCommunity_String;
GUIString WONStatus_UserSeqInUse_String;
GUIString WONStatus_KeyNotActive_String;
GUIString WONStatus_KeyExpired_String;
GUIString WONStatus_BlankNamesNotAllowed_String;
GUIString WONStatus_CaptainRejectedYou_String;
GUIString WONStatus_ClientAlreadyBanned_String;
GUIString WONStatus_ClientAlreadyInGroup_String;
GUIString WONStatus_ClientAlreadyMuted_String;
GUIString WONStatus_ClientBanned_String;
GUIString WONStatus_ClientDoesNotExist_String;
GUIString WONStatus_ClientNotBanned_String;
GUIString WONStatus_ClientNotInGroup_String;
GUIString WONStatus_ClientNotInvited_String;
GUIString WONStatus_ClientNotMuted_String;
GUIString WONStatus_ConnectFailure_String;
GUIString WONStatus_GroupAlreadyClaimed_String;
GUIString WONStatus_GroupAlreadyExists_String;
GUIString WONStatus_GroupClosed_String;
GUIString WONStatus_GroupDeleted_String;
GUIString WONStatus_GroupDoesNotExist_String;
GUIString WONStatus_GroupFull_String;
GUIString WONStatus_GuestNamesReserved_String;
GUIString WONStatus_InvalidPassword_String;
GUIString WONStatus_MustBeAdmin_String;
GUIString WONStatus_MustBeCaptain_String;
GUIString WONStatus_MustBeCaptainOrModerator_String;
GUIString WONStatus_MustBeModerator_String;
GUIString WONStatus_ServerFull_String;
GUIString WONStatus_GroupNameExceedsMaximum_String;
GUIString WONStatus_JoinCanceled_String;
GUIString WONStatus_Throttled_String;
GUIString WONStatus_KeyNotUsed_String;
GUIString WONStatus_EMailPasswordDuplication_String;
GUIString WONStatus_AccountCreateDisabled_String;
GUIString WONStatus_AgeNotOldEnough_String;
GUIString WONStatus_ItemHasDependencies_String;
GUIString WONStatus_OutOfDate_String;
GUIString WONStatus_OutOfDateNoUpdate_String;
GUIString WONStatus_ValidNotLatest_String;
GUIString WONStatus_DataAlreadyExists_String;
GUIString WONStatus_DBError_String;
GUIString WONStatus_ConnRefused_String;

ResourceConfigTablePtr LobbyResource_Init(WONAPI::ResourceConfigTable *theTable)
{
	ResourceConfigTablePtr aTable = theTable;
	if(theTable==NULL)
		aTable = theTable = new ResourceConfigTable;

	theTable->RegisterGlueResourceFunc(LobbyResource_SafeGlueResources);

	theTable->RegisterControlId("ShowCreateAccount",ID_ShowCreateAccount);
	theTable->RegisterControlId("ShowLostPassword",ID_ShowLostPassword);
	theTable->RegisterControlId("ShowLostUserName",ID_ShowLostUserName);
	theTable->RegisterControlId("ShowMOTD",ID_ShowMOTD);
	theTable->RegisterControlId("ShowTOU",ID_ShowTOU);
	theTable->RegisterControlId("LoginScreenBack",ID_LoginScreenBack);
	theTable->RegisterControlId("ShowLoginHelp",ID_ShowLoginHelp);
	theTable->RegisterControlId("ShowInternetScreen",ID_ShowInternetScreen);
	theTable->RegisterControlId("ShowLanScreen",ID_ShowLanScreen);
	theTable->RegisterControlId("Logout",ID_Logout);
	theTable->RegisterControlId("HostGame",ID_HostGame);
	theTable->RegisterControlId("ChangeRoom",ID_ChangeRoom);
	theTable->RegisterControlId("JoinGame",ID_JoinGame);
	theTable->RegisterControlId("LeaveGame",ID_LeaveGame);
	theTable->RegisterControlId("StartGame",ID_StartGame);
	theTable->RegisterControlId("RefreshGameList",ID_RefreshGameList);
	theTable->RegisterControlId("AddFriend",ID_AddFriend);
	theTable->RegisterControlId("RefreshFriends",ID_RefreshFriends);
	theTable->RegisterControlId("AddLANName",ID_AddLANName);
	theTable->RegisterControlId("ChangeLANBroadcast",ID_ChangeLANBroadcast);
	theTable->RegisterControlId("FilterDirtyWordsCheck",ID_FilterDirtyWordsCheck);
	theTable->RegisterControlId("AnonymousToFriendsCheck",ID_AnonymousToFriendsCheck);
	theTable->RegisterControlId("ChooseNetworkAdapter",ID_ChooseNetworkAdapter);

	return aTable;
}

static void *gResources[] =
{
	&LobbyGlobal_Cancel_String,
	&LobbyGlobal_Lan_String,
	&LobbyGlobal_No_String,
	&LobbyGlobal_OK_String,
	&LobbyGlobal_Yes_String,
	&LobbyGlobal_QuickPlay_String,
	&LobbyGlobal_Searching_Image,
	&LobbyGlobal_Firewall_Image,
	&LobbyGlobal_ButtonClick_Sound,
	&LobbyGlobal_LobbyMusic_Sound,
	&LobbyGlobal_AllReady_Sound,
	&LobbyGlobal_AllNotReady_Sound,
	&LobbyGlobal_AskToJoinRecv_Sound,
	&LobbyGlobal_Error_Sound,
	&LobbyGlobal_FriendJoined_Sound,
	&LobbyGlobal_FriendLeft_Sound,
	&LobbyGlobal_InviteRecv_Sound,
	&LobbyGlobal_PlayerJoined_Sound,
	&LobbyGlobal_SierraLogon_Sound,
	&LobbyGlobal_WhisperRecv_Sound,

	&AccountLogic_InvalidCDKeyTitle_String,
	&AccountLogic_InvalidCDKeyDetails_String,

	&AddFriendDialog_StatusTitle_String,
	&AddFriendDialog_StatusDetails_String,

	&AdminActionDialog_Warn_String,
	&AdminActionDialog_Mute_String,
	&AdminActionDialog_UnMute_String,
	&AdminActionDialog_Kick_String,
	&AdminActionDialog_Ban_String,
	&AdminActionDialog_ExplanationOpt_String,
	&AdminActionDialog_ExplanationReq_String,

	&BadUserDialog_OneDay_String,
	&BadUserDialog_OneHour_String,
	&BadUserDialog_OneMinute_String,
	&BadUserDialog_OneSecond_String,
	&BadUserDialog_XDays_String,
	&BadUserDialog_XHours_String,
	&BadUserDialog_XMinutes_String,
	&BadUserDialog_XSeconds_String,
	&BadUserDialog_Infinite_String,

	&ChatCommandLogic_Whisper_String,
	&ChatCommandLogic_Block_String,
	&ChatCommandLogic_Emote_String,
	&ChatCommandLogic_Ignore_String,
	&ChatCommandLogic_Away_String,
	&ChatCommandLogic_Invite_String,
	&ChatCommandLogic_Uninvite_String,
	&ChatCommandLogic_Reply_String,
	&ChatCommandLogic_Clear_String,
	&ChatCommandLogic_Help_String,
	&ChatCommandLogic_ShowTeam_String,
	&ChatCommandLogic_Moderator_String,
	&ChatCommandLogic_ServerMute_String,
	&ChatCommandLogic_ServerBan_String,
	&ChatCommandLogic_Mute_String,
	&ChatCommandLogic_Ban_String,
	&ChatCommandLogic_Unmute_String,
	&ChatCommandLogic_ServerUnmute_String,
	&ChatCommandLogic_Unban_String,
	&ChatCommandLogic_ServerUnban_String,
	&ChatCommandLogic_Alert_String,
	&ChatCommandLogic_Warn_String,
	&ChatCommandLogic_HelpHeader_String,
	&ChatCommandLogic_HelpHelp_String,
	&ChatCommandLogic_ShowTeamHelp_String,
	&ChatCommandLogic_WhisperHelp_String,
	&ChatCommandLogic_BlockHelp_String,
	&ChatCommandLogic_IgnoreHelp_String,
	&ChatCommandLogic_ReplyHelp_String,
	&ChatCommandLogic_EmoteHelp_String,
	&ChatCommandLogic_AwayHelp_String,
	&ChatCommandLogic_ClearHelp_String,
	&ChatCommandLogic_InviteHelp_String,
	&ChatCommandLogic_UninviteHelp_String,
	&ChatCommandLogic_MuteHelp_String,
	&ChatCommandLogic_UnmuteHelp_String,
	&ChatCommandLogic_BanHelp_String,
	&ChatCommandLogic_UnbanHelp_String,
	&ChatCommandLogic_ModeratorHelp_String,
	&ChatCommandLogic_ServerMuteHelp_String,
	&ChatCommandLogic_ServerUnmuteHelp_String,
	&ChatCommandLogic_ServerBanHelp_String,
	&ChatCommandLogic_ServerUnbanHelp_String,
	&ChatCommandLogic_AlertHelp_String,
	&ChatCommandLogic_WarnHelp_String,

	&ChatCtrl_NormalChat_String,
	&ChatCtrl_Emote_String,
	&ChatCtrl_TeamChat_String,
	&ChatCtrl_TeamEmote_String,
	&ChatCtrl_YouWhisperToYourself_String,
	&ChatCtrl_YouWhisper_String,
	&ChatCtrl_SomeoneWhispersToYou_String,
	&ChatCtrl_YouWarnSomeone_String,
	&ChatCtrl_YouWarnYourself_String,
	&ChatCtrl_SomeoneWarnsYou_String,
	&ChatCtrl_SomeoneWarnsSomeone_String,
	&ChatCtrl_EnteringRoom_String,
	&ChatCtrl_LeavingRoom_String,
	&ChatCtrl_Disconnected_String,
	&ChatCtrl_YouWereKicked_String,
	&ChatCtrl_YouWereBannedMinute_String,
	&ChatCtrl_YouWereBannedMinutes_String,
	&ChatCtrl_YouWereBannedHour_String,
	&ChatCtrl_YouWereBannedHours_String,
	&ChatCtrl_YouWereBannedDay_String,
	&ChatCtrl_YouWereBannedDays_String,
	&ChatCtrl_YouWereBannedIndefinitely_String,
	&ChatCtrl_SomeoneWasKicked_String,
	&ChatCtrl_SomeoneWasBannedMinute_String,
	&ChatCtrl_SomeoneWasBannedMinutes_String,
	&ChatCtrl_SomeoneWasBannedHour_String,
	&ChatCtrl_SomeoneWasBannedHours_String,
	&ChatCtrl_SomeoneWasBannedDay_String,
	&ChatCtrl_SomeoneWasBannedDays_String,
	&ChatCtrl_SomeoneWasBannedIndefinitely_String,
	&ChatCtrl_YouWereUnmuted_String,
	&ChatCtrl_YouWereMutedMinute_String,
	&ChatCtrl_YouWereMutedMinutes_String,
	&ChatCtrl_YouWereMutedHour_String,
	&ChatCtrl_YouWereMutedHours_String,
	&ChatCtrl_YouWereMutedDay_String,
	&ChatCtrl_YouWereMutedDays_String,
	&ChatCtrl_YouWereMutedIndefinitely_String,
	&ChatCtrl_SomeoneWasUnmuted_String,
	&ChatCtrl_SomeoneWasMutedMinute_String,
	&ChatCtrl_SomeoneWasMutedMinutes_String,
	&ChatCtrl_SomeoneWasMutedHour_String,
	&ChatCtrl_SomeoneWasMutedHours_String,
	&ChatCtrl_SomeoneWasMutedDay_String,
	&ChatCtrl_SomeoneWasMutedDays_String,
	&ChatCtrl_SomeoneWasMutedIndefinitely_String,
	&ChatCtrl_ModeratorComment_String,
	&ChatCtrl_AlertComment_String,
	&ChatCtrl_ServerAlert_String,
	&ChatCtrl_ServerShutdownStartedMinute_String,
	&ChatCtrl_ServerShutdownStartedMinutes_String,
	&ChatCtrl_ServerShutdownStartedHour_String,
	&ChatCtrl_ServerShutdownStartedHours_String,
	&ChatCtrl_ServerShutdownStartedDay_String,
	&ChatCtrl_ServerShutdownStartedDays_String,
	&ChatCtrl_ServerShutdownAborted_String,
	&ChatCtrl_SomeoneInvitesYou_String,
	&ChatCtrl_SomeoneUninvitesYou_String,
	&ChatCtrl_YouInviteSomeone_String,
	&ChatCtrl_YouUninviteSomeone_String,
	&ChatCtrl_SomeoneInvitesYouExpl_String,
	&ChatCtrl_SomeoneUninvitesYouExpl_String,
	&ChatCtrl_YouInviteSomeoneExpl_String,
	&ChatCtrl_YouUninviteSomeoneExpl_String,
	&ChatCtrl_Accept_String,
	&ChatCtrl_Reject_String,
	&ChatCtrl_SomeoneAsksToJoin_String,
	&ChatCtrl_SomeoneAsksToJoinExpl_String,
	&ChatCtrl_ClientNotOnServer_String,
	&ChatCtrl_ClientInGame_String,
	&ChatCtrl_ClientInChat_String,
	&ChatCtrl_ClientInGameAndChat_String,
	&ChatCtrl_ClientIsAnonymous_String,
	&ChatCtrl_FriendIsChatting_String,
	&ChatCtrl_FriendIsInGameStaging_String,
	&ChatCtrl_FriendIsPlaying_String,
	&ChatCtrl_FriendIsOn_String,
	&ChatCtrl_FriendIsNotOnline_String,
	&ChatCtrl_PlayerKicked_String,
	&ChatCtrl_PlayerBanned_String,
	&ChatCtrl_FriendJoined_String,
	&ChatCtrl_FriendLeft_String,
	&ChatCtrl_TeamMemberJoined_String,
	&ChatCtrl_TeamMemberLeft_String,
	&ChatCtrl_TeamMembers_String,
	&ChatCtrl_IgnoreList_String,

	&ChooseNetworkAdapterDialog_Default_String,
	&ChooseNetworkAdapterDialog_Unknown_String,

	&ClientOptionsCtrl_Whisper_String,
	&ClientOptionsCtrl_Invite_String,
	&ClientOptionsCtrl_Uninvite_String,
	&ClientOptionsCtrl_AddToFriends_String,
	&ClientOptionsCtrl_RemoveFromFriends_String,
	&ClientOptionsCtrl_ClearAFK_String,
	&ClientOptionsCtrl_SetAFK_String,
	&ClientOptionsCtrl_Ignore_String,
	&ClientOptionsCtrl_Unignore_String,
	&ClientOptionsCtrl_Block_String,
	&ClientOptionsCtrl_Unblock_String,
	&ClientOptionsCtrl_Warn_String,
	&ClientOptionsCtrl_Mute_String,
	&ClientOptionsCtrl_UnMute_String,
	&ClientOptionsCtrl_Kick_String,
	&ClientOptionsCtrl_Ban_String,

	&DirectConnectionOptionsCtrl_Connect_String,
	&DirectConnectionOptionsCtrl_Refresh_String,
	&DirectConnectionOptionsCtrl_Remove_String,

	&FriendOptionsCtrl_Locate_String,
	&FriendOptionsCtrl_RefreshList_String,
	&FriendOptionsCtrl_RemoveFromFriends_String,

	&FriendsListCtrl_Unknown_Image,
	&FriendsListCtrl_NotFound_Image,
	&FriendsListCtrl_InChat_Image,
	&FriendsListCtrl_InGame_Image,

	&GameOptionsPopup_Join_String,
	&GameOptionsPopup_Ping_String,
	&GameOptionsPopup_QueryDetails_String,
	&GameOptionsPopup_HideDetails_String,
	&GameOptionsPopup_ShowDetails_String,
	&GameOptionsPopup_Remove_String,

	&GameStagingCtrl_Ready_String,
	&GameStagingCtrl_NotReady_String,
	&GameStagingCtrl_StartGame_String,
	&GameStagingCtrl_LeaveGame_String,
	&GameStagingCtrl_DissolveGame_String,

	&GameStagingLogic_KickedTitle_String,
	&GameStagingLogic_KickedDetails_String,
	&GameStagingLogic_BannedTitle_String,
	&GameStagingLogic_BannedDetails_String,
	&GameStagingLogic_ConnectWarningTitle1_String,
	&GameStagingLogic_ConnectWarningDetails1_String,
	&GameStagingLogic_ConnectWarningTitle2_String,
	&GameStagingLogic_ConnectWarningDetails2_String,
	&GameStagingLogic_ConnectWarningTitle3_String,
	&GameStagingLogic_ConnectWarningDetails3_String,
	&GameStagingLogic_NoPingReplyError_String,
	&GameStagingLogic_GettingGameInfoTitle_String,
	&GameStagingLogic_GettingGameInfoDetails_String,
	&GameStagingLogic_JoinGameErrorTitle_String,
	&GameStagingLogic_VerifyingPingTitle_String,
	&GameStagingLogic_VerifyingPingDetails_String,
	&GameStagingLogic_DissolveGame_String,
	&GameStagingLogic_DissolveGameConfirm_String,
	&GameStagingLogic_LeaveGame_String,
	&GameStagingLogic_LeaveGameConfirm_String,
	&GameStagingLogic_NoQuickPlayGameTitle_String,
	&GameStagingLogic_NoQuickPlayGameDetails_String,

	&GameUserListCtrl_Ban_String,
	&GameUserListCtrl_Kick_String,
	&GameUserListCtrl_Ready_Image,

	&InternetQuickPlay_Welcome_String,
	&InternetQuickPlay_UserWelcome_String,

	&JoinResponse_Joined_String,
	&JoinResponse_ServerFull_String,
	&JoinResponse_InviteOnly_String,
	&JoinResponse_AskToJoin_String,
	&JoinResponse_GameInProgress_String,
	&JoinResponse_InvalidPassword_String,
	&JoinResponse_NotOnLAN_String,
	&JoinResponse_Blocked_String,
	&JoinResponse_Unknown_String,

	&LANLogic_SocketBindError_String,
	&LANLogic_SocketListenError_String,
	&LANLogic_ConnectToGameTitle_String,
	&LANLogic_ConnectToGameDetails_String,
	&LANLogic_CheckingAddressTitle_String,
	&LANLogic_CheckingAddressDetails_String,
	&LANLogic_RegisterTitle_String,
	&LANLogic_RegisterDetails_String,
	&LANLogic_JoinFailure_String,
	&LANLogic_JoinReplyError_String,
	&LANLogic_InvalidSubnet_String,

	&LobbyContainer_InitStart_String,
	&LobbyContainer_InitFailed_String,
	&LobbyContainer_CheckingVersion_String,
	&LobbyContainer_QueryingServers_String,
	&LobbyContainer_NoConnection_String,
	&LobbyContainer_DNSLookupFailed_String,
	&LobbyContainer_DirLookupFailure_String,
	&LobbyContainer_VersionCheckFailure_String,
	&LobbyContainer_InitComplete_String,
	&LobbyContainer_NoServers_String,
	&LobbyContainer_AuthServers_String,
	&LobbyContainer_AccountServers_String,
	&LobbyContainer_VersionServers_String,
	&LobbyContainer_CreateAccountTitle_String,
	&LobbyContainer_CreateAccountDetails_String,
	&LobbyContainer_RetrievePasswordTitle_String,
	&LobbyContainer_RetrievePasswordDetails_String,
	&LobbyContainer_RetrieveUsernameTitle_String,
	&LobbyContainer_RetrieveUsernameDetails_String,
	&LobbyContainer_QueryAccountTitle_String,
	&LobbyContainer_QueryAccountDetails_String,
	&LobbyContainer_UpdateAccountTitle_String,
	&LobbyContainer_UpdateAccountDetails_String,
	&LobbyContainer_LoginTitle_String,
	&LobbyContainer_LoginDetails_String,
	&LobbyContainer_GettingRoomsTitle_String,
	&LobbyContainer_GettingRoomsDetails_String,
	&LobbyContainer_VersionOutOfDataTitle_String,
	&LobbyContainer_VersionOutOfDataDetails_String,

	&LobbyGameStatus_Success_String,
	&LobbyGameStatus_GameFull_String,
	&LobbyGameStatus_GameInProgress_String,
	&LobbyGameStatus_UnpackFailure_String,
	&LobbyGameStatus_NotInvited_String,
	&LobbyGameStatus_CaptainRejectedYou_String,
	&LobbyGameStatus_InvalidPassword_String,
	&LobbyGameStatus_DuplicateName_String,

	&LobbyOptions_DefaultName_String,
	&LobbyOptions_CheckingAddress_String,
	&LobbyOptions_AddressUnavailable_String,

	&RoomCtrl_QueryingServer_String,
	&RoomCtrl_Password_Image,

	&RoomLabelCtrl_LabelPrefix_String,
	&RoomLabelCtrl_LabelServer_String,
	&RoomLabelCtrl_LabelUser_String,
	&RoomLabelCtrl_LabelUsers_String,
	&RoomLabelCtrl_LabelDisconnected_String,

	&RoutingLogic_GetClientListTitle_String,
	&RoutingLogic_GetClientListDetails_String,
	&RoutingLogic_GetGroupListTitle_String,
	&RoutingLogic_GetGroupListDetails_String,
	&RoutingLogic_GameDissolvedTitle_String,
	&RoutingLogic_GameDissolvedDetails_String,
	&RoutingLogic_NoServersTitle_String,
	&RoutingLogic_NoServersDetails_String,
	&RoutingLogic_TooManyFriendsTitle_String,
	&RoutingLogic_TooManyFriendsDetails_String,
	&RoutingLogic_AlreadyJoinedTitle_String,
	&RoutingLogic_AlreadyInGame_String,
	&RoutingLogic_AlreadyInRoom_String,
	&RoutingLogic_JoinGroupTitle_String,
	&RoutingLogic_JoinGroupDetails_String,
	&RoutingLogic_ConnectTitle_String,
	&RoutingLogic_ConnectDetails_String,
	&RoutingLogic_RegisterTitle_String,
	&RoutingLogic_RegisterDetails_String,
	&RoutingLogic_QueryGroupsTitle_String,
	&RoutingLogic_QueryGroupsDetails_String,
	&RoutingLogic_CreateGroupTitle_String,
	&RoutingLogic_CreateGroupDetails_String,
	&RoutingLogic_GetBadUsersTitle_String,
	&RoutingLogic_GetBadUsersDetails_String,
	&RoutingLogic_DisconnectedTitle_String,
	&RoutingLogic_DisconnectedDetails_String,
	&RoutingLogic_AskingCaptainTitle_String,
	&RoutingLogic_AskingCaptainDetails_String,
	&RoutingLogic_CollectingPingsTitle_String,
	&RoutingLogic_CollectingPingsDetails_String,
	&RoutingLogic_GettingGameListTitle_String,
	&RoutingLogic_GettingGameListDetails_String,
	&RoutingLogic_MuteClientTitle_String,
	&RoutingLogic_UnmuteClientTitle_String,
	&RoutingLogic_BanClientTitle_String,
	&RoutingLogic_KickClientTitle_String,
	&RoutingLogic_ModerateDetails_String,
	&RoutingLogic_FriendNotFoundTitle_String,
	&RoutingLogic_FriendNotFoundDetails_String,
	&RoutingLogic_ClientNotFound_String,
	&RoutingLogic_ClientAmbiguous_String,
	&RoutingLogic_InvalidCommand_String,
	&RoutingLogic_RequireName_String,
	&RoutingLogic_BadTimeSpec_String,
	&RoutingLogic_Error_String,
	&RoutingLogic_NotCaptain_String,

	&ServerListCtrl_Pinging_String,
	&ServerListCtrl_PingFailed_String,
	&ServerListCtrl_Querying_String,
	&ServerListCtrl_QueryFailed_String,
	&ServerListCtrl_ListSummary_String,
	&ServerListCtrl_ListPingSummary_String,
	&ServerListCtrl_RefreshList_String,
	&ServerListCtrl_StopRefresh_String,
	&ServerListCtrl_NoFilter_String,
	&ServerListCtrl_AddRemove_String,
	&ServerListCtrl_Add_String,
	&ServerListCtrl_Remove_String,
	&ServerListCtrl_FullFilter_String,
	&ServerListCtrl_NotFullFilter_String,
	&ServerListCtrl_OpenFilter_String,
	&ServerListCtrl_PingFailed_Image,
	&ServerListCtrl_GameExpanded_Image,
	&ServerListCtrl_GameNotExpanded_Image,
	&ServerListCtrl_GameAreInvited_Image,
	&ServerListCtrl_GameNone_Image,
	&ServerListCtrl_GameAskToJoin_Image,
	&ServerListCtrl_GameHasPassword_Image,
	&ServerListCtrl_GameNotInvited_Image,
	&ServerListCtrl_GameInProgress_Image,
	&ServerListCtrl_SkillOpen_Image,
	&ServerListCtrl_SkillNovice_Image,
	&ServerListCtrl_SkillIntermediate_Image,
	&ServerListCtrl_SkillAdvanced_Image,
	&ServerListCtrl_SkillExpert_Image,

	&ServerOptionsCtrl_Connect_String,
	&ServerOptionsCtrl_RefreshServer_String,
	&ServerOptionsCtrl_HideDetails_String,
	&ServerOptionsCtrl_ShowDetails_String,
	&ServerOptionsCtrl_ResfreshList_String,

	&StagingLogic_DisconnectedTitle_String,
	&StagingLogic_DisconnectedDetails_String,
	&StagingLogic_BannedTitle_String,
	&StagingLogic_BannedDetails_String,
	&StagingLogic_KickedTitle_String,
	&StagingLogic_KickedDetails_String,

	&UserListCtrl_Ignore_String,
	&UserListCtrl_Unignore_String,
	&UserListCtrl_Normal_Image,
	&UserListCtrl_Away_Image,
	&UserListCtrl_Admin_Image,
	&UserListCtrl_Moderator_Image,
	&UserListCtrl_Captain_Image,
	&UserListCtrl_Muted_Image,
	&UserListCtrl_Ignored_Image,
	&UserListCtrl_Blocked_Image,

	&WONStatus_Success_String,
	&WONStatus_Killed_String,
	&WONStatus_TimedOut_String,
	&WONStatus_InvalidAddress_String,
	&WONStatus_GeneralFailure_String,
	&WONStatus_AuthFailure_String,
	&WONStatus_FTPFailure_String,
	&WONStatus_HTTPFailure_String,
	&WONStatus_MOTDFailure_String,
	&WONStatus_RoutingFailure_String,
	&WONStatus_GameSpyFailure_String,
	&WONStatus_DirFailure_String,
	&WONStatus_ChatFailure_String,
	&WONStatus_DatabaseFailure_String,
	&WONStatus_NotAllowed_String,
	&WONStatus_NotAuthorized_String,
	&WONStatus_BehindFirewall_String,
	&WONStatus_ExpiredCertificate_String,
	&WONStatus_InvalidCDKey_String,
	&WONStatus_CDKeyBanned_String,
	&WONStatus_CDKeyInUse_String,
	&WONStatus_CRCFailed_String,
	&WONStatus_UserExists_String,
	&WONStatus_UserNotFound_String,
	&WONStatus_BadPassword_String,
	&WONStatus_InvalidUserName_String,
	&WONStatus_BadCommunity_String,
	&WONStatus_NotInCommunity_String,
	&WONStatus_UserSeqInUse_String,
	&WONStatus_KeyNotActive_String,
	&WONStatus_KeyExpired_String,
	&WONStatus_BlankNamesNotAllowed_String,
	&WONStatus_CaptainRejectedYou_String,
	&WONStatus_ClientAlreadyBanned_String,
	&WONStatus_ClientAlreadyInGroup_String,
	&WONStatus_ClientAlreadyMuted_String,
	&WONStatus_ClientBanned_String,
	&WONStatus_ClientDoesNotExist_String,
	&WONStatus_ClientNotBanned_String,
	&WONStatus_ClientNotInGroup_String,
	&WONStatus_ClientNotInvited_String,
	&WONStatus_ClientNotMuted_String,
	&WONStatus_ConnectFailure_String,
	&WONStatus_GroupAlreadyClaimed_String,
	&WONStatus_GroupAlreadyExists_String,
	&WONStatus_GroupClosed_String,
	&WONStatus_GroupDeleted_String,
	&WONStatus_GroupDoesNotExist_String,
	&WONStatus_GroupFull_String,
	&WONStatus_GuestNamesReserved_String,
	&WONStatus_InvalidPassword_String,
	&WONStatus_MustBeAdmin_String,
	&WONStatus_MustBeCaptain_String,
	&WONStatus_MustBeCaptainOrModerator_String,
	&WONStatus_MustBeModerator_String,
	&WONStatus_ServerFull_String,
	&WONStatus_GroupNameExceedsMaximum_String,
	&WONStatus_JoinCanceled_String,
	&WONStatus_Throttled_String,
	&WONStatus_KeyNotUsed_String,
	&WONStatus_EMailPasswordDuplication_String,
	&WONStatus_AccountCreateDisabled_String,
	&WONStatus_AgeNotOldEnough_String,
	&WONStatus_ItemHasDependencies_String,
	&WONStatus_OutOfDate_String,
	&WONStatus_OutOfDateNoUpdate_String,
	&WONStatus_ValidNotLatest_String,
	&WONStatus_DataAlreadyExists_String,
	&WONStatus_DBError_String,
	&WONStatus_ConnRefused_String,


	NULL
};
const GUIString& LobbyResource_GetString(LobbyResourceId theId) { return *(GUIString*)gResources[theId]; }
Image* LobbyResource_GetImage(LobbyResourceId theId) { return *(ImagePtr*)gResources[theId]; }
Sound* LobbyResource_GetSound(LobbyResourceId theId) { return *(SoundPtr*)gResources[theId]; }
Font* LobbyResource_GetFont(LobbyResourceId theId) { return *(FontPtr*)gResources[theId]; }
const Background& LobbyResource_GetBackground(LobbyResourceId theId) { return *(Background*)gResources[theId]; }
DWORD LobbyResource_GetColor(LobbyResourceId theId) { return *(DWORD*)gResources[theId]; }
int LobbyResource_GetInt(LobbyResourceId theId) { return *(int*)gResources[theId]; }
void LobbyResource_SafeGlueResources(ResourceConfigTable *theTable)
{
	ResourceConfig *aConfig;
	aConfig=theTable->SafeGetConfig("LobbyGlobal");
	LobbyGlobal_Cancel_String = aConfig->SafeGetString("Cancel");
	LobbyGlobal_Lan_String = aConfig->SafeGetString("Lan");
	LobbyGlobal_No_String = aConfig->SafeGetString("No");
	LobbyGlobal_OK_String = aConfig->SafeGetString("OK");
	LobbyGlobal_Yes_String = aConfig->SafeGetString("Yes");
	LobbyGlobal_QuickPlay_String = aConfig->SafeGetString("QuickPlay");
	LobbyGlobal_Searching_Image = aConfig->SafeGetImage("Searching");
	LobbyGlobal_Firewall_Image = aConfig->SafeGetImage("Firewall");
	LobbyGlobal_ButtonClick_Sound = aConfig->SafeGetSound("ButtonClick");
	LobbyGlobal_LobbyMusic_Sound = aConfig->SafeGetSound("LobbyMusic");
	LobbyGlobal_AllReady_Sound = aConfig->SafeGetSound("AllReady");
	LobbyGlobal_AllNotReady_Sound = aConfig->SafeGetSound("AllNotReady");
	LobbyGlobal_AskToJoinRecv_Sound = aConfig->SafeGetSound("AskToJoinRecv");
	LobbyGlobal_Error_Sound = aConfig->SafeGetSound("Error");
	LobbyGlobal_FriendJoined_Sound = aConfig->SafeGetSound("FriendJoined");
	LobbyGlobal_FriendLeft_Sound = aConfig->SafeGetSound("FriendLeft");
	LobbyGlobal_InviteRecv_Sound = aConfig->SafeGetSound("InviteRecv");
	LobbyGlobal_PlayerJoined_Sound = aConfig->SafeGetSound("PlayerJoined");
	LobbyGlobal_SierraLogon_Sound = aConfig->SafeGetSound("SierraLogon");
	LobbyGlobal_WhisperRecv_Sound = aConfig->SafeGetSound("WhisperRecv");

	aConfig=theTable->SafeGetConfig("AccountLogic");
	AccountLogic_InvalidCDKeyTitle_String = aConfig->SafeGetString("InvalidCDKeyTitle");
	AccountLogic_InvalidCDKeyDetails_String = aConfig->SafeGetString("InvalidCDKeyDetails");

	aConfig=theTable->SafeGetConfig("AddFriendDialog");
	AddFriendDialog_StatusTitle_String = aConfig->SafeGetString("StatusTitle");
	AddFriendDialog_StatusDetails_String = aConfig->SafeGetString("StatusDetails");

	aConfig=theTable->SafeGetConfig("AdminActionDialog");
	AdminActionDialog_Warn_String = aConfig->SafeGetString("Warn");
	AdminActionDialog_Mute_String = aConfig->SafeGetString("Mute");
	AdminActionDialog_UnMute_String = aConfig->SafeGetString("UnMute");
	AdminActionDialog_Kick_String = aConfig->SafeGetString("Kick");
	AdminActionDialog_Ban_String = aConfig->SafeGetString("Ban");
	AdminActionDialog_ExplanationOpt_String = aConfig->SafeGetString("ExplanationOpt");
	AdminActionDialog_ExplanationReq_String = aConfig->SafeGetString("ExplanationReq");

	aConfig=theTable->SafeGetConfig("BadUserDialog");
	BadUserDialog_OneDay_String = aConfig->SafeGetString("OneDay");
	BadUserDialog_OneHour_String = aConfig->SafeGetString("OneHour");
	BadUserDialog_OneMinute_String = aConfig->SafeGetString("OneMinute");
	BadUserDialog_OneSecond_String = aConfig->SafeGetString("OneSecond");
	BadUserDialog_XDays_String = aConfig->SafeGetString("XDays");
	BadUserDialog_XHours_String = aConfig->SafeGetString("XHours");
	BadUserDialog_XMinutes_String = aConfig->SafeGetString("XMinutes");
	BadUserDialog_XSeconds_String = aConfig->SafeGetString("XSeconds");
	BadUserDialog_Infinite_String = aConfig->SafeGetString("Infinite");

	aConfig=theTable->SafeGetConfig("ChatCommandLogic");
	ChatCommandLogic_Whisper_String = aConfig->SafeGetString("Whisper");
	ChatCommandLogic_Block_String = aConfig->SafeGetString("Block");
	ChatCommandLogic_Emote_String = aConfig->SafeGetString("Emote");
	ChatCommandLogic_Ignore_String = aConfig->SafeGetString("Ignore");
	ChatCommandLogic_Away_String = aConfig->SafeGetString("Away");
	ChatCommandLogic_Invite_String = aConfig->SafeGetString("Invite");
	ChatCommandLogic_Uninvite_String = aConfig->SafeGetString("Uninvite");
	ChatCommandLogic_Reply_String = aConfig->SafeGetString("Reply");
	ChatCommandLogic_Clear_String = aConfig->SafeGetString("Clear");
	ChatCommandLogic_Help_String = aConfig->SafeGetString("Help");
	ChatCommandLogic_ShowTeam_String = aConfig->SafeGetString("ShowTeam");
	ChatCommandLogic_Moderator_String = aConfig->SafeGetString("Moderator");
	ChatCommandLogic_ServerMute_String = aConfig->SafeGetString("ServerMute");
	ChatCommandLogic_ServerBan_String = aConfig->SafeGetString("ServerBan");
	ChatCommandLogic_Mute_String = aConfig->SafeGetString("Mute");
	ChatCommandLogic_Ban_String = aConfig->SafeGetString("Ban");
	ChatCommandLogic_Unmute_String = aConfig->SafeGetString("Unmute");
	ChatCommandLogic_ServerUnmute_String = aConfig->SafeGetString("ServerUnmute");
	ChatCommandLogic_Unban_String = aConfig->SafeGetString("Unban");
	ChatCommandLogic_ServerUnban_String = aConfig->SafeGetString("ServerUnban");
	ChatCommandLogic_Alert_String = aConfig->SafeGetString("Alert");
	ChatCommandLogic_Warn_String = aConfig->SafeGetString("Warn");
	ChatCommandLogic_HelpHeader_String = aConfig->SafeGetString("HelpHeader");
	ChatCommandLogic_HelpHelp_String = aConfig->SafeGetString("HelpHelp");
	ChatCommandLogic_ShowTeamHelp_String = aConfig->SafeGetString("ShowTeamHelp");
	ChatCommandLogic_WhisperHelp_String = aConfig->SafeGetString("WhisperHelp");
	ChatCommandLogic_BlockHelp_String = aConfig->SafeGetString("BlockHelp");
	ChatCommandLogic_IgnoreHelp_String = aConfig->SafeGetString("IgnoreHelp");
	ChatCommandLogic_ReplyHelp_String = aConfig->SafeGetString("ReplyHelp");
	ChatCommandLogic_EmoteHelp_String = aConfig->SafeGetString("EmoteHelp");
	ChatCommandLogic_AwayHelp_String = aConfig->SafeGetString("AwayHelp");
	ChatCommandLogic_ClearHelp_String = aConfig->SafeGetString("ClearHelp");
	ChatCommandLogic_InviteHelp_String = aConfig->SafeGetString("InviteHelp");
	ChatCommandLogic_UninviteHelp_String = aConfig->SafeGetString("UninviteHelp");
	ChatCommandLogic_MuteHelp_String = aConfig->SafeGetString("MuteHelp");
	ChatCommandLogic_UnmuteHelp_String = aConfig->SafeGetString("UnmuteHelp");
	ChatCommandLogic_BanHelp_String = aConfig->SafeGetString("BanHelp");
	ChatCommandLogic_UnbanHelp_String = aConfig->SafeGetString("UnbanHelp");
	ChatCommandLogic_ModeratorHelp_String = aConfig->SafeGetString("ModeratorHelp");
	ChatCommandLogic_ServerMuteHelp_String = aConfig->SafeGetString("ServerMuteHelp");
	ChatCommandLogic_ServerUnmuteHelp_String = aConfig->SafeGetString("ServerUnmuteHelp");
	ChatCommandLogic_ServerBanHelp_String = aConfig->SafeGetString("ServerBanHelp");
	ChatCommandLogic_ServerUnbanHelp_String = aConfig->SafeGetString("ServerUnbanHelp");
	ChatCommandLogic_AlertHelp_String = aConfig->SafeGetString("AlertHelp");
	ChatCommandLogic_WarnHelp_String = aConfig->SafeGetString("WarnHelp");

	aConfig=theTable->SafeGetConfig("ChatCtrl");
	ChatCtrl_NormalChat_String = aConfig->SafeGetString("NormalChat");
	ChatCtrl_Emote_String = aConfig->SafeGetString("Emote");
	ChatCtrl_TeamChat_String = aConfig->SafeGetString("TeamChat");
	ChatCtrl_TeamEmote_String = aConfig->SafeGetString("TeamEmote");
	ChatCtrl_YouWhisperToYourself_String = aConfig->SafeGetString("YouWhisperToYourself");
	ChatCtrl_YouWhisper_String = aConfig->SafeGetString("YouWhisper");
	ChatCtrl_SomeoneWhispersToYou_String = aConfig->SafeGetString("SomeoneWhispersToYou");
	ChatCtrl_YouWarnSomeone_String = aConfig->SafeGetString("YouWarnSomeone");
	ChatCtrl_YouWarnYourself_String = aConfig->SafeGetString("YouWarnYourself");
	ChatCtrl_SomeoneWarnsYou_String = aConfig->SafeGetString("SomeoneWarnsYou");
	ChatCtrl_SomeoneWarnsSomeone_String = aConfig->SafeGetString("SomeoneWarnsSomeone");
	ChatCtrl_EnteringRoom_String = aConfig->SafeGetString("EnteringRoom");
	ChatCtrl_LeavingRoom_String = aConfig->SafeGetString("LeavingRoom");
	ChatCtrl_Disconnected_String = aConfig->SafeGetString("Disconnected");
	ChatCtrl_YouWereKicked_String = aConfig->SafeGetString("YouWereKicked");
	ChatCtrl_YouWereBannedMinute_String = aConfig->SafeGetString("YouWereBannedMinute");
	ChatCtrl_YouWereBannedMinutes_String = aConfig->SafeGetString("YouWereBannedMinutes");
	ChatCtrl_YouWereBannedHour_String = aConfig->SafeGetString("YouWereBannedHour");
	ChatCtrl_YouWereBannedHours_String = aConfig->SafeGetString("YouWereBannedHours");
	ChatCtrl_YouWereBannedDay_String = aConfig->SafeGetString("YouWereBannedDay");
	ChatCtrl_YouWereBannedDays_String = aConfig->SafeGetString("YouWereBannedDays");
	ChatCtrl_YouWereBannedIndefinitely_String = aConfig->SafeGetString("YouWereBannedIndefinitely");
	ChatCtrl_SomeoneWasKicked_String = aConfig->SafeGetString("SomeoneWasKicked");
	ChatCtrl_SomeoneWasBannedMinute_String = aConfig->SafeGetString("SomeoneWasBannedMinute");
	ChatCtrl_SomeoneWasBannedMinutes_String = aConfig->SafeGetString("SomeoneWasBannedMinutes");
	ChatCtrl_SomeoneWasBannedHour_String = aConfig->SafeGetString("SomeoneWasBannedHour");
	ChatCtrl_SomeoneWasBannedHours_String = aConfig->SafeGetString("SomeoneWasBannedHours");
	ChatCtrl_SomeoneWasBannedDay_String = aConfig->SafeGetString("SomeoneWasBannedDay");
	ChatCtrl_SomeoneWasBannedDays_String = aConfig->SafeGetString("SomeoneWasBannedDays");
	ChatCtrl_SomeoneWasBannedIndefinitely_String = aConfig->SafeGetString("SomeoneWasBannedIndefinitely");
	ChatCtrl_YouWereUnmuted_String = aConfig->SafeGetString("YouWereUnmuted");
	ChatCtrl_YouWereMutedMinute_String = aConfig->SafeGetString("YouWereMutedMinute");
	ChatCtrl_YouWereMutedMinutes_String = aConfig->SafeGetString("YouWereMutedMinutes");
	ChatCtrl_YouWereMutedHour_String = aConfig->SafeGetString("YouWereMutedHour");
	ChatCtrl_YouWereMutedHours_String = aConfig->SafeGetString("YouWereMutedHours");
	ChatCtrl_YouWereMutedDay_String = aConfig->SafeGetString("YouWereMutedDay");
	ChatCtrl_YouWereMutedDays_String = aConfig->SafeGetString("YouWereMutedDays");
	ChatCtrl_YouWereMutedIndefinitely_String = aConfig->SafeGetString("YouWereMutedIndefinitely");
	ChatCtrl_SomeoneWasUnmuted_String = aConfig->SafeGetString("SomeoneWasUnmuted");
	ChatCtrl_SomeoneWasMutedMinute_String = aConfig->SafeGetString("SomeoneWasMutedMinute");
	ChatCtrl_SomeoneWasMutedMinutes_String = aConfig->SafeGetString("SomeoneWasMutedMinutes");
	ChatCtrl_SomeoneWasMutedHour_String = aConfig->SafeGetString("SomeoneWasMutedHour");
	ChatCtrl_SomeoneWasMutedHours_String = aConfig->SafeGetString("SomeoneWasMutedHours");
	ChatCtrl_SomeoneWasMutedDay_String = aConfig->SafeGetString("SomeoneWasMutedDay");
	ChatCtrl_SomeoneWasMutedDays_String = aConfig->SafeGetString("SomeoneWasMutedDays");
	ChatCtrl_SomeoneWasMutedIndefinitely_String = aConfig->SafeGetString("SomeoneWasMutedIndefinitely");
	ChatCtrl_ModeratorComment_String = aConfig->SafeGetString("ModeratorComment");
	ChatCtrl_AlertComment_String = aConfig->SafeGetString("AlertComment");
	ChatCtrl_ServerAlert_String = aConfig->SafeGetString("ServerAlert");
	ChatCtrl_ServerShutdownStartedMinute_String = aConfig->SafeGetString("ServerShutdownStartedMinute");
	ChatCtrl_ServerShutdownStartedMinutes_String = aConfig->SafeGetString("ServerShutdownStartedMinutes");
	ChatCtrl_ServerShutdownStartedHour_String = aConfig->SafeGetString("ServerShutdownStartedHour");
	ChatCtrl_ServerShutdownStartedHours_String = aConfig->SafeGetString("ServerShutdownStartedHours");
	ChatCtrl_ServerShutdownStartedDay_String = aConfig->SafeGetString("ServerShutdownStartedDay");
	ChatCtrl_ServerShutdownStartedDays_String = aConfig->SafeGetString("ServerShutdownStartedDays");
	ChatCtrl_ServerShutdownAborted_String = aConfig->SafeGetString("ServerShutdownAborted");
	ChatCtrl_SomeoneInvitesYou_String = aConfig->SafeGetString("SomeoneInvitesYou");
	ChatCtrl_SomeoneUninvitesYou_String = aConfig->SafeGetString("SomeoneUninvitesYou");
	ChatCtrl_YouInviteSomeone_String = aConfig->SafeGetString("YouInviteSomeone");
	ChatCtrl_YouUninviteSomeone_String = aConfig->SafeGetString("YouUninviteSomeone");
	ChatCtrl_SomeoneInvitesYouExpl_String = aConfig->SafeGetString("SomeoneInvitesYouExpl");
	ChatCtrl_SomeoneUninvitesYouExpl_String = aConfig->SafeGetString("SomeoneUninvitesYouExpl");
	ChatCtrl_YouInviteSomeoneExpl_String = aConfig->SafeGetString("YouInviteSomeoneExpl");
	ChatCtrl_YouUninviteSomeoneExpl_String = aConfig->SafeGetString("YouUninviteSomeoneExpl");
	ChatCtrl_Accept_String = aConfig->SafeGetString("Accept");
	ChatCtrl_Reject_String = aConfig->SafeGetString("Reject");
	ChatCtrl_SomeoneAsksToJoin_String = aConfig->SafeGetString("SomeoneAsksToJoin");
	ChatCtrl_SomeoneAsksToJoinExpl_String = aConfig->SafeGetString("SomeoneAsksToJoinExpl");
	ChatCtrl_ClientNotOnServer_String = aConfig->SafeGetString("ClientNotOnServer");
	ChatCtrl_ClientInGame_String = aConfig->SafeGetString("ClientInGame");
	ChatCtrl_ClientInChat_String = aConfig->SafeGetString("ClientInChat");
	ChatCtrl_ClientInGameAndChat_String = aConfig->SafeGetString("ClientInGameAndChat");
	ChatCtrl_ClientIsAnonymous_String = aConfig->SafeGetString("ClientIsAnonymous");
	ChatCtrl_FriendIsChatting_String = aConfig->SafeGetString("FriendIsChatting");
	ChatCtrl_FriendIsInGameStaging_String = aConfig->SafeGetString("FriendIsInGameStaging");
	ChatCtrl_FriendIsPlaying_String = aConfig->SafeGetString("FriendIsPlaying");
	ChatCtrl_FriendIsOn_String = aConfig->SafeGetString("FriendIsOn");
	ChatCtrl_FriendIsNotOnline_String = aConfig->SafeGetString("FriendIsNotOnline");
	ChatCtrl_PlayerKicked_String = aConfig->SafeGetString("PlayerKicked");
	ChatCtrl_PlayerBanned_String = aConfig->SafeGetString("PlayerBanned");
	ChatCtrl_FriendJoined_String = aConfig->SafeGetString("FriendJoined");
	ChatCtrl_FriendLeft_String = aConfig->SafeGetString("FriendLeft");
	ChatCtrl_TeamMemberJoined_String = aConfig->SafeGetString("TeamMemberJoined");
	ChatCtrl_TeamMemberLeft_String = aConfig->SafeGetString("TeamMemberLeft");
	ChatCtrl_TeamMembers_String = aConfig->SafeGetString("TeamMembers");
	ChatCtrl_IgnoreList_String = aConfig->SafeGetString("IgnoreList");

	aConfig=theTable->SafeGetConfig("ChooseNetworkAdapterDialog");
	ChooseNetworkAdapterDialog_Default_String = aConfig->SafeGetString("Default");
	ChooseNetworkAdapterDialog_Unknown_String = aConfig->SafeGetString("Unknown");

	aConfig=theTable->SafeGetConfig("ClientOptionsCtrl");
	ClientOptionsCtrl_Whisper_String = aConfig->SafeGetString("Whisper");
	ClientOptionsCtrl_Invite_String = aConfig->SafeGetString("Invite");
	ClientOptionsCtrl_Uninvite_String = aConfig->SafeGetString("Uninvite");
	ClientOptionsCtrl_AddToFriends_String = aConfig->SafeGetString("AddToFriends");
	ClientOptionsCtrl_RemoveFromFriends_String = aConfig->SafeGetString("RemoveFromFriends");
	ClientOptionsCtrl_ClearAFK_String = aConfig->SafeGetString("ClearAFK");
	ClientOptionsCtrl_SetAFK_String = aConfig->SafeGetString("SetAFK");
	ClientOptionsCtrl_Ignore_String = aConfig->SafeGetString("Ignore");
	ClientOptionsCtrl_Unignore_String = aConfig->SafeGetString("Unignore");
	ClientOptionsCtrl_Block_String = aConfig->SafeGetString("Block");
	ClientOptionsCtrl_Unblock_String = aConfig->SafeGetString("Unblock");
	ClientOptionsCtrl_Warn_String = aConfig->SafeGetString("Warn");
	ClientOptionsCtrl_Mute_String = aConfig->SafeGetString("Mute");
	ClientOptionsCtrl_UnMute_String = aConfig->SafeGetString("UnMute");
	ClientOptionsCtrl_Kick_String = aConfig->SafeGetString("Kick");
	ClientOptionsCtrl_Ban_String = aConfig->SafeGetString("Ban");

	aConfig=theTable->SafeGetConfig("DirectConnectionOptionsCtrl");
	DirectConnectionOptionsCtrl_Connect_String = aConfig->SafeGetString("Connect");
	DirectConnectionOptionsCtrl_Refresh_String = aConfig->SafeGetString("Refresh");
	DirectConnectionOptionsCtrl_Remove_String = aConfig->SafeGetString("Remove");

	aConfig=theTable->SafeGetConfig("FriendOptionsCtrl");
	FriendOptionsCtrl_Locate_String = aConfig->SafeGetString("Locate");
	FriendOptionsCtrl_RefreshList_String = aConfig->SafeGetString("RefreshList");
	FriendOptionsCtrl_RemoveFromFriends_String = aConfig->SafeGetString("RemoveFromFriends");

	aConfig=theTable->SafeGetConfig("FriendsListCtrl");
	FriendsListCtrl_Unknown_Image = aConfig->SafeGetImage("Unknown");
	FriendsListCtrl_NotFound_Image = aConfig->SafeGetImage("NotFound");
	FriendsListCtrl_InChat_Image = aConfig->SafeGetImage("InChat");
	FriendsListCtrl_InGame_Image = aConfig->SafeGetImage("InGame");

	aConfig=theTable->SafeGetConfig("GameOptionsPopup");
	GameOptionsPopup_Join_String = aConfig->SafeGetString("Join");
	GameOptionsPopup_Ping_String = aConfig->SafeGetString("Ping");
	GameOptionsPopup_QueryDetails_String = aConfig->SafeGetString("QueryDetails");
	GameOptionsPopup_HideDetails_String = aConfig->SafeGetString("HideDetails");
	GameOptionsPopup_ShowDetails_String = aConfig->SafeGetString("ShowDetails");
	GameOptionsPopup_Remove_String = aConfig->SafeGetString("Remove");

	aConfig=theTable->SafeGetConfig("GameStagingCtrl");
	GameStagingCtrl_Ready_String = aConfig->SafeGetString("Ready");
	GameStagingCtrl_NotReady_String = aConfig->SafeGetString("NotReady");
	GameStagingCtrl_StartGame_String = aConfig->SafeGetString("StartGame");
	GameStagingCtrl_LeaveGame_String = aConfig->SafeGetString("LeaveGame");
	GameStagingCtrl_DissolveGame_String = aConfig->SafeGetString("DissolveGame");

	aConfig=theTable->SafeGetConfig("GameStagingLogic");
	GameStagingLogic_KickedTitle_String = aConfig->SafeGetString("KickedTitle");
	GameStagingLogic_KickedDetails_String = aConfig->SafeGetString("KickedDetails");
	GameStagingLogic_BannedTitle_String = aConfig->SafeGetString("BannedTitle");
	GameStagingLogic_BannedDetails_String = aConfig->SafeGetString("BannedDetails");
	GameStagingLogic_ConnectWarningTitle1_String = aConfig->SafeGetString("ConnectWarningTitle1");
	GameStagingLogic_ConnectWarningDetails1_String = aConfig->SafeGetString("ConnectWarningDetails1");
	GameStagingLogic_ConnectWarningTitle2_String = aConfig->SafeGetString("ConnectWarningTitle2");
	GameStagingLogic_ConnectWarningDetails2_String = aConfig->SafeGetString("ConnectWarningDetails2");
	GameStagingLogic_ConnectWarningTitle3_String = aConfig->SafeGetString("ConnectWarningTitle3");
	GameStagingLogic_ConnectWarningDetails3_String = aConfig->SafeGetString("ConnectWarningDetails3");
	GameStagingLogic_NoPingReplyError_String = aConfig->SafeGetString("NoPingReplyError");
	GameStagingLogic_GettingGameInfoTitle_String = aConfig->SafeGetString("GettingGameInfoTitle");
	GameStagingLogic_GettingGameInfoDetails_String = aConfig->SafeGetString("GettingGameInfoDetails");
	GameStagingLogic_JoinGameErrorTitle_String = aConfig->SafeGetString("JoinGameErrorTitle");
	GameStagingLogic_VerifyingPingTitle_String = aConfig->SafeGetString("VerifyingPingTitle");
	GameStagingLogic_VerifyingPingDetails_String = aConfig->SafeGetString("VerifyingPingDetails");
	GameStagingLogic_DissolveGame_String = aConfig->SafeGetString("DissolveGame");
	GameStagingLogic_DissolveGameConfirm_String = aConfig->SafeGetString("DissolveGameConfirm");
	GameStagingLogic_LeaveGame_String = aConfig->SafeGetString("LeaveGame");
	GameStagingLogic_LeaveGameConfirm_String = aConfig->SafeGetString("LeaveGameConfirm");
	GameStagingLogic_NoQuickPlayGameTitle_String = aConfig->SafeGetString("NoQuickPlayGameTitle");
	GameStagingLogic_NoQuickPlayGameDetails_String = aConfig->SafeGetString("NoQuickPlayGameDetails");

	aConfig=theTable->SafeGetConfig("GameUserListCtrl");
	GameUserListCtrl_Ban_String = aConfig->SafeGetString("Ban");
	GameUserListCtrl_Kick_String = aConfig->SafeGetString("Kick");
	GameUserListCtrl_Ready_Image = aConfig->SafeGetImage("Ready");

	aConfig=theTable->SafeGetConfig("InternetQuickPlay");
	InternetQuickPlay_Welcome_String = aConfig->SafeGetString("Welcome");
	InternetQuickPlay_UserWelcome_String = aConfig->SafeGetString("UserWelcome");

	aConfig=theTable->SafeGetConfig("JoinResponse");
	JoinResponse_Joined_String = aConfig->SafeGetString("Joined");
	JoinResponse_ServerFull_String = aConfig->SafeGetString("ServerFull");
	JoinResponse_InviteOnly_String = aConfig->SafeGetString("InviteOnly");
	JoinResponse_AskToJoin_String = aConfig->SafeGetString("AskToJoin");
	JoinResponse_GameInProgress_String = aConfig->SafeGetString("GameInProgress");
	JoinResponse_InvalidPassword_String = aConfig->SafeGetString("InvalidPassword");
	JoinResponse_NotOnLAN_String = aConfig->SafeGetString("NotOnLAN");
	JoinResponse_Blocked_String = aConfig->SafeGetString("Blocked");
	JoinResponse_Unknown_String = aConfig->SafeGetString("Unknown");

	aConfig=theTable->SafeGetConfig("LANLogic");
	LANLogic_SocketBindError_String = aConfig->SafeGetString("SocketBindError");
	LANLogic_SocketListenError_String = aConfig->SafeGetString("SocketListenError");
	LANLogic_ConnectToGameTitle_String = aConfig->SafeGetString("ConnectToGameTitle");
	LANLogic_ConnectToGameDetails_String = aConfig->SafeGetString("ConnectToGameDetails");
	LANLogic_CheckingAddressTitle_String = aConfig->SafeGetString("CheckingAddressTitle");
	LANLogic_CheckingAddressDetails_String = aConfig->SafeGetString("CheckingAddressDetails");
	LANLogic_RegisterTitle_String = aConfig->SafeGetString("RegisterTitle");
	LANLogic_RegisterDetails_String = aConfig->SafeGetString("RegisterDetails");
	LANLogic_JoinFailure_String = aConfig->SafeGetString("JoinFailure");
	LANLogic_JoinReplyError_String = aConfig->SafeGetString("JoinReplyError");
	LANLogic_InvalidSubnet_String = aConfig->SafeGetString("InvalidSubnet");

	aConfig=theTable->SafeGetConfig("LobbyContainer");
	LobbyContainer_InitStart_String = aConfig->SafeGetString("InitStart");
	LobbyContainer_InitFailed_String = aConfig->SafeGetString("InitFailed");
	LobbyContainer_CheckingVersion_String = aConfig->SafeGetString("CheckingVersion");
	LobbyContainer_QueryingServers_String = aConfig->SafeGetString("QueryingServers");
	LobbyContainer_NoConnection_String = aConfig->SafeGetString("NoConnection");
	LobbyContainer_DNSLookupFailed_String = aConfig->SafeGetString("DNSLookupFailed");
	LobbyContainer_DirLookupFailure_String = aConfig->SafeGetString("DirLookupFailure");
	LobbyContainer_VersionCheckFailure_String = aConfig->SafeGetString("VersionCheckFailure");
	LobbyContainer_InitComplete_String = aConfig->SafeGetString("InitComplete");
	LobbyContainer_NoServers_String = aConfig->SafeGetString("NoServers");
	LobbyContainer_AuthServers_String = aConfig->SafeGetString("AuthServers");
	LobbyContainer_AccountServers_String = aConfig->SafeGetString("AccountServers");
	LobbyContainer_VersionServers_String = aConfig->SafeGetString("VersionServers");
	LobbyContainer_CreateAccountTitle_String = aConfig->SafeGetString("CreateAccountTitle");
	LobbyContainer_CreateAccountDetails_String = aConfig->SafeGetString("CreateAccountDetails");
	LobbyContainer_RetrievePasswordTitle_String = aConfig->SafeGetString("RetrievePasswordTitle");
	LobbyContainer_RetrievePasswordDetails_String = aConfig->SafeGetString("RetrievePasswordDetails");
	LobbyContainer_RetrieveUsernameTitle_String = aConfig->SafeGetString("RetrieveUsernameTitle");
	LobbyContainer_RetrieveUsernameDetails_String = aConfig->SafeGetString("RetrieveUsernameDetails");
	LobbyContainer_QueryAccountTitle_String = aConfig->SafeGetString("QueryAccountTitle");
	LobbyContainer_QueryAccountDetails_String = aConfig->SafeGetString("QueryAccountDetails");
	LobbyContainer_UpdateAccountTitle_String = aConfig->SafeGetString("UpdateAccountTitle");
	LobbyContainer_UpdateAccountDetails_String = aConfig->SafeGetString("UpdateAccountDetails");
	LobbyContainer_LoginTitle_String = aConfig->SafeGetString("LoginTitle");
	LobbyContainer_LoginDetails_String = aConfig->SafeGetString("LoginDetails");
	LobbyContainer_GettingRoomsTitle_String = aConfig->SafeGetString("GettingRoomsTitle");
	LobbyContainer_GettingRoomsDetails_String = aConfig->SafeGetString("GettingRoomsDetails");
	LobbyContainer_VersionOutOfDataTitle_String = aConfig->SafeGetString("VersionOutOfDataTitle");
	LobbyContainer_VersionOutOfDataDetails_String = aConfig->SafeGetString("VersionOutOfDataDetails");

	aConfig=theTable->SafeGetConfig("LobbyGameStatus");
	LobbyGameStatus_Success_String = aConfig->SafeGetString("Success");
	LobbyGameStatus_GameFull_String = aConfig->SafeGetString("GameFull");
	LobbyGameStatus_GameInProgress_String = aConfig->SafeGetString("GameInProgress");
	LobbyGameStatus_UnpackFailure_String = aConfig->SafeGetString("UnpackFailure");
	LobbyGameStatus_NotInvited_String = aConfig->SafeGetString("NotInvited");
	LobbyGameStatus_CaptainRejectedYou_String = aConfig->SafeGetString("CaptainRejectedYou");
	LobbyGameStatus_InvalidPassword_String = aConfig->SafeGetString("InvalidPassword");
	LobbyGameStatus_DuplicateName_String = aConfig->SafeGetString("DuplicateName");

	aConfig=theTable->SafeGetConfig("LobbyOptions");
	LobbyOptions_DefaultName_String = aConfig->SafeGetString("DefaultName");
	LobbyOptions_CheckingAddress_String = aConfig->SafeGetString("CheckingAddress");
	LobbyOptions_AddressUnavailable_String = aConfig->SafeGetString("AddressUnavailable");

	aConfig=theTable->SafeGetConfig("RoomCtrl");
	RoomCtrl_QueryingServer_String = aConfig->SafeGetString("QueryingServer");
	RoomCtrl_Password_Image = aConfig->SafeGetImage("Password");

	aConfig=theTable->SafeGetConfig("RoomLabelCtrl");
	RoomLabelCtrl_LabelPrefix_String = aConfig->SafeGetString("LabelPrefix");
	RoomLabelCtrl_LabelServer_String = aConfig->SafeGetString("LabelServer");
	RoomLabelCtrl_LabelUser_String = aConfig->SafeGetString("LabelUser");
	RoomLabelCtrl_LabelUsers_String = aConfig->SafeGetString("LabelUsers");
	RoomLabelCtrl_LabelDisconnected_String = aConfig->SafeGetString("LabelDisconnected");

	aConfig=theTable->SafeGetConfig("RoutingLogic");
	RoutingLogic_GetClientListTitle_String = aConfig->SafeGetString("GetClientListTitle");
	RoutingLogic_GetClientListDetails_String = aConfig->SafeGetString("GetClientListDetails");
	RoutingLogic_GetGroupListTitle_String = aConfig->SafeGetString("GetGroupListTitle");
	RoutingLogic_GetGroupListDetails_String = aConfig->SafeGetString("GetGroupListDetails");
	RoutingLogic_GameDissolvedTitle_String = aConfig->SafeGetString("GameDissolvedTitle");
	RoutingLogic_GameDissolvedDetails_String = aConfig->SafeGetString("GameDissolvedDetails");
	RoutingLogic_NoServersTitle_String = aConfig->SafeGetString("NoServersTitle");
	RoutingLogic_NoServersDetails_String = aConfig->SafeGetString("NoServersDetails");
	RoutingLogic_TooManyFriendsTitle_String = aConfig->SafeGetString("TooManyFriendsTitle");
	RoutingLogic_TooManyFriendsDetails_String = aConfig->SafeGetString("TooManyFriendsDetails");
	RoutingLogic_AlreadyJoinedTitle_String = aConfig->SafeGetString("AlreadyJoinedTitle");
	RoutingLogic_AlreadyInGame_String = aConfig->SafeGetString("AlreadyInGame");
	RoutingLogic_AlreadyInRoom_String = aConfig->SafeGetString("AlreadyInRoom");
	RoutingLogic_JoinGroupTitle_String = aConfig->SafeGetString("JoinGroupTitle");
	RoutingLogic_JoinGroupDetails_String = aConfig->SafeGetString("JoinGroupDetails");
	RoutingLogic_ConnectTitle_String = aConfig->SafeGetString("ConnectTitle");
	RoutingLogic_ConnectDetails_String = aConfig->SafeGetString("ConnectDetails");
	RoutingLogic_RegisterTitle_String = aConfig->SafeGetString("RegisterTitle");
	RoutingLogic_RegisterDetails_String = aConfig->SafeGetString("RegisterDetails");
	RoutingLogic_QueryGroupsTitle_String = aConfig->SafeGetString("QueryGroupsTitle");
	RoutingLogic_QueryGroupsDetails_String = aConfig->SafeGetString("QueryGroupsDetails");
	RoutingLogic_CreateGroupTitle_String = aConfig->SafeGetString("CreateGroupTitle");
	RoutingLogic_CreateGroupDetails_String = aConfig->SafeGetString("CreateGroupDetails");
	RoutingLogic_GetBadUsersTitle_String = aConfig->SafeGetString("GetBadUsersTitle");
	RoutingLogic_GetBadUsersDetails_String = aConfig->SafeGetString("GetBadUsersDetails");
	RoutingLogic_DisconnectedTitle_String = aConfig->SafeGetString("DisconnectedTitle");
	RoutingLogic_DisconnectedDetails_String = aConfig->SafeGetString("DisconnectedDetails");
	RoutingLogic_AskingCaptainTitle_String = aConfig->SafeGetString("AskingCaptainTitle");
	RoutingLogic_AskingCaptainDetails_String = aConfig->SafeGetString("AskingCaptainDetails");
	RoutingLogic_CollectingPingsTitle_String = aConfig->SafeGetString("CollectingPingsTitle");
	RoutingLogic_CollectingPingsDetails_String = aConfig->SafeGetString("CollectingPingsDetails");
	RoutingLogic_GettingGameListTitle_String = aConfig->SafeGetString("GettingGameListTitle");
	RoutingLogic_GettingGameListDetails_String = aConfig->SafeGetString("GettingGameListDetails");
	RoutingLogic_MuteClientTitle_String = aConfig->SafeGetString("MuteClientTitle");
	RoutingLogic_UnmuteClientTitle_String = aConfig->SafeGetString("UnmuteClientTitle");
	RoutingLogic_BanClientTitle_String = aConfig->SafeGetString("BanClientTitle");
	RoutingLogic_KickClientTitle_String = aConfig->SafeGetString("KickClientTitle");
	RoutingLogic_ModerateDetails_String = aConfig->SafeGetString("ModerateDetails");
	RoutingLogic_FriendNotFoundTitle_String = aConfig->SafeGetString("FriendNotFoundTitle");
	RoutingLogic_FriendNotFoundDetails_String = aConfig->SafeGetString("FriendNotFoundDetails");
	RoutingLogic_ClientNotFound_String = aConfig->SafeGetString("ClientNotFound");
	RoutingLogic_ClientAmbiguous_String = aConfig->SafeGetString("ClientAmbiguous");
	RoutingLogic_InvalidCommand_String = aConfig->SafeGetString("InvalidCommand");
	RoutingLogic_RequireName_String = aConfig->SafeGetString("RequireName");
	RoutingLogic_BadTimeSpec_String = aConfig->SafeGetString("BadTimeSpec");
	RoutingLogic_Error_String = aConfig->SafeGetString("Error");
	RoutingLogic_NotCaptain_String = aConfig->SafeGetString("NotCaptain");

	aConfig=theTable->SafeGetConfig("ServerListCtrl");
	ServerListCtrl_Pinging_String = aConfig->SafeGetString("Pinging");
	ServerListCtrl_PingFailed_String = aConfig->SafeGetString("PingFailed");
	ServerListCtrl_Querying_String = aConfig->SafeGetString("Querying");
	ServerListCtrl_QueryFailed_String = aConfig->SafeGetString("QueryFailed");
	ServerListCtrl_ListSummary_String = aConfig->SafeGetString("ListSummary");
	ServerListCtrl_ListPingSummary_String = aConfig->SafeGetString("ListPingSummary");
	ServerListCtrl_RefreshList_String = aConfig->SafeGetString("RefreshList");
	ServerListCtrl_StopRefresh_String = aConfig->SafeGetString("StopRefresh");
	ServerListCtrl_NoFilter_String = aConfig->SafeGetString("NoFilter");
	ServerListCtrl_AddRemove_String = aConfig->SafeGetString("AddRemove");
	ServerListCtrl_Add_String = aConfig->SafeGetString("Add");
	ServerListCtrl_Remove_String = aConfig->SafeGetString("Remove");
	ServerListCtrl_FullFilter_String = aConfig->SafeGetString("FullFilter");
	ServerListCtrl_NotFullFilter_String = aConfig->SafeGetString("NotFullFilter");
	ServerListCtrl_OpenFilter_String = aConfig->SafeGetString("OpenFilter");
	ServerListCtrl_PingFailed_Image = aConfig->SafeGetImage("PingFailed");
	ServerListCtrl_GameExpanded_Image = aConfig->SafeGetImage("GameExpanded");
	ServerListCtrl_GameNotExpanded_Image = aConfig->SafeGetImage("GameNotExpanded");
	ServerListCtrl_GameAreInvited_Image = aConfig->SafeGetImage("GameAreInvited");
	ServerListCtrl_GameNone_Image = aConfig->SafeGetImage("GameNone");
	ServerListCtrl_GameAskToJoin_Image = aConfig->SafeGetImage("GameAskToJoin");
	ServerListCtrl_GameHasPassword_Image = aConfig->SafeGetImage("GameHasPassword");
	ServerListCtrl_GameNotInvited_Image = aConfig->SafeGetImage("GameNotInvited");
	ServerListCtrl_GameInProgress_Image = aConfig->SafeGetImage("GameInProgress");
	ServerListCtrl_SkillOpen_Image = aConfig->SafeGetImage("SkillOpen");
	ServerListCtrl_SkillNovice_Image = aConfig->SafeGetImage("SkillNovice");
	ServerListCtrl_SkillIntermediate_Image = aConfig->SafeGetImage("SkillIntermediate");
	ServerListCtrl_SkillAdvanced_Image = aConfig->SafeGetImage("SkillAdvanced");
	ServerListCtrl_SkillExpert_Image = aConfig->SafeGetImage("SkillExpert");

	aConfig=theTable->SafeGetConfig("ServerOptionsCtrl");
	ServerOptionsCtrl_Connect_String = aConfig->SafeGetString("Connect");
	ServerOptionsCtrl_RefreshServer_String = aConfig->SafeGetString("RefreshServer");
	ServerOptionsCtrl_HideDetails_String = aConfig->SafeGetString("HideDetails");
	ServerOptionsCtrl_ShowDetails_String = aConfig->SafeGetString("ShowDetails");
	ServerOptionsCtrl_ResfreshList_String = aConfig->SafeGetString("ResfreshList");

	aConfig=theTable->SafeGetConfig("StagingLogic");
	StagingLogic_DisconnectedTitle_String = aConfig->SafeGetString("DisconnectedTitle");
	StagingLogic_DisconnectedDetails_String = aConfig->SafeGetString("DisconnectedDetails");
	StagingLogic_BannedTitle_String = aConfig->SafeGetString("BannedTitle");
	StagingLogic_BannedDetails_String = aConfig->SafeGetString("BannedDetails");
	StagingLogic_KickedTitle_String = aConfig->SafeGetString("KickedTitle");
	StagingLogic_KickedDetails_String = aConfig->SafeGetString("KickedDetails");

	aConfig=theTable->SafeGetConfig("UserListCtrl");
	UserListCtrl_Ignore_String = aConfig->SafeGetString("Ignore");
	UserListCtrl_Unignore_String = aConfig->SafeGetString("Unignore");
	UserListCtrl_Normal_Image = aConfig->SafeGetImage("Normal");
	UserListCtrl_Away_Image = aConfig->SafeGetImage("Away");
	UserListCtrl_Admin_Image = aConfig->SafeGetImage("Admin");
	UserListCtrl_Moderator_Image = aConfig->SafeGetImage("Moderator");
	UserListCtrl_Captain_Image = aConfig->SafeGetImage("Captain");
	UserListCtrl_Muted_Image = aConfig->SafeGetImage("Muted");
	UserListCtrl_Ignored_Image = aConfig->SafeGetImage("Ignored");
	UserListCtrl_Blocked_Image = aConfig->SafeGetImage("Blocked");

	aConfig=theTable->SafeGetConfig("WONStatus");
	WONStatus_Success_String = aConfig->SafeGetString("Success");
	WONStatus_Killed_String = aConfig->SafeGetString("Killed");
	WONStatus_TimedOut_String = aConfig->SafeGetString("TimedOut");
	WONStatus_InvalidAddress_String = aConfig->SafeGetString("InvalidAddress");
	WONStatus_GeneralFailure_String = aConfig->SafeGetString("GeneralFailure");
	WONStatus_AuthFailure_String = aConfig->SafeGetString("AuthFailure");
	WONStatus_FTPFailure_String = aConfig->SafeGetString("FTPFailure");
	WONStatus_HTTPFailure_String = aConfig->SafeGetString("HTTPFailure");
	WONStatus_MOTDFailure_String = aConfig->SafeGetString("MOTDFailure");
	WONStatus_RoutingFailure_String = aConfig->SafeGetString("RoutingFailure");
	WONStatus_GameSpyFailure_String = aConfig->SafeGetString("GameSpyFailure");
	WONStatus_DirFailure_String = aConfig->SafeGetString("DirFailure");
	WONStatus_ChatFailure_String = aConfig->SafeGetString("ChatFailure");
	WONStatus_DatabaseFailure_String = aConfig->SafeGetString("DatabaseFailure");
	WONStatus_NotAllowed_String = aConfig->SafeGetString("NotAllowed");
	WONStatus_NotAuthorized_String = aConfig->SafeGetString("NotAuthorized");
	WONStatus_BehindFirewall_String = aConfig->SafeGetString("BehindFirewall");
	WONStatus_ExpiredCertificate_String = aConfig->SafeGetString("ExpiredCertificate");
	WONStatus_InvalidCDKey_String = aConfig->SafeGetString("InvalidCDKey");
	WONStatus_CDKeyBanned_String = aConfig->SafeGetString("CDKeyBanned");
	WONStatus_CDKeyInUse_String = aConfig->SafeGetString("CDKeyInUse");
	WONStatus_CRCFailed_String = aConfig->SafeGetString("CRCFailed");
	WONStatus_UserExists_String = aConfig->SafeGetString("UserExists");
	WONStatus_UserNotFound_String = aConfig->SafeGetString("UserNotFound");
	WONStatus_BadPassword_String = aConfig->SafeGetString("BadPassword");
	WONStatus_InvalidUserName_String = aConfig->SafeGetString("InvalidUserName");
	WONStatus_BadCommunity_String = aConfig->SafeGetString("BadCommunity");
	WONStatus_NotInCommunity_String = aConfig->SafeGetString("NotInCommunity");
	WONStatus_UserSeqInUse_String = aConfig->SafeGetString("UserSeqInUse");
	WONStatus_KeyNotActive_String = aConfig->SafeGetString("KeyNotActive");
	WONStatus_KeyExpired_String = aConfig->SafeGetString("KeyExpired");
	WONStatus_BlankNamesNotAllowed_String = aConfig->SafeGetString("BlankNamesNotAllowed");
	WONStatus_CaptainRejectedYou_String = aConfig->SafeGetString("CaptainRejectedYou");
	WONStatus_ClientAlreadyBanned_String = aConfig->SafeGetString("ClientAlreadyBanned");
	WONStatus_ClientAlreadyInGroup_String = aConfig->SafeGetString("ClientAlreadyInGroup");
	WONStatus_ClientAlreadyMuted_String = aConfig->SafeGetString("ClientAlreadyMuted");
	WONStatus_ClientBanned_String = aConfig->SafeGetString("ClientBanned");
	WONStatus_ClientDoesNotExist_String = aConfig->SafeGetString("ClientDoesNotExist");
	WONStatus_ClientNotBanned_String = aConfig->SafeGetString("ClientNotBanned");
	WONStatus_ClientNotInGroup_String = aConfig->SafeGetString("ClientNotInGroup");
	WONStatus_ClientNotInvited_String = aConfig->SafeGetString("ClientNotInvited");
	WONStatus_ClientNotMuted_String = aConfig->SafeGetString("ClientNotMuted");
	WONStatus_ConnectFailure_String = aConfig->SafeGetString("ConnectFailure");
	WONStatus_GroupAlreadyClaimed_String = aConfig->SafeGetString("GroupAlreadyClaimed");
	WONStatus_GroupAlreadyExists_String = aConfig->SafeGetString("GroupAlreadyExists");
	WONStatus_GroupClosed_String = aConfig->SafeGetString("GroupClosed");
	WONStatus_GroupDeleted_String = aConfig->SafeGetString("GroupDeleted");
	WONStatus_GroupDoesNotExist_String = aConfig->SafeGetString("GroupDoesNotExist");
	WONStatus_GroupFull_String = aConfig->SafeGetString("GroupFull");
	WONStatus_GuestNamesReserved_String = aConfig->SafeGetString("GuestNamesReserved");
	WONStatus_InvalidPassword_String = aConfig->SafeGetString("InvalidPassword");
	WONStatus_MustBeAdmin_String = aConfig->SafeGetString("MustBeAdmin");
	WONStatus_MustBeCaptain_String = aConfig->SafeGetString("MustBeCaptain");
	WONStatus_MustBeCaptainOrModerator_String = aConfig->SafeGetString("MustBeCaptainOrModerator");
	WONStatus_MustBeModerator_String = aConfig->SafeGetString("MustBeModerator");
	WONStatus_ServerFull_String = aConfig->SafeGetString("ServerFull");
	WONStatus_GroupNameExceedsMaximum_String = aConfig->SafeGetString("GroupNameExceedsMaximum");
	WONStatus_JoinCanceled_String = aConfig->SafeGetString("JoinCanceled");
	WONStatus_Throttled_String = aConfig->SafeGetString("Throttled");
	WONStatus_KeyNotUsed_String = aConfig->SafeGetString("KeyNotUsed");
	WONStatus_EMailPasswordDuplication_String = aConfig->SafeGetString("EMailPasswordDuplication");
	WONStatus_AccountCreateDisabled_String = aConfig->SafeGetString("AccountCreateDisabled");
	WONStatus_AgeNotOldEnough_String = aConfig->SafeGetString("AgeNotOldEnough");
	WONStatus_ItemHasDependencies_String = aConfig->SafeGetString("ItemHasDependencies");
	WONStatus_OutOfDate_String = aConfig->SafeGetString("OutOfDate");
	WONStatus_OutOfDateNoUpdate_String = aConfig->SafeGetString("OutOfDateNoUpdate");
	WONStatus_ValidNotLatest_String = aConfig->SafeGetString("ValidNotLatest");
	WONStatus_DataAlreadyExists_String = aConfig->SafeGetString("DataAlreadyExists");
	WONStatus_DBError_String = aConfig->SafeGetString("DBError");
	WONStatus_ConnRefused_String = aConfig->SafeGetString("ConnRefused");

}

void LobbyResource_UnloadResources()
{
	LobbyGlobal_Cancel_String.erase();
	LobbyGlobal_Lan_String.erase();
	LobbyGlobal_No_String.erase();
	LobbyGlobal_OK_String.erase();
	LobbyGlobal_Yes_String.erase();
	LobbyGlobal_QuickPlay_String.erase();
	LobbyGlobal_Searching_Image = NULL;
	LobbyGlobal_Firewall_Image = NULL;
	LobbyGlobal_ButtonClick_Sound = NULL;
	LobbyGlobal_LobbyMusic_Sound = NULL;
	LobbyGlobal_AllReady_Sound = NULL;
	LobbyGlobal_AllNotReady_Sound = NULL;
	LobbyGlobal_AskToJoinRecv_Sound = NULL;
	LobbyGlobal_Error_Sound = NULL;
	LobbyGlobal_FriendJoined_Sound = NULL;
	LobbyGlobal_FriendLeft_Sound = NULL;
	LobbyGlobal_InviteRecv_Sound = NULL;
	LobbyGlobal_PlayerJoined_Sound = NULL;
	LobbyGlobal_SierraLogon_Sound = NULL;
	LobbyGlobal_WhisperRecv_Sound = NULL;
	AccountLogic_InvalidCDKeyTitle_String.erase();
	AccountLogic_InvalidCDKeyDetails_String.erase();
	AddFriendDialog_StatusTitle_String.erase();
	AddFriendDialog_StatusDetails_String.erase();
	AdminActionDialog_Warn_String.erase();
	AdminActionDialog_Mute_String.erase();
	AdminActionDialog_UnMute_String.erase();
	AdminActionDialog_Kick_String.erase();
	AdminActionDialog_Ban_String.erase();
	AdminActionDialog_ExplanationOpt_String.erase();
	AdminActionDialog_ExplanationReq_String.erase();
	BadUserDialog_OneDay_String.erase();
	BadUserDialog_OneHour_String.erase();
	BadUserDialog_OneMinute_String.erase();
	BadUserDialog_OneSecond_String.erase();
	BadUserDialog_XDays_String.erase();
	BadUserDialog_XHours_String.erase();
	BadUserDialog_XMinutes_String.erase();
	BadUserDialog_XSeconds_String.erase();
	BadUserDialog_Infinite_String.erase();
	ChatCommandLogic_Whisper_String.erase();
	ChatCommandLogic_Block_String.erase();
	ChatCommandLogic_Emote_String.erase();
	ChatCommandLogic_Ignore_String.erase();
	ChatCommandLogic_Away_String.erase();
	ChatCommandLogic_Invite_String.erase();
	ChatCommandLogic_Uninvite_String.erase();
	ChatCommandLogic_Reply_String.erase();
	ChatCommandLogic_Clear_String.erase();
	ChatCommandLogic_Help_String.erase();
	ChatCommandLogic_ShowTeam_String.erase();
	ChatCommandLogic_Moderator_String.erase();
	ChatCommandLogic_ServerMute_String.erase();
	ChatCommandLogic_ServerBan_String.erase();
	ChatCommandLogic_Mute_String.erase();
	ChatCommandLogic_Ban_String.erase();
	ChatCommandLogic_Unmute_String.erase();
	ChatCommandLogic_ServerUnmute_String.erase();
	ChatCommandLogic_Unban_String.erase();
	ChatCommandLogic_ServerUnban_String.erase();
	ChatCommandLogic_Alert_String.erase();
	ChatCommandLogic_Warn_String.erase();
	ChatCommandLogic_HelpHeader_String.erase();
	ChatCommandLogic_HelpHelp_String.erase();
	ChatCommandLogic_ShowTeamHelp_String.erase();
	ChatCommandLogic_WhisperHelp_String.erase();
	ChatCommandLogic_BlockHelp_String.erase();
	ChatCommandLogic_IgnoreHelp_String.erase();
	ChatCommandLogic_ReplyHelp_String.erase();
	ChatCommandLogic_EmoteHelp_String.erase();
	ChatCommandLogic_AwayHelp_String.erase();
	ChatCommandLogic_ClearHelp_String.erase();
	ChatCommandLogic_InviteHelp_String.erase();
	ChatCommandLogic_UninviteHelp_String.erase();
	ChatCommandLogic_MuteHelp_String.erase();
	ChatCommandLogic_UnmuteHelp_String.erase();
	ChatCommandLogic_BanHelp_String.erase();
	ChatCommandLogic_UnbanHelp_String.erase();
	ChatCommandLogic_ModeratorHelp_String.erase();
	ChatCommandLogic_ServerMuteHelp_String.erase();
	ChatCommandLogic_ServerUnmuteHelp_String.erase();
	ChatCommandLogic_ServerBanHelp_String.erase();
	ChatCommandLogic_ServerUnbanHelp_String.erase();
	ChatCommandLogic_AlertHelp_String.erase();
	ChatCommandLogic_WarnHelp_String.erase();
	ChatCtrl_NormalChat_String.erase();
	ChatCtrl_Emote_String.erase();
	ChatCtrl_TeamChat_String.erase();
	ChatCtrl_TeamEmote_String.erase();
	ChatCtrl_YouWhisperToYourself_String.erase();
	ChatCtrl_YouWhisper_String.erase();
	ChatCtrl_SomeoneWhispersToYou_String.erase();
	ChatCtrl_YouWarnSomeone_String.erase();
	ChatCtrl_YouWarnYourself_String.erase();
	ChatCtrl_SomeoneWarnsYou_String.erase();
	ChatCtrl_SomeoneWarnsSomeone_String.erase();
	ChatCtrl_EnteringRoom_String.erase();
	ChatCtrl_LeavingRoom_String.erase();
	ChatCtrl_Disconnected_String.erase();
	ChatCtrl_YouWereKicked_String.erase();
	ChatCtrl_YouWereBannedMinute_String.erase();
	ChatCtrl_YouWereBannedMinutes_String.erase();
	ChatCtrl_YouWereBannedHour_String.erase();
	ChatCtrl_YouWereBannedHours_String.erase();
	ChatCtrl_YouWereBannedDay_String.erase();
	ChatCtrl_YouWereBannedDays_String.erase();
	ChatCtrl_YouWereBannedIndefinitely_String.erase();
	ChatCtrl_SomeoneWasKicked_String.erase();
	ChatCtrl_SomeoneWasBannedMinute_String.erase();
	ChatCtrl_SomeoneWasBannedMinutes_String.erase();
	ChatCtrl_SomeoneWasBannedHour_String.erase();
	ChatCtrl_SomeoneWasBannedHours_String.erase();
	ChatCtrl_SomeoneWasBannedDay_String.erase();
	ChatCtrl_SomeoneWasBannedDays_String.erase();
	ChatCtrl_SomeoneWasBannedIndefinitely_String.erase();
	ChatCtrl_YouWereUnmuted_String.erase();
	ChatCtrl_YouWereMutedMinute_String.erase();
	ChatCtrl_YouWereMutedMinutes_String.erase();
	ChatCtrl_YouWereMutedHour_String.erase();
	ChatCtrl_YouWereMutedHours_String.erase();
	ChatCtrl_YouWereMutedDay_String.erase();
	ChatCtrl_YouWereMutedDays_String.erase();
	ChatCtrl_YouWereMutedIndefinitely_String.erase();
	ChatCtrl_SomeoneWasUnmuted_String.erase();
	ChatCtrl_SomeoneWasMutedMinute_String.erase();
	ChatCtrl_SomeoneWasMutedMinutes_String.erase();
	ChatCtrl_SomeoneWasMutedHour_String.erase();
	ChatCtrl_SomeoneWasMutedHours_String.erase();
	ChatCtrl_SomeoneWasMutedDay_String.erase();
	ChatCtrl_SomeoneWasMutedDays_String.erase();
	ChatCtrl_SomeoneWasMutedIndefinitely_String.erase();
	ChatCtrl_ModeratorComment_String.erase();
	ChatCtrl_AlertComment_String.erase();
	ChatCtrl_ServerAlert_String.erase();
	ChatCtrl_ServerShutdownStartedMinute_String.erase();
	ChatCtrl_ServerShutdownStartedMinutes_String.erase();
	ChatCtrl_ServerShutdownStartedHour_String.erase();
	ChatCtrl_ServerShutdownStartedHours_String.erase();
	ChatCtrl_ServerShutdownStartedDay_String.erase();
	ChatCtrl_ServerShutdownStartedDays_String.erase();
	ChatCtrl_ServerShutdownAborted_String.erase();
	ChatCtrl_SomeoneInvitesYou_String.erase();
	ChatCtrl_SomeoneUninvitesYou_String.erase();
	ChatCtrl_YouInviteSomeone_String.erase();
	ChatCtrl_YouUninviteSomeone_String.erase();
	ChatCtrl_SomeoneInvitesYouExpl_String.erase();
	ChatCtrl_SomeoneUninvitesYouExpl_String.erase();
	ChatCtrl_YouInviteSomeoneExpl_String.erase();
	ChatCtrl_YouUninviteSomeoneExpl_String.erase();
	ChatCtrl_Accept_String.erase();
	ChatCtrl_Reject_String.erase();
	ChatCtrl_SomeoneAsksToJoin_String.erase();
	ChatCtrl_SomeoneAsksToJoinExpl_String.erase();
	ChatCtrl_ClientNotOnServer_String.erase();
	ChatCtrl_ClientInGame_String.erase();
	ChatCtrl_ClientInChat_String.erase();
	ChatCtrl_ClientInGameAndChat_String.erase();
	ChatCtrl_ClientIsAnonymous_String.erase();
	ChatCtrl_FriendIsChatting_String.erase();
	ChatCtrl_FriendIsInGameStaging_String.erase();
	ChatCtrl_FriendIsPlaying_String.erase();
	ChatCtrl_FriendIsOn_String.erase();
	ChatCtrl_FriendIsNotOnline_String.erase();
	ChatCtrl_PlayerKicked_String.erase();
	ChatCtrl_PlayerBanned_String.erase();
	ChatCtrl_FriendJoined_String.erase();
	ChatCtrl_FriendLeft_String.erase();
	ChatCtrl_TeamMemberJoined_String.erase();
	ChatCtrl_TeamMemberLeft_String.erase();
	ChatCtrl_TeamMembers_String.erase();
	ChatCtrl_IgnoreList_String.erase();
	ChooseNetworkAdapterDialog_Default_String.erase();
	ChooseNetworkAdapterDialog_Unknown_String.erase();
	ClientOptionsCtrl_Whisper_String.erase();
	ClientOptionsCtrl_Invite_String.erase();
	ClientOptionsCtrl_Uninvite_String.erase();
	ClientOptionsCtrl_AddToFriends_String.erase();
	ClientOptionsCtrl_RemoveFromFriends_String.erase();
	ClientOptionsCtrl_ClearAFK_String.erase();
	ClientOptionsCtrl_SetAFK_String.erase();
	ClientOptionsCtrl_Ignore_String.erase();
	ClientOptionsCtrl_Unignore_String.erase();
	ClientOptionsCtrl_Block_String.erase();
	ClientOptionsCtrl_Unblock_String.erase();
	ClientOptionsCtrl_Warn_String.erase();
	ClientOptionsCtrl_Mute_String.erase();
	ClientOptionsCtrl_UnMute_String.erase();
	ClientOptionsCtrl_Kick_String.erase();
	ClientOptionsCtrl_Ban_String.erase();
	DirectConnectionOptionsCtrl_Connect_String.erase();
	DirectConnectionOptionsCtrl_Refresh_String.erase();
	DirectConnectionOptionsCtrl_Remove_String.erase();
	FriendOptionsCtrl_Locate_String.erase();
	FriendOptionsCtrl_RefreshList_String.erase();
	FriendOptionsCtrl_RemoveFromFriends_String.erase();
	FriendsListCtrl_Unknown_Image = NULL;
	FriendsListCtrl_NotFound_Image = NULL;
	FriendsListCtrl_InChat_Image = NULL;
	FriendsListCtrl_InGame_Image = NULL;
	GameOptionsPopup_Join_String.erase();
	GameOptionsPopup_Ping_String.erase();
	GameOptionsPopup_QueryDetails_String.erase();
	GameOptionsPopup_HideDetails_String.erase();
	GameOptionsPopup_ShowDetails_String.erase();
	GameOptionsPopup_Remove_String.erase();
	GameStagingCtrl_Ready_String.erase();
	GameStagingCtrl_NotReady_String.erase();
	GameStagingCtrl_StartGame_String.erase();
	GameStagingCtrl_LeaveGame_String.erase();
	GameStagingCtrl_DissolveGame_String.erase();
	GameStagingLogic_KickedTitle_String.erase();
	GameStagingLogic_KickedDetails_String.erase();
	GameStagingLogic_BannedTitle_String.erase();
	GameStagingLogic_BannedDetails_String.erase();
	GameStagingLogic_ConnectWarningTitle1_String.erase();
	GameStagingLogic_ConnectWarningDetails1_String.erase();
	GameStagingLogic_ConnectWarningTitle2_String.erase();
	GameStagingLogic_ConnectWarningDetails2_String.erase();
	GameStagingLogic_ConnectWarningTitle3_String.erase();
	GameStagingLogic_ConnectWarningDetails3_String.erase();
	GameStagingLogic_NoPingReplyError_String.erase();
	GameStagingLogic_GettingGameInfoTitle_String.erase();
	GameStagingLogic_GettingGameInfoDetails_String.erase();
	GameStagingLogic_JoinGameErrorTitle_String.erase();
	GameStagingLogic_VerifyingPingTitle_String.erase();
	GameStagingLogic_VerifyingPingDetails_String.erase();
	GameStagingLogic_DissolveGame_String.erase();
	GameStagingLogic_DissolveGameConfirm_String.erase();
	GameStagingLogic_LeaveGame_String.erase();
	GameStagingLogic_LeaveGameConfirm_String.erase();
	GameStagingLogic_NoQuickPlayGameTitle_String.erase();
	GameStagingLogic_NoQuickPlayGameDetails_String.erase();
	GameUserListCtrl_Ban_String.erase();
	GameUserListCtrl_Kick_String.erase();
	GameUserListCtrl_Ready_Image = NULL;
	InternetQuickPlay_Welcome_String.erase();
	InternetQuickPlay_UserWelcome_String.erase();
	JoinResponse_Joined_String.erase();
	JoinResponse_ServerFull_String.erase();
	JoinResponse_InviteOnly_String.erase();
	JoinResponse_AskToJoin_String.erase();
	JoinResponse_GameInProgress_String.erase();
	JoinResponse_InvalidPassword_String.erase();
	JoinResponse_NotOnLAN_String.erase();
	JoinResponse_Blocked_String.erase();
	JoinResponse_Unknown_String.erase();
	LANLogic_SocketBindError_String.erase();
	LANLogic_SocketListenError_String.erase();
	LANLogic_ConnectToGameTitle_String.erase();
	LANLogic_ConnectToGameDetails_String.erase();
	LANLogic_CheckingAddressTitle_String.erase();
	LANLogic_CheckingAddressDetails_String.erase();
	LANLogic_RegisterTitle_String.erase();
	LANLogic_RegisterDetails_String.erase();
	LANLogic_JoinFailure_String.erase();
	LANLogic_JoinReplyError_String.erase();
	LANLogic_InvalidSubnet_String.erase();
	LobbyContainer_InitStart_String.erase();
	LobbyContainer_InitFailed_String.erase();
	LobbyContainer_CheckingVersion_String.erase();
	LobbyContainer_QueryingServers_String.erase();
	LobbyContainer_NoConnection_String.erase();
	LobbyContainer_DNSLookupFailed_String.erase();
	LobbyContainer_DirLookupFailure_String.erase();
	LobbyContainer_VersionCheckFailure_String.erase();
	LobbyContainer_InitComplete_String.erase();
	LobbyContainer_NoServers_String.erase();
	LobbyContainer_AuthServers_String.erase();
	LobbyContainer_AccountServers_String.erase();
	LobbyContainer_VersionServers_String.erase();
	LobbyContainer_CreateAccountTitle_String.erase();
	LobbyContainer_CreateAccountDetails_String.erase();
	LobbyContainer_RetrievePasswordTitle_String.erase();
	LobbyContainer_RetrievePasswordDetails_String.erase();
	LobbyContainer_RetrieveUsernameTitle_String.erase();
	LobbyContainer_RetrieveUsernameDetails_String.erase();
	LobbyContainer_QueryAccountTitle_String.erase();
	LobbyContainer_QueryAccountDetails_String.erase();
	LobbyContainer_UpdateAccountTitle_String.erase();
	LobbyContainer_UpdateAccountDetails_String.erase();
	LobbyContainer_LoginTitle_String.erase();
	LobbyContainer_LoginDetails_String.erase();
	LobbyContainer_GettingRoomsTitle_String.erase();
	LobbyContainer_GettingRoomsDetails_String.erase();
	LobbyContainer_VersionOutOfDataTitle_String.erase();
	LobbyContainer_VersionOutOfDataDetails_String.erase();
	LobbyGameStatus_Success_String.erase();
	LobbyGameStatus_GameFull_String.erase();
	LobbyGameStatus_GameInProgress_String.erase();
	LobbyGameStatus_UnpackFailure_String.erase();
	LobbyGameStatus_NotInvited_String.erase();
	LobbyGameStatus_CaptainRejectedYou_String.erase();
	LobbyGameStatus_InvalidPassword_String.erase();
	LobbyGameStatus_DuplicateName_String.erase();
	LobbyOptions_DefaultName_String.erase();
	LobbyOptions_CheckingAddress_String.erase();
	LobbyOptions_AddressUnavailable_String.erase();
	RoomCtrl_QueryingServer_String.erase();
	RoomCtrl_Password_Image = NULL;
	RoomLabelCtrl_LabelPrefix_String.erase();
	RoomLabelCtrl_LabelServer_String.erase();
	RoomLabelCtrl_LabelUser_String.erase();
	RoomLabelCtrl_LabelUsers_String.erase();
	RoomLabelCtrl_LabelDisconnected_String.erase();
	RoutingLogic_GetClientListTitle_String.erase();
	RoutingLogic_GetClientListDetails_String.erase();
	RoutingLogic_GetGroupListTitle_String.erase();
	RoutingLogic_GetGroupListDetails_String.erase();
	RoutingLogic_GameDissolvedTitle_String.erase();
	RoutingLogic_GameDissolvedDetails_String.erase();
	RoutingLogic_NoServersTitle_String.erase();
	RoutingLogic_NoServersDetails_String.erase();
	RoutingLogic_TooManyFriendsTitle_String.erase();
	RoutingLogic_TooManyFriendsDetails_String.erase();
	RoutingLogic_AlreadyJoinedTitle_String.erase();
	RoutingLogic_AlreadyInGame_String.erase();
	RoutingLogic_AlreadyInRoom_String.erase();
	RoutingLogic_JoinGroupTitle_String.erase();
	RoutingLogic_JoinGroupDetails_String.erase();
	RoutingLogic_ConnectTitle_String.erase();
	RoutingLogic_ConnectDetails_String.erase();
	RoutingLogic_RegisterTitle_String.erase();
	RoutingLogic_RegisterDetails_String.erase();
	RoutingLogic_QueryGroupsTitle_String.erase();
	RoutingLogic_QueryGroupsDetails_String.erase();
	RoutingLogic_CreateGroupTitle_String.erase();
	RoutingLogic_CreateGroupDetails_String.erase();
	RoutingLogic_GetBadUsersTitle_String.erase();
	RoutingLogic_GetBadUsersDetails_String.erase();
	RoutingLogic_DisconnectedTitle_String.erase();
	RoutingLogic_DisconnectedDetails_String.erase();
	RoutingLogic_AskingCaptainTitle_String.erase();
	RoutingLogic_AskingCaptainDetails_String.erase();
	RoutingLogic_CollectingPingsTitle_String.erase();
	RoutingLogic_CollectingPingsDetails_String.erase();
	RoutingLogic_GettingGameListTitle_String.erase();
	RoutingLogic_GettingGameListDetails_String.erase();
	RoutingLogic_MuteClientTitle_String.erase();
	RoutingLogic_UnmuteClientTitle_String.erase();
	RoutingLogic_BanClientTitle_String.erase();
	RoutingLogic_KickClientTitle_String.erase();
	RoutingLogic_ModerateDetails_String.erase();
	RoutingLogic_FriendNotFoundTitle_String.erase();
	RoutingLogic_FriendNotFoundDetails_String.erase();
	RoutingLogic_ClientNotFound_String.erase();
	RoutingLogic_ClientAmbiguous_String.erase();
	RoutingLogic_InvalidCommand_String.erase();
	RoutingLogic_RequireName_String.erase();
	RoutingLogic_BadTimeSpec_String.erase();
	RoutingLogic_Error_String.erase();
	RoutingLogic_NotCaptain_String.erase();
	ServerListCtrl_Pinging_String.erase();
	ServerListCtrl_PingFailed_String.erase();
	ServerListCtrl_Querying_String.erase();
	ServerListCtrl_QueryFailed_String.erase();
	ServerListCtrl_ListSummary_String.erase();
	ServerListCtrl_ListPingSummary_String.erase();
	ServerListCtrl_RefreshList_String.erase();
	ServerListCtrl_StopRefresh_String.erase();
	ServerListCtrl_NoFilter_String.erase();
	ServerListCtrl_AddRemove_String.erase();
	ServerListCtrl_Add_String.erase();
	ServerListCtrl_Remove_String.erase();
	ServerListCtrl_FullFilter_String.erase();
	ServerListCtrl_NotFullFilter_String.erase();
	ServerListCtrl_OpenFilter_String.erase();
	ServerListCtrl_PingFailed_Image = NULL;
	ServerListCtrl_GameExpanded_Image = NULL;
	ServerListCtrl_GameNotExpanded_Image = NULL;
	ServerListCtrl_GameAreInvited_Image = NULL;
	ServerListCtrl_GameNone_Image = NULL;
	ServerListCtrl_GameAskToJoin_Image = NULL;
	ServerListCtrl_GameHasPassword_Image = NULL;
	ServerListCtrl_GameNotInvited_Image = NULL;
	ServerListCtrl_GameInProgress_Image = NULL;
	ServerListCtrl_SkillOpen_Image = NULL;
	ServerListCtrl_SkillNovice_Image = NULL;
	ServerListCtrl_SkillIntermediate_Image = NULL;
	ServerListCtrl_SkillAdvanced_Image = NULL;
	ServerListCtrl_SkillExpert_Image = NULL;
	ServerOptionsCtrl_Connect_String.erase();
	ServerOptionsCtrl_RefreshServer_String.erase();
	ServerOptionsCtrl_HideDetails_String.erase();
	ServerOptionsCtrl_ShowDetails_String.erase();
	ServerOptionsCtrl_ResfreshList_String.erase();
	StagingLogic_DisconnectedTitle_String.erase();
	StagingLogic_DisconnectedDetails_String.erase();
	StagingLogic_BannedTitle_String.erase();
	StagingLogic_BannedDetails_String.erase();
	StagingLogic_KickedTitle_String.erase();
	StagingLogic_KickedDetails_String.erase();
	UserListCtrl_Ignore_String.erase();
	UserListCtrl_Unignore_String.erase();
	UserListCtrl_Normal_Image = NULL;
	UserListCtrl_Away_Image = NULL;
	UserListCtrl_Admin_Image = NULL;
	UserListCtrl_Moderator_Image = NULL;
	UserListCtrl_Captain_Image = NULL;
	UserListCtrl_Muted_Image = NULL;
	UserListCtrl_Ignored_Image = NULL;
	UserListCtrl_Blocked_Image = NULL;
	WONStatus_Success_String.erase();
	WONStatus_Killed_String.erase();
	WONStatus_TimedOut_String.erase();
	WONStatus_InvalidAddress_String.erase();
	WONStatus_GeneralFailure_String.erase();
	WONStatus_AuthFailure_String.erase();
	WONStatus_FTPFailure_String.erase();
	WONStatus_HTTPFailure_String.erase();
	WONStatus_MOTDFailure_String.erase();
	WONStatus_RoutingFailure_String.erase();
	WONStatus_GameSpyFailure_String.erase();
	WONStatus_DirFailure_String.erase();
	WONStatus_ChatFailure_String.erase();
	WONStatus_DatabaseFailure_String.erase();
	WONStatus_NotAllowed_String.erase();
	WONStatus_NotAuthorized_String.erase();
	WONStatus_BehindFirewall_String.erase();
	WONStatus_ExpiredCertificate_String.erase();
	WONStatus_InvalidCDKey_String.erase();
	WONStatus_CDKeyBanned_String.erase();
	WONStatus_CDKeyInUse_String.erase();
	WONStatus_CRCFailed_String.erase();
	WONStatus_UserExists_String.erase();
	WONStatus_UserNotFound_String.erase();
	WONStatus_BadPassword_String.erase();
	WONStatus_InvalidUserName_String.erase();
	WONStatus_BadCommunity_String.erase();
	WONStatus_NotInCommunity_String.erase();
	WONStatus_UserSeqInUse_String.erase();
	WONStatus_KeyNotActive_String.erase();
	WONStatus_KeyExpired_String.erase();
	WONStatus_BlankNamesNotAllowed_String.erase();
	WONStatus_CaptainRejectedYou_String.erase();
	WONStatus_ClientAlreadyBanned_String.erase();
	WONStatus_ClientAlreadyInGroup_String.erase();
	WONStatus_ClientAlreadyMuted_String.erase();
	WONStatus_ClientBanned_String.erase();
	WONStatus_ClientDoesNotExist_String.erase();
	WONStatus_ClientNotBanned_String.erase();
	WONStatus_ClientNotInGroup_String.erase();
	WONStatus_ClientNotInvited_String.erase();
	WONStatus_ClientNotMuted_String.erase();
	WONStatus_ConnectFailure_String.erase();
	WONStatus_GroupAlreadyClaimed_String.erase();
	WONStatus_GroupAlreadyExists_String.erase();
	WONStatus_GroupClosed_String.erase();
	WONStatus_GroupDeleted_String.erase();
	WONStatus_GroupDoesNotExist_String.erase();
	WONStatus_GroupFull_String.erase();
	WONStatus_GuestNamesReserved_String.erase();
	WONStatus_InvalidPassword_String.erase();
	WONStatus_MustBeAdmin_String.erase();
	WONStatus_MustBeCaptain_String.erase();
	WONStatus_MustBeCaptainOrModerator_String.erase();
	WONStatus_MustBeModerator_String.erase();
	WONStatus_ServerFull_String.erase();
	WONStatus_GroupNameExceedsMaximum_String.erase();
	WONStatus_JoinCanceled_String.erase();
	WONStatus_Throttled_String.erase();
	WONStatus_KeyNotUsed_String.erase();
	WONStatus_EMailPasswordDuplication_String.erase();
	WONStatus_AccountCreateDisabled_String.erase();
	WONStatus_AgeNotOldEnough_String.erase();
	WONStatus_ItemHasDependencies_String.erase();
	WONStatus_OutOfDate_String.erase();
	WONStatus_OutOfDateNoUpdate_String.erase();
	WONStatus_ValidNotLatest_String.erase();
	WONStatus_DataAlreadyExists_String.erase();
	WONStatus_DBError_String.erase();
	WONStatus_ConnRefused_String.erase();
}

