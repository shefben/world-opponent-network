#ifndef __LobbyResource_H__
#define __LobbyResource_H__
#include "WONGUI/WONGUIConfig/ResourceConfig.h"
#include "WONGUI/StringLocalize.h"
#include "WONGUI/EventTypes.h"

extern WONAPI::GUIString LobbyGlobal_Cancel_String;
extern WONAPI::GUIString LobbyGlobal_Lan_String;
extern WONAPI::GUIString LobbyGlobal_No_String;
extern WONAPI::GUIString LobbyGlobal_OK_String;
extern WONAPI::GUIString LobbyGlobal_Yes_String;
extern WONAPI::GUIString LobbyGlobal_QuickPlay_String;
extern WONAPI::ImagePtr LobbyGlobal_Searching_Image;
extern WONAPI::ImagePtr LobbyGlobal_Firewall_Image;
extern WONAPI::SoundPtr LobbyGlobal_ButtonClick_Sound;
extern WONAPI::SoundPtr LobbyGlobal_LobbyMusic_Sound;
extern WONAPI::SoundPtr LobbyGlobal_AllReady_Sound;
extern WONAPI::SoundPtr LobbyGlobal_AllNotReady_Sound;
extern WONAPI::SoundPtr LobbyGlobal_AskToJoinRecv_Sound;
extern WONAPI::SoundPtr LobbyGlobal_Error_Sound;
extern WONAPI::SoundPtr LobbyGlobal_FriendJoined_Sound;
extern WONAPI::SoundPtr LobbyGlobal_FriendLeft_Sound;
extern WONAPI::SoundPtr LobbyGlobal_InviteRecv_Sound;
extern WONAPI::SoundPtr LobbyGlobal_PlayerJoined_Sound;
extern WONAPI::SoundPtr LobbyGlobal_SierraLogon_Sound;
extern WONAPI::SoundPtr LobbyGlobal_WhisperRecv_Sound;

extern WONAPI::GUIString AccountLogic_InvalidCDKeyTitle_String;
extern WONAPI::GUIString AccountLogic_InvalidCDKeyDetails_String;

extern WONAPI::GUIString AddFriendDialog_StatusTitle_String;
extern WONAPI::GUIString AddFriendDialog_StatusDetails_String;

extern WONAPI::GUIString AdminActionDialog_Warn_String;
extern WONAPI::GUIString AdminActionDialog_Mute_String;
extern WONAPI::GUIString AdminActionDialog_UnMute_String;
extern WONAPI::GUIString AdminActionDialog_Kick_String;
extern WONAPI::GUIString AdminActionDialog_Ban_String;
extern WONAPI::GUIString AdminActionDialog_ExplanationOpt_String;
extern WONAPI::GUIString AdminActionDialog_ExplanationReq_String;

extern WONAPI::GUIString BadUserDialog_OneDay_String;
extern WONAPI::GUIString BadUserDialog_OneHour_String;
extern WONAPI::GUIString BadUserDialog_OneMinute_String;
extern WONAPI::GUIString BadUserDialog_OneSecond_String;
extern WONAPI::GUIString BadUserDialog_XDays_String;
extern WONAPI::GUIString BadUserDialog_XHours_String;
extern WONAPI::GUIString BadUserDialog_XMinutes_String;
extern WONAPI::GUIString BadUserDialog_XSeconds_String;
extern WONAPI::GUIString BadUserDialog_Infinite_String;

extern WONAPI::GUIString ChatCommandLogic_Whisper_String;
extern WONAPI::GUIString ChatCommandLogic_Block_String;
extern WONAPI::GUIString ChatCommandLogic_Emote_String;
extern WONAPI::GUIString ChatCommandLogic_Ignore_String;
extern WONAPI::GUIString ChatCommandLogic_Away_String;
extern WONAPI::GUIString ChatCommandLogic_Invite_String;
extern WONAPI::GUIString ChatCommandLogic_Uninvite_String;
extern WONAPI::GUIString ChatCommandLogic_Reply_String;
extern WONAPI::GUIString ChatCommandLogic_Clear_String;
extern WONAPI::GUIString ChatCommandLogic_Help_String;
extern WONAPI::GUIString ChatCommandLogic_ShowTeam_String;
extern WONAPI::GUIString ChatCommandLogic_Moderator_String;
extern WONAPI::GUIString ChatCommandLogic_ServerMute_String;
extern WONAPI::GUIString ChatCommandLogic_ServerBan_String;
extern WONAPI::GUIString ChatCommandLogic_Mute_String;
extern WONAPI::GUIString ChatCommandLogic_Ban_String;
extern WONAPI::GUIString ChatCommandLogic_Unmute_String;
extern WONAPI::GUIString ChatCommandLogic_ServerUnmute_String;
extern WONAPI::GUIString ChatCommandLogic_Unban_String;
extern WONAPI::GUIString ChatCommandLogic_ServerUnban_String;
extern WONAPI::GUIString ChatCommandLogic_Alert_String;
extern WONAPI::GUIString ChatCommandLogic_Warn_String;
extern WONAPI::GUIString ChatCommandLogic_HelpHeader_String;
extern WONAPI::GUIString ChatCommandLogic_HelpHelp_String;
extern WONAPI::GUIString ChatCommandLogic_ShowTeamHelp_String;
extern WONAPI::GUIString ChatCommandLogic_WhisperHelp_String;
extern WONAPI::GUIString ChatCommandLogic_BlockHelp_String;
extern WONAPI::GUIString ChatCommandLogic_IgnoreHelp_String;
extern WONAPI::GUIString ChatCommandLogic_ReplyHelp_String;
extern WONAPI::GUIString ChatCommandLogic_EmoteHelp_String;
extern WONAPI::GUIString ChatCommandLogic_AwayHelp_String;
extern WONAPI::GUIString ChatCommandLogic_ClearHelp_String;
extern WONAPI::GUIString ChatCommandLogic_InviteHelp_String;
extern WONAPI::GUIString ChatCommandLogic_UninviteHelp_String;
extern WONAPI::GUIString ChatCommandLogic_MuteHelp_String;
extern WONAPI::GUIString ChatCommandLogic_UnmuteHelp_String;
extern WONAPI::GUIString ChatCommandLogic_BanHelp_String;
extern WONAPI::GUIString ChatCommandLogic_UnbanHelp_String;
extern WONAPI::GUIString ChatCommandLogic_ModeratorHelp_String;
extern WONAPI::GUIString ChatCommandLogic_ServerMuteHelp_String;
extern WONAPI::GUIString ChatCommandLogic_ServerUnmuteHelp_String;
extern WONAPI::GUIString ChatCommandLogic_ServerBanHelp_String;
extern WONAPI::GUIString ChatCommandLogic_ServerUnbanHelp_String;
extern WONAPI::GUIString ChatCommandLogic_AlertHelp_String;
extern WONAPI::GUIString ChatCommandLogic_WarnHelp_String;

extern WONAPI::GUIString ChatCtrl_NormalChat_String;
extern WONAPI::GUIString ChatCtrl_Emote_String;
extern WONAPI::GUIString ChatCtrl_TeamChat_String;
extern WONAPI::GUIString ChatCtrl_TeamEmote_String;
extern WONAPI::GUIString ChatCtrl_YouWhisperToYourself_String;
extern WONAPI::GUIString ChatCtrl_YouWhisper_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWhispersToYou_String;
extern WONAPI::GUIString ChatCtrl_YouWarnSomeone_String;
extern WONAPI::GUIString ChatCtrl_YouWarnYourself_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWarnsYou_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWarnsSomeone_String;
extern WONAPI::GUIString ChatCtrl_EnteringRoom_String;
extern WONAPI::GUIString ChatCtrl_LeavingRoom_String;
extern WONAPI::GUIString ChatCtrl_Disconnected_String;
extern WONAPI::GUIString ChatCtrl_YouWereKicked_String;
extern WONAPI::GUIString ChatCtrl_YouWereBannedMinute_String;
extern WONAPI::GUIString ChatCtrl_YouWereBannedMinutes_String;
extern WONAPI::GUIString ChatCtrl_YouWereBannedHour_String;
extern WONAPI::GUIString ChatCtrl_YouWereBannedHours_String;
extern WONAPI::GUIString ChatCtrl_YouWereBannedDay_String;
extern WONAPI::GUIString ChatCtrl_YouWereBannedDays_String;
extern WONAPI::GUIString ChatCtrl_YouWereBannedIndefinitely_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasKicked_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedMinute_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedMinutes_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedHour_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedHours_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedDay_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedDays_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasBannedIndefinitely_String;
extern WONAPI::GUIString ChatCtrl_YouWereUnmuted_String;
extern WONAPI::GUIString ChatCtrl_YouWereMutedMinute_String;
extern WONAPI::GUIString ChatCtrl_YouWereMutedMinutes_String;
extern WONAPI::GUIString ChatCtrl_YouWereMutedHour_String;
extern WONAPI::GUIString ChatCtrl_YouWereMutedHours_String;
extern WONAPI::GUIString ChatCtrl_YouWereMutedDay_String;
extern WONAPI::GUIString ChatCtrl_YouWereMutedDays_String;
extern WONAPI::GUIString ChatCtrl_YouWereMutedIndefinitely_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasUnmuted_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedMinute_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedMinutes_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedHour_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedHours_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedDay_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedDays_String;
extern WONAPI::GUIString ChatCtrl_SomeoneWasMutedIndefinitely_String;
extern WONAPI::GUIString ChatCtrl_ModeratorComment_String;
extern WONAPI::GUIString ChatCtrl_AlertComment_String;
extern WONAPI::GUIString ChatCtrl_ServerAlert_String;
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedMinute_String;
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedMinutes_String;
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedHour_String;
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedHours_String;
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedDay_String;
extern WONAPI::GUIString ChatCtrl_ServerShutdownStartedDays_String;
extern WONAPI::GUIString ChatCtrl_ServerShutdownAborted_String;
extern WONAPI::GUIString ChatCtrl_SomeoneInvitesYou_String;
extern WONAPI::GUIString ChatCtrl_SomeoneUninvitesYou_String;
extern WONAPI::GUIString ChatCtrl_YouInviteSomeone_String;
extern WONAPI::GUIString ChatCtrl_YouUninviteSomeone_String;
extern WONAPI::GUIString ChatCtrl_SomeoneInvitesYouExpl_String;
extern WONAPI::GUIString ChatCtrl_SomeoneUninvitesYouExpl_String;
extern WONAPI::GUIString ChatCtrl_YouInviteSomeoneExpl_String;
extern WONAPI::GUIString ChatCtrl_YouUninviteSomeoneExpl_String;
extern WONAPI::GUIString ChatCtrl_Accept_String;
extern WONAPI::GUIString ChatCtrl_Reject_String;
extern WONAPI::GUIString ChatCtrl_SomeoneAsksToJoin_String;
extern WONAPI::GUIString ChatCtrl_SomeoneAsksToJoinExpl_String;
extern WONAPI::GUIString ChatCtrl_ClientNotOnServer_String;
extern WONAPI::GUIString ChatCtrl_ClientInGame_String;
extern WONAPI::GUIString ChatCtrl_ClientInChat_String;
extern WONAPI::GUIString ChatCtrl_ClientInGameAndChat_String;
extern WONAPI::GUIString ChatCtrl_ClientIsAnonymous_String;
extern WONAPI::GUIString ChatCtrl_FriendIsChatting_String;
extern WONAPI::GUIString ChatCtrl_FriendIsInGameStaging_String;
extern WONAPI::GUIString ChatCtrl_FriendIsPlaying_String;
extern WONAPI::GUIString ChatCtrl_FriendIsOn_String;
extern WONAPI::GUIString ChatCtrl_FriendIsNotOnline_String;
extern WONAPI::GUIString ChatCtrl_PlayerKicked_String;
extern WONAPI::GUIString ChatCtrl_PlayerBanned_String;
extern WONAPI::GUIString ChatCtrl_FriendJoined_String;
extern WONAPI::GUIString ChatCtrl_FriendLeft_String;
extern WONAPI::GUIString ChatCtrl_TeamMemberJoined_String;
extern WONAPI::GUIString ChatCtrl_TeamMemberLeft_String;
extern WONAPI::GUIString ChatCtrl_TeamMembers_String;
extern WONAPI::GUIString ChatCtrl_IgnoreList_String;

extern WONAPI::GUIString ChooseNetworkAdapterDialog_Default_String;
extern WONAPI::GUIString ChooseNetworkAdapterDialog_Unknown_String;

extern WONAPI::GUIString ClientOptionsCtrl_Whisper_String;
extern WONAPI::GUIString ClientOptionsCtrl_Invite_String;
extern WONAPI::GUIString ClientOptionsCtrl_Uninvite_String;
extern WONAPI::GUIString ClientOptionsCtrl_AddToFriends_String;
extern WONAPI::GUIString ClientOptionsCtrl_RemoveFromFriends_String;
extern WONAPI::GUIString ClientOptionsCtrl_ClearAFK_String;
extern WONAPI::GUIString ClientOptionsCtrl_SetAFK_String;
extern WONAPI::GUIString ClientOptionsCtrl_Ignore_String;
extern WONAPI::GUIString ClientOptionsCtrl_Unignore_String;
extern WONAPI::GUIString ClientOptionsCtrl_Block_String;
extern WONAPI::GUIString ClientOptionsCtrl_Unblock_String;
extern WONAPI::GUIString ClientOptionsCtrl_Warn_String;
extern WONAPI::GUIString ClientOptionsCtrl_Mute_String;
extern WONAPI::GUIString ClientOptionsCtrl_UnMute_String;
extern WONAPI::GUIString ClientOptionsCtrl_Kick_String;
extern WONAPI::GUIString ClientOptionsCtrl_Ban_String;

extern WONAPI::GUIString DirectConnectionOptionsCtrl_Connect_String;
extern WONAPI::GUIString DirectConnectionOptionsCtrl_Refresh_String;
extern WONAPI::GUIString DirectConnectionOptionsCtrl_Remove_String;

extern WONAPI::GUIString FriendOptionsCtrl_Locate_String;
extern WONAPI::GUIString FriendOptionsCtrl_RefreshList_String;
extern WONAPI::GUIString FriendOptionsCtrl_RemoveFromFriends_String;

extern WONAPI::ImagePtr FriendsListCtrl_Unknown_Image;
extern WONAPI::ImagePtr FriendsListCtrl_NotFound_Image;
extern WONAPI::ImagePtr FriendsListCtrl_InChat_Image;
extern WONAPI::ImagePtr FriendsListCtrl_InGame_Image;

extern WONAPI::GUIString GameOptionsPopup_Join_String;
extern WONAPI::GUIString GameOptionsPopup_Ping_String;
extern WONAPI::GUIString GameOptionsPopup_QueryDetails_String;
extern WONAPI::GUIString GameOptionsPopup_HideDetails_String;
extern WONAPI::GUIString GameOptionsPopup_ShowDetails_String;
extern WONAPI::GUIString GameOptionsPopup_Remove_String;

extern WONAPI::GUIString GameStagingCtrl_Ready_String;
extern WONAPI::GUIString GameStagingCtrl_NotReady_String;
extern WONAPI::GUIString GameStagingCtrl_StartGame_String;
extern WONAPI::GUIString GameStagingCtrl_LeaveGame_String;
extern WONAPI::GUIString GameStagingCtrl_DissolveGame_String;

extern WONAPI::GUIString GameStagingLogic_KickedTitle_String;
extern WONAPI::GUIString GameStagingLogic_KickedDetails_String;
extern WONAPI::GUIString GameStagingLogic_BannedTitle_String;
extern WONAPI::GUIString GameStagingLogic_BannedDetails_String;
extern WONAPI::GUIString GameStagingLogic_ConnectWarningTitle1_String;
extern WONAPI::GUIString GameStagingLogic_ConnectWarningDetails1_String;
extern WONAPI::GUIString GameStagingLogic_ConnectWarningTitle2_String;
extern WONAPI::GUIString GameStagingLogic_ConnectWarningDetails2_String;
extern WONAPI::GUIString GameStagingLogic_ConnectWarningTitle3_String;
extern WONAPI::GUIString GameStagingLogic_ConnectWarningDetails3_String;
extern WONAPI::GUIString GameStagingLogic_NoPingReplyError_String;
extern WONAPI::GUIString GameStagingLogic_GettingGameInfoTitle_String;
extern WONAPI::GUIString GameStagingLogic_GettingGameInfoDetails_String;
extern WONAPI::GUIString GameStagingLogic_JoinGameErrorTitle_String;
extern WONAPI::GUIString GameStagingLogic_VerifyingPingTitle_String;
extern WONAPI::GUIString GameStagingLogic_VerifyingPingDetails_String;
extern WONAPI::GUIString GameStagingLogic_DissolveGame_String;
extern WONAPI::GUIString GameStagingLogic_DissolveGameConfirm_String;
extern WONAPI::GUIString GameStagingLogic_LeaveGame_String;
extern WONAPI::GUIString GameStagingLogic_LeaveGameConfirm_String;
extern WONAPI::GUIString GameStagingLogic_NoQuickPlayGameTitle_String;
extern WONAPI::GUIString GameStagingLogic_NoQuickPlayGameDetails_String;

extern WONAPI::GUIString GameUserListCtrl_Ban_String;
extern WONAPI::GUIString GameUserListCtrl_Kick_String;
extern WONAPI::ImagePtr GameUserListCtrl_Ready_Image;

extern WONAPI::GUIString InternetQuickPlay_Welcome_String;
extern WONAPI::GUIString InternetQuickPlay_UserWelcome_String;

extern WONAPI::GUIString JoinResponse_Joined_String;
extern WONAPI::GUIString JoinResponse_ServerFull_String;
extern WONAPI::GUIString JoinResponse_InviteOnly_String;
extern WONAPI::GUIString JoinResponse_AskToJoin_String;
extern WONAPI::GUIString JoinResponse_GameInProgress_String;
extern WONAPI::GUIString JoinResponse_InvalidPassword_String;
extern WONAPI::GUIString JoinResponse_NotOnLAN_String;
extern WONAPI::GUIString JoinResponse_Blocked_String;
extern WONAPI::GUIString JoinResponse_Unknown_String;

extern WONAPI::GUIString LANLogic_SocketBindError_String;
extern WONAPI::GUIString LANLogic_SocketListenError_String;
extern WONAPI::GUIString LANLogic_ConnectToGameTitle_String;
extern WONAPI::GUIString LANLogic_ConnectToGameDetails_String;
extern WONAPI::GUIString LANLogic_CheckingAddressTitle_String;
extern WONAPI::GUIString LANLogic_CheckingAddressDetails_String;
extern WONAPI::GUIString LANLogic_RegisterTitle_String;
extern WONAPI::GUIString LANLogic_RegisterDetails_String;
extern WONAPI::GUIString LANLogic_JoinFailure_String;
extern WONAPI::GUIString LANLogic_JoinReplyError_String;
extern WONAPI::GUIString LANLogic_InvalidSubnet_String;

extern WONAPI::GUIString LobbyContainer_InitStart_String;
extern WONAPI::GUIString LobbyContainer_InitFailed_String;
extern WONAPI::GUIString LobbyContainer_CheckingVersion_String;
extern WONAPI::GUIString LobbyContainer_QueryingServers_String;
extern WONAPI::GUIString LobbyContainer_NoConnection_String;
extern WONAPI::GUIString LobbyContainer_DNSLookupFailed_String;
extern WONAPI::GUIString LobbyContainer_DirLookupFailure_String;
extern WONAPI::GUIString LobbyContainer_VersionCheckFailure_String;
extern WONAPI::GUIString LobbyContainer_InitComplete_String;
extern WONAPI::GUIString LobbyContainer_NoServers_String;
extern WONAPI::GUIString LobbyContainer_AuthServers_String;
extern WONAPI::GUIString LobbyContainer_AccountServers_String;
extern WONAPI::GUIString LobbyContainer_VersionServers_String;
extern WONAPI::GUIString LobbyContainer_CreateAccountTitle_String;
extern WONAPI::GUIString LobbyContainer_CreateAccountDetails_String;
extern WONAPI::GUIString LobbyContainer_RetrievePasswordTitle_String;
extern WONAPI::GUIString LobbyContainer_RetrievePasswordDetails_String;
extern WONAPI::GUIString LobbyContainer_RetrieveUsernameTitle_String;
extern WONAPI::GUIString LobbyContainer_RetrieveUsernameDetails_String;
extern WONAPI::GUIString LobbyContainer_QueryAccountTitle_String;
extern WONAPI::GUIString LobbyContainer_QueryAccountDetails_String;
extern WONAPI::GUIString LobbyContainer_UpdateAccountTitle_String;
extern WONAPI::GUIString LobbyContainer_UpdateAccountDetails_String;
extern WONAPI::GUIString LobbyContainer_LoginTitle_String;
extern WONAPI::GUIString LobbyContainer_LoginDetails_String;
extern WONAPI::GUIString LobbyContainer_GettingRoomsTitle_String;
extern WONAPI::GUIString LobbyContainer_GettingRoomsDetails_String;
extern WONAPI::GUIString LobbyContainer_VersionOutOfDataTitle_String;
extern WONAPI::GUIString LobbyContainer_VersionOutOfDataDetails_String;

extern WONAPI::GUIString LobbyGameStatus_Success_String;
extern WONAPI::GUIString LobbyGameStatus_GameFull_String;
extern WONAPI::GUIString LobbyGameStatus_GameInProgress_String;
extern WONAPI::GUIString LobbyGameStatus_UnpackFailure_String;
extern WONAPI::GUIString LobbyGameStatus_NotInvited_String;
extern WONAPI::GUIString LobbyGameStatus_CaptainRejectedYou_String;
extern WONAPI::GUIString LobbyGameStatus_InvalidPassword_String;
extern WONAPI::GUIString LobbyGameStatus_DuplicateName_String;

extern WONAPI::GUIString LobbyOptions_DefaultName_String;
extern WONAPI::GUIString LobbyOptions_CheckingAddress_String;
extern WONAPI::GUIString LobbyOptions_AddressUnavailable_String;

extern WONAPI::GUIString RoomCtrl_QueryingServer_String;
extern WONAPI::ImagePtr RoomCtrl_Password_Image;

extern WONAPI::GUIString RoomLabelCtrl_LabelPrefix_String;
extern WONAPI::GUIString RoomLabelCtrl_LabelServer_String;
extern WONAPI::GUIString RoomLabelCtrl_LabelUser_String;
extern WONAPI::GUIString RoomLabelCtrl_LabelUsers_String;
extern WONAPI::GUIString RoomLabelCtrl_LabelDisconnected_String;

extern WONAPI::GUIString RoutingLogic_GetClientListTitle_String;
extern WONAPI::GUIString RoutingLogic_GetClientListDetails_String;
extern WONAPI::GUIString RoutingLogic_GetGroupListTitle_String;
extern WONAPI::GUIString RoutingLogic_GetGroupListDetails_String;
extern WONAPI::GUIString RoutingLogic_GameDissolvedTitle_String;
extern WONAPI::GUIString RoutingLogic_GameDissolvedDetails_String;
extern WONAPI::GUIString RoutingLogic_NoServersTitle_String;
extern WONAPI::GUIString RoutingLogic_NoServersDetails_String;
extern WONAPI::GUIString RoutingLogic_TooManyFriendsTitle_String;
extern WONAPI::GUIString RoutingLogic_TooManyFriendsDetails_String;
extern WONAPI::GUIString RoutingLogic_AlreadyJoinedTitle_String;
extern WONAPI::GUIString RoutingLogic_AlreadyInGame_String;
extern WONAPI::GUIString RoutingLogic_AlreadyInRoom_String;
extern WONAPI::GUIString RoutingLogic_JoinGroupTitle_String;
extern WONAPI::GUIString RoutingLogic_JoinGroupDetails_String;
extern WONAPI::GUIString RoutingLogic_ConnectTitle_String;
extern WONAPI::GUIString RoutingLogic_ConnectDetails_String;
extern WONAPI::GUIString RoutingLogic_RegisterTitle_String;
extern WONAPI::GUIString RoutingLogic_RegisterDetails_String;
extern WONAPI::GUIString RoutingLogic_QueryGroupsTitle_String;
extern WONAPI::GUIString RoutingLogic_QueryGroupsDetails_String;
extern WONAPI::GUIString RoutingLogic_CreateGroupTitle_String;
extern WONAPI::GUIString RoutingLogic_CreateGroupDetails_String;
extern WONAPI::GUIString RoutingLogic_GetBadUsersTitle_String;
extern WONAPI::GUIString RoutingLogic_GetBadUsersDetails_String;
extern WONAPI::GUIString RoutingLogic_DisconnectedTitle_String;
extern WONAPI::GUIString RoutingLogic_DisconnectedDetails_String;
extern WONAPI::GUIString RoutingLogic_AskingCaptainTitle_String;
extern WONAPI::GUIString RoutingLogic_AskingCaptainDetails_String;
extern WONAPI::GUIString RoutingLogic_CollectingPingsTitle_String;
extern WONAPI::GUIString RoutingLogic_CollectingPingsDetails_String;
extern WONAPI::GUIString RoutingLogic_GettingGameListTitle_String;
extern WONAPI::GUIString RoutingLogic_GettingGameListDetails_String;
extern WONAPI::GUIString RoutingLogic_MuteClientTitle_String;
extern WONAPI::GUIString RoutingLogic_UnmuteClientTitle_String;
extern WONAPI::GUIString RoutingLogic_BanClientTitle_String;
extern WONAPI::GUIString RoutingLogic_KickClientTitle_String;
extern WONAPI::GUIString RoutingLogic_ModerateDetails_String;
extern WONAPI::GUIString RoutingLogic_FriendNotFoundTitle_String;
extern WONAPI::GUIString RoutingLogic_FriendNotFoundDetails_String;
extern WONAPI::GUIString RoutingLogic_ClientNotFound_String;
extern WONAPI::GUIString RoutingLogic_ClientAmbiguous_String;
extern WONAPI::GUIString RoutingLogic_InvalidCommand_String;
extern WONAPI::GUIString RoutingLogic_RequireName_String;
extern WONAPI::GUIString RoutingLogic_BadTimeSpec_String;
extern WONAPI::GUIString RoutingLogic_Error_String;
extern WONAPI::GUIString RoutingLogic_NotCaptain_String;

extern WONAPI::GUIString ServerListCtrl_Pinging_String;
extern WONAPI::GUIString ServerListCtrl_PingFailed_String;
extern WONAPI::GUIString ServerListCtrl_Querying_String;
extern WONAPI::GUIString ServerListCtrl_QueryFailed_String;
extern WONAPI::GUIString ServerListCtrl_ListSummary_String;
extern WONAPI::GUIString ServerListCtrl_ListPingSummary_String;
extern WONAPI::GUIString ServerListCtrl_RefreshList_String;
extern WONAPI::GUIString ServerListCtrl_StopRefresh_String;
extern WONAPI::GUIString ServerListCtrl_NoFilter_String;
extern WONAPI::GUIString ServerListCtrl_AddRemove_String;
extern WONAPI::GUIString ServerListCtrl_Add_String;
extern WONAPI::GUIString ServerListCtrl_Remove_String;
extern WONAPI::GUIString ServerListCtrl_FullFilter_String;
extern WONAPI::GUIString ServerListCtrl_NotFullFilter_String;
extern WONAPI::GUIString ServerListCtrl_OpenFilter_String;
extern WONAPI::ImagePtr ServerListCtrl_PingFailed_Image;
extern WONAPI::ImagePtr ServerListCtrl_GameExpanded_Image;
extern WONAPI::ImagePtr ServerListCtrl_GameNotExpanded_Image;
extern WONAPI::ImagePtr ServerListCtrl_GameAreInvited_Image;
extern WONAPI::ImagePtr ServerListCtrl_GameNone_Image;
extern WONAPI::ImagePtr ServerListCtrl_GameAskToJoin_Image;
extern WONAPI::ImagePtr ServerListCtrl_GameHasPassword_Image;
extern WONAPI::ImagePtr ServerListCtrl_GameNotInvited_Image;
extern WONAPI::ImagePtr ServerListCtrl_GameInProgress_Image;
extern WONAPI::ImagePtr ServerListCtrl_SkillOpen_Image;
extern WONAPI::ImagePtr ServerListCtrl_SkillNovice_Image;
extern WONAPI::ImagePtr ServerListCtrl_SkillIntermediate_Image;
extern WONAPI::ImagePtr ServerListCtrl_SkillAdvanced_Image;
extern WONAPI::ImagePtr ServerListCtrl_SkillExpert_Image;

extern WONAPI::GUIString ServerOptionsCtrl_Connect_String;
extern WONAPI::GUIString ServerOptionsCtrl_RefreshServer_String;
extern WONAPI::GUIString ServerOptionsCtrl_HideDetails_String;
extern WONAPI::GUIString ServerOptionsCtrl_ShowDetails_String;
extern WONAPI::GUIString ServerOptionsCtrl_ResfreshList_String;

extern WONAPI::GUIString StagingLogic_DisconnectedTitle_String;
extern WONAPI::GUIString StagingLogic_DisconnectedDetails_String;
extern WONAPI::GUIString StagingLogic_BannedTitle_String;
extern WONAPI::GUIString StagingLogic_BannedDetails_String;
extern WONAPI::GUIString StagingLogic_KickedTitle_String;
extern WONAPI::GUIString StagingLogic_KickedDetails_String;

extern WONAPI::GUIString UserListCtrl_Ignore_String;
extern WONAPI::GUIString UserListCtrl_Unignore_String;
extern WONAPI::ImagePtr UserListCtrl_Normal_Image;
extern WONAPI::ImagePtr UserListCtrl_Away_Image;
extern WONAPI::ImagePtr UserListCtrl_Admin_Image;
extern WONAPI::ImagePtr UserListCtrl_Moderator_Image;
extern WONAPI::ImagePtr UserListCtrl_Captain_Image;
extern WONAPI::ImagePtr UserListCtrl_Muted_Image;
extern WONAPI::ImagePtr UserListCtrl_Ignored_Image;
extern WONAPI::ImagePtr UserListCtrl_Blocked_Image;

extern WONAPI::GUIString WONStatus_Success_String;
extern WONAPI::GUIString WONStatus_Killed_String;
extern WONAPI::GUIString WONStatus_TimedOut_String;
extern WONAPI::GUIString WONStatus_InvalidAddress_String;
extern WONAPI::GUIString WONStatus_GeneralFailure_String;
extern WONAPI::GUIString WONStatus_AuthFailure_String;
extern WONAPI::GUIString WONStatus_FTPFailure_String;
extern WONAPI::GUIString WONStatus_HTTPFailure_String;
extern WONAPI::GUIString WONStatus_MOTDFailure_String;
extern WONAPI::GUIString WONStatus_RoutingFailure_String;
extern WONAPI::GUIString WONStatus_GameSpyFailure_String;
extern WONAPI::GUIString WONStatus_DirFailure_String;
extern WONAPI::GUIString WONStatus_ChatFailure_String;
extern WONAPI::GUIString WONStatus_DatabaseFailure_String;
extern WONAPI::GUIString WONStatus_NotAllowed_String;
extern WONAPI::GUIString WONStatus_NotAuthorized_String;
extern WONAPI::GUIString WONStatus_BehindFirewall_String;
extern WONAPI::GUIString WONStatus_ExpiredCertificate_String;
extern WONAPI::GUIString WONStatus_InvalidCDKey_String;
extern WONAPI::GUIString WONStatus_CDKeyBanned_String;
extern WONAPI::GUIString WONStatus_CDKeyInUse_String;
extern WONAPI::GUIString WONStatus_CRCFailed_String;
extern WONAPI::GUIString WONStatus_UserExists_String;
extern WONAPI::GUIString WONStatus_UserNotFound_String;
extern WONAPI::GUIString WONStatus_BadPassword_String;
extern WONAPI::GUIString WONStatus_InvalidUserName_String;
extern WONAPI::GUIString WONStatus_BadCommunity_String;
extern WONAPI::GUIString WONStatus_NotInCommunity_String;
extern WONAPI::GUIString WONStatus_UserSeqInUse_String;
extern WONAPI::GUIString WONStatus_KeyNotActive_String;
extern WONAPI::GUIString WONStatus_KeyExpired_String;
extern WONAPI::GUIString WONStatus_BlankNamesNotAllowed_String;
extern WONAPI::GUIString WONStatus_CaptainRejectedYou_String;
extern WONAPI::GUIString WONStatus_ClientAlreadyBanned_String;
extern WONAPI::GUIString WONStatus_ClientAlreadyInGroup_String;
extern WONAPI::GUIString WONStatus_ClientAlreadyMuted_String;
extern WONAPI::GUIString WONStatus_ClientBanned_String;
extern WONAPI::GUIString WONStatus_ClientDoesNotExist_String;
extern WONAPI::GUIString WONStatus_ClientNotBanned_String;
extern WONAPI::GUIString WONStatus_ClientNotInGroup_String;
extern WONAPI::GUIString WONStatus_ClientNotInvited_String;
extern WONAPI::GUIString WONStatus_ClientNotMuted_String;
extern WONAPI::GUIString WONStatus_ConnectFailure_String;
extern WONAPI::GUIString WONStatus_GroupAlreadyClaimed_String;
extern WONAPI::GUIString WONStatus_GroupAlreadyExists_String;
extern WONAPI::GUIString WONStatus_GroupClosed_String;
extern WONAPI::GUIString WONStatus_GroupDeleted_String;
extern WONAPI::GUIString WONStatus_GroupDoesNotExist_String;
extern WONAPI::GUIString WONStatus_GroupFull_String;
extern WONAPI::GUIString WONStatus_GuestNamesReserved_String;
extern WONAPI::GUIString WONStatus_InvalidPassword_String;
extern WONAPI::GUIString WONStatus_MustBeAdmin_String;
extern WONAPI::GUIString WONStatus_MustBeCaptain_String;
extern WONAPI::GUIString WONStatus_MustBeCaptainOrModerator_String;
extern WONAPI::GUIString WONStatus_MustBeModerator_String;
extern WONAPI::GUIString WONStatus_ServerFull_String;
extern WONAPI::GUIString WONStatus_GroupNameExceedsMaximum_String;
extern WONAPI::GUIString WONStatus_JoinCanceled_String;
extern WONAPI::GUIString WONStatus_Throttled_String;
extern WONAPI::GUIString WONStatus_KeyNotUsed_String;
extern WONAPI::GUIString WONStatus_EMailPasswordDuplication_String;
extern WONAPI::GUIString WONStatus_AccountCreateDisabled_String;
extern WONAPI::GUIString WONStatus_AgeNotOldEnough_String;
extern WONAPI::GUIString WONStatus_ItemHasDependencies_String;
extern WONAPI::GUIString WONStatus_OutOfDate_String;
extern WONAPI::GUIString WONStatus_OutOfDateNoUpdate_String;
extern WONAPI::GUIString WONStatus_ValidNotLatest_String;
extern WONAPI::GUIString WONStatus_DataAlreadyExists_String;
extern WONAPI::GUIString WONStatus_DBError_String;
extern WONAPI::GUIString WONStatus_ConnRefused_String;

WONAPI::ResourceConfigTablePtr LobbyResource_Init(WONAPI::ResourceConfigTable *theTable = NULL);
enum LobbyResourceCtrlId
{
	LobbyResourceCtrlId_Min = WONAPI::ControlId_UserMin,
	ID_ShowCreateAccount,
	ID_ShowLostPassword,
	ID_ShowLostUserName,
	ID_ShowMOTD,
	ID_ShowTOU,
	ID_LoginScreenBack,
	ID_ShowLoginHelp,
	ID_ShowInternetScreen,
	ID_ShowLanScreen,
	ID_Logout,
	ID_HostGame,
	ID_ChangeRoom,
	ID_JoinGame,
	ID_LeaveGame,
	ID_StartGame,
	ID_RefreshGameList,
	ID_AddFriend,
	ID_RefreshFriends,
	ID_AddLANName,
	ID_ChangeLANBroadcast,
	ID_FilterDirtyWordsCheck,
	ID_AnonymousToFriendsCheck,
	ID_ChooseNetworkAdapter,

};
enum LobbyResourceId
{
	LobbyGlobal_Cancel_String_Id,
	LobbyGlobal_Lan_String_Id,
	LobbyGlobal_No_String_Id,
	LobbyGlobal_OK_String_Id,
	LobbyGlobal_Yes_String_Id,
	LobbyGlobal_QuickPlay_String_Id,
	LobbyGlobal_Searching_Image_Id,
	LobbyGlobal_Firewall_Image_Id,
	LobbyGlobal_ButtonClick_Sound_Id,
	LobbyGlobal_LobbyMusic_Sound_Id,
	LobbyGlobal_AllReady_Sound_Id,
	LobbyGlobal_AllNotReady_Sound_Id,
	LobbyGlobal_AskToJoinRecv_Sound_Id,
	LobbyGlobal_Error_Sound_Id,
	LobbyGlobal_FriendJoined_Sound_Id,
	LobbyGlobal_FriendLeft_Sound_Id,
	LobbyGlobal_InviteRecv_Sound_Id,
	LobbyGlobal_PlayerJoined_Sound_Id,
	LobbyGlobal_SierraLogon_Sound_Id,
	LobbyGlobal_WhisperRecv_Sound_Id,

	AccountLogic_InvalidCDKeyTitle_String_Id,
	AccountLogic_InvalidCDKeyDetails_String_Id,

	AddFriendDialog_StatusTitle_String_Id,
	AddFriendDialog_StatusDetails_String_Id,

	AdminActionDialog_Warn_String_Id,
	AdminActionDialog_Mute_String_Id,
	AdminActionDialog_UnMute_String_Id,
	AdminActionDialog_Kick_String_Id,
	AdminActionDialog_Ban_String_Id,
	AdminActionDialog_ExplanationOpt_String_Id,
	AdminActionDialog_ExplanationReq_String_Id,

	BadUserDialog_OneDay_String_Id,
	BadUserDialog_OneHour_String_Id,
	BadUserDialog_OneMinute_String_Id,
	BadUserDialog_OneSecond_String_Id,
	BadUserDialog_XDays_String_Id,
	BadUserDialog_XHours_String_Id,
	BadUserDialog_XMinutes_String_Id,
	BadUserDialog_XSeconds_String_Id,
	BadUserDialog_Infinite_String_Id,

	ChatCommandLogic_Whisper_String_Id,
	ChatCommandLogic_Block_String_Id,
	ChatCommandLogic_Emote_String_Id,
	ChatCommandLogic_Ignore_String_Id,
	ChatCommandLogic_Away_String_Id,
	ChatCommandLogic_Invite_String_Id,
	ChatCommandLogic_Uninvite_String_Id,
	ChatCommandLogic_Reply_String_Id,
	ChatCommandLogic_Clear_String_Id,
	ChatCommandLogic_Help_String_Id,
	ChatCommandLogic_ShowTeam_String_Id,
	ChatCommandLogic_Moderator_String_Id,
	ChatCommandLogic_ServerMute_String_Id,
	ChatCommandLogic_ServerBan_String_Id,
	ChatCommandLogic_Mute_String_Id,
	ChatCommandLogic_Ban_String_Id,
	ChatCommandLogic_Unmute_String_Id,
	ChatCommandLogic_ServerUnmute_String_Id,
	ChatCommandLogic_Unban_String_Id,
	ChatCommandLogic_ServerUnban_String_Id,
	ChatCommandLogic_Alert_String_Id,
	ChatCommandLogic_Warn_String_Id,
	ChatCommandLogic_HelpHeader_String_Id,
	ChatCommandLogic_HelpHelp_String_Id,
	ChatCommandLogic_ShowTeamHelp_String_Id,
	ChatCommandLogic_WhisperHelp_String_Id,
	ChatCommandLogic_BlockHelp_String_Id,
	ChatCommandLogic_IgnoreHelp_String_Id,
	ChatCommandLogic_ReplyHelp_String_Id,
	ChatCommandLogic_EmoteHelp_String_Id,
	ChatCommandLogic_AwayHelp_String_Id,
	ChatCommandLogic_ClearHelp_String_Id,
	ChatCommandLogic_InviteHelp_String_Id,
	ChatCommandLogic_UninviteHelp_String_Id,
	ChatCommandLogic_MuteHelp_String_Id,
	ChatCommandLogic_UnmuteHelp_String_Id,
	ChatCommandLogic_BanHelp_String_Id,
	ChatCommandLogic_UnbanHelp_String_Id,
	ChatCommandLogic_ModeratorHelp_String_Id,
	ChatCommandLogic_ServerMuteHelp_String_Id,
	ChatCommandLogic_ServerUnmuteHelp_String_Id,
	ChatCommandLogic_ServerBanHelp_String_Id,
	ChatCommandLogic_ServerUnbanHelp_String_Id,
	ChatCommandLogic_AlertHelp_String_Id,
	ChatCommandLogic_WarnHelp_String_Id,

	ChatCtrl_NormalChat_String_Id,
	ChatCtrl_Emote_String_Id,
	ChatCtrl_TeamChat_String_Id,
	ChatCtrl_TeamEmote_String_Id,
	ChatCtrl_YouWhisperToYourself_String_Id,
	ChatCtrl_YouWhisper_String_Id,
	ChatCtrl_SomeoneWhispersToYou_String_Id,
	ChatCtrl_YouWarnSomeone_String_Id,
	ChatCtrl_YouWarnYourself_String_Id,
	ChatCtrl_SomeoneWarnsYou_String_Id,
	ChatCtrl_SomeoneWarnsSomeone_String_Id,
	ChatCtrl_EnteringRoom_String_Id,
	ChatCtrl_LeavingRoom_String_Id,
	ChatCtrl_Disconnected_String_Id,
	ChatCtrl_YouWereKicked_String_Id,
	ChatCtrl_YouWereBannedMinute_String_Id,
	ChatCtrl_YouWereBannedMinutes_String_Id,
	ChatCtrl_YouWereBannedHour_String_Id,
	ChatCtrl_YouWereBannedHours_String_Id,
	ChatCtrl_YouWereBannedDay_String_Id,
	ChatCtrl_YouWereBannedDays_String_Id,
	ChatCtrl_YouWereBannedIndefinitely_String_Id,
	ChatCtrl_SomeoneWasKicked_String_Id,
	ChatCtrl_SomeoneWasBannedMinute_String_Id,
	ChatCtrl_SomeoneWasBannedMinutes_String_Id,
	ChatCtrl_SomeoneWasBannedHour_String_Id,
	ChatCtrl_SomeoneWasBannedHours_String_Id,
	ChatCtrl_SomeoneWasBannedDay_String_Id,
	ChatCtrl_SomeoneWasBannedDays_String_Id,
	ChatCtrl_SomeoneWasBannedIndefinitely_String_Id,
	ChatCtrl_YouWereUnmuted_String_Id,
	ChatCtrl_YouWereMutedMinute_String_Id,
	ChatCtrl_YouWereMutedMinutes_String_Id,
	ChatCtrl_YouWereMutedHour_String_Id,
	ChatCtrl_YouWereMutedHours_String_Id,
	ChatCtrl_YouWereMutedDay_String_Id,
	ChatCtrl_YouWereMutedDays_String_Id,
	ChatCtrl_YouWereMutedIndefinitely_String_Id,
	ChatCtrl_SomeoneWasUnmuted_String_Id,
	ChatCtrl_SomeoneWasMutedMinute_String_Id,
	ChatCtrl_SomeoneWasMutedMinutes_String_Id,
	ChatCtrl_SomeoneWasMutedHour_String_Id,
	ChatCtrl_SomeoneWasMutedHours_String_Id,
	ChatCtrl_SomeoneWasMutedDay_String_Id,
	ChatCtrl_SomeoneWasMutedDays_String_Id,
	ChatCtrl_SomeoneWasMutedIndefinitely_String_Id,
	ChatCtrl_ModeratorComment_String_Id,
	ChatCtrl_AlertComment_String_Id,
	ChatCtrl_ServerAlert_String_Id,
	ChatCtrl_ServerShutdownStartedMinute_String_Id,
	ChatCtrl_ServerShutdownStartedMinutes_String_Id,
	ChatCtrl_ServerShutdownStartedHour_String_Id,
	ChatCtrl_ServerShutdownStartedHours_String_Id,
	ChatCtrl_ServerShutdownStartedDay_String_Id,
	ChatCtrl_ServerShutdownStartedDays_String_Id,
	ChatCtrl_ServerShutdownAborted_String_Id,
	ChatCtrl_SomeoneInvitesYou_String_Id,
	ChatCtrl_SomeoneUninvitesYou_String_Id,
	ChatCtrl_YouInviteSomeone_String_Id,
	ChatCtrl_YouUninviteSomeone_String_Id,
	ChatCtrl_SomeoneInvitesYouExpl_String_Id,
	ChatCtrl_SomeoneUninvitesYouExpl_String_Id,
	ChatCtrl_YouInviteSomeoneExpl_String_Id,
	ChatCtrl_YouUninviteSomeoneExpl_String_Id,
	ChatCtrl_Accept_String_Id,
	ChatCtrl_Reject_String_Id,
	ChatCtrl_SomeoneAsksToJoin_String_Id,
	ChatCtrl_SomeoneAsksToJoinExpl_String_Id,
	ChatCtrl_ClientNotOnServer_String_Id,
	ChatCtrl_ClientInGame_String_Id,
	ChatCtrl_ClientInChat_String_Id,
	ChatCtrl_ClientInGameAndChat_String_Id,
	ChatCtrl_ClientIsAnonymous_String_Id,
	ChatCtrl_FriendIsChatting_String_Id,
	ChatCtrl_FriendIsInGameStaging_String_Id,
	ChatCtrl_FriendIsPlaying_String_Id,
	ChatCtrl_FriendIsOn_String_Id,
	ChatCtrl_FriendIsNotOnline_String_Id,
	ChatCtrl_PlayerKicked_String_Id,
	ChatCtrl_PlayerBanned_String_Id,
	ChatCtrl_FriendJoined_String_Id,
	ChatCtrl_FriendLeft_String_Id,
	ChatCtrl_TeamMemberJoined_String_Id,
	ChatCtrl_TeamMemberLeft_String_Id,
	ChatCtrl_TeamMembers_String_Id,
	ChatCtrl_IgnoreList_String_Id,

	ChooseNetworkAdapterDialog_Default_String_Id,
	ChooseNetworkAdapterDialog_Unknown_String_Id,

	ClientOptionsCtrl_Whisper_String_Id,
	ClientOptionsCtrl_Invite_String_Id,
	ClientOptionsCtrl_Uninvite_String_Id,
	ClientOptionsCtrl_AddToFriends_String_Id,
	ClientOptionsCtrl_RemoveFromFriends_String_Id,
	ClientOptionsCtrl_ClearAFK_String_Id,
	ClientOptionsCtrl_SetAFK_String_Id,
	ClientOptionsCtrl_Ignore_String_Id,
	ClientOptionsCtrl_Unignore_String_Id,
	ClientOptionsCtrl_Block_String_Id,
	ClientOptionsCtrl_Unblock_String_Id,
	ClientOptionsCtrl_Warn_String_Id,
	ClientOptionsCtrl_Mute_String_Id,
	ClientOptionsCtrl_UnMute_String_Id,
	ClientOptionsCtrl_Kick_String_Id,
	ClientOptionsCtrl_Ban_String_Id,

	DirectConnectionOptionsCtrl_Connect_String_Id,
	DirectConnectionOptionsCtrl_Refresh_String_Id,
	DirectConnectionOptionsCtrl_Remove_String_Id,

	FriendOptionsCtrl_Locate_String_Id,
	FriendOptionsCtrl_RefreshList_String_Id,
	FriendOptionsCtrl_RemoveFromFriends_String_Id,

	FriendsListCtrl_Unknown_Image_Id,
	FriendsListCtrl_NotFound_Image_Id,
	FriendsListCtrl_InChat_Image_Id,
	FriendsListCtrl_InGame_Image_Id,

	GameOptionsPopup_Join_String_Id,
	GameOptionsPopup_Ping_String_Id,
	GameOptionsPopup_QueryDetails_String_Id,
	GameOptionsPopup_HideDetails_String_Id,
	GameOptionsPopup_ShowDetails_String_Id,
	GameOptionsPopup_Remove_String_Id,

	GameStagingCtrl_Ready_String_Id,
	GameStagingCtrl_NotReady_String_Id,
	GameStagingCtrl_StartGame_String_Id,
	GameStagingCtrl_LeaveGame_String_Id,
	GameStagingCtrl_DissolveGame_String_Id,

	GameStagingLogic_KickedTitle_String_Id,
	GameStagingLogic_KickedDetails_String_Id,
	GameStagingLogic_BannedTitle_String_Id,
	GameStagingLogic_BannedDetails_String_Id,
	GameStagingLogic_ConnectWarningTitle1_String_Id,
	GameStagingLogic_ConnectWarningDetails1_String_Id,
	GameStagingLogic_ConnectWarningTitle2_String_Id,
	GameStagingLogic_ConnectWarningDetails2_String_Id,
	GameStagingLogic_ConnectWarningTitle3_String_Id,
	GameStagingLogic_ConnectWarningDetails3_String_Id,
	GameStagingLogic_NoPingReplyError_String_Id,
	GameStagingLogic_GettingGameInfoTitle_String_Id,
	GameStagingLogic_GettingGameInfoDetails_String_Id,
	GameStagingLogic_JoinGameErrorTitle_String_Id,
	GameStagingLogic_VerifyingPingTitle_String_Id,
	GameStagingLogic_VerifyingPingDetails_String_Id,
	GameStagingLogic_DissolveGame_String_Id,
	GameStagingLogic_DissolveGameConfirm_String_Id,
	GameStagingLogic_LeaveGame_String_Id,
	GameStagingLogic_LeaveGameConfirm_String_Id,
	GameStagingLogic_NoQuickPlayGameTitle_String_Id,
	GameStagingLogic_NoQuickPlayGameDetails_String_Id,

	GameUserListCtrl_Ban_String_Id,
	GameUserListCtrl_Kick_String_Id,
	GameUserListCtrl_Ready_Image_Id,

	InternetQuickPlay_Welcome_String_Id,
	InternetQuickPlay_UserWelcome_String_Id,

	JoinResponse_Joined_String_Id,
	JoinResponse_ServerFull_String_Id,
	JoinResponse_InviteOnly_String_Id,
	JoinResponse_AskToJoin_String_Id,
	JoinResponse_GameInProgress_String_Id,
	JoinResponse_InvalidPassword_String_Id,
	JoinResponse_NotOnLAN_String_Id,
	JoinResponse_Blocked_String_Id,
	JoinResponse_Unknown_String_Id,

	LANLogic_SocketBindError_String_Id,
	LANLogic_SocketListenError_String_Id,
	LANLogic_ConnectToGameTitle_String_Id,
	LANLogic_ConnectToGameDetails_String_Id,
	LANLogic_CheckingAddressTitle_String_Id,
	LANLogic_CheckingAddressDetails_String_Id,
	LANLogic_RegisterTitle_String_Id,
	LANLogic_RegisterDetails_String_Id,
	LANLogic_JoinFailure_String_Id,
	LANLogic_JoinReplyError_String_Id,
	LANLogic_InvalidSubnet_String_Id,

	LobbyContainer_InitStart_String_Id,
	LobbyContainer_InitFailed_String_Id,
	LobbyContainer_CheckingVersion_String_Id,
	LobbyContainer_QueryingServers_String_Id,
	LobbyContainer_NoConnection_String_Id,
	LobbyContainer_DNSLookupFailed_String_Id,
	LobbyContainer_DirLookupFailure_String_Id,
	LobbyContainer_VersionCheckFailure_String_Id,
	LobbyContainer_InitComplete_String_Id,
	LobbyContainer_NoServers_String_Id,
	LobbyContainer_AuthServers_String_Id,
	LobbyContainer_AccountServers_String_Id,
	LobbyContainer_VersionServers_String_Id,
	LobbyContainer_CreateAccountTitle_String_Id,
	LobbyContainer_CreateAccountDetails_String_Id,
	LobbyContainer_RetrievePasswordTitle_String_Id,
	LobbyContainer_RetrievePasswordDetails_String_Id,
	LobbyContainer_RetrieveUsernameTitle_String_Id,
	LobbyContainer_RetrieveUsernameDetails_String_Id,
	LobbyContainer_QueryAccountTitle_String_Id,
	LobbyContainer_QueryAccountDetails_String_Id,
	LobbyContainer_UpdateAccountTitle_String_Id,
	LobbyContainer_UpdateAccountDetails_String_Id,
	LobbyContainer_LoginTitle_String_Id,
	LobbyContainer_LoginDetails_String_Id,
	LobbyContainer_GettingRoomsTitle_String_Id,
	LobbyContainer_GettingRoomsDetails_String_Id,
	LobbyContainer_VersionOutOfDataTitle_String_Id,
	LobbyContainer_VersionOutOfDataDetails_String_Id,

	LobbyGameStatus_Success_String_Id,
	LobbyGameStatus_GameFull_String_Id,
	LobbyGameStatus_GameInProgress_String_Id,
	LobbyGameStatus_UnpackFailure_String_Id,
	LobbyGameStatus_NotInvited_String_Id,
	LobbyGameStatus_CaptainRejectedYou_String_Id,
	LobbyGameStatus_InvalidPassword_String_Id,
	LobbyGameStatus_DuplicateName_String_Id,

	LobbyOptions_DefaultName_String_Id,
	LobbyOptions_CheckingAddress_String_Id,
	LobbyOptions_AddressUnavailable_String_Id,

	RoomCtrl_QueryingServer_String_Id,
	RoomCtrl_Password_Image_Id,

	RoomLabelCtrl_LabelPrefix_String_Id,
	RoomLabelCtrl_LabelServer_String_Id,
	RoomLabelCtrl_LabelUser_String_Id,
	RoomLabelCtrl_LabelUsers_String_Id,
	RoomLabelCtrl_LabelDisconnected_String_Id,

	RoutingLogic_GetClientListTitle_String_Id,
	RoutingLogic_GetClientListDetails_String_Id,
	RoutingLogic_GetGroupListTitle_String_Id,
	RoutingLogic_GetGroupListDetails_String_Id,
	RoutingLogic_GameDissolvedTitle_String_Id,
	RoutingLogic_GameDissolvedDetails_String_Id,
	RoutingLogic_NoServersTitle_String_Id,
	RoutingLogic_NoServersDetails_String_Id,
	RoutingLogic_TooManyFriendsTitle_String_Id,
	RoutingLogic_TooManyFriendsDetails_String_Id,
	RoutingLogic_AlreadyJoinedTitle_String_Id,
	RoutingLogic_AlreadyInGame_String_Id,
	RoutingLogic_AlreadyInRoom_String_Id,
	RoutingLogic_JoinGroupTitle_String_Id,
	RoutingLogic_JoinGroupDetails_String_Id,
	RoutingLogic_ConnectTitle_String_Id,
	RoutingLogic_ConnectDetails_String_Id,
	RoutingLogic_RegisterTitle_String_Id,
	RoutingLogic_RegisterDetails_String_Id,
	RoutingLogic_QueryGroupsTitle_String_Id,
	RoutingLogic_QueryGroupsDetails_String_Id,
	RoutingLogic_CreateGroupTitle_String_Id,
	RoutingLogic_CreateGroupDetails_String_Id,
	RoutingLogic_GetBadUsersTitle_String_Id,
	RoutingLogic_GetBadUsersDetails_String_Id,
	RoutingLogic_DisconnectedTitle_String_Id,
	RoutingLogic_DisconnectedDetails_String_Id,
	RoutingLogic_AskingCaptainTitle_String_Id,
	RoutingLogic_AskingCaptainDetails_String_Id,
	RoutingLogic_CollectingPingsTitle_String_Id,
	RoutingLogic_CollectingPingsDetails_String_Id,
	RoutingLogic_GettingGameListTitle_String_Id,
	RoutingLogic_GettingGameListDetails_String_Id,
	RoutingLogic_MuteClientTitle_String_Id,
	RoutingLogic_UnmuteClientTitle_String_Id,
	RoutingLogic_BanClientTitle_String_Id,
	RoutingLogic_KickClientTitle_String_Id,
	RoutingLogic_ModerateDetails_String_Id,
	RoutingLogic_FriendNotFoundTitle_String_Id,
	RoutingLogic_FriendNotFoundDetails_String_Id,
	RoutingLogic_ClientNotFound_String_Id,
	RoutingLogic_ClientAmbiguous_String_Id,
	RoutingLogic_InvalidCommand_String_Id,
	RoutingLogic_RequireName_String_Id,
	RoutingLogic_BadTimeSpec_String_Id,
	RoutingLogic_Error_String_Id,
	RoutingLogic_NotCaptain_String_Id,

	ServerListCtrl_Pinging_String_Id,
	ServerListCtrl_PingFailed_String_Id,
	ServerListCtrl_Querying_String_Id,
	ServerListCtrl_QueryFailed_String_Id,
	ServerListCtrl_ListSummary_String_Id,
	ServerListCtrl_ListPingSummary_String_Id,
	ServerListCtrl_RefreshList_String_Id,
	ServerListCtrl_StopRefresh_String_Id,
	ServerListCtrl_NoFilter_String_Id,
	ServerListCtrl_AddRemove_String_Id,
	ServerListCtrl_Add_String_Id,
	ServerListCtrl_Remove_String_Id,
	ServerListCtrl_FullFilter_String_Id,
	ServerListCtrl_NotFullFilter_String_Id,
	ServerListCtrl_OpenFilter_String_Id,
	ServerListCtrl_PingFailed_Image_Id,
	ServerListCtrl_GameExpanded_Image_Id,
	ServerListCtrl_GameNotExpanded_Image_Id,
	ServerListCtrl_GameAreInvited_Image_Id,
	ServerListCtrl_GameNone_Image_Id,
	ServerListCtrl_GameAskToJoin_Image_Id,
	ServerListCtrl_GameHasPassword_Image_Id,
	ServerListCtrl_GameNotInvited_Image_Id,
	ServerListCtrl_GameInProgress_Image_Id,
	ServerListCtrl_SkillOpen_Image_Id,
	ServerListCtrl_SkillNovice_Image_Id,
	ServerListCtrl_SkillIntermediate_Image_Id,
	ServerListCtrl_SkillAdvanced_Image_Id,
	ServerListCtrl_SkillExpert_Image_Id,

	ServerOptionsCtrl_Connect_String_Id,
	ServerOptionsCtrl_RefreshServer_String_Id,
	ServerOptionsCtrl_HideDetails_String_Id,
	ServerOptionsCtrl_ShowDetails_String_Id,
	ServerOptionsCtrl_ResfreshList_String_Id,

	StagingLogic_DisconnectedTitle_String_Id,
	StagingLogic_DisconnectedDetails_String_Id,
	StagingLogic_BannedTitle_String_Id,
	StagingLogic_BannedDetails_String_Id,
	StagingLogic_KickedTitle_String_Id,
	StagingLogic_KickedDetails_String_Id,

	UserListCtrl_Ignore_String_Id,
	UserListCtrl_Unignore_String_Id,
	UserListCtrl_Normal_Image_Id,
	UserListCtrl_Away_Image_Id,
	UserListCtrl_Admin_Image_Id,
	UserListCtrl_Moderator_Image_Id,
	UserListCtrl_Captain_Image_Id,
	UserListCtrl_Muted_Image_Id,
	UserListCtrl_Ignored_Image_Id,
	UserListCtrl_Blocked_Image_Id,

	WONStatus_Success_String_Id,
	WONStatus_Killed_String_Id,
	WONStatus_TimedOut_String_Id,
	WONStatus_InvalidAddress_String_Id,
	WONStatus_GeneralFailure_String_Id,
	WONStatus_AuthFailure_String_Id,
	WONStatus_FTPFailure_String_Id,
	WONStatus_HTTPFailure_String_Id,
	WONStatus_MOTDFailure_String_Id,
	WONStatus_RoutingFailure_String_Id,
	WONStatus_GameSpyFailure_String_Id,
	WONStatus_DirFailure_String_Id,
	WONStatus_ChatFailure_String_Id,
	WONStatus_DatabaseFailure_String_Id,
	WONStatus_NotAllowed_String_Id,
	WONStatus_NotAuthorized_String_Id,
	WONStatus_BehindFirewall_String_Id,
	WONStatus_ExpiredCertificate_String_Id,
	WONStatus_InvalidCDKey_String_Id,
	WONStatus_CDKeyBanned_String_Id,
	WONStatus_CDKeyInUse_String_Id,
	WONStatus_CRCFailed_String_Id,
	WONStatus_UserExists_String_Id,
	WONStatus_UserNotFound_String_Id,
	WONStatus_BadPassword_String_Id,
	WONStatus_InvalidUserName_String_Id,
	WONStatus_BadCommunity_String_Id,
	WONStatus_NotInCommunity_String_Id,
	WONStatus_UserSeqInUse_String_Id,
	WONStatus_KeyNotActive_String_Id,
	WONStatus_KeyExpired_String_Id,
	WONStatus_BlankNamesNotAllowed_String_Id,
	WONStatus_CaptainRejectedYou_String_Id,
	WONStatus_ClientAlreadyBanned_String_Id,
	WONStatus_ClientAlreadyInGroup_String_Id,
	WONStatus_ClientAlreadyMuted_String_Id,
	WONStatus_ClientBanned_String_Id,
	WONStatus_ClientDoesNotExist_String_Id,
	WONStatus_ClientNotBanned_String_Id,
	WONStatus_ClientNotInGroup_String_Id,
	WONStatus_ClientNotInvited_String_Id,
	WONStatus_ClientNotMuted_String_Id,
	WONStatus_ConnectFailure_String_Id,
	WONStatus_GroupAlreadyClaimed_String_Id,
	WONStatus_GroupAlreadyExists_String_Id,
	WONStatus_GroupClosed_String_Id,
	WONStatus_GroupDeleted_String_Id,
	WONStatus_GroupDoesNotExist_String_Id,
	WONStatus_GroupFull_String_Id,
	WONStatus_GuestNamesReserved_String_Id,
	WONStatus_InvalidPassword_String_Id,
	WONStatus_MustBeAdmin_String_Id,
	WONStatus_MustBeCaptain_String_Id,
	WONStatus_MustBeCaptainOrModerator_String_Id,
	WONStatus_MustBeModerator_String_Id,
	WONStatus_ServerFull_String_Id,
	WONStatus_GroupNameExceedsMaximum_String_Id,
	WONStatus_JoinCanceled_String_Id,
	WONStatus_Throttled_String_Id,
	WONStatus_KeyNotUsed_String_Id,
	WONStatus_EMailPasswordDuplication_String_Id,
	WONStatus_AccountCreateDisabled_String_Id,
	WONStatus_AgeNotOldEnough_String_Id,
	WONStatus_ItemHasDependencies_String_Id,
	WONStatus_OutOfDate_String_Id,
	WONStatus_OutOfDateNoUpdate_String_Id,
	WONStatus_ValidNotLatest_String_Id,
	WONStatus_DataAlreadyExists_String_Id,
	WONStatus_DBError_String_Id,
	WONStatus_ConnRefused_String_Id,

};
const WONAPI::GUIString& LobbyResource_GetString(LobbyResourceId theId);
WONAPI::Image* LobbyResource_GetImage(LobbyResourceId theId);
WONAPI::Sound* LobbyResource_GetSound(LobbyResourceId theId);
WONAPI::Font* LobbyResource_GetFont(LobbyResourceId theId);
const WONAPI::Background& LobbyResource_GetBackground(LobbyResourceId theId);
DWORD LobbyResource_GetColor(LobbyResourceId theId);
int LobbyResource_GetInt(LobbyResourceId theId);
void LobbyResource_SafeGlueResources(WONAPI::ResourceConfigTable *theTable);
void LobbyResource_UnloadResources();

#endif
