// MADE OBSOLETE BY GetHTTPDocumentOp

#ifndef __WON_GETMOTDOP_H__
#define __WON_GETMOTDOP_H__
#include "WONShared.h"

#include "GetHTTPDocumentOp.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetMOTDOp : public GetHTTPDocumentOp
{
public:
	GetMOTDOp(const std::string &theProduct) : GetHTTPDocumentOp(theProduct, HTTPDocType_MOTD) { }

	void SetSysMOTDPath(const std::string &thePath)			{ SetDocumentPath(HTTPDocOwner_Sys,thePath); }
	void SetGameMOTDPath(const std::string &thePath)		{ SetDocumentPath(HTTPDocOwner_Game,thePath); }

	const std::string& GetSysMOTDPath()  const { return GetDocumentPath(HTTPDocOwner_Sys); }
	const std::string& GetGameMOTDPath() const { return GetDocumentPath(HTTPDocOwner_Game);}

	bool SysMOTDIsNew()  const { return DocumentIsNew(HTTPDocOwner_Sys);  }
	bool GameMOTDIsNew() const { return DocumentIsNew(HTTPDocOwner_Game); }

	ByteBufferPtr GetSysMOTD()  const { return GetDocument(HTTPDocOwner_Sys);  }
	ByteBufferPtr GetGameMOTD() const { return GetDocument(HTTPDocOwner_Game); }	
};

typedef SmartPtr<GetMOTDOp> GetMOTDOpPtr;

}; // namespace WONAPI


#endif
