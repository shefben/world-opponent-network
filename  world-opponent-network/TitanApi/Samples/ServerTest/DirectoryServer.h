#include "Server.h"

/* The directory server is basically a a node/folder structured server.  
 * It is used for everything from, holding chatroom informaation
 * to holding Game Patch information.
 * 
 * This server uses Mysql to hold all its 'large' database structure*/

class DirectoryServer : public Server
{
public:
	DirectoryServer(int port);
	~DirectoryServer();
	int start();
private:
	SOCKET listeningSocket;
	char name[100];
	int port;
	static void doConnect(SOCKET socket,DWORD clientIp, char * serverName);
	static unsigned int __stdcall waitForConnections(void * param);
	DWORD externalIp;
/*	// Servers - Directory service entity->name
	static const unsigned short *serverAuth = L"AuthServer";
	static const unsigned short *serverRouting = L"TitanRoutingServer";
	static const unsigned short *serverFactory = L"TitanFactoryServer";
	static const unsigned short *serverProfile = L"TitanProfileServer";
	static const unsigned short *serverFirewall = L"TitanFirewallDetector";
	
	//Routing dirservice->Datatypes 
	// Client counts
	static const WONCommon::RawBuffer dataClientCount = (U8 *) "__RSClientCount";
	// Flags
	static const WONCommon::RawBuffer dataFlags = (U8 *) "__RSRoomFlags";
	// Permanent
	static const WONCommon::RawBuffer dataPermanent = (U8 *) "IsPerm";
*/
};
DWORD WINAPI dirRecv(LPVOID lpParam);
int GetAuthPacket(unsigned char ** packetptr);
