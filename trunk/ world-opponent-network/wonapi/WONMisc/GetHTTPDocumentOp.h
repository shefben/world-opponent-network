#ifndef __WON_GETHTTPDOCUMENTOP_H__
#define __WON_GETHTTPDOCUMENTOP_H__
#include "WONShared.h"

#include "WONCommon/AsyncOpTracker.h"
#include "WONCommon/ByteBuffer.h"
#include "HTTPGetOp.h"
#include "HTTPEngine.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum HTTPDocType
{
	HTTPDocType_MOTD = 0,		// regular MOTD
	HTTPDocType_TOU  = 1,		// terms of use
	HTTPDocType_ADV  = 2,		// Ad Banner (Cross-promotion)
	HTTPDocType_Count
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum HTTPDocOwner
{
	HTTPDocOwner_Sys = 0,		// system document
	HTTPDocOwner_Game = 1,		// game specific document
	HTTPDocOwner_Count
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetHTTPDocumentOp : public AsyncOpWithTracker
{
private:
	HTTPDocType	mDocType;

	std::string mProduct;
	std::string mExtraConfig;
	HTTPSessionPtr mHTTPSession;

	bool		mDocumentIsNew[2];
	bool		mTriedDefault[2];
	WONStatus	mDocumentStatus[2];
	int			mHTTPStatus[2];
	time_t		mModifiedTime[2];
	std::string mDocumentPath[2];

	virtual void RunHook();
	virtual bool CallbackHook(AsyncOp *theOp, int theParam);
	void DoFinish();

	void OpFinished(HTTPGetOp *theOp, int theDocumentOwner);
	void TryHTTPOp(HTTPDocOwner theOwner, bool forceDefault = false);

private:
	static void CheckHostAndPath(HTTPDocType = HTTPDocType_MOTD);

public:
	GetHTTPDocumentOp(const std::string &theProduct, HTTPDocType theDocType);

	void SetDocType(HTTPDocType theDocType);
	HTTPDocType GetDocType() { return mDocType; }

	void SetExtraConfig(const std::string &theExtraConfig) { mExtraConfig = theExtraConfig; }

	void				SetDocumentPath(HTTPDocOwner theOwner, const std::string &thePath)	{ mDocumentPath[theOwner]  = thePath; }
	bool				DocumentIsNew(HTTPDocOwner theOwner) const  { return mDocumentIsNew[theOwner]; }
	const std::string&	GetDocumentPath(HTTPDocOwner theOwner)	const { return mDocumentPath[theOwner];  }
	time_t				GetModifiedTime(HTTPDocOwner theOwner) const { return mModifiedTime[theOwner]; }
	ByteBufferPtr		GetDocument(HTTPDocOwner theOwner) const;
	WONStatus			GetDocumentStatus(HTTPDocOwner theOwner) { return mDocumentStatus[theOwner]; }
	int					GetDocumentHTTPStatus(HTTPDocOwner theOwner) { return mHTTPStatus[theOwner]; }
	bool				DocumentIsValid(HTTPDocOwner theOwner);

	void SetHTTPSession(HTTPSession *theSession) { mHTTPSession = theSession; }
};

typedef SmartPtr<GetHTTPDocumentOp> GetHTTPDocumentOpPtr;

}; // namespace WONAPI


#endif
