#ifndef __WON_DIRENTITYREPLYPARSER_H__
#define __WON_DIRENTITYREPLYPARSER_H__
#include "WONShared.h"

#include "DirEntity.h"
#include "WONStatus.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DirEntityReplyParser
{
public:
	static WONStatus ParseMultiEntityReply(const void *theMsg, unsigned long theMsgLen, DirEntityList &theList,unsigned char* theSeq=NULL);
	static WONStatus ParseSingleEntityReply(const void *theMsg, unsigned long theMsgLen, DirEntityPtr &theEntity);

	static bool OnlyDirs(DWORD theFlags);
	static bool OnlyServices(DWORD theServices);
};

} // namespace WONAPI

#endif
