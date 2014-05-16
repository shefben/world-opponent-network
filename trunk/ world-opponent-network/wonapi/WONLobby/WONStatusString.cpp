
#include "WONStatus.h"
#include "WONStatusString.h"
#include "WONCommon/StringUtil.h"
#include "WONGUI/StringLocalize.h"
#include "LobbyResource.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static GUIString GeneralStatusError(const GUIString &theStr, WONStatus theStatus)
{
	return StringLocalize::GetStr(theStr,WONStatusToString(theStatus));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString WONAPI::FriendlyWONStatusToString(WONStatus theStatus)
{
	GUIString aString;

	switch(theStatus)
	{
	case WS_Success: return WONStatus_Success_String;
	case WS_TimedOut: return WONStatus_TimedOut_String; 
	case WS_Killed:	return WONStatus_Killed_String; 												
	case WS_InvalidAddress: return WONStatus_InvalidAddress_String;

	case WS_Pending: 
	case WS_Incomplete: 
	case WS_None: 												
	case WS_AwaitingCompletion: 
	case WS_InvalidMessage: 										
	case WS_FailedToOpenFile: 									
	case WS_FailedToWriteToFile: 		
	case WS_BlockMode_NotSupported: 
	case WS_AsyncMode_NotSupported: 
	case WS_InvalidPublicKey: 
	case WS_MessageUnpackFailure: 
	case WS_AuthRequired: 
	case WS_NoReply: 
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);

// AsyncSocket
	case WS_AsyncSocket_ConnectFailed:  return WONStatus_ConnRefused_String;
	case WS_AsyncSocket_InvalidSocket: 
	case WS_AsyncSocket_PartialSendTo: 						
	case WS_AsyncSocket_StreamNotAllowed: 
	case WS_AsyncSocket_DatagramNotAllowed: 					
	case WS_AsyncSocket_Shutdown: 					
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);
// SocketOp
	case WS_SocketOp_InvalidSocket: 						
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);

// RecvMsgOp
	case WS_RecvMsg_InvalidLengthFieldSize: 
	case WS_RecvMsg_InvalidMessageLength: 					
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);

// Auth Peer to Peer
	case WS_PeerAuthClient_Challenge1DecryptFailure: 
	case WS_PeerAuthClient_Challenge1InvalidSecretLen: 				
	case WS_PeerAuthClient_Challenge1InvalidSecretKey: 				
	case WS_PeerAuthClient_Challenge1CertificateUnpackFailure: 
	case WS_PeerAuthClient_Challenge1CertificateVerifyFailure: 		
	case WS_PeerAuthClient_Challenge2EncryptFailure: 	
	case WS_PeerAuthClient_CompleteDecryptFailure: 				
	case WS_PeerAuthClient_CompleteInvalidSecretLen: 				
	case WS_PeerAuthClient_CompleteInvalidSecretKey: 				
	case WS_PeerAuthClient_InvalidServiceType: 				
	case WS_PeerAuthClient_UnexpectedChallenge: 					
	case WS_PeerAuthClient_InvalidMessage: 					
	case WS_PeerAuthClient_MsgUnpackFailure: 						
		return GeneralStatusError(WONStatus_AuthFailure_String,theStatus);

	case WS_PeerAuthServer_NotStarted: 
	case WS_PeerAuthServer_InvalidServiceType: 
	case WS_PeerAuthServer_UnexpectedRequest: 
	case WS_PeerAuthServer_UnexpectedChallenge: 
	case WS_PeerAuthServer_InvalidMessage: 
	case WS_PeerAuthServer_MsgUnpackFailure: 		
	case WS_PeerAuthServer_InvalidAuthMode: 
	case WS_PeerAuthServer_InvalidEncryptType: 
	case WS_PeerAuthServer_InvalidClientCertificate: 
	case WS_PeerAuthServer_ExpiredClientCertificate: 
	case WS_PeerAuthServer_FailedToVerifyClientCertificate: 
	case WS_PeerAuthServer_FailedToEncryptWithClientPubKey: 
	case WS_PeerAuthServer_FailedToDecryptWithPrivateKey: 
	case WS_PeerAuthServer_InvalidSecretA: 
	case WS_PeerAuthServer_InvalidSecretB: 
		return GeneralStatusError(WONStatus_AuthFailure_String,theStatus);

// PeerAuthOp
	case WS_PeerAuth_GetCertFailure: 
		return GeneralStatusError(WONStatus_AuthFailure_String,theStatus);

// Server Request Op
	case WS_ServerReq_FailedAllServers: 
	case WS_ServerReq_NoServersSpecified: 
	case WS_ServerReq_NeedAuthContext: 					
	case WS_ServerReq_TryNextServer: 					
	case WS_ServerReq_Send: 						
	case WS_ServerReq_Recv: 
	case WS_ServerReq_ExitCommunicationLoop: 
	case WS_ServerReq_SessionExpired: 
	case WS_ServerReq_GetCertFailure: 
	case WS_ServerReq_InvalidReplyHeader: 
	case WS_ServerReq_UnpackFailure: 
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);

// Get Cert Op
	case WS_GetCert_PubKeyEncryptFailure: 
	case WS_GetCert_InvalidPubKeyReply: 				
	case WS_GetCert_InvalidLoginReply: 					
	case WS_GetCert_UnableToFindHashFile: 
	case WS_GetCert_UnexpectedLoginChallenge: 			
	case WS_GetCert_NeedVerifierKey: 			
	case WS_GetCert_FailedToVerifyPubKeyBlock: 
	case WS_GetCert_InvalidCertificate: 
	case WS_GetCert_InvalidPubKeyBlock: 
	case WS_GetCert_DecryptFailure: 
	case WS_GetCert_InvalidPrivateKey: 
	case WS_GetCert_InvalidSecretConfirm: 
	case WS_GetCert_MissingCertificate: 
		return GeneralStatusError(WONStatus_AuthFailure_String,theStatus);

// Auth Data
	case WS_AuthData_SetCertificateUnpackFailed: 
	case WS_AuthData_SetPubKeyBlockUnpackFailed: 			
	case WS_AuthData_SetCertificateVerifyFailed: 			
	case WS_AuthData_SetPrivateKeyFailed: 			
		return GeneralStatusError(WONStatus_AuthFailure_String,theStatus);

// Auth Session
	case WS_AuthSession_EncryptFailure: 
	case WS_AuthSession_DecryptSessionIdMismatch: 
	case WS_AuthSession_DecryptFailure: 		
	case WS_AuthSession_DecryptBadLen: 					
	case WS_AuthSession_DecryptInvalidSequenceNum: 
	case WS_AuthSession_DecryptUnpackFailure: 		
		return GeneralStatusError(WONStatus_AuthFailure_String,theStatus);

// Server Connection
	case WS_ServerConnection_RecvNotAllowedInAsyncMode: 
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);

// FTP Op
	case WS_FTP_InvalidResponse: 
	case WS_FTP_StatusError: 
	case WS_FTP_InvalidPasvResponse: 
		return GeneralStatusError(WONStatus_FTPFailure_String,theStatus);

// HTTP Op
	case WS_HTTP_InvalidHeader: 
	case WS_HTTP_StatusError: 
	case WS_HTTP_InvalidRedirect: 
	case WS_HTTP_TooManyRedirects: 
	case WS_HTTP_Redirected: 
	case WS_HTTP_SendSuccess: 
		return GeneralStatusError(WONStatus_HTTPFailure_String,theStatus);

// GetMOTD Op
	case WS_GetMOTD_SysNotFound: 
	case WS_GetMOTD_GameNotFound: 
		return GeneralStatusError(WONStatus_MOTDFailure_String,theStatus);

// BaseMsgRequest 
	case WS_BaseMsgRequest_NoRequest: 
	case WS_BaseMsgRequest_UnpackFailure: 					
	case WS_BaseMsgRequest_BadHeaderType: 			
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);
	
// RoutingOp
	case WS_RoutingOp_ReplyUnpackFailure: 
	case WS_RoutingOp_DontWantReply: 
	case WS_RoutingOp_NeedMoreReplies: 
		return GeneralStatusError(WONStatus_RoutingFailure_String,theStatus);

// GameSpy Support
	case WS_GameSpySupport_UnhandledError: 
	case WS_GameSpySupport_WinSockError: 
	case WS_GameSpySupport_BindError: 
	case WS_GameSpySupport_DNSError: 
	case WS_GameSpySupport_ConnError: 
		return GeneralStatusError(WONStatus_GameSpyFailure_String,theStatus);

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Server Status   ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

	case WS_CommServ_NotAllowed: return WONStatus_NotAllowed_String;
	case WS_CommServ_NotAuthorized: return WONStatus_NotAuthorized_String;
	case WS_CommServ_BehindFirewall: return WONStatus_BehindFirewall_String;

	case WS_CommServ_BadPeerCertificate:
	case WS_CommServ_ExpiredPeerCertificate:
		return WONStatus_ExpiredCertificate_String;

	case WS_CommServ_Failure:
	case WS_CommServ_NotAvailable:
	case WS_CommServ_InvalidParameters:
	case WS_CommServ_AlreadyExists:
	case WS_CommServ_MessageExceedsMax:
	case WS_CommServ_SessionNotFound:
	case WS_CommServ_ResourcesLow:
	case WS_CommServ_NoPortAvailable:
	case WS_CommServ_TimedOut:
	case WS_CommServ_NotSupported:
	case WS_CommServ_DecryptionFailed:
	case WS_CommServ_InvalidSessionId:
	case WS_CommServ_InvalidSequenceNumber:
	case WS_CommServ_InvalidMessage:
	case WS_CommServ_StatusCommon_BannedAddress:
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);

// AuthServer Status
    case WS_AuthServ_BadCDKey:
    case WS_AuthServ_InvalidCDKey:
		return WONStatus_InvalidCDKey_String;

	case WS_AuthServ_CDKeyBanned: return WONStatus_CDKeyBanned_String;
	case WS_AuthServ_CDKeyInUse: return WONStatus_CDKeyInUse_String;
	case WS_AuthServ_CRCFailed: return WONStatus_CRCFailed_String;
    case WS_AuthServ_UserExists: return WONStatus_UserExists_String;
	case WS_AuthServ_UserNotFound: return WONStatus_UserNotFound_String;
    case WS_AuthServ_BadPassword: return WONStatus_BadPassword_String;
    case WS_AuthServ_InvalidUserName: return WONStatus_InvalidUserName_String;
	case WS_AuthServ_BadCommunity: return WONStatus_BadCommunity_String;
    case WS_AuthServ_NotInCommunity: return WONStatus_NotInCommunity_String;
    case WS_AuthServ_UserSeqInUse: return WONStatus_UserSeqInUse_String;
    case WS_AuthServ_KeyNotActive: return WONStatus_KeyNotActive_String;
    case WS_AuthServ_KeyExpired: return WONStatus_KeyExpired_String;

    case WS_AuthServ_BadPubKeyBlock:
    case WS_AuthServ_SetCUDError:
		return GeneralStatusError(WONStatus_AuthFailure_String,theStatus);

// DirServer Status
	case WS_DirServ_DirNotFound:
	case WS_DirServ_ServiceNotFound:
	case WS_DirServ_DirExists:
	case WS_DirServ_ServiceExists:
	case WS_DirServ_DirIsFull:
	case WS_DirServ_AlreadyConnected:
	case WS_DirServ_EntityTooLarge:
	case WS_DirServ_MaxDataObjects:
	case WS_DirServ_BadDataOffset:
	case WS_DirServ_InvalidPath:
	case WS_DirServ_InvalidGetFlags:
	case WS_DirServ_InvalidKey:
	case WS_DirServ_InvalidMode:
	case WS_DirServ_InvalidLifespan:
	case WS_DirServ_InvalidDataObject:
	case WS_DirServ_NoDataObjects:
	case WS_DirServ_DataObjectExists:
	case WS_DirServ_DataObjectNotFound:
	case WS_DirServ_InvalidACLType:
	case WS_DirServ_PermissionExists:
	case WS_DirServ_PermissionNotFound:
	case WS_DirServ_MaxPermissions:
	case WS_DirServ_NoACLs:
	case WS_DirServ_MultiGetPartialFailure:
	case WS_DirServ_MultiGetFailedAllRequests:
		return GeneralStatusError(WONStatus_DirFailure_String,theStatus);

	// Routing ServerG2
	case WS_RoutingServG2_BlankNamesNotAllowed: return WONStatus_BlankNamesNotAllowed_String;
	case WS_RoutingServG2_CaptainRejectedYou: return WONStatus_CaptainRejectedYou_String;
	case WS_RoutingServG2_ClientAlreadyBanned: return WONStatus_ClientAlreadyBanned_String;
	case WS_RoutingServG2_ClientAlreadyInGroup: return WONStatus_ClientAlreadyInGroup_String;
	case WS_RoutingServG2_ClientAlreadyMuted: return WONStatus_ClientAlreadyMuted_String;
	case WS_RoutingServG2_ClientBanned: return WONStatus_ClientBanned_String;
	case WS_RoutingServG2_ClientDoesNotExist: return WONStatus_ClientDoesNotExist_String;
	case WS_RoutingServG2_ClientNotBanned: return WONStatus_ClientNotBanned_String;
	case WS_RoutingServG2_ClientNotInGroup: return WONStatus_ClientNotInGroup_String;
	case WS_RoutingServG2_ClientNotInvited: return WONStatus_ClientNotInvited_String;
	case WS_RoutingServG2_ClientNotMuted: return WONStatus_ClientNotMuted_String;
	case WS_RoutingServG2_ConnectFailure: return WONStatus_ConnectFailure_String;
	case WS_RoutingServG2_GroupAlreadyClaimed: return WONStatus_GroupAlreadyClaimed_String;
	case WS_RoutingServG2_GroupAlreadyExists: return WONStatus_GroupAlreadyExists_String;
	case WS_RoutingServG2_GroupClosed: return WONStatus_GroupClosed_String;
	case WS_RoutingServG2_GroupDeleted: return WONStatus_GroupDeleted_String;
	case WS_RoutingServG2_GroupDoesNotExist: return WONStatus_GroupDoesNotExist_String;
	case WS_RoutingServG2_GroupFull: return WONStatus_GroupFull_String;
	case WS_RoutingServG2_GuestNamesReserved: return WONStatus_GuestNamesReserved_String;
	case WS_RoutingServG2_InvalidPassword: return WONStatus_InvalidPassword_String;
	case WS_RoutingServG2_MustBeAdmin: return WONStatus_MustBeAdmin_String;
	case WS_RoutingServG2_MustBeCaptain: return WONStatus_MustBeCaptain_String;
	case WS_RoutingServG2_MustBeCaptainOrModerator: return WONStatus_MustBeCaptainOrModerator_String;
	case WS_RoutingServG2_MustBeModerator: return WONStatus_MustBeModerator_String;
	case WS_RoutingServG2_ServerFull: return WONStatus_ServerFull_String;
	case WS_RoutingServG2_GroupNameExceedsMaximum: return WONStatus_GroupNameExceedsMaximum_String;
	case WS_RoutingServG2_JoinCanceled: return WONStatus_JoinCanceled_String;
	case WS_RoutingServG2_Throttled: return WONStatus_Throttled_String;

	case WS_RoutingServG2_ShutdownTimerAlreadyExists:
	case WS_RoutingServG2_ShutdownTimerDoesNotExist:
	case WS_RoutingServG2_SubscriptionAlreadyExists:
	case WS_RoutingServG2_SubscriptionDoesNotExist:
	case WS_RoutingServG2_SubscriptionDoesNotExists:
	case WS_RoutingServG2_TooManyMemberships:
	case WS_RoutingServG2_TooManyRecipients:
	case WS_RoutingServG2_MustBeAuthenticated:
	case WS_RoutingServG2_ClientAlreadyExists:
	case WS_RoutingServG2_ClientAlreadyRegistered:
	case WS_RoutingServG2_ClientNotPermitted:
	case WS_RoutingServG2_DuplicateDataObject:
	case WS_RoutingServG2_InvalidContentType:
	case WS_RoutingServG2_InvalidWONUserId:
	case WS_RoutingServG2_LoginTypeNotSupported:
	case WS_RoutingServG2_MustBeClient:
	case WS_RoutingServG2_MustBeSelf:
	case WS_RoutingServG2_MustBeVisible:
	case WS_RoutingServG2_NoPendingJoin:
	case WS_RoutingServG2_NoUsernameInCertificate:
	case WS_RoutingServG2_ObjectDoesNotExist:
	case WS_RoutingServG2_ObserversNotAllowed:
	case WS_RoutingServG2_OffsetTooLarge:
	case WS_RoutingServG2_Pending:
		return GeneralStatusError(WONStatus_ChatFailure_String,theStatus);

	// DBProxy Server
	case WS_DBProxyServ_UserExists: return WONStatus_UserExists_String;
	case WS_DBProxyServ_InvalidUserName: return WONStatus_InvalidUserName_String;
	case WS_DBProxyServ_KeyInUse: return WONStatus_CDKeyInUse_String;
	case WS_DBProxyServ_UserDoesNotExist: return WONStatus_UserNotFound_String;
	case WS_DBProxyServ_InvalidCDKey: return WONStatus_InvalidCDKey_String;
	case WS_DBProxyServ_KeyNotUsed: return WONStatus_KeyNotUsed_String;
	case WS_DBProxyServ_EMailPasswordDuplication: return WONStatus_EMailPasswordDuplication_String;
	case WS_DBProxyServ_AccountCreateDisabled: return WONStatus_AccountCreateDisabled_String;
	case WS_DBProxyServ_InvalidPassword: return WONStatus_BadPassword_String;
	case WS_DBProxyServ_AgeNotOldEnough: return WONStatus_AgeNotOldEnough_String;
	case WS_DBProxyServ_ItemHasDependencies: return WONStatus_ItemHasDependencies_String;
	case WS_DBProxyServ_OutOfDate: return WONStatus_OutOfDate_String;
	case WS_DBProxyServ_OutOfDateNoUpdate: return WONStatus_OutOfDateNoUpdate_String;
	case WS_DBProxyServ_ValidNotLatest: return WONStatus_ValidNotLatest_String;
	case WS_DBProxyServ_DataAlreadyExists: return WONStatus_DataAlreadyExists_String;
	case WS_DBProxyServ_DBError: return WONStatus_DBError_String;

	case WS_DBProxyServ_NoData:
	case WS_DBProxyServ_NoModule:
		return GeneralStatusError(WONStatus_DatabaseFailure_String,theStatus);

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Winsock Status   ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
	case WS_WSAECONNREFUSED: return WONStatus_ConnRefused_String;


	case WS_WSAEINTR:
	case WS_WSAEBADF:
	case WS_WSAEACCES:
	case WS_WSAEFAULT:
	case WS_WSAEINVAL:
	case WS_WSAEMFILE:
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);

	case WS_WSAEWOULDBLOCK:
	case WS_WSAEINPROGRESS:
	case WS_WSAEALREADY:
	case WS_WSAENOTSOCK:
	case WS_WSAEDESTADDRREQ:
	case WS_WSAEMSGSIZE:
	case WS_WSAEPROTOTYPE:
	case WS_WSAENOPROTOOPT:
	case WS_WSAEPROTONOSUPPORT:
	case WS_WSAESOCKTNOSUPPORT:
	case WS_WSAEOPNOTSUPP:
	case WS_WSAEPFNOSUPPORT:
	case WS_WSAEAFNOSUPPORT:
	case WS_WSAEADDRINUSE:
	case WS_WSAEADDRNOTAVAIL:
	case WS_WSAENETDOWN:
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);

	case WS_WSAENETUNREACH:
	case WS_WSAENETRESET:
	case WS_WSAECONNABORTED:
	case WS_WSAECONNRESET:
	case WS_WSAENOBUFS:
	case WS_WSAEISCONN:
	case WS_WSAENOTCONN:
	case WS_WSAESHUTDOWN:
	case WS_WSAETOOMANYREFS:
	case WS_WSAETIMEDOUT:
	case WS_WSAELOOP:
	case WS_WSAENAMETOOLONG:
	case WS_WSAEHOSTDOWN:
	case WS_WSAEHOSTUNREACH:
	case WS_WSAENOTEMPTY:
	case WS_WSAEPROCLIM:
	case WS_WSAEUSERS:
	case WS_WSAEDQUOT:
	case WS_WSAESTALE:
	case WS_WSAEREMOTE:
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);

	case WS_WSASYSNOTREADY:
	case WS_WSAVERNOTSUPPORTED:
	case WS_WSANOTINITIALISED:
	case WS_WSAEDISCON:
	default:
		return GeneralStatusError(WONStatus_GeneralFailure_String,theStatus);
	}
}
