#include "server.h"

class RoutingServer :	public Server
{
public:
	RoutingServer(int port);
	~RoutingServer();
	int start();
	char * getName();
	int getStatus();

private:
	SOCKET listeningSocket;
	char name[100];
	int status;
	int port;
	static void doConnect(SOCKET socket,DWORD clientIp, char * serverName);
	static unsigned int __stdcall waitForConnections(void * param);
	DWORD externalIp;
};
DWORD WINAPI receive_cmds(LPVOID lpParam);
void PrintUnknownPacket(const unsigned char *, unsigned int);
