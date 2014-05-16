#include "server.h"

#if 0
class FactoryServer :	public Server
{
public:
	FactoryServer(int port);
	~FactoryServer();
	int start();
	char * getName();
	int getStatus();

private:
	SOCKET listeningSocket;
	char name[100];
	int status;
	int port;
	//	static void doConnect(SOCKET socket,DWORD clientIp, char * serverName);
	//	static unsigned int __stdcall waitForConnections(void * param);
	DWORD externalIp;
};
#endif