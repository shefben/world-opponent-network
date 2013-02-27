#ifndef HLDS_LINUX_FS_REMAKE
#define HLDS_LINUX_FS_REMAKE

#include "Q_memdef.h"
#include "endianin.h"
#include <stdarg.h>
//This particular chunk of the code--file IO--requires C++ libs.  Therefore
//the external functions that we hijack must be left un-mangled with extern "C".

//These defines are arbitrary, but if you end up dereferencing a file pointer
//in a debugger, they read 'file' 'none', and 'pak '
#define HL_FILE_UNASSIGNED       0x656e6F6e
#define HL_FILE_REGULAR          0x656c6966
//#define HL_FILE_WRITEONLY        0x
//#define HL_FILE_READWRITE        0xEDFA0500
#define HL_FILE_ARCHIVEREADONLY  0x206b6170

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
typedef class hl_pak_file_s {

private:
   int identifier;

public:
   hl_pak_file_s();
   ~hl_pak_file_s();
} hl_pak_file_t;

typedef class hl_file_s {

   int identifier;

   //a little waste, but oh well, deal with it later.
   boost::filesystem::filebuf file;
 //  class hl_pak_file_s embeddedPAK;

public:
   hl_file_s();
   ~hl_file_s();
   void OpenFile(const boost::filesystem::path &, std::ios_base::openmode);
   int OpenFileNoThrow(const boost::filesystem::path &, std::ios_base::openmode);
   void Close();
   int GetFileSize();
   int ReadFile(char *, std::streamsize);
   int WriteFile(char *, std::streamsize);
   int Seekg(long int, std::ios_base::seekdir);
   int Seekp(long int, std::ios_base::seekdir);
   int Tellg();
   int Tellp();
   int Sync();
   int IsEoF();
   int Error();
} hl_file_t;
#else

//These exist so that C functions don't have to resort to passing void *s to objects.
//Them adding, subtracting, or generally messing with the data in any way is a bad idea.

//For some reason one of my GCCs chokes on this.  And by 'this' I mean any typedef struct
//at this particular location.  Why this location, I do not know.  Preprocessor bug?
//Must be; it works on newer versions.
typedef struct hl_pak_file_s hl_pak_file_t;

typedef struct hl_file_s hl_file_t;
#endif

//The mostly unused FSMC functions

void FSMC_HintFileShouldStayInCache(void * arg0);
void FSMC_HintResourcesNeeded(void * arg0);
void FSMC_SetPreloadEnabled(int arg0); //bool methinks

//The HL FS functions
void FS_AddSearchPath(const char *arg0, char *arg4, int arg8);
void FS_Close(hl_file_t *arg0);
void FS_CreateDirHierarchy(char *arg0, char *arg4);
int  FS_EndOfFile(hl_file_t *arg0);
int  FS_FPrintf(hl_file_t *arg0, const char *arg4, ...);
int  FS_FileExists(char *arg0);
int  FS_FileSize(char *arg0); //not in 386
void Sys_FindClose();
const char * Sys_FindFirst(const char *arg0, char *arg4); //or is it two char*?
const char * Sys_FindNext(char *arg0);
int  FS_Flush(hl_file_t *arg0);
int  FS_GetFileTime(char *arg0);
int  FS_GetGameDir(char *, int);
void FS_GetInterfaceVersion(char *arg0, int arg4);
void FS_GetLocalCopy(char *arg0);
char * FS_GetLocalPath(char *arg0, char *arg4, int arg8);
void * FS_GetReadBuffer(void *arg0, int *arg4);
int  FS_HintResourceNeed(char *arg0, int arg4);
int  FS_IsOk(hl_file_t *arg0);
int  FS_LoadLibrary(char *arg0);
void FS_LogLevelLoadStarted(char *arg0);
hl_file_t * FS_Open(const char *arg0, const char *arg4);
hl_file_t * FS_OpenPathID(const char *arg0, const char *arg4, const char *arg8);
int  FS_Read(void *arg0, size_t arg4, size_t arg8, hl_file_t *argC);
void FS_ReleaseReadBuffer(void *arg0, void *arg4);
void FS_RemoveFile(char *arg0);
int  FS_Rename(char *arg0, char *arg4);
int  FS_Seek(hl_file_t *arg0, long int arg4, int arg8);
int  FS_Size(hl_file_t *arg0);
int  FS_Tell(hl_file_t *arg0);
int  FS_Unlink(char * arg0);
int  FS_Write(void *arg0, size_t arg4, size_t arg8, hl_file_t *argC);
int  FileCopy(hl_file_t *arg0, hl_file_t *arg4, int arg8);
int  FileSize(void *arg0);
int  FileSystem_Init(char * arg0);
int  FileSystem_SetGameDirectory(const char * arg0, const char * arg4);
void FileSystem_Shutdown();
int  FileSystem_LoadDLL__FPFPCcPi_P14IBaseInterface(void * function0);
void FileSystem_UnloadDLL__Fv();



//The functions that are empty or unused in the Linux stdio binary
void FSMC_FlushAccessedFiles(void);
void FSMC_FlushAllFiles(void);
void FSMC_HintFileNeeded(void * arg0);
int  FSMC_IsFileInCache(void * arg0, void * arg4); //unused
void FSMC_PausePreloadThread(void);
void FSMC_ReportUsage(void);
void FSMC_ResumePreloadThread(void);
void FS_CancelWaitForResources(hl_file_t *arg0);
void FS_FileTimeToString(char *arg0, int arg4, long int arg8);
int  FS_FindIsDirectory(void * arg0);
int  FS_FullPathToRelativePath(char * arg0, char * arg4);
int  FS_GetCharacter(hl_file_t *arg0);
int  FS_GetCurrentDirectory(char *arg0, int arg4);
int  FS_GetWaitForResourcesProgress(int arg0, float * arg4, int arg8);
int  FS_IsDirectory(char * arg0);
void FS_LogLevelLoadFinished(char *arg0);
int  FS_ParseFile(char *arg0, char *arg4, int arg8);
int  FS_PauseResourcePreloading();
void FS_PrintOpenedFiles();
char * FS_ReadLine(char *arg0, int arg4, hl_file_t *arg8);
int  FS_RemoveSearchPath(char *arg0, char *arg4);
void FS_RemoveAllSearchPaths();
int  FS_ResumeResourcePreloading();
int  FS_SetVBuf(void *arg0, char *arg4, int arg8, long argC);
void FS_SetWarningFunc(void * arg0);
void FS_SetWarningLevel(void * arg0);
int  FS_WaitForResources(char * arg0);



#ifdef __cplusplus
}
#endif

#endif
