/* The FS set of files is complicated.  It WAS a wrapper for the STDIO
// that also seemed to track everything.
// The fact that it's a DLL, filesystem_stdio_i386.so, complicates matters. As
// does its heavy use of classes.  But in reality, the biggest problem with
// recreating the file system lies in the different OSes out there.
//
// And it was fun and interesting recreating it, until I stumbled upon the Boost
// filesystem library which greatly cuts down on this hassle.  PAK files are
// still an issue, but right now most of this is a wrapper for Boost.
//
// HL definitely uses its own struct that includes a file * and sizing info.
// However, all code outside of the file handling functions (what FS points to)
// appear to leave the internals well alone.
*/

//GDB hates this.  It keeps making GDB crash.  But the program itself seems fine.

#include <iostream>
#include <vector>
#include <string>

#if(defined(_MSC_VER) || defined(__BORLANDC__))
 #include <winsock2.h>
 #include <windows.h>
 #ifndef PATH_MAX
  #define PATH_MAX MAX_PATH
 #endif
#endif

#include <ctype.h>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include "FS.hpp"

extern "C" {
#include "report.h"
#include "common.h"
}

/*** Globals ***/

/* Vectors.  When in C++... */


static std::vector <bfs::path> LoadedPaths;
static std::vector <std::string> LoadedPathNames;

int NumLoadedPaths = 0;
//unfortunately manipulating these is tricky, and just not very well designed...
int PositionOfRootDir = -1;
int PositionOfGameDir = -1;
int PositionOfBaseDir = -1;

//global hack used for FindFile system.  Is it really worth the effort to fix?
typedef class stupid_dir_FindStuff_s {

   private:
      int pathplace;
      bfs::directory_iterator * itr; //hackish but I can't figure out a way around it
      bfs::directory_iterator end_itr;

   public:
      stupid_dir_FindStuff_s();
      ~stupid_dir_FindStuff_s();
      int FS_FindFirst(const char *);
      int FS_FindNext();
      void FS_FindClose();

      bool IsActive();
      int SearchDir();
      int SearchDirs();
      int WildcardCompare();

      //Calling this 'cheap' in the middle of all this would be pedantic
      bfs::path SubDirs;
      bfs::path FileName;
      std::string FoundFileName;

} stupid_dir_FindStuff_t;

static stupid_dir_FindStuff_t FindFileClassThingy;



/*** Local functions ***/

void DebugPrintSearchPaths() {

   int i;

   std::cout << "DebugPrintSearchPaths:" << std::endl;

   for(i = 0; i < NumLoadedPaths; i++) {
      std::cout << "\"" << LoadedPaths[i].directory_string() << "\"--" << LoadedPathNames[i] << std::endl;
   }
   if(PositionOfRootDir != -1) {
      std::cout << "The root directory is \"" << LoadedPaths[PositionOfRootDir].directory_string() << "\"" << std::endl;
   }
   if(PositionOfGameDir != -1) {
      std::cout << "The game directory is \"" << LoadedPaths[PositionOfGameDir].directory_string() << "\"" << std::endl;
   }
   if(PositionOfBaseDir != -1) {
      std::cout << "The base directory is \"" << LoadedPaths[PositionOfBaseDir].directory_string() << "\"" << std::endl;
   }
}
inline void fsErrors(const char * function, bfs::filesystem_error &berrno) {

   std::cout << "Filesystem error caught in " << function << ": " << berrno.what() << std::endl;

   int err = berrno.system_error();

   if(err) {
      std::string s;
      bfs::system_message(err, s);
      std::cout << " System message: " << s << std::endl;
   }
}

/*** Exported functions ***/

//There is no cache system.  I noticed one fo the hints was when MD5ing a file
//that was later meant to be opened.  Still seems like a bad design though.
void FSMC_HintFileShouldStayInCache(void * arg0) {
   Sys_Warning("%s Passed variables:\n\n %X--%s", __FUNCTION__, arg0, arg0);
}
void FSMC_HintResourcesNeeded(void * arg0) {
   Sys_Warning("%s Passed variables:\n\n %X--%s", __FUNCTION__, arg0, arg0);
}
void FSMC_SetPreloadEnabled(int arg0) {
   if(arg0) {
      Sys_Warning("%s Passed variables:\n %X\n", __FUNCTION__, arg0);
   }
}

//The HL FS functions

/* Adds a search path to our static global vector whatever.  Must be
// a directory and must not already exist (equivalence in this case means
// the entries point to the same place AND have the same titles.  Though
// I've never seen a sufficiently complicated system, there's potential
// for a legitimate app to have the same entries with different titles
*/
void FS_AddSearchPath(const char * directory, char * title, int HasPriority) {

   int i;

   try {

      if(bfs::is_directory(directory) == 0) {
         //Bad user, giving us a bad path.
         std::cout << "FS_AddSearchPath: \"" << directory << "\" was not listed as a valid directory." << std::endl;

         return;
      }

      //we ought to see if it's already here...
      for(i = 0; i < NumLoadedPaths; i++) {
         if((equivalent(LoadedPaths[i], directory)) && (LoadedPathNames[i] == title)) {
            //This entry exists, don't add it twice.
            return;
         }
      }

      if(HasPriority) {
         LoadedPaths.insert(LoadedPaths.begin(), directory);
         LoadedPathNames.insert(LoadedPathNames.begin(), title);
         if(PositionOfRootDir != -1) { PositionOfRootDir++; }
         if(PositionOfGameDir != -1) { PositionOfGameDir++; }
         if(PositionOfBaseDir != -1) { PositionOfBaseDir++; }
      }
      else {
         LoadedPaths.push_back(directory);
         LoadedPathNames.push_back(title);
      }
      NumLoadedPaths++;

//      DebugPrintSearchPaths();
   }
   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
   }
   catch(...) { std::cout << "unhandled error in FS_AddSearchPath caught" << std::endl; }
}

void FS_Close(hl_file_t *stream) {

   stream->Close();
   delete stream;
}
// it isn't clear if this should handle multiple dirs on one path.
void FS_CreateDirHierarchy(char *arg0, char *arg4) {

   int IndexOfPathsToUseAsBase = -1;
   int i;

   if(arg4 != NULL) {
      for(i = 0; i < NumLoadedPaths; i++) {
         if(LoadedPathNames[i] == arg4) {
            //found it
            IndexOfPathsToUseAsBase = i;
            break;
         }
      }
   }
   if(IndexOfPathsToUseAsBase == -1) {
      if(PositionOfGameDir != -1) { IndexOfPathsToUseAsBase = PositionOfGameDir; }
      else if(PositionOfBaseDir != -1) { IndexOfPathsToUseAsBase = PositionOfBaseDir; }
      else if(PositionOfRootDir != -1) { IndexOfPathsToUseAsBase = PositionOfRootDir; }
      else { /*should never get here, but no way to signify error if we do*/ return; }
   }

   try {

      //This does NOT throw if the dir already exists, so it's safe to not check.
      bfs::create_directories(LoadedPaths[IndexOfPathsToUseAsBase] / arg0);

   }

   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
   }
}
int  FS_EndOfFile(hl_file_t *stream) {

   return(stream->IsEoF());
}
int  FS_FPrintf(hl_file_t *stream, const char *format, ...) {

   /* the quick hack way to do this would be to pass our arguments to vsprintf,
   // dump it on the stack, and then write our temp array to the file.
   // With the whole 'variable' aspect present, I don't believe there's any way
   // we could loop around vsprintf in order to write really large outputs
   // one temporary buffer at a time.

   // Alternatively we could do the outputting ourself and effectively remake
   // vsprintf with that objective in mind.  It'd basically be 'strchr %,
   // output all of the text before the % and after the last break, then output
   // our variable in string form, repeat'.  Wouldn't need a temp buffer any
   // larger than the largest number sprintf can output.  but that's a lot of
   // research and a lot of work for an almost never used function.
   */

   va_list argp;
   int length;
   char TempArray[1110]; //arbitrary size

    va_start(argp, format);
   length = Q_vsnprintf(TempArray, 1110, format, argp);
   va_end(argp);

   try {
      // 'Write' doesn't care about nulls.  There are no off by on errors here,
      // and the possible lack of a terminating null is irrelevant.
      return(stream->WriteFile(TempArray, length));
   }
   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
      return(-1);
   }
}


//These are called right before the FS_ equvalents and therefore make
//better hijacks.  FS equivs made into class.
void Sys_FindClose() {

   FindFileClassThingy.FS_FindClose();

}
const char * Sys_FindFirst(const char *inSearchString, char *outBaseFileName) {

  //arg4 is a pre-allocated character array we assume to be MAX_PATH long
   //if exists, we copy the file's name sans any path info in it.

   if(FindFileClassThingy.IsActive()) {
      Sys_Warning("Sys_FindFirst: Looks like you didn't close an earlier session.");
      Sys_FindClose();
   }

   if(FindFileClassThingy.FS_FindFirst(inSearchString)) {
      if(outBaseFileName != NULL) {

         Q_strncpy(outBaseFileName, FindFileClassThingy.FoundFileName.c_str(), PATH_MAX);
         return(outBaseFileName); //why waste it
      }
      return(FindFileClassThingy.FoundFileName.c_str());
   }
   else { return(NULL); }
}
const char * Sys_FindNext(char *outBaseFileName) {

   if(FindFileClassThingy.IsActive() == 0) {
      Sys_Warning("Sys_FindNext: Called without calling Sys_FindFirst.  Very bad.");
      return(NULL);
   }

   if(FindFileClassThingy.FS_FindNext()) {
      if(outBaseFileName != NULL) {

         Q_strncpy(outBaseFileName, FindFileClassThingy.FoundFileName.c_str(), PATH_MAX);
         return(outBaseFileName);
      }
      return(FindFileClassThingy.FoundFileName.c_str());
   }
   else { return(NULL); }
}
int  FS_FileExists(char *arg0) {

   int i = 0;

   while(i < NumLoadedPaths) {

      if(bfs::is_regular(LoadedPaths[i] / arg0)) { return(1); }
      i++;
   }
   return(1);
}
int  FS_FileSize(char *arg0) {

   int i = 0;
   bfs::path file;

   try {
      for(i = 0; i < NumLoadedPaths; i++) {

         file = (LoadedPaths[i] / arg0);
         if(bfs::exists(file)) {

            return(bfs::file_size(file));
         }
      }
   }
   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
   }
   //no idea what a proper fail value would be
   return(0);
}
int  FS_Flush(hl_file_t *stream) {

   Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, stream, stream);
   EXIT();

   return(stream->Sync());
}
int  FS_GetFileTime(char *arg0) { //passed string "maps/undertow.bsp"

   //what this really wants is st_mtime, which Boost can get.
   //Unfortunately last_write_time throws if the file is missing.

   int i = 0;
   bfs::path file;

   try {
      for(i = 0; i < NumLoadedPaths; i++) {

         file = (LoadedPaths[i] / arg0);
         if(bfs::exists(file)) {

            i = bfs::last_write_time(file);
            if(i == 0) { i++; }//possible, 0 means fail for HL
            return(i);
         }
      }
   }
   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
   }
   return(0);
}
int  FS_GetGameDir(char * out, int size) { //bad form, but necessary for now.

   int IndexOfPathsToUseAsBase;

   if(PositionOfGameDir != -1) { IndexOfPathsToUseAsBase = PositionOfGameDir; }
   else if(PositionOfBaseDir != -1) { IndexOfPathsToUseAsBase = PositionOfBaseDir; }
   else if(PositionOfRootDir != -1) { IndexOfPathsToUseAsBase = PositionOfRootDir; }
   else { Sys_Error("FS_GetGameDir: Called before paths were set up"); }

   Q_strncpy(out, LoadedPaths[IndexOfPathsToUseAsBase].string().c_str(), size-1); //path -> string -> char array
   out[size-1] = '\0';
   //fixme, extra strlen used.
   return(strlen(out));
}
void FS_GetInterfaceVersion(char *arg0, int arg4) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4); EXIT(); }
//This does jack for non-steam, but we still must hijack it since it normally points to a DLL.
void FS_GetLocalCopy(char *arg0) {

   //hack to circumvent global variable
   char * str = strstr(arg0, "(null)");
   if(str == NULL) { return; }
   str+=6;
   sprintf(arg0, ".%s", str);
}
// doing "maps *" will cause this to hit, but the arguments aren't quite known yet.
// Since the map searching is half-done, I'll fix it soon.
char * FS_GetLocalPath(char *arg0, char *arg4, int arg8) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n%X\n", __FUNCTION__, arg0, arg0, arg4, arg4, arg8); EXIT(); }
void * FS_GetReadBuffer(void *arg0, int *arg4) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4); EXIT(); }
int  FS_HintResourceNeed(char *arg0, int arg4) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4); EXIT(); }
int  FS_IsOk(hl_file_t *stream) {

   //Normally I figure the EOF counts as an error, but apparently not in the same manner as/
   //a CRC function keeps failing due to this.
   return(stream->Error() == 0); //if they are both zero, the statement is true (1)
}
int  FS_LoadLibrary(char *arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
//I assume this logs things.  I assume it's not critical then.
void FS_LogLevelLoadStarted(char *arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); }

/* DESCRIPTION: FS_Open/FS_OpenPathID
//
// These two functions are designed to open files that should be hidden
// somewhere in one of the file paths we have.  Many errors can occur in
// this situation, and since they must also search PAK files, it's not
// all a wrapper for boost FS.
//
// Returns a pointer to our file class on success, NULL on failure.
*/
hl_file_t * FS_Open(const char * filename, const char * mode) {

//is_regular(file_status s) is_regular(const Path& p);
   hl_file_t * file;

   /*in my libs, this is a long int, with no special value for 'all clear'.
   //Now granted since it's not specified, there may be an iostream
   //implementation SOMEWHERE that either doesn't use the bits of an int or
   //doesn't regard all zeros as being all clear.  But until I run up against
   //that library, the following brutal hack works.
   */
   std::ios_base::openmode rwa = (std::ios_base::openmode)0;


   if(strchr(mode, 'r') != NULL) {
      rwa |= std::ios::in;

      if(strchr(mode, '+') != NULL) {
         rwa |= std::ios::out;
      }
   }
   else if(strchr(mode, 'w') != NULL) {
      rwa |= std::ios::out | std::ios::trunc;

      if(strchr(mode, '+') != NULL) {
         rwa |= std::ios::in;
      }
   }
   else if(strchr(mode, 'a') != NULL) {
      rwa |= std::ios::in | std::ios::app;

      if(strchr(mode, '+') != NULL) {
         rwa |= std::ios::out;
      }
   }
   else {
      std::cout << "FS_Open: Invalid mode \"" << mode << "\"." << std::endl;
      return(NULL);
   }

   if(strchr(mode, 'b') != NULL) {
      rwa |= std::ios::binary;
   }

   try {

      int i;
      file = new hl_file_t();

      for(i = 0; i < NumLoadedPaths; i++) {
         if(file->OpenFileNoThrow(LoadedPaths[i] / filename, rwa)) { return(file); }
      }

      //if linux, check lowercase
      #ifndef _WIN32
       char lowercase[PATH_MAX+1];
       for(i = 0; filename[i] != '\0' && i < PATH_MAX; i++) { //a temp hack for a temp allowance
          lowercase[i] = tolower(filename[i]);
       }
       lowercase[i] = '\0';

       for(i = 0; i < NumLoadedPaths; i++) {
          if(file->OpenFileNoThrow(LoadedPaths[i] / lowercase, rwa)) {

             std::cout << "FS_Open: \"" << filename << "\" was found, but with incorrect (lower)case." << std::endl;
             return(file);
          }
       }
      #endif

      std::cout << "FS_Open: \"" << filename << "\" not found." << std::endl;
      delete file;
   }

   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
   }
   catch(...) { }

   return(NULL);
}
hl_file_t * FS_OpenPathID(const char * filename, const char * mode, const char * title) {

   hl_file_t * file;
   std::ios_base::openmode rwa = (std::ios_base::openmode)0;

   if(strchr(mode, 'r') != NULL) {
      rwa |= std::ios::in;

      if(strchr(mode, '+') != NULL) {
         rwa |= std::ios::out;
      }
   }
   else if(strchr(mode, 'w') != NULL) {
      rwa |= std::ios::out | std::ios::trunc;

      if(strchr(mode, '+') != NULL) {
         rwa |= std::ios::in;
      }
   }
   else if(strchr(mode, 'a') != NULL) {
      rwa |= std::ios::in | std::ios::app;

      if(strchr(mode, '+') != NULL) {
         rwa |= std::ios::out;
      }
   }
   else {
      std::cout << "FS_Open: Invalid mode \"" << mode << "\"." << std::endl;
      return(NULL);
   }

   if(strchr(mode, 'b') != NULL) {
      rwa |= std::ios::binary;
   }

   try {

      int i = 0;
      file = new hl_file_t();

      //Try opening the file.  make sure titles match.
      for(i = 0; i < NumLoadedPaths; i++) {
         if((LoadedPathNames[i] == title) && (file->OpenFileNoThrow(LoadedPaths[i] / filename, rwa))) { return(file); }
      }

      //if linux, lowercase and try again.
      char lowercase[PATH_MAX+1];
      for(i = 0; filename[i] != '\0' && i < PATH_MAX; i++) { //a temp hack for a temp allowance
         lowercase[i] = tolower(filename[i]);
      }
      lowercase[i] = '\0';

      for(i = 0; i < NumLoadedPaths; i++) {
         if((LoadedPathNames[i] == title) && (file->OpenFileNoThrow(LoadedPaths[i] / filename, rwa))) {

            std::cout << "FS_OpenPathID: \"" << filename << "\" was found, but with incorrect (lower)case." << std::endl;
            return(file);
         }
      }
      std::cout << "FS_OpenPathID: \"" << filename << "\" not found in \"" << title << "\"." << std::endl;
      delete file;
   }

   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
   }
   catch(...) { }

   return(NULL);
}
int FS_Read(void * ptr, size_t size, size_t count, hl_file_t *stream) {

   return(stream->ReadFile((char *)ptr, count*size));
}
void FS_ReleaseReadBuffer(void *arg0, void *arg4) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4); EXIT(); }
void FS_RemoveFile(char *arg0, char * arg4) {

   int IndexOfPathsToUseAsBase = -1;
   int i;

   if(arg4 != NULL) {
      for(i = 0; i < NumLoadedPaths; i++) {
         if(LoadedPathNames[i] == arg4) {
            //found it
            IndexOfPathsToUseAsBase = i;
            break;
         }
      }
   }
   if(IndexOfPathsToUseAsBase == -1) {
      if(PositionOfGameDir != -1) { IndexOfPathsToUseAsBase = PositionOfGameDir; }
      else if(PositionOfBaseDir != -1) { IndexOfPathsToUseAsBase = PositionOfBaseDir; }
      else if(PositionOfRootDir != -1) { IndexOfPathsToUseAsBase = PositionOfRootDir; }
      else { /*should never get here, but no way to signify error if we do*/ return; }
   }

   try {
      //This does NOT throw if the file doesn't exist
      bfs::remove(LoadedPaths[IndexOfPathsToUseAsBase] / arg0);
   }
   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
   }
}
int  FS_Rename(char *arg0, char *arg4) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4); EXIT(); }
int  FS_Seek(hl_file_t *stream, long int offset, int origin) {

   //convert between C SEEK_ and c++ std::ios::seekdir.
   //These are enumerated, and in fact I think the std
   //requires the two be equal, but I'm not banking on it.

   if(origin == SEEK_SET) { origin = std::ios_base::beg; }
   else if(origin == SEEK_CUR) { origin = std::ios_base::cur; }
   else if(origin == SEEK_END) { origin = std::ios_base::end; }
   else { Sys_Error("FS_Seek: Invalid SEEK position used."); }

   try {

      stream->Seekg(offset, (std::ios_base::seekdir)origin);
      stream->Seekp(offset, (std::ios_base::seekdir)origin); //This is hopefully the right thing to do.
   }
   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
      return(-1);
   }
   return(0);
}
int  FS_Size(hl_file_t *stream) {

   return(stream->GetFileSize());
}
int  FS_Tell(hl_file_t *stream) {

   int i;

   //Problem: We need to return the position indicator.  We have two...
   try {
      i = stream->Tellg();
      if(!i) { i = stream->Tellp(); }
   }
   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
      return(-1);
   }
   return(i);
}
int  FS_Unlink(char * arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
int  FS_Write(void * ptr, size_t size, size_t count, hl_file_t *stream) {

   return(stream->WriteFile((char *)ptr, count*size));
}
int  FileCopy(hl_file_t *arg0, hl_file_t *arg4, int arg8) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4, arg8, arg8); EXIT(); }
int  FileSize(void *arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }

/* Init and destroy, no surprises there.
// Init returns 1 on success.  It's suppossed to load that filesystem dll,
// but I'd rather it didn't.
// arg0 is the base directory of Half-Life ("." is what I see it as).
// arg4 appears to be a function pointer.  Some init thing for the library
// I'm sure, but we can ignore it.
*/
int FileSystem_Init(char * PassedDir) {

   const char *gamedir;
   const char *basedir;

   char passbasedir[PATH_MAX] = "";
   char passgamedir[PATH_MAX] = "";

   try {

      LoadedPaths.reserve(512);
      LoadedPathNames.reserve(196);

      FS_AddSearchPath(PassedDir, "ENGINE", 0);
      PositionOfRootDir = 0;

      gamedir = COM_GetParmAfterParmCase("-game");

      if(gamedir[0] != '\0') { //GetParm passes "" on failure.

         snprintf(passgamedir, PATH_MAX-1, "%s/%s", PassedDir, gamedir);
         passgamedir[PATH_MAX-1] = '\0';
         gamedir = passgamedir;
      }

      //else we pass null.  Big deal..

      //drunk test of the day: organize the if() brackets so that there aren't
      // duplicate checks or code.  I need more beerjuice to figure it out.
      basedir = COM_GetParmAfterParmCase("-basedir");
      if(basedir[0] == '\0') {
         basedir = COM_GetParmAfterParmCase("-defaultgamedir");
      }
      if(basedir[0] == '\0') {
         memcpy(passbasedir, "./valve", sizeof("./valve"));
         basedir = passbasedir;
      }
      else {
         snprintf(passbasedir, PATH_MAX-1, "%s/%s", PassedDir, basedir);
         passbasedir[PATH_MAX-1] = '\0';
         basedir = passbasedir;
      }

      FileSystem_SetGameDirectory(basedir, gamedir);
      return(1);
   }
   catch(bfs::filesystem_error &berrno) {
      fsErrors(__FUNCTION__, berrno);
      return(0);
   }
   catch(...) { return(0); }
}
void FileSystem_Shutdown() {

   LoadedPaths.clear();
   LoadedPathNames.clear();
}

//I'm pretty sure it's 'basedir' and 'gamedir'.  'basedir' is almost always 'valve'.
int  FileSystem_SetGameDirectory(const char * basedir, const char * gamedir) {

   if(basedir == NULL || basedir[0] == '\0') { return(0); }

   FS_AddSearchPath(basedir, "DEFAULTGAME", 1);
   PositionOfBaseDir = 0;

   if(gamedir != NULL && gamedir[0] != '\0' && bfs::equivalent(basedir, gamedir) == 0) {

      FS_AddSearchPath(gamedir, "GAME", 1);
      PositionOfGameDir = 0;
   }
   return(1);
}
int  FileSystem_LoadDLL__FPFPCcPi_P14IBaseInterface(void * arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
void FileSystem_UnloadDLL__Fv() { Sys_Warning("%s \n\n", __FUNCTION__); EXIT(); }

void FSMC_FlushAccessedFiles(void) { Sys_Warning("%s \n\n", __FUNCTION__); EXIT(); }
void FSMC_FlushAllFiles(void) { Sys_Warning("%s \n\n", __FUNCTION__); EXIT(); }
void FSMC_HintFileNeeded(void * arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
int  FSMC_IsFileInCache(void * arg0, void * arg4) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4);  EXIT(); }
void FSMC_PausePreloadThread(void) { Sys_Warning("%s \n\n", __FUNCTION__); EXIT(); }
void FSMC_ReportUsage(void) { Sys_Warning("%s \n\n", __FUNCTION__); EXIT(); }
void FSMC_ResumePreloadThread(void) { Sys_Warning("%s \n\n", __FUNCTION__); EXIT(); }

void FS_CancelWaitForResources(hl_file_t * arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
int  FS_FileExist(char *arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
void FS_FileTimeToString(char *arg0, int arg4, long int arg8) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4, arg8, arg8); EXIT(); }
int  FS_FindIsDirectory(void * arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
int  FS_FullPathToRelativePath(char * arg0, char * arg4) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4); EXIT(); }
int  FS_GetCharacter(hl_file_t *arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
int  FS_GetCurrentDirectory(char *arg0, int arg4) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4); EXIT(); }
int  FS_GetWaitForResourcesProgress(int arg0, float * arg4, int arg8) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4, arg8, arg8); EXIT(); }
int  FS_IsDirectory(char * arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
void FS_LogLevelLoadFinished(char *arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
int  FS_ParseFile(char *arg0, char *arg4, int arg8) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4, arg8, arg8); EXIT(); }
int  FS_PauseResourcePreloading() { Sys_Warning("%s \n\n", __FUNCTION__); EXIT(); }
void FS_PrintOpenedFiles() { Sys_Warning("%s \n\n", __FUNCTION__); EXIT(); }
char * FS_ReadLine(char *arg0, int arg4, hl_file_t *arg8) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4, arg8, arg8); EXIT(); }
int  FS_RemoveSearchPath(char *arg0, char *arg4) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4); Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4); EXIT(); }
void FS_RemoveAllSearchPaths() { Sys_Warning("%s \n\n", __FUNCTION__); EXIT(); }
int  FS_ResumeResourcePreloading() { Sys_Warning("%s \n\n", __FUNCTION__); EXIT(); }
int  FS_SetVBuf(void *arg0, char *arg4, int arg8, long argC) { Sys_Warning("%s Passed variables:\n\n %X %s\n%X--%s\n%X--%s\n%X--%s\n", __FUNCTION__, arg0, arg0, arg4, arg4, arg8, arg8, argC, argC); EXIT(); }
void FS_SetWarningFunc(void * arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
void FS_SetWarningLevel(void * arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }
int  FS_WaitForResources(char * arg0) { Sys_Warning("%s Passed variables:\n\n %X--%s\n", __FUNCTION__, arg0, arg0); EXIT(); }

int CompareStringsWild(std::string filenamestring, std::string matchstring) {

   int i = 0, j = 0;

   while(matchstring[i] != '\0') {

      if(matchstring[i] == '*') {

         i++;
         while(matchstring[i] == '*' || matchstring[i] == '?') { i++; }
         if(matchstring[i] == '\0') { return(1); }

         while(1) {

            while(matchstring[i] != filenamestring[j] && filenamestring[j] != '\0') { j++; }

            if(filenamestring[j] == '\0') { return(0); }

            std::string temp1 = filenamestring.substr(j);
            std::string temp2 = matchstring.substr(i);
            if(CompareStringsWild(temp1, temp2) == 1) { return(1); }
            j++;
         }
      }
      else if(matchstring[i] == '?') {
         if(filenamestring[j] == '\0') { return(0); }
         i++;
         j++;
      }
      else if(matchstring[i] == filenamestring[j]) {
         i++;
         j++;
      }
      else { return(0); }
   }
   if(filenamestring[j] == '\0') { return(1); }
   return(0);

}


/*** the findfile 'class' ***/

/* To paraphrase the MSDN FindFile crap:
//
// Sys_FindX functions are basically wrappers for the FS_FindX functions.
// That's how HL does it.  HL also uses a static variable; there are WAY
// too many problems here related to race conditions, possible threading,
// order, etc. that I'm just going to have to say 'to hell with it' until the
// structure can be redone.  Just don't go calling lots of other functions in
// between calls to the FS sysetm.
*/


stupid_dir_FindStuff_s::stupid_dir_FindStuff_s() {
   pathplace = -1;
}
stupid_dir_FindStuff_s::~stupid_dir_FindStuff_s() {
   if(pathplace != -1) { FS_FindClose(); }
}

int stupid_dir_FindStuff_s::SearchDir() {

   while(*itr != end_itr) {
      if(WildcardCompare()) {

         FoundFileName = (*itr)->path().leaf();
         (*itr)++;
         return(1);
      }
      (*itr)++;
   }

   return(0);
}
int stupid_dir_FindStuff_s::SearchDirs() {

   bfs::system_error_type ec;
   while(1) {

      if(SearchDir() == 0) {
         pathplace++;
         if(pathplace >= NumLoadedPaths) {
            return(0);
         }
         else {
            delete itr;
            //Any errors will almost certainly be 'dirdoesn'texist'.  We don't care, throwing seems excessive, use nothrow version.
            itr = new bfs::directory_iterator(LoadedPaths[pathplace] / SubDirs, ec);
         }
      }
      else { return(1); }
   }
}
int stupid_dir_FindStuff_s::WildcardCompare() {

   return(CompareStringsWild((*itr)->path().leaf(), FileName.string()));
}

// This is passed the file name to search for (? and * wildcards dealt with)
// The argument is split up if needed into two other vars in case a directory
// is passed as part of the file name.  Root paths are not accounted for and
// should not be used.
int stupid_dir_FindStuff_s::FS_FindFirst(const char *arg0) {


   pathplace = 0;
   if(NumLoadedPaths <= pathplace) { return(0); }

   FileName = arg0;
   if(FileName.has_branch_path()) {
      SubDirs = FileName.branch_path();
      FileName = FileName.leaf();
   }

   //I tried to find a better way, some way to reuse iterators or
   //use the directory_entry class, but it was not to be.
   itr = new bfs::directory_iterator(LoadedPaths[pathplace] / SubDirs);

   return(SearchDirs());
}
int stupid_dir_FindStuff_s::FS_FindNext() {

   return(SearchDirs());
}
void stupid_dir_FindStuff_s::FS_FindClose() {

   delete itr;
   pathplace = -1;
}
bool stupid_dir_FindStuff_s::IsActive() {

   return(pathplace != -1);
}






/*** The file class ***/

hl_file_s::hl_file_s() {

   identifier = HL_FILE_UNASSIGNED;
}
hl_file_s::~hl_file_s() {

   switch(identifier) {
   case HL_FILE_UNASSIGNED:

      break; //no files to destroy...
   case HL_FILE_REGULAR: {
      Close();
      std::cout << "hl_file_s destructor: Object destroyed with open file.  Don't be lazy, programmer." << std::endl;
      break;
   }
   default:
      std::cout << "hl_file_s destructor: unhandled 'identifier'.  Bad.  Very bad." << std::endl;
   }
}



void hl_file_s::OpenFile(const bfs::path &filename, std::ios_base::openmode rwb) {

   if(identifier != HL_FILE_UNASSIGNED) { throw(99); }

   file.open(filename, rwb);
   if(file.is_open() == 0) { throw(98); }
   identifier = HL_FILE_REGULAR;

}
int  hl_file_s::OpenFileNoThrow(const bfs::path &filename, std::ios_base::openmode rwb) {

   if(identifier == HL_FILE_UNASSIGNED) {

      file.open(filename, rwb);
      if(file.is_open()) {
         identifier = HL_FILE_REGULAR;
         return(1);
      }
   }
   return(0);
}
void hl_file_s::Close() {

   if(identifier == HL_FILE_REGULAR) {

      file.close();
      identifier = HL_FILE_UNASSIGNED;
   }
   else {
      Sys_Error("hl_file_s::GetFileSize: passed invalid hl_file_t structure.");
   }
}
int  hl_file_s::GetFileSize() {

    std::streampos begin, end, old; //always an int of SOME kind, yes?

   if(identifier == HL_FILE_REGULAR) {

      old = file.pubseekoff(0, std::ios_base::cur, std::ios_base::in);
      begin = file.pubseekoff(0, std::ios_base::beg, std::ios_base::in);
      end = file.pubseekoff(0, std::ios_base::end, std::ios_base::in);
      file.pubseekpos(old, std::ios_base::in);

      return(end-begin);
   }
   else {
      Sys_Error("hl_file_s::GetFileSize: passed invalid hl_file_t structure.");
   }
}
int  hl_file_s::ReadFile(char * BufferToReadInto, std::streamsize SizeofBuffer) {

   if(identifier == HL_FILE_REGULAR) {

      return(file.sgetn(BufferToReadInto, SizeofBuffer));
   }
   else {
      Sys_Error("hl_file_s::ReadFile: passed invalid hl_file_t structure.");
   }
}
int  hl_file_s::Seekg(long int arg4, std::ios_base::seekdir arg8) {

   if(identifier == HL_FILE_REGULAR) {

      return(file.pubseekoff(arg4, arg8, std::ios_base::in));
   }
   else {
      Sys_Error("hl_file_s::Seekg: passed invalid hl_file_t structure.");
   }
}
int  hl_file_s::Seekp(long int arg4, std::ios_base::seekdir arg8) {

   if(identifier == HL_FILE_REGULAR) {

      return(file.pubseekoff(arg4, arg8, std::ios_base::out));
   }
   else {
      Sys_Error("hl_file_s::Seekp: passed invalid hl_file_t structure.");
   }
}
int  hl_file_s::Tellg() {

   if(identifier == HL_FILE_REGULAR) {

      return(file.pubseekoff(0, std::ios_base::cur, std::ios_base::in));
   }
   else {
      Sys_Error("hl_file_s::Tellg: passed invalid hl_file_t structure.");
   }
}
int  hl_file_s::Tellp() {

   if(identifier == HL_FILE_REGULAR) {

      return(file.pubseekoff(0, std::ios_base::cur, std::ios_base::in));
   }
   else {
      Sys_Error("hl_file_s::p: passed invalid hl_file_t structure.");
   }
}
int  hl_file_s::Sync() {

   if(identifier == HL_FILE_REGULAR) {

      return(file.pubsync());
   }
   else {
      Sys_Error("hl_file_s::p: passed invalid hl_file_t structure.");
   }
}
int  hl_file_s::IsEoF() {

   if(identifier == HL_FILE_REGULAR) {

      int i = file.sgetc();
      if(i == EOF) { return(1); }
      return(0);
   }
   else {
      Sys_Error("hl_file_s::p: passed invalid hl_file_t structure.");
   }
}
int  hl_file_s::Error() {

   //until I can figure out what filebuf does in case of error...
   return(0);
}
int  hl_file_s::WriteFile(char * BufferToWriteFrom, std::streamsize SizeofBuffer) {

   if(identifier == HL_FILE_REGULAR) {

      return(file.sputn(BufferToWriteFrom, SizeofBuffer));
   }
   else {
      Sys_Error("hl_file_s::ReadFile: passed invalid hl_file_t structure.");
   }
}
