#ifndef __WON_SENDFILEOP_H__
#define __WON_SENDFILEOP_H__
#include "WONShared.h"
#include "SocketOp.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SendFileOp : public SocketOp
{
protected:
	FILE *mFile;
	std::string mFilePath;

	enum { BUF_SIZE = 8192 };
	char mBuf[BUF_SIZE];
	ReadBuffer mBytes;

	void CloseFile();
	bool ReadFromFile();

	virtual WONStatus Start();
	virtual WONStatus Continue();
	virtual void CleanupHook();

public:
	SendFileOp(const std::string &theFilePath, AsyncSocket *theSocket = NULL);
};
typedef SmartPtr<SendFileOp> SendFileOpPtr;

} // namespace WONAPI

#endif

