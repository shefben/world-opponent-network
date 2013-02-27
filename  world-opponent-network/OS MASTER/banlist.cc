#include "banlist.h"

/******** Local macros ********/

/* Description: CompareIPs
//
// Takes two IPs and compares them.  Since they're just
// ints, that's not a big deal.
*/
#define CompareIPs(IPa,IPb) ((IPa)==(IPb))


/* Description: CompareIPs
//
// Takes two IPs and compares them, with a subnet mask
// ANDed to them both.  Network order, always.
*/
#define CompareIPsWithCIDR(IPa,IPb,Maskc) (((IPa)&(Maskc))==((IPb)&(Maskc)))

/******** Constructor/Destructor ********/

IPBanList::IPBanList() {
   pthread_mutex_init(&Banlist_undesirables_lock, NULL);
   BeginningOfBanlist = NULL;

   //Since 0 and -1 are special addresses that also double as invalid catches,
   //ban them outright.
   BanIP(0, htonl(0xFF000000));
   BanIP(INADDR_NONE, htonl(0xFF000000));


}
IPBanList::~IPBanList() {

   IPBanListStructure_t* ptr;
   while(BeginningOfBanlist != NULL) {
      ptr = BeginningOfBanlist;
      BeginningOfBanlist = BeginningOfBanlist->next;
      free(ptr);
   }
   pthread_mutex_destroy(&Banlist_undesirables_lock);

}


/**** SockAddr ban functions ****/

/* DESCRIPTION: BanSockaddr
//
// Takes a sockaddr, and calls BanIP with the sockaddr's address.
//
*/
int IPBanList::BanSockaddr(struct sockaddr_in sender) {

   return(BanIP(sender.sin_addr.s_addr, 0xFFFFFFFF));
}

/* DESCRIPTION: UnbanSockaddr
//
// Just strips the IP and calls the corresponding IP function.
//
*/
int IPBanList::UnbanSockaddr(struct sockaddr_in sender) {
   return(UnbanIP(sender.sin_addr.s_addr, 0xFFFFFFFF));
}

/* DESCRIPTION: IsSockaddrBanned
//
// Takes a sockaddr, strips out the sin_addr, and passes it on as an int.
//
*/
int IPBanList::IsSockaddrBanned(struct sockaddr_in sender) {

   return(IsIPBanned(sender.sin_addr.s_addr));
}


/**** IPAddr ban functions ****/

/* DESCRIPTION: BanIP
//
// Mallocs a new IPBanStructure and copies the passed int in, after checking
// to see if it's banned or not.  Freed in the destructor.
//
// Reutns 0 on success, or 1 if the address is already banned.
*/
int IPBanList::BanIP(unsigned long int senderIP, unsigned long int CIDR) {

   if(IsIPBanned(senderIP & CIDR)) { return(1); }


pthread_mutex_lock(&Banlist_undesirables_lock);

   IPBanListStructure_t * ptr = (IPBanListStructure_t *)malloc(sizeof(IPBanListStructure_t));
   ptr->next = BeginningOfBanlist;
   ptr->address = senderIP;
   ptr->subnetmask = CIDR;
   BeginningOfBanlist = ptr;

pthread_mutex_unlock(&Banlist_undesirables_lock);

   return(0);
}

/* DESCRIPTION: UnbanIP
//
// Searches for and removes (frees) an IP that is passed.
// Returns 0 on success or 255 if the IP wasn't present.
//
*/
int IPBanList::UnbanIP(unsigned long int senderIP, unsigned long int CIDR) {

pthread_mutex_lock(&Banlist_undesirables_lock);

   IPBanListStructure_t* ptr = BeginningOfBanlist;
   IPBanListStructure_t* ptr2 = NULL;

   while(ptr != NULL) {

      if(CompareIPs(senderIP, ptr->address) && (CIDR == (ptr->subnetmask))) {
         if(ptr2 == NULL) { //First IP in the list
            BeginningOfBanlist = BeginningOfBanlist->next;
            free(ptr);
         }
         else {
            ptr2->next = ptr->next;
            free(ptr);
         }

         pthread_mutex_unlock(&Banlist_undesirables_lock);
         return(0);
      }

      ptr2 = ptr;
      ptr = ptr->next;
   }

pthread_mutex_unlock(&Banlist_undesirables_lock);

   return(255);
}

/* DESCRIPTION: IsIPBanned
//
// Takes an unsigned int (can be any of the S_un structs typecast)
// for comparison, and compares them to the ban list.
//
*/
int IPBanList::IsIPBanned(unsigned long int senderIP) {

pthread_mutex_lock(&Banlist_undesirables_lock);

   IPBanListStructure_t* ptr = BeginningOfBanlist;
   while(ptr != NULL) {
      if(CompareIPsWithCIDR(senderIP, ptr->address, ptr->subnetmask) == 1) {

         pthread_mutex_unlock(&Banlist_undesirables_lock);
         return(1);
      }
      ptr = ptr->next;
   }

pthread_mutex_unlock(&Banlist_undesirables_lock);

   return(0);

}

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
int IPBanList::ParseBanString(const char *arg, const char ToBanOrNotToBan) {

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

   if (IPaddress == INADDR_NONE || IPaddress == 0) {
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

/* DESCRIPTON: RetrieveBanlist
//
// Much like the WONnetwk function.
//  They pass us a void pointer, we return numbers.
*/
void IPBanList::RetrieveBanlist(void **address, unsigned long int* IP, unsigned long int* CIDR) {

   IPBanListStructure_t *ptr = (IPBanListStructure_t*)*address;


   if(ptr == NULL) {
      ptr = BeginningOfBanlist;
   }
   else {
      ptr = ptr->next;
   }

      //At this stage we have a ptr variable which points to a valid master
      //entry, or to NULL, meaning we should just leave.

   if(ptr == NULL) {
      *address = NULL;
      return;
   }

   *address = (void *)ptr;
   *CIDR = ptr->subnetmask;
   *IP = ptr->address;
}
