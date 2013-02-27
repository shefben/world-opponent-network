/* The SV_ functions are pretty much a good chunk of code, divided into
// separate segments that can't be easily distinguished by name.  They use many
// global variables and will often build onto each other in a very heirarchial
// manner.  Most of them handle structs which, unfortunately, are very very
// different from what's in QW.  Piecing together the structs alone has taken
// a lot of the time.
//
// This file contains portions of code directly related to the sending and
// receiving of packets (NET_) as well as the code one level above that which
// handles the individual frames.
*/

#ifndef OSHLDS_NETCHAN_HEADER
#define OSHLDS_NETCHAN_HEADER

#include "endianin.h"
#include "common.h"
#include "Mod.h"
#include "edict.h"
#include "crc.h"
#include "secmod.h"

//The networking includes
#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#undef IPPROTO_IPV6 //the netadr_t struct is used by mods.  We can't change it.
#define SOCKETCLOSE closesocket

typedef int socklen_t;
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ECONNRESET WSAECONNRESET
#define ECONNREFUSED WSAECONNREFUSED
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#define ENETRESET WSAENETRESET

#else

#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>

#define SOCKET int
#define SOCKETCLOSE close

#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1 //IIRC for Unix this is 0, for Windows it's -1
#endif

#define MAX_MODELS 0x200
#define MAX_SOUNDS 0x200
#define MAX_GNERIC 0x200
#define MAX_LOOPBACK 4

#define MAX_UDP_ROUTABLE_SIZE 1400 //That damn split packet is nu'un but trouble.
#define SIZEOF_splitpacket_header_t 9
#define SPLIT_UDP_PACKET_SIZE   1391 //(MAX_UDP_ROUTABLE_SIZE - sizeof(splitpacket_header_t))

#define MAX_UDP_PACKET_DATA_SIZE 8346 //Steam adds an extra zero to this or so.  Overkill.
#define MAX_UDP_PACKET_SIZE 8400  //That's data_size + (num_split_packets*split_overhead).  I don't think we'll need it though.
#define MAX_NUMBER_OF_SPLIT_PACKETS 6 //MAX_UDP_PACKET_SIZE / SPLIT_UDP_PACKET_SIZE

//typedefs and enums

//resources and stuff, here to prevent circular dependencies
typedef enum {
   t_sound = 0,
   t_skin,
   t_model,
   t_decal,
   t_generic,
   t_eventscript
} resourcetype_t;

typedef struct _resourceinfo_s{
   int size;
} _resourceinfo_t;
typedef struct resourceinfo_s {
   _resourceinfo_t info[8];
} resourceinfo_t;
typedef struct resource_s { //0x88

   char              szFileName[64]; //00 File name to download/precache.
   resourcetype_t    type;                // 40 t_sound, t_skin, t_model, t_decal.
   int               nIndex;              // 44 For t_decals
   int               nDownloadSize;       // 48 Size in Bytes if this must be downloaded.
   unsigned char     ucFlags;

// For handling client to client resource propagation
   unsigned char     rgucMD5_hash[16];    // 4d To determine if we already have it.
   unsigned char     playernum;           // 5d Which player index this resource is associated with, if it's a custom resource.

   unsigned char     rguc_reserved[32]; // 5e For future expansion
   struct resource_s *pNext;              // 80 Next in chain.
   struct resource_s *pPrev;              // 84
} resource_t;
typedef struct customization_s {

   qboolean bInUse;     // 00 Is this customization in use;
   resource_t resource; // 04 The resource_t for this customization
   qboolean bTranslated; // 8c Has the raw data been translated into a useable format?
//160                  //  (e.g., raw decal .wad make into texture_t *)
   int nUserData1; // 90 Customization specific data
   int nUserData2; // 94 Customization specific data (seems like a size for decals)
   void *pInfo;          // 98 Buffer that holds the data structure that references the data (e.g., the cachewad_t)
   void *pBuffer;       // 9c Buffer that holds the data for the customization (the raw .wad data)
   struct customization_s *pNext; // A0 Next in chain
} customization_t;




typedef enum {

   NA_UNUSED,
   NA_LOOPBACK,
   NA_BROADCAST,
   NA_IP,
   NA_IPX,
   NA_IPX_BROADCAST,
} netadrtype_t;
typedef enum {

   NS_CLIENT,
   NS_SERVER,
   NS_Unknown2
} netsrc_t;
typedef enum {

   force_exactfile,               // File on client must exactly match server's file
   force_model_samebounds,            // For model files only, the geometry must fit in the same bbox
   force_model_specifybounds,         // For model files only, the geometry must fit in the specified bbox
   force_model_specifybounds_if_avail,   // For Steam model files only, the geometry must fit in the specified bbox (if the file is available)
} FORCE_TYPE;

typedef struct netadr_s { //sizeof 20, 0x14.  This struct cannot be modified (damnation) for IPv6 support--mods use it.

   //The question on my mind is how can we fit an IPv6 address in here without breaking mods.
   netadrtype_t type;
   unsigned char ip[4]; //must be network order
   unsigned char ipx[10]; //HA-HAHAHAHAHAhahahaha still need it for padding
   unsigned short port;
} netadr_t;

//This is all under my control.
typedef struct splitpacket_s {

   netadr_t SendingHost;
   uint32 SequenceNumber;
   unsigned int TotalChunks;
   unsigned int ReceivedChunks; //actually bit checks
   unsigned int CurrentSize;
   unsigned int offset;
   byte data[MAX_UDP_PACKET_DATA_SIZE];
} splitpacket_t;
typedef struct splitpacket_header_s {

   uint32 feffffff;
   uint32 SequenceNumber;
   BYTE _4_packetnum4_packettotal; //Four bits are for the packet's ID, four for the total number of packets.  IDs start at 0, so the last packet would be like 3|4 not 3|3.
   BYTE padding1;
   BYTE padding2;
   BYTE padding3; //round it out, make sure these align properly
} splitpacket_header_t;

typedef struct event_args_s { //0x48 SDK, matches the DELTA for g_EventDataDefinition (aka event_t)

   int      flags; //00
   // Transmitted
   int      entindex; //04
   float   origin[3]; //08
   float   angles[3]; //14
   float   velocity[3]; //20 not transmitted?

   int      ducking; //2c
   float   fparam1; //30
   float   fparam2;
   int      iparam1;
   int      iparam2;
   int      bparam1; //40
   int      bparam2; //44
} event_args_t;
typedef struct event_info_s { //SDK 0x58

   unsigned short index; //00           // 0 implies not in use
   short packet_index; //02     // Use data from state info for entity in delta_packet .  -1 implies separate info based on event
                            // parameter signature
   short entity_index; //04     // The edict this event is associated with
   float fire_time; //08       // if non-zero, the time when the event should be fired ( fixed up on the client )

   event_args_t args; //0c sizeof(0x48)
   int     flags;   //54      // Reliable or not, etc.
} event_info_t;

typedef struct ConsistencyData_s { //0x2C

   const char * name;
   int32 padding04;
   int32 padding08;
   int32 padding0C;
   FORCE_TYPE padding10_force_state; //10
   vec3_t mins, maxs;
} ConsistencyData_t;

typedef struct entity_state_s { //SDK //0x154

   // Fields which are filled in by routines outside of delta compression
   int         entityType; //00
   // Index into cl_entities array for this entity.
   int         number;
   float      msg_time;

   // Message number last time the player/entity state was updated.
   int         messagenum;

   // Fields which can be transitted and reconstructed over the network stream
   vec3_t      origin; //10
   vec3_t      angles; //1c

   int         modelindex; //28
   int         sequence; //2c
   float      frame;
   int         colormap;
   short      skin;
   short      solid;
   int         effects;
   float      scale;

   byte      eflags; //44

   // Render information
   int         rendermode;
   int         renderamt;
   color24_t   rendercolor;
   int         renderfx;

   int         movetype;
   float      animtime; //5c
   float      framerate;
   int         body;
   byte      controller[4];
   byte      blending[4];
   vec3_t      velocity; //70

   // Send bbox down to client for use during prediction.
   vec3_t      mins; //7c
   vec3_t      maxs; //88

   int         aiment; //94
   // If owned by a player, the index of that player ( for projectiles ).
   int         owner;

   // Friction, for prediction.
   float      friction; //9c
   // Gravity multiplier
   float      gravity;

   // PLAYER SPECIFIC
   int         team;
   int         playerclass;
   int         health;
   qboolean   spectator;
   int         weaponmodel;
   int         gaitsequence;
   // If standing on conveyor, e.g.
   vec3_t      basevelocity; //bc
   // Use the crouched hull, or the regular player hull.
   int         usehull; //c8
   // Latched buttons last time state updated.
   int         oldbuttons;
   // -1 = in air, else pmove entity number
   int         onground;
   int         iStepLeft;
   // How fast we are falling
   float      flFallVelocity;

   float      fov; //dc
   int         weaponanim;

   // Parametric movement overrides
   vec3_t            startpos; //e4
   vec3_t            endpos; //f0
   float            impacttime; //fc
   float            starttime;

   // For mods
   int         iuser1;
   int         iuser2;
   int         iuser3;
   int         iuser4;
   float      fuser1;
   float      fuser2;
   float      fuser3;
   float      fuser4;
   vec3_t      vuser1; //124
   vec3_t      vuser2; //130
   vec3_t      vuser3; //13c
   vec3_t      vuser4; //148
} entity_state_t;
typedef struct packet_entities_s { //28

  //float      servertime; //??
//int      num_entities; //??
//int      max_entities; //??
   uint32 max_entities; //num, max, it's really hard to tell right now.
   uint32 padding04;
   uint32 padding08;
   uint32 padding0c;
   uint32 padding10;
   uint32 padding14;
   uint32 padding18;
   uint32 padding1c;
   uint32 padding20;
   entity_state_t   *entities; //24
} packet_entities_t;

typedef struct OddEntityContainer_s { //This is not of variable size.  This is the whole thing.

   uint32 padding00; //a counter
   int32 padding04[0x40]; //string_ts I think, so basically offset char pointers.
   entity_state_t padding104[0x40];
} OddEntityContainer_t;

typedef struct clientdata_s {

   vec3_t            origin; //00
   vec3_t            velocity; //0c

   int               viewmodel; //18
   vec3_t            punchangle; //1c
   int               flags; //28
   int               waterlevel; //2c
   int               watertype; //30
   vec3_t            view_ofs; //34
   float            health; //40

   int               bInDuck;

   int               weapons; // remove?

   int               flTimeStepSound;
   int               flDuckTime; //50
   int               flSwimTime;
   int               waterjumptime;

   float            maxspeed; //5c

   float            fov; //60
   int               weaponanim;

   int               m_iId;
   int               ammo_shells;
   int               ammo_nails; //70
   int               ammo_cells;
   int               ammo_rockets;
   float            m_flNextAttack;

   int               tfstate; //80

   int               pushmsec;

   int               deadflag; //88

   char            physinfo[0x100]; //8c, MAX_PHYSINFO_STRING

   // For mods
   int               iuser1; //18c
   int               iuser2;
   int               iuser3;
   int               iuser4;
   float            fuser1; //19c
   float            fuser2;
   float            fuser3;
   float            fuser4;
   vec3_t            vuser1; //1ac
   vec3_t            vuser2; //1b8
   vec3_t            vuser3; //1c4
   vec3_t            vuser4; //1d0
} clientdata_t;
typedef struct weapon_data_s { //0x58 SDK

   int         m_iId; //00
   int         m_iClip;

   float      m_flNextPrimaryAttack;
   float      m_flNextSecondaryAttack;
   float      m_flTimeWeaponIdle; //10

   int         m_fInReload;
   int         m_fInSpecialReload;
   float      m_flNextReload;
   float      m_flPumpTime; //20
   float      m_fReloadTime;

   float      m_fAimedDamage;
   float      m_fNextAimBonus;
   int         m_fInZoom; //30
   int         m_iWeaponState;

   int         iuser1;
   int         iuser2;
   int         iuser3; //40
   int         iuser4;
   float      fuser1;
   float      fuser2;
   float      fuser3; //50
   float      fuser4;
} weapon_data_t;
typedef struct client_frame_s { //0x1810

   double sent_time; //confirmed
   float  ping_time; //confirmed
   clientdata_t clientdata_0C; //0x0c, size if 0x1DC
   weapon_data_t weapondata_1e8[0x20]; //1e8, size 0x58, total is B00
   char dragons[0xB00]; //CE8

   packet_entities_t entities; //17E8 + 0x28 = 1810

} client_frame_t;

typedef struct loopmsg_s {

   BYTE data[MAX_UDP_PACKET_DATA_SIZE];
   unsigned int datalen;
} loopmsg_t;

typedef struct loopback_s {

   loopmsg_t msgs[MAX_LOOPBACK];
   int get;
   int send;
} loopback_t;
typedef struct global_net_messages_s {

   struct global_net_messages_s *next;
   qboolean DoNotFree;
   BYTE *data;
   netadr_t from;
   int cursize;//or is it maxsize?
} global_net_messages_t;

typedef struct fragbuf_s { //0x6A8 bytes

   struct fragbuf_s *next; //def +0
   uint32 ShortID; //It is confirmed that the high 16 bits are a counter that act as sequence numbers.
   sizebuf_t buffer; //20 0x14
   byte packetsizedbuffer[1400]; //+28
   int boolean1; //+1428
   int boolean2;
   char padding[260]; //+1436, this appears to be the exact size
   int fragoffset; //0x6a0
   int fragsize;
} fragbuf_t;
typedef struct fragbufholder_s {

   struct fragbufholder_s *next; //Definite +0
   int count;
   fragbuf_t *fragbuf; //Definite +8
} fragbufholder_t;


typedef struct flowstruct2_s { //12 bytes.

   int32 padding00;
   double padding04;

} flowstruct2_t;
typedef struct flowstruct1_s { //404 bytes large

   flowstruct2_t array[32];

   int32 last_received;
   double padding184;
   float KBflowrate;
   float KBflowrateaverage;

} flowstruct1_t;

typedef struct netchan_s { //size is 0x2414h.

   netsrc_t socketToUse;
   netadr_t remote_address;

   unsigned int client_index; //This variable's basically just nice to have around.  We never use it.
   float last_received; //Last received communication.
   float first_received; //When they connected

   double rate; //Guessed due to proximity in QW struct.  Not perfect match.
   double cleartime; //Almost certain since it's zerod out in clearing.

   //There's an annoying unix/windows discrepancy here--windows insists that
   //doubles are aligned to an 8 byte boundary, unix 4.  Mods don't have
   //access to netchans though, so we're okay.

   uint32 incoming_sequence;
   uint32 incoming_acknowledged;
   uint32 incoming_reliable_acknowledged; //When I think boolean, I think true false.  This is one bit, but not true/false.
   uint32 incoming_reliable_sequence; //one bit, usually inverse of above.

   uint32 outgoing_sequence; //frame sequence or something like that.
   uint32 outgoing_reliable_sequence; //one bit
   uint32 outgoing_last_reliable_sequence;

   struct client_s * owning_client; //Who owns this netchan.  excluding cls, this should always point roughly 28 or so bytes ahead of the netchan.
   int (*fn_GetFragmentSize)(struct client_s *); //SV_GetFragmentSize

   sizebuf_t netchan_message; //using the name 'message' is bad.  message is too common a word.
   byte netchan_message_buf[0xF98]; //3990 in code, rounded up to 4 byte marker

   int  reliable_length; //size of buffer below
   byte reliable_buf[0xF98];

   fragbufholder_t * padding1FA0[2]; //a linked list.  This stuff all seems to be for file transfers.
   qboolean padding1FA8[2]; //bools
   uint32 padding1FB0[2];
   fragbuf_t * padding1FB8[2]; //'packets' to be sent methinks
   uint32 NumberOfFragbufsIn1FC0[2];
   uint16 padding1FC8[2];
   uint16 padding1FCC[2];
   fragbuf_t * padding1FD0[2];
   qboolean padding1FD8[2];
   char filename[0x104];
   char * TemporaryBuffer;
   int32 TemporaryBufferSize; //20EC?
   flowstruct1_t flow[2]; //two elements that are 404 bytes long 0x194

} netchan_t;

typedef struct client_s { //20204, 0x4eec

   //There are two working theories on how some of these vars work.  It WAS believed
   //that padding00 was a bitmask and 04 was related to spectating.
   //Another theory is that the first four ints are bools, 00 04 and 0c of which
   //are "active" "spawned", and "connected"--but possibly NOT in that order...

   qboolean padding00_used; //Connected is set to 1 before this, so I guess active, not used.
   int32 padding04;
   int32 padding08; //this is identified now as a bool signalling the client's need to have ent data sent to it.
   qboolean padding0C_connected; //confirmed
   int32 padding10; //custom resources?
   int32 padding14;
   int32 padding18;

   netchan_t netchan1C;

   int32 padding2430; //chokecount, maybe.
   int32 padding2434_delta_sequence;
   int32 padding2438_IsFakeClient;
   int32 padding243C_proxy;
   usercmd_t padding2440; //0x34
   double padding2474_LastCommandTime; //These three were quickly guessed at.
   double padding247C_LastRunTime;
   double padding2484_NextRunTime;

   float ping;
   float packet_loss;
   int32 padding2494;
   int32 padding2498;
   double padding249C_TimeOfNextPingCalc;
   double padding24A4; //deals with time bases
   sizebuf_t sizebuf24AC; //datagram
   char datagram_buffer[0xFA0];
   double padding3460_connection_started;
   double next_update_time;
   double cl_updaterate;
   int32 padding3478_SendNextTick;
   int32 padding347C_SkipNextTick;
   client_frame_t * padding3480_frames;
   event_info_t events[0x40]; //3484 The sizes match
   edict_t * padding4A84; //One of these refers to the client's edict.  I'll sort it out one of these days.
   const edict_t * padding4A88; //Okay, it's later.  It seems this is a 'target' or 'destination' or something.  It might also be a camera, like the screens in cs_assault you look through.
   int32 padding4A8C_PlayerUID; //Nothing to do with authing.  Just the order clients connected in.
   int authentication_method;
   uint32 authentication_WonID; //WonID!
//   int64 paddingID64;
//   int32 padding4AA0;
   char InfoKeyBuffer[0x100]; //called userinfo actually
   uint32 padding4BA4;
   float padding4BA8;
   char authentication_CD_key[0x40]; //This seems to hold the CD key, in printed hex.
   char PlayerName[0x20]; //0x4bec
   int32 topcolor;//0x4c0c
   int32 bottomcolor;
   int32 padding4C14; //unused
   resource_t resource4C18;
   resource_t resource4CA0;
   int32 padding4D28; //unused
   int32 padding4D2C; //HasPropagatedCustomResources (like the decal)
   customization_t padding4D30; //sizeof(a4)
   CRC32_t local_map_CRC; //Partly a guess, but makes sense.
   qboolean cl_lw; //cl_lw = 0 means servers confirm weaponfire.
   qboolean cl_lc; //lag compensation
   char UnknownStruct4DE0[0x100]; //identified as physinfo
   int32 padding4EE0;
   int32 padding4EE4; //bits?
   int32 padding4EE8;
} client_t;


typedef struct client_static_s { //basically global_cls, mostly only used by the client

   // Current state.  We KNOW this is +0.
   cactive_t state;
   netchan_t netchan; //Not 100% confirmed (will be difficult since inline)

   char padding[0x1034];

   int padding340C;
   char padding5[0x104]; //3410
   char paddingstr[0x40]; //3514
   char padding4[0x800]; //3554
   char userinfo[0x104]; //3d54
   char padding2[0x208];
   int padding4060;
   //eax+4064 = demoplayback in QW, matches up sort've.
   enum { DPB_0, DPB_1, DPB_2, DPB_3 } demoplayback; //int for all intents.
   int padding4068;
   char padding3[0x320];
   int padding438C;

} client_static_t;
//global_sv
typedef struct server_s { //sizeof 0x4640C

   qboolean active; //Confirmed +0, false when server is going down
   int32 padding04; //looks to be 'pause'
   int32 padding08;
   double time0c;
   double time14;
   int lastcheck;
   double lastchecktime; //20
   //name held "undertow" when debugging.
   char name[64]; //28 A character array is known to be +28, called by SVC_info.  The size oft he array might be 32 tho.
   char name2[0x40]; //68
   char name3[0x40]; //a8
   char name4[0x40];//e8

   model_t *worldmodel;
   CRC32_t map_CRC;
   char usedpadding[0x10]; //client DLL digest

   resource_t ConsistencyData[0x500]; //140.  0x88 * 0x500 = 0x2A800
   uint32 padding2A940_SomeMaxValue; //2A940, it's the total consistency data things.  And it's only allowed to use 0x0C bytes.

   ConsistencyData_t padding2A944[0x200]; //2a944
   int32 padding30144_SomeTotalValue; //30144

   char *model_precache[MAX_MODELS]; //30148 +800
   model_t *models[512]; // 30948, unconfirmed array number.
   char padding31148[0x200]; // 200 character booleans?

   struct TemporaryEventStructure {

      short int index;
      const char * str;
      int size;
      void * mem;
   } padding31348[0x100]; //This is now known to be 0x100 0x10 byte structs related to events.

   char *sound_precache[MAX_SOUNDS]; //32348
   uint16 padding32B48[0x0400]; //index table, seems to be one short shorter than this.

   uint32 padding33348;
   char *generic_precache[MAX_GNERIC]; //3334c

   char padding33B4C[0x200][0x40]; //Strings.  An array of strings.

   uint32 padding3bb4c; //a count variable for the number of used strings.
   char *padding3BB50[0x40]; //size is guessed at, written in SV_WriteSpawn
   uint32 num_edicts; //3bc50
   uint32 max_edicts;
   edict_t *edicts; //3bc58
   entity_state_t * edstates;
   OddEntityContainer_t * padding3bc60_struct; //used in writedeltaheader.  It's a large struct that's largely a mystery.
   //mystery struct: +0 = count, +4 = int array?, +104 = entity_state_t[]

   uint32 padding3bc64;
   sizebuf_t reliable_datagram; //3bc68
   char reliablebuffer[0xFA0]; //3bc7C //maybe it's not so reliable.  Ah well.

   sizebuf_t sizebuf3CC1C; //3cc1c reliable buffer msg_all
   char OtherReliableBuffer[0xFA0]; //3cc30

   sizebuf_t sizebuf3dbd0_multicast; //3DBD0 multicast
   char MulticastBuffer[0x400]; //3dbe4 obviously used in the sizebuf.
   sizebuf_t spectator_datagram; //3DFE4
   char SpectatorBuffer[0x400];   //3dff8
   sizebuf_t signon_datagram; //3e3f8 signon
   char SignonBuffer[0x8000];   //3e40c
} server_t;
//From QW, global_svs
typedef struct server_static_s { //sizeof 0xf4, possibly 0x100

   int pad00; //This var somehow indicates the server's running state.
   client_t * clients;
   unsigned int allocated_client_slots; //+08
   unsigned int total_client_slots; //+0C
   unsigned int ServerCount;
   uint32 padding14; //flags
   uint32 padding18;
   uint32 padding1C;
   uint32 padding20;
   uint32 padding24;
   uint32 padding28;
   uint32 padding2C;
   uint32 padding30;
   uint32 padding34;

   double padding38;
   double padding40;

   uint32 padding48; //This to 7C is all some struct.
   uint32 padding4C;
   uint32 padding50;
   float  padding54;
   float  padding58;
   uint32 padding5C;
   uint32 padding60;
   float  padding64;
   float  padding68;
   uint32 padding6C;
   float  padding70;
   uint32 padding74;
   float  padding78;
   float  padding7C;

   security_module_t module80[4];

   uint32 paddingE0;
   uint32 paddingE4;
   uint32 paddingE8; //IsSecure
   double paddingEC_inittime;
   uint32 paddingF4;
   uint32 paddingF8;
   uint32 paddingFC;
/*
   gametype_e   gametype;
   int         spawncount;         // number of servers spawned since start,
                           // used to check late spawns

   int socketip;
   int socketip6;
   int socketipx;

#ifdef TCPCONNECT
   int sockettcp;
   global_svtcpstream_t *tcpstreams;
#endif

   client_t   clients[MAX_CLIENTS];
   int         serverflags;      // episode completion information

   double      last_heartbeat;
   int         heartbeat_sequence;
   global_svstats_t   stats;

   char      info[MAX_SERVERINFO_STRING];

   // log messages are used so that fraglog processes can get stats
   int         logsequence;   // the message currently being filled
   double      logtime;      // time of last swap
   sizebuf_t   log[2];
   qbyte      log_buf[2][MAX_DATAGRAM];

   challenge_t   challenges[MAX_CHALLENGES];   // to prevent invalid IPs from connecting

   bannedips_t *bannedips;
   filteredips_t *filteredips;

   char progsnames[MAX_PROGS][32];
   progsnum_t progsnum[MAX_PROGS];
   int numprogs;

#ifdef PROTOCOLEXTENSIONS
   unsigned long fteprotocolextensions;
#endif

   qboolean demoplayback;
   qboolean demorecording;
   qboolean msgfromdemo;

   int language;   //the server operators language

   levelcache_t *levcache;
*/
} server_static_t;


//globals
extern client_static_t global_cls;
extern server_static_t global_svs;
extern server_t global_sv;

extern client_t * global_host_client;

extern netadr_t global_net_local_adr;
extern netadr_t global_net_from;
extern sizebuf_t global_net_message;
extern int global_net_drop;
extern qboolean global_noip;

//functions
void NET_Init();
void NET_SendPacket(netsrc_t, int, byte *, netadr_t);
qboolean NET_GetPacket(netsrc_t);
qboolean NET_CompareAdr(netadr_t, netadr_t);
qboolean NET_StringToAdr(const char *, netadr_t *);
char * NET_AdrToString(netadr_t);
int NET_AdrToStringThread(netadr_t, char *, int);
qboolean NET_IsConfigured();
void NET_Config(qboolean);
qboolean NET_CompareBaseAdr(netadr_t, netadr_t);
qboolean NET_CompareClassBAdr(netadr_t, netadr_t);
qboolean NET_IsLocalAddress(netadr_t);
void NET_Shutdown();

void Netchan_Init();
void Netchan_Setup(netsrc_t, netchan_t *, netadr_t, int, client_t *, void *);
qboolean Netchan_CanPacket(netchan_t *);
void Netchan_Transmit(netchan_t *, int, char *);
void Netchan_Clear(netchan_t *);
void Netchan_CreateFragments(qboolean, netchan_t *, sizebuf_t *);
void Netchan_FragSend(netchan_t *);
qboolean Netchan_IncomingReady(netchan_t *);
int Netchan_CopyFileFragments(netchan_t *);
int Netchan_CopyNormalFragments(netchan_t *);
qboolean Netchan_Process(netchan_t *);
void Netchan_OutOfBandPrint(netsrc_t, netadr_t, char *, ...);

#endif
