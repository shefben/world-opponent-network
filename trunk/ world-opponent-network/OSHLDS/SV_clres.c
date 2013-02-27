#include <ctype.h>


#include "SV_clres.h"

#include "common.h"
#include "draw.h"
#include "host.h"
#include "HPAK.h"
#include "Modding.h"
#include "pf.h"
#include "R.h"
#include "SV_mcast.h"
#include "VecMath.h"

/*** GLOBALS ***/

static cvar_t cvar_sv_allow_upload   = {   "sv_allow_upload", "1",   FCVAR_NONE, 1.0, NULL };
static cvar_t cvar_sv_allow_download = { "sv_allow_download", "1",   FCVAR_NONE, 1.0, NULL };
static cvar_t cvar_sv_send_logos     = {     "sv_send_logos", "1",   FCVAR_NONE, 1.0, NULL };
static cvar_t cvar_sv_send_resources = { "sv_send_resources", "1",   FCVAR_NONE, 1.0, NULL };
static cvar_t cvar_mp_consistency    = {    "mp_consistency", "1", FCVAR_SERVER, 1.0, NULL };

/*** FUNCTIONS ***/

//This is not well commented, as it was all pretty much on my 'just get
//it done' list.  It needs to be revisited.

//customizations
int COM_CreateCustomization(customization_t *pListHead, resource_t *pResource, int playernumber, int flags, customization_t **pCustomization, int *nLumps) {

   customization_t * var_4;
   int var_8_problem;
   tempwad_t * var_10_tex;


   var_8_problem = 0;
   if(pCustomization != NULL) {
      *pCustomization = NULL;
   }

   var_4 = Q_Malloc(sizeof(customization_t));
   CHECK_MEMORY_MALLOC(var_4);

   var_4->bInUse = 0;
   var_4->resource = *pResource; //Copy the entire struct, not pointers.
   var_4->bTranslated = 0;
   var_4->nUserData1 = 0;
   var_4->nUserData2 = 0;
   var_4->pInfo = NULL;
   var_4->pBuffer = NULL;
   var_4->pNext = NULL;

   if(pResource->nDownloadSize <= 0) { var_8_problem = 1; }
   else {

      var_4->bInUse = 1;
      //pBuffer is a void pointer, which points to data.  We DO want to dereference the void pointer--it's not an error.
      if((flags & 0x01) != 0 && HPAK_GetDataPointer("custom.hpk", pResource, (byte **)&(var_4->pBuffer), NULL) == 0) { var_8_problem = 1; }
      else {

         var_4->pBuffer = COM_LoadFileForMe(pResource->szFileName, NULL);
         if((var_4->resource.ucFlags & 0x04) == 0 || var_4->resource.type != t_decal) { /*break, except we're not in a loop*/ }
         else {

            var_4->resource.playernum = playernumber;
            if(CustomDecal_Validate(var_4->pBuffer, pResource->nDownloadSize) == NULL) { var_8_problem = 1; }
            else if((flags & 0x04) != 0) { }
            else {

               //This isn't the most logical of casts, but it kindve fits for a decal, and the struct doesn't contradict any of the known instructions.
               var_4->pInfo = Q_Malloc(sizeof(tempwad_t));
               CHECK_MEMORY_MALLOC(var_4);
               Q_memset(var_4->pInfo, 0, sizeof(tempwad_t));
               var_10_tex = var_4->pInfo;

               if(pResource->nDownloadSize < 0x400 || pResource->nDownloadSize > 0x4000 ||
                  CustomDecal_Init(var_10_tex, var_4->pBuffer, pResource->nDownloadSize, playernumber) == 0 ||
                  var_10_tex->num_items2 <= 0) { var_8_problem = 1; }
               else {

                  if(nLumps != NULL) { *nLumps = var_10_tex->num_items2; }
                  var_4->bTranslated = 1;
                  var_4->nUserData1 = 0;
                  var_4->nUserData2 = var_10_tex->num_items2;
                  if(flags & 0x02) {

                     Q_Free(var_10_tex->filename);
                     Q_Free(var_10_tex->padding04);
                     Q_Free(var_10_tex->padding10);
                     Q_Free(var_10_tex);
                     var_4->pInfo = NULL;
                  }
               }
               //There may be a subtle memory leak here, but I'll have to do CustomDecal_init before I'm certain.
            }
         }
      }
   }

   if(var_8_problem == 0) {

      if(pCustomization != NULL) {
         *pCustomization = var_4;
      }

      var_4->pNext = pListHead->pNext;
      pListHead->pNext = var_4;
   }
   else {

      if(var_4->pBuffer != NULL) {
         Q_Free(var_4->pBuffer); //Note that, if ever the CON_FreeFile does more than just call Q_Free, this will need changing.
      }
      if(var_4->pInfo != NULL) {
         Q_Free(var_4->pInfo);
      }
      Q_Free(var_4);
   }

   return(var_8_problem == 0);
}
void SV_CreateCustomizationList(client_t * cl) {

   resource_t * ptr;
   customization_t * ptr2;
   customization_t * newCust;
   int var_c, LumpCount;



   if(cl->padding4D30.pNext != NULL) {
      Con_Printf("%s: This function DOES in fact leak memory.\n");
   }
   cl->padding4D30.pNext = NULL;


   for(ptr = cl->resource4C18.pNext; ptr != &(cl->resource4C18); ptr = ptr->pNext) {

      var_c = 0;
      for(ptr2 = cl->padding4D30.pNext; ptr2 != NULL; ptr2 = ptr2->pNext) {

         if(Q_memcmp(ptr2->resource.rgucMD5_hash, ptr->rgucMD5_hash, 0x10) == 0) {

            var_c = 1;
            break;
         }
      }
      if(var_c == 1) {
         Con_Printf("%s: Duplicate resource detected.\n", __FUNCTION__);
         continue;
      }

      //create it.
      LumpCount = 0;
      if(COM_CreateCustomization(&(cl->padding4D30), ptr, -1, 3, &newCust, &LumpCount) == 0) {
         Con_Printf("%s: Ignoring custom decal.\n", __FUNCTION__);
         continue;
      }

      newCust->nUserData2 = LumpCount;
      gEntityInterface.pfnPlayerCustomization(cl->padding4A84, newCust);
   }
}
void COM_ClearCustomizationList(customization_t *pHead) { //There was a second, boolean argument that is not required.

   customization_t * NextList, * CurrentList;
   tempwad_t * wad;
   tempdata_t * wad_data;
   int i;


   //pHead can't be null within the current program design.
   //The way it works is the client_t structure basically wastes all but two
   //pointers in the customization structure.  So we go straight for pNext.
   if(pHead == NULL || pHead->pNext == NULL) { //pHead can't be null within the current program design.
      return;
   }
   CurrentList = pHead->pNext;

   do {

      NextList = CurrentList->pNext;

      if(CurrentList->bInUse != 0 && CurrentList->pBuffer != NULL) {
         Q_Free(CurrentList->pBuffer);
      }

      if(CurrentList->bInUse != 0 && CurrentList->pInfo != NULL) {

         if(CurrentList->resource.type == t_decal) {

            /* R_DecalRemoveAll is EMPTY in server build.
            if(bCleanDecals && global_cls.state == ca_active) {
               //R_DecalRemoveAll(var_8->resource.playernum + 1);
            }
            */

            wad = (tempwad_t *)(CurrentList->pInfo); //var_8_CurrentList->pInfo can be a pointer to multiple datatypes.  The specifics are less clear.

            Q_Free(wad->padding10);
            wad_data = wad->padding04;

            for(i = 0; i < wad->num_items; i++, wad_data++) {

               if(Cache_Check(&(wad_data->cache)) != 0) {
                  Cache_Free(&(wad_data->cache));
               }
            }

            Q_Free(wad->filename);
            Q_Free(wad->padding04);
         }
         Q_Free(CurrentList->pInfo);
      }

      Q_Free(CurrentList);
      CurrentList = NextList;
   } while(CurrentList != NULL);

   CurrentList->pNext = NULL;
}
void SV_Customization(client_t * cl, resource_t * ptrResource, int arg_8) {

   unsigned int i;
   int var_8;


   var_8 = -1;
   for(i = 0, global_host_client = global_svs.clients; i < global_svs.allocated_client_slots; i++, global_host_client++) {

      if(global_host_client == cl) {
         var_8 = i;
         break;
      }
   }

   if(var_8 == -1) {
      Sys_Error("%s: Couldn't find client index.  Which should not be possible.\n", __FUNCTION__);
   }


   for(i = 0, global_host_client = global_svs.clients; i < global_svs.allocated_client_slots; i++, global_host_client++) {

      if((global_host_client->padding00_used == 0 && global_host_client->padding04 == 0) ||
         global_host_client->padding2438_IsFakeClient != 0) {

         continue;
      }
      if(global_host_client != cl || arg_8 == 0) {

         MSG_WriteByte(&(global_host_client->netchan1C.netchan_message), '.');
         MSG_WriteByte(&(global_host_client->netchan1C.netchan_message), var_8);
         MSG_WriteByte(&(global_host_client->netchan1C.netchan_message), ptrResource->type);
         MSG_WriteString(&(global_host_client->netchan1C.netchan_message), ptrResource->szFileName);
         MSG_WriteShort(&(global_host_client->netchan1C.netchan_message), ptrResource->nIndex);
         MSG_WriteLong(&(global_host_client->netchan1C.netchan_message), ptrResource->nDownloadSize);
         MSG_WriteByte(&(global_host_client->netchan1C.netchan_message), ptrResource->ucFlags);
         if(ptrResource->ucFlags & 0x04) {
            SZ_Write(&(global_host_client->netchan1C.netchan_message), ptrResource->rgucMD5_hash, 0x10);
         }
      }
   }
}
void SV_PropagateCustomizations() {

   unsigned int i;
   customization_t * ptr;
   client_t * cl;


   cl = global_host_client;

   for(i = 0, global_host_client = global_svs.clients; i < global_svs.allocated_client_slots; i++, global_host_client++) {

      if((global_host_client->padding00_used == 0 && global_host_client->padding04 == 0) ||
         global_host_client->padding2438_IsFakeClient != 0) {

         continue;
      }

      for(ptr = global_host_client->padding4D30.pNext; ptr != NULL; ptr = ptr->pNext) {

         if(ptr->bInUse == 0) { continue; }

         MSG_WriteByte(&(cl->netchan1C.netchan_message), '.');
         MSG_WriteByte(&(cl->netchan1C.netchan_message), i);
         MSG_WriteByte(&(cl->netchan1C.netchan_message), ptr->resource.type);
         MSG_WriteString(&(cl->netchan1C.netchan_message), ptr->resource.szFileName);
         MSG_WriteShort(&(cl->netchan1C.netchan_message), ptr->resource.nIndex);
         MSG_WriteLong(&(cl->netchan1C.netchan_message), ptr->resource.nDownloadSize);
         MSG_WriteByte(&(cl->netchan1C.netchan_message), ptr->resource.ucFlags);
         if(ptr->resource.ucFlags & 0x04) {
            SZ_Write(&(cl->netchan1C.netchan_message), ptr->resource.rgucMD5_hash, 0x10);
         }
      }
   }

   global_host_client = cl;
}

//resources
void SV_AddResource(int type, const char * filename, int size, int flags, int index) {

   resource_t * global_sv_res;

   if(global_sv.padding2A940_SomeMaxValue > 0x4FF) {
      Sys_Error("%s: Too many resources on server.\n", __FUNCTION__);
   }

   global_sv_res = &global_sv.ConsistencyData[global_sv.padding2A940_SomeMaxValue];
   global_sv.padding2A940_SomeMaxValue++;

   global_sv_res->type = type;
   Q_snprintf(global_sv_res->szFileName, sizeof(global_sv_res->szFileName)-1, "%s", filename);
   global_sv_res->szFileName[sizeof(global_sv_res->szFileName)-1] = '\0';
   global_sv_res->nIndex = index;
   global_sv_res->nDownloadSize = size;
   global_sv_res->ucFlags = flags;
}

void SV_CreateResourceList() {

   int i, size;
   char ** ptr;
   char TempBuffer[0x100]; //That'd better be enough.
   int var_8;


   global_sv.padding2A940_SomeMaxValue = 0;

   for(i = 1, ptr = &(global_sv.generic_precache[1]); *ptr != NULL; i++, ptr++) {

      size = COM_FileSize(*ptr);
      if(size < 0) { size = 0; }

      SV_AddResource(4, *ptr, size, 1, i);
   }

   var_8 = 0;
   TempBuffer[sizeof(TempBuffer)-1] = '\0';
   for(i = 1, ptr = &(global_sv.sound_precache[1]); *ptr != NULL; i++, ptr++) {

      if((*ptr)[0] == '!') {
         if(var_8 == 0) {

            SV_AddResource(0, "!", 0, 1, i);
            var_8 = 1;
         }
      }
      else {

         Q_snprintf(TempBuffer, sizeof(TempBuffer)-1, "sound/%s", *ptr);
         size = COM_FileSize(TempBuffer);
         if(size < 0) { size = 0; }

         SV_AddResource(0, *ptr, size, 0, i);
      }
   }

   for(i = 1, ptr = &(global_sv.model_precache[1]); *ptr != NULL; i++, ptr++) {

      size = COM_FileSize(*ptr);
      if(size < 0) { size = 0; }

      SV_AddResource(2, *ptr, size, global_sv.padding31148[i], i);
   }

   for(i = 0; i < global_sv_decalnamecount; i++) {

      SV_AddResource(3, global_sv_decalnames[i], Draw_DecalSize(i), 0, i);
   }

   for(i = 1; i < 0x100; i++) {

      if(global_sv.padding31348[i].mem == NULL) { break; }

      SV_AddResource(5, global_sv.padding31348[i].str, global_sv.padding31348[i].size, 1, i);
   }
}
void SV_CreateGenericResources() {

   char filename[0x100]; //pretty sure the max length is 40, but I cannot be sure.
   byte * file;
   const char * ptr;


   COM_StripExtension(global_sv.name4, filename);
   COM_DefaultExtension(filename, ".res");
   COM_FixSlashes(filename);

   file = COM_LoadFileForMe(filename, NULL);
   if(file == NULL) { return; }


   Con_Printf("%s: Precaching from %s", __FUNCTION__, filename);

   ptr = file;
   global_sv.padding3bb4c = 0;

   while(1) {

      ptr = COM_Parse(ptr);
      if(global_com_token[0] == '\0') { break; }

      //This could be tweaked.
      if(Q_strstr(global_com_token, "..") != NULL ||
         Q_strstr(global_com_token, ":") != NULL ||
         Q_strstr(global_com_token, "/") != NULL ||
         Q_strstr(global_com_token, ".cfg") != NULL ||
         Q_strstr(global_com_token, ".bat") != NULL ||
         Q_strstr(global_com_token, ".exe") != NULL ||
         Q_strstr(global_com_token, ".com") != NULL ||
         Q_strstr(global_com_token, ".lst") != NULL ||
         Q_strstr(global_com_token, ".vbs") != NULL ||
         Q_strstr(global_com_token, ".dll") != NULL) {

         Con_Printf("%s: Bad resource name/path %s.", __FUNCTION__, global_com_token);
         continue;
      }

      Q_strncpy(global_sv.padding33B4C[global_sv.padding3bb4c], global_com_token, 0x3F);
      global_sv.padding33B4C[global_sv.padding3bb4c][0x3F] = '\0';

      PF_precache_generic_I(global_sv.padding33B4C[global_sv.padding3bb4c]);
      Con_Printf("%s: Parsed %s.\n", __FUNCTION__, global_com_token);
      global_sv.padding3bb4c++;
   }

   COM_FreeFile(file);
}
void SV_RegisterResources() {

   resource_t * ptr;
   client_t * cl;


   cl = global_host_client;
   cl->padding10 = 0;
   for(ptr = cl->resource4C18.pNext; ptr != &(cl->resource4C18); ptr = ptr->pNext) {

      SV_CreateCustomizationList(cl);
      SV_Customization(cl, ptr, 1);
   }
}
int SV_UploadComplete(client_t * cl) {

   if(&(cl->resource4CA0) != cl->resource4CA0.pNext) { return(0); }

   SV_RegisterResources();
   SV_PropagateCustomizations();

   cl->padding4D2C = 1;
   return(1);
}
int SV_RequestMissingResources() {


   if(global_host_client->padding10 != 0 && global_host_client->padding4D2C == 0) {

      SV_UploadComplete(global_host_client);
   }

   //we always return 0.  What's the point of returning it then?
   return(0);
}
void SV_RequestMissingResourcesFromClients() {

   unsigned int i;

   for(i = 0, global_host_client = global_svs.clients; i < global_svs.allocated_client_slots; i++, global_host_client++) {
      if(global_host_client->padding00_used != 0 || global_host_client->padding04 != 0) {
         while(SV_RequestMissingResources() != 0) { }
      }
   }
}

/* DESCRIPTION: SV_AddToResourceList
// LOCATION:
// PATH: SV_ParseResourceList, SV_MoveToOnHandList
//
// links an element into a list.  Yay.
*/
void SV_AddToResourceList(resource_t * arg0, resource_t * arg4) {

   if(arg0 == NULL || arg4 == NULL) {
      Sys_Error("%s: Passed NULL arg.\n", __FUNCTION__);
   }
   if(arg0->pNext != NULL || arg0->pPrev != NULL) {
      Con_Printf("%s: Passed resource has non-null list pointers.  Appears to already be linked.\n", __FUNCTION__);
      return;
   }

   //links to the beginning of something I think
   arg0->pNext = arg4->pNext;
   arg4->pNext->pPrev = arg0;

   arg4->pNext = arg0;
   arg0->pPrev = arg4;
}
int COM_SizeofResourceList(resource_t * pList, resourceinfo_t * ri) {

   resource_t * ptr;
   int size;

   Q_memset(ri, 0, sizeof(resourceinfo_t));

   //This is a circular linked list.
   size = 0;
   for(ptr = pList->pNext; ptr != pList; ptr = ptr->pNext) {

      size += ptr->nDownloadSize;
      if(ptr->type == t_model && ptr->nIndex == 1) {

         ri->info[6].size += ptr->nDownloadSize;
      }
      else {

         ri->info[ptr->type].size += ptr->nDownloadSize;
      }
   }

   //Did we just miss the pList we were passed?  I think we did.
   return(size);
}
/* DESCRIPTION: SV_RemoveFromResourceList
// LOCATION:
// PATH: SV_ClearResourceList, SV_MoveToOnHandList
//
// Unlinks an element.
*/
void SV_RemoveFromResourceList(resource_t * arg0) {

   arg0->pPrev->pNext = arg0->pNext;
   arg0->pNext->pPrev = arg0->pPrev;

   arg0->pNext = arg0->pPrev = NULL;
}
/* DESCRIPTION: SV_ClearResourceList
// LOCATION:
// PATH: SV_ParseResourceList, SV_ClearResourceLists
//
// Wipes all elements in a list.  This helps to identify that the
// passed list element, which seems to be a dummy element, is NEVER
// removed and will point to itself if the list is empty.
*/
void SV_ClearResourceList(resource_t * arg0) {

   resource_t * ptr, * ptr2;

   for(ptr = arg0->pNext; ptr != arg0 && ptr != NULL; ptr = ptr2) {

      ptr2 = ptr->pNext;

      SV_RemoveFromResourceList(ptr);
      Q_Free(ptr);
   }

   arg0->pNext = arg0->pPrev = arg0;
}
void SV_ClearResourceLists(client_t * cl) {

   SV_ClearResourceList(&(cl->resource4C18));
   SV_ClearResourceList(&(cl->resource4CA0));
}

/* DESCRIPTION: SV_MoveToOnHandList
// LOCATION:
// PATH: SV_BatchUploadRequest, SV_ProcessFile
//
// Moves an element from one list to another.
*/
void SV_MoveToOnHandList(resource_t * ptrResource) {

   if(ptrResource == NULL) {

      Con_Printf("%s: NULL pointer argument.\n", __FUNCTION__);
   }
   else {

      SV_RemoveFromResourceList(ptrResource);
      SV_AddToResourceList(ptrResource, &(global_host_client->resource4C18));
   }
}

/* DESCRIPTION: SV_CheckFile
// LOCATION:
// PATH: SV_ParseResourceList->SV_BatchUploadRequest
//
// This function, called by only one other fuction, illustrates how sometimes
// the silly programmer will use annoying formats to complicate the relatively
// simple process of passing data.  It has been reworked.
//
// It just checks to see if we should request an element from a client.
// If we need to, this function adds the request to the outgoing buffer and
// returns 0.
*/
int SV_CheckFile(sizebuf_t * msg, const char * hash) {

   resource_t var_88_resource;
   char buffer[33];


   //Don't honor requests.
   if(cvar_sv_allow_upload.value == 0) { return(1); }

   //See if we have it.
   Q_memset(&var_88_resource, 0, sizeof(resource_t));
   Q_memcpy(var_88_resource.rgucMD5_hash, hash, 0x10);

   if(HPAK_GetDataPointer("custom.hpk", &var_88_resource, NULL, NULL)) { return(1); }

   //Was not here, request it.
   MSG_WriteByte(msg, 0x09);
   MSG_WriteBuf(msg, sizeof("upload \"!MD5")-1, "upload \"!MD5");
   MD5_Print_safe(hash, buffer);
   MSG_WriteBuf(msg, sizeof(buffer)-1, buffer);
   MSG_WriteString(msg, "\"");

   return(0);
}

/* DESCRIPTION: SV_BatchUploadRequest
// LOCATION:
// PATH: SV_ParseResourceList
//
//
*/
void SV_BatchUploadRequest(client_t * cl) {

   resource_t * ptr, * ptr2;

   for(ptr = cl->resource4CA0.pNext; ptr != &cl->resource4CA0; ptr = ptr2) {

      ptr2 = ptr->pNext;

      if((ptr->ucFlags & 0x02) == 0) {
         SV_MoveToOnHandList(ptr);
         continue;
      }
      if(ptr->type != t_decal) {
         continue;
      }
      if((ptr->ucFlags & 0x04) == 0) {
         Con_Printf("%s: Non customization in upload queue.\n", __FUNCTION__);
         SV_MoveToOnHandList(ptr);
         continue;
      }

      if(SV_CheckFile(&(cl->netchan1C.netchan_message), ptr->rgucMD5_hash) != 0) {
         SV_MoveToOnHandList(ptr);
      }
   }
}

/* DESCRIPTION: SV_EstimateNeededResources
// LOCATION:
// PATH: SV_ParseResourceList
//
// Simple enough.  If we can't find a resource, we add its size to
// our estimation.  We also mark a bit that I assume means 'not here'.
*/
int SV_EstimateNeededResources() {

   int size;
   resource_t * ptr;


   size = 0;
   for(ptr = global_host_client->resource4CA0.pNext; ptr != &global_host_client->resource4CA0; ptr = ptr->pNext) {

      if(ptr->type == t_decal && HPAK_ResourceForHash("custom.hpk", ptr->rgucMD5_hash, NULL) == 0 && ptr->nDownloadSize != 0) {

         size += ptr->nDownloadSize;
         ptr->ucFlags |= 0x02;
      }
   }

   return(size);
}

/* DESCRIPTION: SV_ParseResourceList
// LOCATION:
// PATH: SV_Frame->SV_ReadPackets->SV_ExecuteClientMessage
//
//
*/
void SV_ParseResourceList() {

   int i, var_8_short1, var_C_size;
   resource_t * var_10_resource_p;
   resourceinfo_t var_30_info;


   var_8_short1 = MSG_ReadShort();

   SV_ClearResourceList(&(global_host_client->resource4C18));
   SV_ClearResourceList(&(global_host_client->resource4CA0));

   for(i = 0; i < var_8_short1; i++) {

      var_10_resource_p = Q_Malloc(sizeof(resource_t));
      CHECK_MEMORY_MALLOC(var_10_resource_p);

      Q_strncpy(var_10_resource_p->szFileName, MSG_ReadString(), sizeof(var_10_resource_p->szFileName)-1);
      var_10_resource_p->szFileName[sizeof(var_10_resource_p->szFileName)-1] = '\0';
      var_10_resource_p->type = MSG_ReadByte();
      var_10_resource_p->nIndex = MSG_ReadShort();
      var_10_resource_p->nDownloadSize = MSG_ReadLong();
      var_10_resource_p->ucFlags = MSG_ReadByte();

      var_10_resource_p->pNext = NULL;
      var_10_resource_p->pPrev = NULL;

      var_10_resource_p->ucFlags &= 0xFD;

      if(var_10_resource_p->ucFlags & 0x04) {
         MSG_ReadBuf(0x10, var_10_resource_p->rgucMD5_hash);
      }

      SV_AddToResourceList(var_10_resource_p, &(global_host_client->resource4CA0));
   }

   if(cvar_sv_allow_upload.value != 0) {

      Con_Printf("%s: Verifying and uploading resources...\n", __FUNCTION__);

      var_C_size = COM_SizeofResourceList(&(global_host_client->resource4CA0), &var_30_info);
      if(var_C_size != 0) {

         //lots of size prints here that I am ignoring.
         Con_Printf("Resources to request: %i bytes.\n", SV_EstimateNeededResources());
      }
   }

   global_host_client->padding10 = 1;
   global_host_client->padding4D2C = 0;
   SV_BatchUploadRequest(global_host_client);
}


//consistency list
int SV_FileInConsistencyList(resource_t * resource, ConsistencyData_t ** a4) {

   int i;

   i = 0;
   while(1) {

      if(global_sv.padding2A944[i].name == NULL) { return(0); } //That's how we know we're at the end.
      if(Q_strcasecmp(global_sv.padding2A944[i].name, resource->szFileName) == 0) { break; }

      i++;
   }

   //Hey, we found a match.
   if(a4 != NULL) {
      *a4 = &(global_sv.padding2A944[i]);
   }

   return(1);
}
int SV_TransferConsistencyInfo() {

   int i, total;
   ConsistencyData_t * var_114;
   char buffer[0x100]; //Since there's that hard limit of 0x40, this can be refined.
   vec3_t var_120, var_12C;
   resource_t * ptr;


   buffer[sizeof(buffer)-1] = '\0';
   total = 0;
   for(i = 0; i < global_sv.padding2A940_SomeMaxValue; i++) {

      ptr = &(global_sv.ConsistencyData[i]);
      if((ptr->ucFlags & 0x80) != 0) { continue; }
      if(SV_FileInConsistencyList(ptr, &var_114) == 0) { continue; }

      ptr->ucFlags |= 0x80;
      if(ptr->type == t_sound) {
         Q_snprintf(buffer, sizeof(buffer)-1, "sound/%s", ptr->szFileName);
      }
      else {
         Q_snprintf(buffer, sizeof(buffer)-1, "%s", ptr->szFileName);
      }

      MD5_Hash_File(ptr->rgucMD5_hash, buffer, 0, 0); //What if it fails?

      if(ptr->type == t_model) {

         switch(var_114->padding10_force_state) {

         case 1: //This is an enum actually.

            if(R_GetStudioBounds(buffer, var_120, var_12C) == 0) {
               Sys_Error("%s: Couldn't get bounds.\n");
            }

            Q_memcpy(&(ptr->rguc_reserved[0x01]), var_120, 0x0C);
            Q_memcpy(&(ptr->rguc_reserved[0x0D]), var_12C, 0x0C);
            ptr->rguc_reserved[0x00] = var_114->padding10_force_state;
            COM_Munge1(ptr->rguc_reserved, 0x20, global_svs.ServerCount);
            break;

         case 2:
         case 3:

            Q_memcpy(&(ptr->rguc_reserved[0x01]), var_114->mins, 0x0C);
            Q_memcpy(&(ptr->rguc_reserved[0x0D]), var_114->maxs, 0x0C);
            ptr->rguc_reserved[0x00] = var_114->padding10_force_state;
            COM_Munge1(ptr->rguc_reserved, 0x20, global_svs.ServerCount);
            break;

         default:
            break;
         }
      }
   }

   return(total);
}

/* DESCRIPTION: SV_ParseConsistencyResponse
// LOCATION: sv_user.c (as part of SV_ExecuteClientMessage)
// PATH: SV_Frame->SV_ReadPackets->SV_ExecuteClientMessage
//
//
*/
void SV_ParseConsistencyResponse() {

   int i;
   char ModDisconnectMessage[0x110]; //The mod files say 256, knowing mod writers...

   char var_A8_Another0x20chunk[0x20];
//Struct design (note with so many bitreads, it wouldn't be feasable to make one)
// byte 1 = some identifier
// byte 2-0c = vec3_t byte 0d-19 = vec3_t remainder is not accessed here.
   char var_88_NullStruct[0x20];
   char var_68_Struct[0x20];
   int var_B0;
   int var_18_status;
   unsigned int var_4;
   int var_c_count;
   vec3_t var_3c, var_48;
   vec3_t var_24, var_30;
   resource_t * var_14_ConsistencyStruct;


   Q_memset(var_88_NullStruct, 0, sizeof(var_88_NullStruct));
   var_B0 = MSG_ReadShort();
   COM_UnMunge1(global_net_message.data + global_msg_readcount, var_B0, global_svs.ServerCount);

   var_18_status = 0;
   var_c_count = 0;
   MSG_StartBitReading(&global_net_message);

   while(1) {

      if(MSG_ReadBits(1) == 0) { break; }
      var_4 = MSG_ReadBits(0x0c);
      if(var_4 >= global_sv.padding2A940_SomeMaxValue) {

         var_18_status = -1;
         break;
      }

      var_14_ConsistencyStruct = &(global_sv.ConsistencyData[var_4]);
      if((var_14_ConsistencyStruct->ucFlags & 0x80) == 0) { break; }

      memcpy(var_68_Struct, var_14_ConsistencyStruct->rguc_reserved, sizeof(var_68_Struct));
      if(memcmp(var_68_Struct, var_88_NullStruct, sizeof(var_68_Struct)) == 0) {

         if(MSG_ReadBits(0x20) != *(int *)var_14_ConsistencyStruct->rgucMD5_hash) { //This seems noteworthy.
            var_18_status = var_4+1;
         }
      }
      else {

         MSG_ReadBitData((byte *)(var_3c), 0x0C); //that's 12 bytes, not bits.
         MSG_ReadBitData((byte *)(var_48), 0x0C);
         Q_memcpy(var_A8_Another0x20chunk, var_14_ConsistencyStruct->rguc_reserved, sizeof(var_A8_Another0x20chunk));

         COM_UnMunge1(var_A8_Another0x20chunk, sizeof(var_A8_Another0x20chunk), global_svs.ServerCount);

         switch(var_A8_Another0x20chunk[0]) {

         case 1:
            Q_memcpy(var_24, var_A8_Another0x20chunk + 1, 0x0C);
            Q_memcpy(var_30, var_A8_Another0x20chunk + 13, 0x0C);
            if(VectorCompare(var_3c, var_24) == 0 || VectorCompare(var_48, var_30) == 0) {
               var_18_status = var_4+1;
            }

         case 2:
            Q_memcpy(var_24, var_A8_Another0x20chunk + 1, 0x0C);
            Q_memcpy(var_30, var_A8_Another0x20chunk + 13, 0x0C);

            for(i = 0; i < 3; i++) {
               if(var_3c > var_24 || var_48 < var_30) {
                  var_18_status = var_4+1;
                  break;
               }
            }
            break;

         case 3:
            Q_memcpy(var_24, var_A8_Another0x20chunk + 1, 0x0C);
            Q_memcpy(var_30, var_A8_Another0x20chunk + 13, 0x0C);

            if(var_3c[0] == -1 && var_3c[1] == -1 && var_3c[2] == -1 &&
               var_48[0] == -1 && var_48[1] == -1 && var_48[2] == -1) {

               break;
            }
            for(i = 0; i < 3; i++) {
               if(var_3c > var_24 || var_48 < var_30) {
                  var_18_status = var_4+1;
                  break;
               }
            }
            break;

         default:
            global_msg_badread = 1;
            var_18_status = var_4+1;
         }
      }
      if(global_msg_badread != 0) { break; }
      var_c_count++;
   }

   //whew
   MSG_EndBitReading(&global_net_message);

   if(var_18_status < 0 || var_c_count != global_sv.padding30144_SomeTotalValue) {

      global_msg_badread = 1;
      Con_Printf("%s: Client %s sent bad file data.\n", __FUNCTION__, global_host_client->PlayerName);
      SV_DropClient(global_host_client, 0, "Bad file data.");
      return;
   }

   if(var_18_status > 0) {

      //This function gives the Mod the option of letting the client on anyway.
      if(gEntityInterface.pfnInconsistentFile(global_host_client->padding4A84, global_sv.ConsistencyData[var_18_status - 1].szFileName, ModDisconnectMessage) != 0) {

         SV_ClientPrintf("%s", ModDisconnectMessage);
         SV_DropClient(global_host_client, 0, "Bad file %s", ModDisconnectMessage);
      }
      return;
   }
   //and finally...

   global_host_client->padding18 = 0;
}

//This function is linked to and depends heavily on SV_SendResources.  Treat the two as the same fucntion.
void SV_SendConsistencyList() { //This is passed an ignored sizebuf.

   unsigned int i, var_C, var_10;
   resource_t * res;


   if(cvar_mp_consistency.value == 0 || global_sv.padding30144_SomeTotalValue == 0 || global_host_client->padding243C_proxy != 0) {

      global_host_client->padding18 = 0;
      MSG_WriteBits(0, 1);
   }
   else {

      var_C = 0;
      global_host_client->padding18 = 1;
      MSG_WriteBits(1, 1);

      for(i = 0; i < global_sv.padding2A940_SomeMaxValue; i++) {

         res = &(global_sv.ConsistencyData[i]);
         if((res->ucFlags & 0x80) == 0) { continue; }

         var_10 = i - var_C;
         MSG_WriteBits(1, 1);
         if(var_10 > 0x1f) { //apparently HL allows two different bit depths here.

            MSG_WriteBits(0, 1);
            MSG_WriteBits(var_10, 0x0A);
         }
         else {

            MSG_WriteBits(1, 1);
            MSG_WriteBits(var_10, 5);
         }
         var_C = i;
      }

      MSG_WriteBits(0, 1);
   }
}

//This function could be sped up easily.
void SV_SendResources(sizebuf_t * sz) {

   unsigned int i;
   byte temprguc[32];


   Q_memset(temprguc, 0, sizeof(temprguc));

   MSG_WriteByte(sz, 0x2D);
   MSG_WriteLong(sz, global_svs.ServerCount);
   MSG_WriteLong(sz, 0);
   MSG_WriteByte(sz, 0x2B);

   MSG_StartBitWriting(sz);
   MSG_WriteBits(global_sv.padding2A940_SomeMaxValue, 0x0C);

   for(i = 0; i < global_sv.padding2A940_SomeMaxValue; i++) {

      MSG_WriteBits(global_sv.ConsistencyData[i].type, 4);
      MSG_WriteBitString(global_sv.ConsistencyData[i].szFileName);
      MSG_WriteBits(global_sv.ConsistencyData[i].nIndex, 0x0C);
      MSG_WriteBits(global_sv.ConsistencyData[i].nDownloadSize, 0x18);
      MSG_WriteBits(global_sv.ConsistencyData[i].ucFlags, 3);

      if(global_sv.ConsistencyData[i].ucFlags & 4) {
         MSG_WriteBitData(global_sv.ConsistencyData[i].rgucMD5_hash, 0x10);
      }

      if(Q_memcmp(temprguc, global_sv.ConsistencyData[i].rguc_reserved, sizeof(temprguc)) != 0) {

         MSG_WriteBits(1, 1);
         MSG_WriteBitData(global_sv.ConsistencyData[i].rguc_reserved, 0x20);

      }
      else {
         MSG_WriteBits(0, 1);
      }
   }

   SV_SendConsistencyList();
   MSG_EndBitWriting(sz);
}



void SV_CL_RES_Init() {

   Cvar_RegisterVariable(&cvar_sv_allow_upload);
   Cvar_RegisterVariable(&cvar_sv_allow_download);
   Cvar_RegisterVariable(&cvar_sv_send_logos);
   Cvar_RegisterVariable(&cvar_sv_send_resources);
   Cvar_RegisterVariable(&cvar_mp_consistency);
}
