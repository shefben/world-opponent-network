// ServerManager.cpp: implementation of the ServerManager class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ServerManager.h"
#include "socketTools.h"
#include "DirectoryServer.h"
#include "AuthServer.h"

ServerManager::ServerManager()
{
	nbServers=0;
	prepareServer("DirectorServerPort",dirPort); 
	prepareServer("AuthenticationServerPort",auPort);

}
ServerManager::~ServerManager()
{
}
Server ** ServerManager::getServers()
{
	return servers;
}
int ServerManager::getNbServers()
{
	return nbServers;
}
void ServerManager::prepareServer(char * type, DWORD port)
{
	 if (!strcmp(type,"DirectorServerPort"))
	{
		servers[nbServers]=new DirectoryServer (port);
		nbServers++;
	}
	if (!strcmp(type,"AuthenticationServerPort"))
	{
		servers[nbServers]=new AuthServer (port);
		nbServers++;
	}
	
}

void ServerManager::startServers()
{
	for (int ind=0;ind<nbServers;ind++)
	{
		servers[ind]->start();
	}
}
