#include "banlist.h"
#include "cmd.h"
#include "cvar.h"
#include "report.h"
#include "Q_memdef.h"
#include "FS.hpp"
#include "inetaton.h"
#include "libcprops-0.1.8/avl.h"

#include <time.h>

#define SIZEOF_CD_KEY 32

/*** Structs ***/
typedef struct IP_avl_container_s {

   cp_avltree * RootNodeOfTree;
   struct IP_avl_container_s * PreviousMask;
   struct IP_avl_container_s * NextMask;

   uint32 subnetmask;
} IP_avl_container_t;


/*** Globals ***/
static IP_avl_container_t * ip_banlist = NULL;
static cp_avltree * cd_banlist = NULL;
//static cp_avltree * name_banlist = NULL;

       cvar_t cvar_sv_filterban = { "sv_filterban", "1", 0, 0, 0 };
       cvar_t cvar_announce_bans = { "announce_bans", "1", FCVAR_PROTECTED , 0, 0 };
static cvar_t cvar_cd_key_ban_file = { "cd_key_ban_file", "cdbans.cfg", 0, 0, 0 };
static cvar_t cvar_ip_addr_ban_file = { "ip_addr_ban_file", "ipbans.cfg", 0, 0, 0 };
static cvar_t cvar_name_ban_file = { "name_ban_file", "namebans.cfg", 0, 0, 0 };


/*** Functions ***/
void Banlist_Init() {

   Banlist_ReadBansFromFile();
}
void Banlist_CvarInit() {

   Cvar_RegisterVariable(&cvar_sv_filterban);
   Cvar_RegisterVariable(&cvar_cd_key_ban_file);
   Cvar_RegisterVariable(&cvar_ip_addr_ban_file);
   Cvar_RegisterVariable(&cvar_name_ban_file);
   Cvar_RegisterVariable(&cvar_announce_bans);
}
void Banlist_ClearBans() {

   void ClearCDKeys();
   void ClearIPv4();
}
void Banlist_ReadBansFromFile() {

   Banlist_ReadBansFromFile_CD();
   Banlist_ReadBansFromFile_IPv4();
}
void Banlist_WriteBansToFile() {

   Banlist_WriteBansToFile_CD();
   Banlist_WriteBansToFile_IPv4();
}
void Banlist_Shutdown() {

   Banlist_WriteBansToFile();
   Banlist_ClearBans();
}


/*** CD keys ***/
int CompareCDkeys(void * a, void * b) { //wrapper for memcmp, since they won't know the key size.
   return(Q_memcmp(a, b, SIZEOF_CD_KEY));
}
void * KeyDup(void * a) { //Similarly, an implementation of strdup that knows the sizes.
   register void * ret;

   ret = Q_Malloc(SIZEOF_CD_KEY);
   CHECK_MEMORY_MALLOC(ret);
   return(Q_memcpy(ret, a, SIZEOF_CD_KEY));
}
void KeyFree(void * a) { //and again for free, though this time I guess we could've just used free.
   Q_Free(a);
}
int CDKeyCallBackFunction(cp_avlnode * node, hl_file_t * fp) {

   // This function has to conform to the callback's requests.  We return !0 if
   // we want to terminate early.  Otherwise we write our node to the file.

   //We are traversing the tree.  Don't modify it.
   if((unsigned)time(NULL) > (unsigned int)node->value) { return(0); } //expired
   if(FS_FPrintf(fp, "%s %010u\n", (char *)node->key, (unsigned int)node->value) == -1) { return(1); }
   return(0);
}


/* DESCRIPTION: BanCDKey
//
// The easiest of the bans.  CD keys don't have vague comparisons; the size
// is constant, and there are no wildcards.
//
// Returns 0 on success, or nonzero (i) if there was a problem (malloc and so).
*/
int BanCDKey(char key[32], unsigned int time) {

   //First things first:  If this is the first of our banned keys,
   //allocate a new list.
   if(cd_banlist == NULL) {
      cd_banlist = cp_avltree_create_by_option(COLLECTION_MODE_NOSYNC | //WE handle locking.
                                               COLLECTION_MODE_COPY | //keys must be copied into memory
                                               COLLECTION_MODE_DEEP,
                                               (cp_compare_fn)CompareCDkeys,
                                               (cp_copy_fn)KeyDup, KeyFree, //We might want to wrap this so we can choke if a memory failure occurs.
                                               NULL, NULL);
      if(cd_banlist == NULL) {
         Con_Printf("%s: Memory failure allocating new binary tree.  Ban not added.\n", __FUNCTION__);
         return(1);
      }
   }

   //well, that was fun.  Insert it.
   //This library's pretty nice, if hard to configure for mingw.
   //I tried writing my own AVL tree.  It was very frustrating.
   if(cp_avltree_insert(cd_banlist, (void *)key, (void *)time) == NULL) {

      //oh oh, memory failure.
         Con_Printf("%s: Failed to add key. Probably memory, but I'm too lazy to check.\n", __FUNCTION__);
         return(1);
   }
   return(0);
}
/* DESCRIPTION: UnbanCDKey
//
// Searches for and removes (frees) a CD key that is banned.
// Returns 0 on success or nonzero (1) if not found.
//
*/
int UnbanCDKey(char key[32]) {


   if(cd_banlist == NULL) { return(1); }

   if(cp_avltree_delete(cd_banlist, key) == NULL) {

      //This function returns the mapped value, which if you've forgotten,
      //is actually an int (void *d).  Or null if not found.  That makes this a
      //little odd, but legit.  We just have to make sure 0 is never a valid
      //value for ban time, which since that's in 1970, is reasonable.

      return(1);
   }

   if(cp_avltree_count(cd_banlist) == 0) { //tree is empty
      cp_avltree_destroy(cd_banlist);
      cd_banlist = NULL;
   }

   return(0);
}
/* DESCRIPTION: IsCDKeyBanned
//
// Searches the list.  Returns 1 if a key should not be allowed, or
// 0 if it should be.  Ah, but here's the catch: the cvar sv_filterban
// reverses the criteria.  If sv_filterban is 1, as is default, anybody
// on the list is denied access.  But if sv_filterban is 0, then this becomes
// an allow list, and if you're NOT on the list, you aren't getting in.
//
*/
int IsCDKeyBanned(char key[32]) {

   unsigned int ban_expiration;
   int RetIfUnlisted;
   int RetIfListed;

   //This is wholly unnecessary.  Helps make this clearer though, instead of rampant nots.
   if(cvar_sv_filterban.value == 1) { //normal behaviour, list acts as blacklist.
      RetIfUnlisted = 0;
      RetIfListed = 1;
   }
   else { //inverse behavour, list is now whitelist.
      RetIfUnlisted = 1;
      RetIfListed = 0;
   }


   if(cd_banlist == NULL) { return(RetIfUnlisted); }

   ban_expiration = (unsigned int)cp_avltree_get(cd_banlist, key);
   if(ban_expiration == 0) { //not found
      return(RetIfUnlisted);
   }
   else if(ban_expiration == (unsigned int)-1) { //entry never expires
      return(RetIfListed);
   }
   else if(ban_expiration < (unsigned)time(NULL)) { //Ban is still listed, but has expired.

      cp_avltree_delete(cd_banlist, key);
      return(RetIfUnlisted);
   }
   else { //ban hasn't expired.  Destroy them.
      return(RetIfListed);
   }
}

void ClearCDKeys() {
   cp_avltree_destroy(cd_banlist);
   cd_banlist = NULL;
}
void Banlist_ReadBansFromFile_CD() {

   //If these ban lists somehow can manage to get too unwieldly, well,
   //we'll have to read them in parts now won't we...
   unsigned int i, bantime, curtime;
   char tempkey[SIZEOF_CD_KEY];
   char * filep;
   const char * curfilep;



   filep = COM_LoadFileForMe(cvar_cd_key_ban_file.string, NULL);
   if(filep == NULL) { return; } //There not being one is normal for some.

   curfilep = filep;
   i = 0;
   curtime = time(NULL);
   while(1) {

      curfilep = COM_Parse(curfilep);
      if(global_com_token[0] == '\0') { break; }

      //First should be the key.
      if(strlen(global_com_token) != 32) {

         Con_Printf("%s: Expected CD key, got %s.\n", __FUNCTION__, global_com_token);
         continue; //continue because it could just be some garbage, and that way we can recover.
      }

      //We could make sure each char is hex, but why bother?  If the admin's
      //screwing around, that's his problem.
      Q_memcpy(tempkey, global_com_token, SIZEOF_CD_KEY);

      //Now get the time.
      curfilep = COM_Parse(curfilep);
      if(global_com_token[0] == '\0') { break; }

      bantime = Q_atoi(global_com_token);
      if(bantime < curtime && bantime != (unsigned)-1) { continue; }

      //Add it.
      BanCDKey(tempkey, bantime);
      i++;
   }

   Con_Printf("Loaded %u CD keys into %s.\n", i, (cvar_sv_filterban.value == 1) ? "blacklist" : "whitelist");
   COM_FreeFile(filep);
}
void Banlist_WriteBansToFile_CD() {

   hl_file_t * fp;


   if(cd_banlist == NULL || cvar_cd_key_ban_file.string[0] == '\0') { return; }

   fp = FS_Open(cvar_cd_key_ban_file.string, "wb");
   if(fp == NULL) { //Not easy.  A readonly file could do it though.

      Con_Printf("%s: Couldn't open file \"%s\" for writing.\n", __FUNCTION__, cvar_cd_key_ban_file.string);
      return;
   }


   //Okay, open file.  We can sprintf strings in bulk, output each line, etc.
   //Plenty of options, but this usually doesn't happen during resource
   //intensive times, so it doesn't really matter.

   //This function can traverse the tree in three ways.  or each node it calls
   //the passed function, and gives that function the node and the last arg.
   cp_avltree_callback(cd_banlist, (cp_callback_fn)CDKeyCallBackFunction, (void *)fp);

   //We're done.
   FS_Close(fp);
}


/*** IPv4 ***/
//The callback function only lets us pass one arg.  We have two.  Let's pass a struct.
typedef struct tempstruct_s {

   hl_file_t * fp;
   char SubnetMaskStr[16];
} tempstruct_t;

int CompareIPv4(uint32 a, uint32 b) { //Oh, the horror.
   return(a-b);
}
int IPv4CallBackFunction(cp_avlnode * node, tempstruct_t * args) {

   union ipocts {
      uint32 asNum;
      byte asByte[4];
   } IPv4;


   IPv4.asNum = hosttobe32((uint32)node->key); //We need to write this as chars, so we'll have an easier time in big endian.

   //We are traversing the tree.  Don't modify it.
   if((unsigned)time(NULL) > (unsigned int)node->value) { return(0); } //expired
   if(FS_FPrintf(args->fp, "%u.%u.%u.%u %s %010u\n", IPv4.asByte[0], IPv4.asByte[1], IPv4.asByte[2], IPv4.asByte[3], args->SubnetMaskStr, (unsigned int)node->value) == -1) { return(1); }
   return(0);
}


/* DESCRIPTION: BanIPv4
//
// A slightly more complicated set of bans.  Different subnet masks (there are
// only 33 possible values) have different trees
//
// Returns 0 on success, or nonzero (i) if there was a problem (malloc and so).
*/
int __BanIPv4(uint32 IPv4, IP_avl_container_t * tree, unsigned int time) {

   if(cp_avltree_insert(tree->RootNodeOfTree, (void *)(IPv4 & tree->subnetmask), (void *)time) == NULL) {

      //failed for some reason.
      Con_Printf("%s: Failed to add key.  This is most likely a memory error.\n", __FUNCTION__);
      return(1);
   }
   return(0);
}
int BanIPv4(uint32 IPv4, uint32 SubnetMask, unsigned int time) {

   IP_avl_container_t * ptr, * new_ptr;


   //As a bit of paranoia, check the mask.  I need a PARANOIA define...
   switch(SubnetMask) {

   case 0x00000000:
   case 0x00000001:
   case 0x00000003:
   case 0x00000007:
   case 0x0000000F:
   case 0x0000001F:
   case 0x0000003F:
   case 0x0000007F:
   case 0x000000FF:
   case 0x000001FF:
   case 0x000003FF:
   case 0x000007FF:
   case 0x00000FFF:
   case 0x00001FFF:
   case 0x00003FFF:
   case 0x00007FFF:
   case 0x0000FFFF:
   case 0x0001FFFF:
   case 0x0003FFFF:
   case 0x0007FFFF:
   case 0x000FFFFF:
   case 0x001FFFFF:
   case 0x003FFFFF:
   case 0x007FFFFF:
   case 0x00FFFFFF:
   case 0x01FFFFFF:
   case 0x03FFFFFF:
   case 0x07FFFFFF:
   case 0x0FFFFFFF:
   case 0x1FFFFFFF:
   case 0x3FFFFFFF:
   case 0x7FFFFFFF:
   case 0xFFFFFFFF:
      break;

   default:
      Sys_Error("%s: Bad subnet %X.\n", __FUNCTION__, SubnetMask);
   }

   //Find the correct container for our subnet mask.
   for(ptr = ip_banlist; ptr != NULL; ptr = ptr->NextMask) {

      if(ptr->subnetmask == SubnetMask) {
         return(__BanIPv4(IPv4, ptr, time));
      }
      else if(ptr->subnetmask > SubnetMask) {
         break;
      }
   }

   //If we are here, we didn't find it.  Malloc a new entry.

   new_ptr = Q_Malloc(sizeof(IP_avl_container_t));
   if(new_ptr == NULL) {
      Con_Printf("%s: Memory failure when adding entry.\n", __FUNCTION__);
      return(1);
   }
   new_ptr->RootNodeOfTree = cp_avltree_create((cp_compare_fn)CompareIPv4);
   if(new_ptr->RootNodeOfTree == NULL) {
      Con_Printf("%s: Memory failure when adding entry.\n", __FUNCTION__);
      Q_Free(new_ptr);
      return(1);
   }
   new_ptr->subnetmask = SubnetMask;
   if(__BanIPv4(IPv4, new_ptr, time) != 0) { //Geez, we failed to allocate the first node.

      cp_avltree_destroy(new_ptr->RootNodeOfTree);
      Q_Free(new_ptr);
      return(1);
   }


   //Now to link.
   if(ip_banlist == NULL) { //If ip_banlist == null, we are the first entry.  And the last.

      ip_banlist = new_ptr;
      ip_banlist->PreviousMask = ip_banlist->NextMask = NULL;
   }

   else if(ptr != NULL) { //if ptr != NULL, we know who we need to throw it in front of.

      new_ptr->PreviousMask = ptr->PreviousMask;
      new_ptr->NextMask = ptr;
      ptr->PreviousMask = new_ptr;
      if(new_ptr->PreviousMask != NULL) {
         new_ptr->PreviousMask->NextMask = new_ptr;
      }
      else { //It can only be null if ptr is also ip_banlist
         ip_banlist = new_ptr;
      }
   }
   else { //Well shoot, we'll have to loop through to find the last node.  Doesn't happen often though, so big deal.

      for(ptr = ip_banlist; ptr->NextMask != NULL; ptr = ptr->NextMask) { }

      ptr->NextMask = new_ptr;
      new_ptr->PreviousMask = ptr;
      new_ptr->NextMask = NULL;
   }

   //And that's everything, I think.
   return(0);
}
/* DESCRIPTION: UnbanIPv4
//
// First we find the matching subnet addr.
// Then we find the ban.
*/
int __UnbanIPv4(uint32 IPv4, IP_avl_container_t * tree) {

   if(cp_avltree_delete(tree->RootNodeOfTree, (void *)(IPv4 & tree->subnetmask)) == NULL) {
      return(1);
   }
   return(0);
}
int UnbanIPv4(uint32 IPv4, uint32 SubnetMask) {

   IP_avl_container_t * ptr;


   //Paranoia again.
   switch(SubnetMask) {

   case 0x00000000:
   case 0x00000001:
   case 0x00000003:
   case 0x00000007:
   case 0x0000000F:
   case 0x0000001F:
   case 0x0000003F:
   case 0x0000007F:
   case 0x000000FF:
   case 0x000001FF:
   case 0x000003FF:
   case 0x000007FF:
   case 0x00000FFF:
   case 0x00001FFF:
   case 0x00003FFF:
   case 0x00007FFF:
   case 0x0000FFFF:
   case 0x0001FFFF:
   case 0x0003FFFF:
   case 0x0007FFFF:
   case 0x000FFFFF:
   case 0x001FFFFF:
   case 0x003FFFFF:
   case 0x007FFFFF:
   case 0x00FFFFFF:
   case 0x01FFFFFF:
   case 0x03FFFFFF:
   case 0x07FFFFFF:
   case 0x0FFFFFFF:
   case 0x1FFFFFFF:
   case 0x3FFFFFFF:
   case 0x7FFFFFFF:
   case 0xFFFFFFFF:
      break;

   default:
      Sys_Error("%s: Bad subnet %X.\n", __FUNCTION__, SubnetMask);
   }

   //Match subnets.
   for(ptr = ip_banlist; ptr != NULL; ptr = ptr->NextMask) {

      if(ptr->subnetmask == SubnetMask) {
         if(__UnbanIPv4(IPv4, ptr) != 0) { return(1); } //was not found

         //If we are here, we deleted a node.  We now need to see if we must
         //delete the tree as well.
         if(cp_avltree_count(ptr->RootNodeOfTree) == 0) { //tree is empty

            //destroy tree
            cp_avltree_destroy(ptr->RootNodeOfTree);

            //unlink node
            if(ptr->PreviousMask != NULL) {
               ptr->PreviousMask = ptr->NextMask;
            }
            else {
               ip_banlist = ptr->NextMask;
            }
            if(ptr->NextMask != NULL) {
               ptr->NextMask->PreviousMask = ptr->PreviousMask;
            }

            //destroy node
            Q_Free(ptr);
         }

         return(0);
      }
      else if(ptr->subnetmask > SubnetMask) {
         break;
      }
   }

   return(1); //Didn't even find the proper container.
}
/* DESCRIPTION: IsIPv4Banned
//
// Searches each of the lists.  Same return behaviour as CDKeyBanned.
*/
void * __IsIPv4Banned(uint32 IPv4, IP_avl_container_t * tree) {

   return(cp_avltree_get(tree->RootNodeOfTree, (void *)(IPv4 & tree->subnetmask)));
}
int IsIPv4Banned(uint32 IPv4) {

   int RetIfUnlisted;
   int RetIfListed;
   unsigned int ban_expiration;
   IP_avl_container_t * ptr, * ptr2;


   if(cvar_sv_filterban.value == 1) {
      RetIfUnlisted = 0;
      RetIfListed = 1;
   }
   else { //inverse behavour, list is now whitelist.
      RetIfUnlisted = 1;
      RetIfListed = 0;
   }


   for(ptr = ip_banlist; ptr != NULL; ptr = ptr2) {

      ptr2 = ptr->NextMask;


      ban_expiration = (unsigned int)__IsIPv4Banned(IPv4, ptr);
      if(ban_expiration == 0) { //not found
         continue;
      }
      else if(ban_expiration == (unsigned int)-1) { //entry never expires
         return(RetIfListed);
      }
      else if(ban_expiration < (unsigned)time(NULL)) { //Ban is still listed, but has expired.  They could however be explicitly banned by a more precise mask.  Regardless, delete old ban.

         __UnbanIPv4(IPv4, ptr);
         if(cp_avltree_count(ptr->RootNodeOfTree) == 0) { //tree is empty

            cp_avltree_destroy(ptr->RootNodeOfTree);
            if(ptr->PreviousMask != NULL) {
               ptr->PreviousMask = ptr->NextMask;
            }
            else {
               ip_banlist = ptr->NextMask;
            }
            if(ptr->NextMask != NULL) {
               ptr->NextMask->PreviousMask = ptr->PreviousMask;
            }

            Q_Free(ptr);
         }

         continue;
      }
      else { //ban hasn't expired.  Destroy them.
         return(RetIfListed);
      }
   }

   //Okay, we got here.  No matches then.
   return(RetIfUnlisted);
}
/* DESCRIPTION: IsSockaddrBanned
//
// Takes a sockaddr, strips out the sin_addr, and passes it on as an int.
// If we update to IPv6 (that'll be tricky in itself), that won't cut it.
*/
int IsSockaddrBanned(struct sockaddr_in * sender) {

   return(IsIPv4Banned(sender->sin_addr.s_addr));
}


void ClearIPv4() {

   IP_avl_container_t * ptr, * ptr2;

   for(ptr = ip_banlist; ptr != NULL; ptr = ptr2) {

      ptr2 = ptr->NextMask;
      cp_avltree_destroy(ptr->RootNodeOfTree);
      Q_Free(ptr);
   }

   ip_banlist = NULL;
}
void Banlist_ReadBansFromFile_IPv4() {

   struct in_addr IPv4, CIDR;
   unsigned int i, bantime, curtime;
   char * filep;
   const char * curfilep;


   filep = COM_LoadFileForMe(cvar_ip_addr_ban_file.string, NULL);
   if(filep == NULL) { return; } //There not being one is normal for some.

   curfilep = filep;
   i = 0;
   curtime = time(NULL);
   while(1) {

      curfilep = COM_Parse(curfilep);
      if(global_com_token[0] == '\0') { break; }

      //First should be the IP.  We can't use inet_addr easily, since -1 is a valid address that we expect to see.
      if(inet_aton(global_com_token, &IPv4) == 0) {

         Con_Printf("%s: Expected IP address, got %s.  Corrupt list?\n", __FUNCTION__, global_com_token);
         continue; //continue because it could just be some garbage, and we can probably recover.
      }

      //Now the sub, which'll probably be 255.255.255.255
      curfilep = COM_Parse(curfilep);
      if(global_com_token[0] == '\0') { break; }

      if(inet_aton(global_com_token, &CIDR) == 0) {

         Con_Printf("%s: Expected subnet mask, got %s.  Corrupt list?\n", __FUNCTION__, global_com_token);
         continue;
      }

      //And finally the time.
      curfilep = COM_Parse(curfilep);
      if(global_com_token[0] == '\0') { break; }

      bantime = Q_atoi(global_com_token);
      if(bantime < curtime && bantime != (unsigned)-1) { continue; }

      //Add it.
      BanIPv4(ntohl(IPv4.s_addr), ntohl(CIDR.s_addr), bantime);
      i++;
   }

   Con_Printf("Loaded %u IP addresses into %s.\n", i, (cvar_sv_filterban.value == 1) ? "blacklist" : "whitelist");
   COM_FreeFile(filep);
}
void Banlist_WriteBansToFile_IPv4() {

   union ipocts {
      uint32 asNum;
      byte asByte[4];
   } sub;
   IP_avl_container_t * ptr;
   tempstruct_t args;



   if(ip_banlist == NULL || cvar_ip_addr_ban_file.string[0] == '\0') { return; }

   args.fp = FS_Open(cvar_ip_addr_ban_file.string, "wb");
   if(args.fp == NULL) {

      Con_Printf("%s: Couldn't open file \"%s\" for writing.\n", __FUNCTION__, cvar_ip_addr_ban_file.string);
      return;
   }

   for(ptr = ip_banlist; ptr != NULL; ptr = ptr->NextMask) {

      //Especially for large bans, it's likely we'll be writing the same
      //subnet mask tons of times.  Save us trouble; cache it.
      sub.asNum = hosttobe32(ptr->subnetmask);
      sprintf(args.SubnetMaskStr, "%u.%u.%u.%u", sub.asByte[0], sub.asByte[1], sub.asByte[2], sub.asByte[3]); //can never be larger than 15+1 bytes.

      cp_avltree_callback(ptr->RootNodeOfTree, (cp_callback_fn)IPv4CallBackFunction, (void *)&args);
   }

   FS_Close(args.fp);
}




#if 0
/* DESCRIPTION: ParseBanString
//
// Parses a string from input into a workable ban.
// Returns 0 on success.
//
// Examples: (input will remove the preceding 'ban')
// 4.224.234.200 //bans that address
// 3.3.3.3 255.255.255.0 //bans all 3.3.3.x addresses
// 3.3.3.0/23 // bans anything on 3.3.[2-3].x I think.
*/
int ParseBanString(const char *arg, const char ToBanOrNotToBan) {

   char temparray[80];
   char *ptr = NULL;
   unsigned int IPaddress;
   unsigned int CIDR = 0xFFFFFFFF;
   short int temp;

   strncpy(temparray, arg, 79);

   ptr = strpbrk(temparray, " /"); // Is there a space or a slash afterwards?
   if(ptr != NULL) { // Yes there is.

      ptr[0] = '\0';
      do { ptr++; } while(*ptr == ' ' || *ptr == '/');

      temp = atoi(ptr);
      if(temp < 1 || temp > 32) { //Not CIDR.  Don't be fooled by the var names
         CIDR = inet_addr(ptr);
      }
      else { //else it must be that CIDR notation.
         CIDR = 0;
         for(IPaddress = 0; IPaddress < 32; IPaddress++) { //Var reuse as a counter.

            if(temp >= 0) {
               temp--;
               CIDR++;
            }
            CIDR = CIDR<<1;
         }
         CIDR = htonl(CIDR);
      }
      if (CIDR == INADDR_NONE || CIDR == 0) { //banning everyone sounds a lil extreme
         return(126);
      }
   }

   //Now for the actual address.

   IPaddress = inet_addr(temparray);

   if(IPaddress == INADDR_NONE || IPaddress == 0) {
      // Not an Ip address.  That's bad.
      return(127);
   }

   if(ToBanOrNotToBan) { //Or, did the user type 'un' in front of that 'ban'?
      return(BanIP(IPaddress, CIDR));
   }
   else {
      return(UnbanIP(IPaddress, CIDR));
   }
}
#endif
