#ifndef __WON_HTTPGETOP_H__
#define __WON_HTTPGETOP_H__
#include "WONShared.h"

#include "WONCommon/AsyncOpTracker.h"
#include "WONSocket/BlockingSocket.h"
#include "HTTPCache.h"


namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class HTTPGetOp : public AsyncOpWithTracker
{
private:
	BlockingSocketPtr mSocket;
	HTTPCachePtr mHTTPCache;
	HTTPCacheEntryPtr mHTTPCacheEntry;

	std::string mHost;
	std::string mRemotePath;
	std::string mLocalPath;
	bool mIfRange;
	bool mContentIsChunked;
	bool mIsFirstChunk;
	bool mBinaryFile;

	bool mCloseConnection;
	bool mDoClose; // set to true if either mCloseConnection is true of if server specifies to close
	bool mDoRangeResume;

	std::string mProxyHost;

	std::string mRedirectHost;
	std::string mRedirectPath;

	time_t mServerTime;
	time_t mExpireTime;
	time_t mLastModified;
	time_t mLocalFileModifyTime;
	size_t mLocalFileSize;
	std::string mEntityTag;
	std::string mContentType;

	DWORD mContentLength;
	DWORD mChunkLength;
	ByteBufferPtr mContent;
	WriteBuffer mContentAccumulator;

	int mHTTPStatus;
	bool mContentIsNew;

	OpCompletionBasePtr mRecvChunkCompletion;
	DWORD mRecvChunkSize;

	bool mOnlySend;
	bool mOnlyRecv;

	bool mAutoRedirect;
	int mNumRedirects;

	static std::string mStaticProxyHost;
	static std::string mProxyHostFile;

private:
	bool ExtractStatus(const char *theStatusLine);
	bool ExtractHeaderLine(const char *theHeaderLine);
	bool ExtractRedirect(const char *theLocation);
	bool ExtractChunkLength(const char *theLine);

	void FinishRedirect(HTTPGetOp *theOp);
	void FollowRedirect();
	void DoneRecvHeader();
	bool DoneRecvContent(RecvBytesOp *theOp);
	
	enum TrackType
	{
		HTTP_Track_Connect = 1,
		HTTP_Track_RecvStatus = 2,
		HTTP_Track_RecvHeader = 3,
		HTTP_Track_RecvContent = 4,
		HTTP_Track_Redirect = 5,
		HTTP_Track_RecvChunkLength = 6,
		HTTP_Track_RecvChunkCRLF = 7, // CRLF after chunk data!
		HTTP_Track_RecvFooter = 8,
		HTTP_Track_SendRequest = 9

	};
	bool DoConnect();
	bool SendRequest();
	void AsyncRecvCRMsg(TrackType theType);
	void RecvContent();
	bool RecvChunkLength();
	void DoFinish();

protected:
	virtual void RunHook();
	virtual void CleanupHook();
	virtual bool CallbackHook(AsyncOp *theOp, int theParam);
		
	bool CheckStatus(WONStatus theStatus);

public:
	HTTPGetOp(const std::string &theHost, const std::string &theRemotePath);
	HTTPGetOp(const std::string &theURL);

	const std::string& GetHost() { return mHost; }
	const std::string& GetRedirectHost() { return mRedirectHost; }
	const std::string& GetRedirectPath() { return mRedirectPath; }

	const std::string& GetEndingHost() { return mRedirectHost.empty()?mHost:mRedirectHost; }
	const std::string& GetEndingPath() { return mRedirectPath.empty()?mRemotePath:mRedirectPath; }

	void SetLocalPath(const std::string &thePath) { mLocalPath = thePath; }
	const std::string& GetLocalPath() { return mLocalPath; }
	void SetHTTPCache(HTTPCache *theCache) { mHTTPCache = theCache; }

	void SetBinaryFile(bool isBinary) { mBinaryFile = isBinary; }

	void SetRemotePath(const std::string &thePath) { mRemotePath = thePath; }
	const std::string& GetRemotePath() { return mRemotePath; }

	void SetProxy(const std::string &theHost, unsigned short thePort);
	void SetProxy(const std::string &theHostAndPort);

	void SetNumRedirects(int theNum) { mNumRedirects = theNum; }
	int GetNumRedirects() { return mNumRedirects; }
	void SetAutoRedirect(bool doAutoRedirect) { mAutoRedirect = doAutoRedirect; }
	void SetOnlySend() { mOnlySend = true; mOnlyRecv = false; }
	void SetOnlyRecv() { mOnlyRecv = true; mOnlySend = false; }
	void SetSendAndRecv() { mOnlySend = false; mOnlyRecv = false; }


	void SetRecvChunkCompletion(OpCompletionBase *theCompletion, DWORD theChunkSize = 10000);

	time_t GetLastModifiedTime();
	const std::string& GetContentType() { return mContentType; }

	ByteBufferPtr GetContent() { return mContent; }

	int GetHTTPStatus() { return mHTTPStatus; }
	bool ContentIsNew() { return mContentIsNew; }

	void SetDoRangeResume(bool doRangeResume) { mDoRangeResume = doRangeResume; }
	void SetCloseConnection(bool closeConnection) { mCloseConnection = closeConnection; }
	void SetSocket(BlockingSocket *theSocket);
	BlockingSocket* GetSocket();

	static void ReadProxyHostFile(bool force = true);	// read static proxy info from file
	static bool WriteProxyHostFile(const std::string &theHostAndPort);	// write proxy info to file
	static const std::string& GetStaticProxyHost();
	static void SetProxyHostFileName(std::string &theFileName); // set file name and path for proxy info file
};

typedef SmartPtr<HTTPGetOp> HTTPGetOpPtr;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


}; // namespace WONAPI

#endif
