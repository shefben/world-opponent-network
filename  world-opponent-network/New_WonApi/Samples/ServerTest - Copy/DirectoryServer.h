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
};
DWORD WINAPI dirRecv(LPVOID lpParam);
int GetAuthPacket(unsigned char ** packetptr);
