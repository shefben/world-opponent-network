/*
// This class came from the master, though classes were removed.
// Bans can be by IP or by CD key.  IP bans are checked right after a packet
// is received.  CD key bans are checked for connect messages, since that's
// the only time they are transmitted.
*/
#ifndef OSHLDS_BANLIST_HEADER
#define OSHLDS_BANLIST_HEADER
#include "NET.h"


//Todo: use in_addr instead of uint32 everywhere.  Just in case.

void Banlist_Init();
void Banlist_CvarInit();
void Banlist_ClearBans();
void Banlist_ReadBansFromFile();
void Banlist_WriteBansToFile();
void Banlist_Shutdown();


int IsSockaddrBanned(struct sockaddr_in *);

int BanIPv4(uint32, uint32, unsigned int); //s_addr may be typecast to int
int UnbanIPv4(uint32, uint32);
int IsIPv4Banned(uint32);

//The char arrays are all const, but as they are cast to void *s, it is not reasonable to declare this.
int BanCDKey(char key[32], unsigned int time);
int UnbanCDKey(char key[32]);
int IsCDKeyBanned(char key[32]);

int BanName(char name[32], unsigned int time);
int UnbanName(char name[32]);
int IsNameBanned(char name[32]);



void ClearCDKeys();
void Banlist_ReadBansFromFile_CD();
void Banlist_WriteBansToFile_CD();

void ClearIPv4();
void Banlist_ReadBansFromFile_IPv4();
void Banlist_WriteBansToFile_IPv4();


#endif
