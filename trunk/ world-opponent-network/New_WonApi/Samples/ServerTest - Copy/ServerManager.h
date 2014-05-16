// ServerManager.h: interface for the ServerManager class.
//
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Server.h"

class ServerManager  
{
public:
	ServerManager();
	virtual ~ServerManager();
	
	Server ** getServers();
	int getNbServers();
	void startServers();
	void setUpdating();
private:
	Server * servers[20];
	int nbServers;
	void prepareServer(char * type, DWORD port);
};
