#include "server.h"

class AuthServer :	public Server
{
public:
	AuthServer(int port);
	~AuthServer();
	int start();
	void auth1challenge(SOCKET client,char buffer);
private:
	SOCKET listeningSocket;
	char name[100];
	int port;
	static void doConnect(SOCKET socket,DWORD clientIp, char * serverName);
	static unsigned int __stdcall waitForConnections(void * param);
	DWORD externalIp;
};
namespace WONMsg {
    class TRawMsg;
    class TMessage;
};
DWORD WINAPI authRecv(LPVOID lpParam);
		/*WONMsg::TMsgAuth1Request aMsg;
		  aMsg.SetAuthMode(WONAuth::AUTH_SESSION);
		  aMsg.SetEncryptMode(WONAuth::ENCRYPT_BLOWFISH);
		  aMsg.SetEncryptFlags(WONAuth::EFLAGS_NONE);
		  aMsg.SetRawBuf(mCertificate->GetRaw(),mCertificate->GetRawLen());
          aMsg.Pack();
		  

OR
		aBuf.AppendShort(thePubKeyBlock.GetBlockId());
		aBuf.AppendByte(mNeedKeyFlg);
		aBuf.AppendByte(mCreateAcctFlg);
		aBuf.Append_PW_STRING(mUserName);
		aBuf.Append_PW_STRING(mCommunityName);
		aBuf.Append_PW_STRING(mNicknameKey);
		aBuf.Append_PW_STRING(mPassword);
		aBuf.Append_PW_STRING(mNewPassword);
	 	aBuf.AppendShort(mCDKey.size());

		  WONMsg::TMsgAuth1LoginRequestHL aMsg;
		  aMsg.SetKeyBlockId(mPublicKeyBlock->GetBlockId());
		  aMsg.SetNeedKeyFlg(mPrivateKey==NULL);
		  aMsg.SetCreateAcctFlg(mCreateAccount);
		  aMsg.SetUserName(StringToWString(mLoginName));
		  aMsg.SetCommunityName(StringToWString("Halflife"));
		  aMsg.SetPassword(StringToWString(mPassword));
		  aMsg.SetNewPassword(StringToWString(mNewPassword));

		*/