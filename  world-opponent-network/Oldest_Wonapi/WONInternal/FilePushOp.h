#ifndef __WON_FILE_PUSH_OP_H__
#define __WON_FILE_PUSH_OP_H__

#include "WONServer/ServerOp.h"

namespace WONAPI
{

class FilePushOp : public ServerOp
{
public:
	FilePushOp(const IPAddr &theAddr);
	virtual ~FilePushOp(void);

	void SetFile(const std::string& theFileName) { mFileName = theFileName; }
	void SetRemoteFile(const std::string& theRemoteFileName) { mRemoteFileName = theRemoteFileName; }
	void SetCompress(bool isCompress) { mIsCompress = isCompress; }
	void SetModifyTime(FILETIME theModifyTime) { mModifyTime = theModifyTime; }
	void SetProgressCompletion(OpCompletionBasePtr theCompletion) { mProgressCompletion = theCompletion; }
	void SetBlockTransferSize(DWORD theSize) { mBlockTransferSize = theSize; }
	void SetComment(const std::string& theComment) { mComment = theComment; }

	DWORD GetTotalBytesSent() const { return mTotalBytesSent; }
	DWORD GetLastBytesSent() const { return mLastBytesSent; }
private:
	class AutoFile
	{
	private:
		FILE *mFileP;
		
	public:
		AutoFile() : mFileP(NULL) { }
		~AutoFile() { Detach(); }

		void Attach(FILE *theFileP) { Detach(); mFileP = theFileP; }
		void Detach() { if(mFileP) fclose(mFileP); mFileP = NULL; }
		FILE* GetFile() { return mFileP; }
	};

	AutoFile mFile;
	IPAddr mConnectAddr;

protected:
	std::string         mFileName;
	std::string         mRemoteFileName;
	bool                mIsCompress;
	FILETIME            mModifyTime;
	OpCompletionBasePtr mProgressCompletion;
	DWORD				mTotalBytesSent;
	DWORD				mLastBytesSent;
	Blowfish			mEncryptKey;
	DWORD				mBlockTransferSize;
	std::string			mComment;

	enum TrackTypes
	{
		TRACK_AUTH				= 1,
		TRACK_STREAM_REQUEST	= 2,
		TRACK_STATUS_REPLY		= 3,
		TRACK_ACK				= 4
	};

	virtual bool CallbackHook(AsyncOp *theOp, int theId);

	bool CheckError(WONStatus theStatus);
	bool GetEncryptKey(PeerAuthOp *theOp);
	void DoProgressCompletion();

	bool HandleFactStatusReply(const ByteBuffer *theReply);
	bool RecvFactStatusReply();

	bool HandleAck(char theAck);
	bool RecvAck();

	bool SendFileChunk();

	bool SendStreamFileRequest();
	
	virtual void RunHook(void);
	virtual void CleanupHook(void);

};

typedef SmartPtr<FilePushOp> FilePushOpPtr;

}; // namespace WONAPI

#endif
