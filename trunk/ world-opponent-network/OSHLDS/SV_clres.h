/* Resource lists, and the like.
// Couldn't really get a great name in eight letters.
*/

#ifndef OSHLDS_SV_CLRES_HEADER
#define OSHLDS_SV_CLRES_HEADER

#include "endianin.h"
#include "NET.h"

//functions
int COM_CreateCustomization(customization_t *, resource_t *, int, int, customization_t **, int *);
void COM_ClearCustomizationList(customization_t *);

void SV_CreateResourceList();
void SV_CreateGenericResources();
void SV_RequestMissingResourcesFromClients();
void SV_ClearResourceLists(client_t *);
void SV_MoveToOnHandList(resource_t *);
void SV_ParseResourceList();

int SV_TransferConsistencyInfo();
void SV_ParseConsistencyResponse();
void SV_SendResources(sizebuf_t *);

void SV_CL_RES_Init();

#endif
