
#pragma warning (disable : 4786)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <memory>
#include <string>
#include <io.h>
#include <sys/stat.h>

using namespace std;

bool ExportDLL(const char* theDLLFile);
int FindNoCase(const string& theStrToSearch, const char* theSearchStr, int theStartPos = 0);

static int gCommandsCount = 34;
int gProductIndex = -1;


static char* gCommandsP[34] = 
{
	"ConfigFile",
	"WorkingFolder",
	"ProductName",
	"DisplayName",
	"Version",
	"PatchFolder",
	"PatchTypes",
	"DirectoryServer",
	"WelcomeDlgHelpFile",
	"VersionCheckDlgHelpFile",
	"OptionalPatchDlgHelpFile",
	"SelectDlgHelpFile",
	"DownloadDlgHelpFile",
	"ProxyDlgHelpFile",
	"VisitHostDlgHelpFile",
	"HelpExe",
	"ExtraConfig",
	"AutoStart",
	"MonitorPatch",
	"PatchSucceededValue",
	"VersionExe",
	"LaunchExe",
	"LaunchParams",
	"MotdTimeout",
	"VersionTimeout",
	"PatchTimeout",
	"PatchDescriptionTimeout",
	"ValidateVersionFile",
	"NoPatchURL",
	"UpdateVersion",
	"ResourceDirectory",
	"Resizable",
	"StartWidth",
	"StartHeight"
};

static int gRemoveCommandsCount = 6;
static char* gRemoveCommandsP[6] = 
{
	"CurrentVersion",
	"CustomResourceFile",
	"LanguageResourceFile",
	"ResourceDllFile",
	"LanguageDllFile",
	"Debug"
};

static int gProductCount = 8;
static char* gProductP[8][3] = 
{
	{"Emperor",			"",				"",},
	{"NR2002",			"",				"PapyRes.dll"},
	{"EEAOC",			"",				"EEAOCSkin.dll"},
	{"EmpireEarth",		"zVersion.cfg",	"EESkin.dll"},
	{"HoyleBoard2003",	"",				""},
	{"HoyleCard2003",	"",				""},
	{"HoyleCasino2003", "",				""},
	{"HoylePuzzle2003"	"",				""}
};

int main(int argc, char** argv)
{
	_chmod("SierraUp.cfg", _S_IREAD | _S_IWRITE);
	FILE* aReadFileP = fopen("SierraUp.cfg", "rt");
	if(aReadFileP == NULL)
		return -1;

	_chmod("SierraUpNew.cfg", _S_IREAD | _S_IWRITE);
	remove("SierraUpNew.cfg");
	FILE* aWriteFileP = fopen("SierraUpNew.cfg", "wt+");
	if(aWriteFileP == NULL)
	{
		fclose(aReadFileP);
		return -2;
	}

	bool isResourceDirectory = false;
	gProductIndex = -1;
	string aFinalStr, aLanguageStr;
	char aBuffer[2048];
	while(fgets(aBuffer, 2048, aReadFileP))
	{
		aFinalStr = aBuffer;
		if(!((aBuffer[0] == '/' && aBuffer[1] == '/') || aBuffer[0] == ';'))
		{
			for(string::iterator anItr = aFinalStr.begin(); anItr != aFinalStr.end();)
			{
				if(isspace(*anItr))
					anItr = aFinalStr.erase(anItr);
				else
					break;
			}

			// Get the product index if this is the ProductName configuration line
			if(FindNoCase(aFinalStr, "ProductName") == 0)
			{
				for(int i = 0; i < gProductCount; i++)
				{
					if(FindNoCase(aFinalStr, gProductP[i][0]) != -1)
					{
						gProductIndex = i;
						break;
					}
				}
			}

			// Get the language if this is the ExtraConfig configuration line
			if(FindNoCase(aFinalStr, "ExtraConfig") == 0)
			{
				aLanguageStr = aFinalStr.substr(11);
				for(anItr = aLanguageStr.begin(); anItr != aLanguageStr.end();)
				{
					if(isspace(*anItr))
						anItr = aLanguageStr.erase(anItr);
					else
						++anItr;
				}
				if(aLanguageStr.at(0) == ':')
					aLanguageStr = aLanguageStr.substr(1);

				if(aLanguageStr.at(0) == '"')
					aLanguageStr = aLanguageStr.substr(1);

				aLanguageStr = aLanguageStr.substr(0,2);
			}

			// Set the Resource Directory to SierraUpRes
			if(FindNoCase(aFinalStr, "ResourceDirectory") == 0)
			{
				aFinalStr = "ResourceDirectory: SierraUpRes\n";
				isResourceDirectory = true;
			}

			// Change CurrentVersion to Version
			if(FindNoCase(aFinalStr, "CurrentVersion") == 0)
				aFinalStr = aFinalStr.substr(7);

			// Remove old commands that are no longer supported
			bool isDone = false;
			for(int i = 0; i < gRemoveCommandsCount; i++)
			{
				if(FindNoCase(aFinalStr, gRemoveCommandsP[i]) == 0)
				{
					aFinalStr.insert(0, "//");
					isDone = true;
					break;
				}
			}

			if(!isDone)
			{
				// Append the colon to a command (if needed)
				for(int i = 0; i < gCommandsCount; i++)
				{
					if(FindNoCase(aFinalStr, gCommandsP[i]) == 0)
					{
						if(aFinalStr.at(strlen(gCommandsP[i])) != ':')
							aFinalStr.insert(strlen(gCommandsP[i]), ":");

						break;
					}
				}
			}
		}

		if(aFinalStr.empty() || aFinalStr[aFinalStr.length()-1] != '\n')
			fprintf(aWriteFileP, "%s\n", aFinalStr.c_str());
		else
			fprintf(aWriteFileP, "%s", aFinalStr.c_str());
	}

	if(aLanguageStr.empty())
	{
		fprintf(aWriteFileP, "ExtraConfig: EN\n");
		aLanguageStr = "EN";
	}

	if(!isResourceDirectory)
		fprintf(aWriteFileP, "ResourceDirectory: SierraUpRes\n");

	fclose(aReadFileP);
	fclose(aWriteFileP);

	// Special code for EmpireEarth to deal with zVersion.cfg file
	if(gProductIndex==3)
	{
		_chmod(gProductP[3][1], _S_IREAD | _S_IWRITE);
		FILE* anOldVersionFileP = fopen(gProductP[3][1], "rt");
		if(!anOldVersionFileP)
			return -3;

		_chmod("zVersionNew.cfg", _S_IREAD | _S_IWRITE);
		FILE* aNewVersionFileP = fopen("zVersionNew.cfg", "wt");
		if(!aNewVersionFileP)
		{
			fclose(anOldVersionFileP);
			return -4;
		}

		while(fgets(aBuffer, 2048, aReadFileP))
		{
			aFinalStr = aBuffer;
			if(!((aBuffer[0] == '/' && aBuffer[1] == '/') || aBuffer[0] == ';'))
			{
				for(string::iterator anItr = aFinalStr.begin(); anItr != aFinalStr.end();)
				{
					if(isspace(*anItr))
						anItr = aFinalStr.erase(anItr);
					else
						break;
				}

				// Change CurrentVersion to Version:
				if(FindNoCase(aFinalStr, "CurrentVersion") == 0)
				{
					aFinalStr = aFinalStr.substr(7);
					if(aFinalStr.at(7) != ':')
						aFinalStr.insert(7, ":");
				}
			}

			if(aFinalStr.empty() || aFinalStr[aFinalStr.length()-1] != '\n')
				fprintf(aNewVersionFileP, "%s\n", aFinalStr.c_str());
			else
				fprintf(aNewVersionFileP, "%s", aFinalStr.c_str());
		}

		fclose(anOldVersionFileP);
		fclose(aNewVersionFileP);

		remove(gProductP[3][1]);
		if(CopyFile("zVersionNew.cfg", gProductP[3][1], FALSE) == 0)
			return -5;

		remove("zVersionNew.cfg");
	}

	// Delete the old config and copy the new one
	remove("SierraUp.cfg");
	if(CopyFile("SierraUpNew.cfg", "SierraUp.cfg", FALSE) == 0)
		return -6;

	remove("SierraUpNew.cfg");

	// Setup language specific resource file
	_chmod("SierraUpRes/_LanguageUpdateResource.cfg", _S_IREAD | _S_IWRITE);
	remove("SierraUpRes/_LanguageUpdateResource.cfg");
	aLanguageStr[0] = toupper(aLanguageStr.at(0));
	aLanguageStr[1] = toupper(aLanguageStr.at(1));
	if(aLanguageStr != "EN")
	{
		string anOldLanguageFile = "SierraUpRes/_UpdateResource_";
		anOldLanguageFile += aLanguageStr;
		anOldLanguageFile += ".cfg";
		CopyFile(anOldLanguageFile.c_str(), "SierraUpRes/_LanguageUpdateResource.cfg", FALSE);
	}

	// Extract DLL resources
	if(gProductIndex != -1)
	{
		if(strlen(gProductP[gProductIndex][2]) > 0)
			ExportDLL(gProductP[gProductIndex][2]);
	}

	// Set up game specific config file
	string anOldGameFile = "SierraUpRes/";
	switch(gProductIndex)
	{
	case 1: // NR2002
		anOldGameFile += "NR2002_GameUpdateResource.cfg";
		break;
	case 2: // EEAOC
	case 3: // EE
		anOldGameFile += "EEAOC_GameUpdateResource.cfg";
		break;
	default:
		break;
	}

	switch(gProductIndex)
	{
	case 1: // NR2002
	case 2: // EEAOC
	case 3: // EE
		_chmod("SierraUpRes/_GameUpdateResource.cfg", _S_IREAD | _S_IWRITE);
		remove("SierraUpRes/_GameUpdateResource.cfg");
		CopyFile(anOldGameFile.c_str(), "SierraUpRes/_GameUpdateResource.cfg", FALSE);
		break;
	default:
		break;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int FindNoCase(const string& theStrToSearch, const char* theSearchStr, int theStartPos)
{
	int p1,p2;
	int cp = theStartPos;
	const int len1 = theStrToSearch.length();
	const int len2 = strlen(theSearchStr);
	const char *data1 = theStrToSearch.data();
	const char *data2 = theSearchStr;
	while(cp < len1)
	{
		p1 = cp;
		p2 = 0;
		while(p1<len1 && p2<len2)
		{
			if(toupper(data1[p1])!=toupper(data2[p2]))
				break;

			p1++; p2++;
		}
		if(p2==len2)
			return p1-len2;

		cp++;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AutoFile
{
public:
	FILE *mFile;

	AutoFile(FILE *theFile) : mFile(theFile) { }
	~AutoFile() { if(mFile) fclose(mFile); }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WriteBitmapResource(FILE *theFile, void *theData, DWORD theDataLen)
{
	BITMAPINFOHEADER *aBitmapHeader = (BITMAPINFOHEADER*)theData;

	DWORD aColorTableSize = 0;
	switch (aBitmapHeader->biBitCount)
	{
        case 24:                                      // has color table
			aColorTableSize = 0; 
            break;
        case 16:
        case 32:
            aColorTableSize = sizeof(DWORD)*3;
            break;
        default:
			if(aBitmapHeader->biClrUsed>0)
	            aColorTableSize = sizeof(RGBQUAD)*(1<<aBitmapHeader->biClrUsed);
			else
				aColorTableSize = sizeof(RGBQUAD)*(1<<aBitmapHeader->biBitCount);
		    break;
    }


	BITMAPFILEHEADER aFileHeader;
	memset(&aFileHeader,0,sizeof(aFileHeader));
	aFileHeader.bfType = 'B' | ('M' << 8);
	aFileHeader.bfSize = sizeof(aFileHeader) + theDataLen;
	aFileHeader.bfOffBits = sizeof(aFileHeader) + aBitmapHeader->biSize + aColorTableSize;

	fwrite(&aFileHeader,1,sizeof(aFileHeader),theFile);
	fwrite(aBitmapHeader,1,theDataLen,theFile);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma pack( push )
#pragma pack( 1 )
typedef struct
{
    BYTE        bWidth;          // Width, in pixels, of the image
    BYTE        bHeight;         // Height, in pixels, of the image
    BYTE        bColorCount;     // Number of colors in image (0 if >=8bpp)
    BYTE        bReserved;       // Reserved ( must be 0)
    WORD        wPlanes;         // Color Planes
    WORD        wBitCount;       // Bits per pixel
    DWORD       dwBytesInRes;    // How many bytes in this resource?
    DWORD       dwImageOffset;   // Where in the file is this image?
} ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct
{
    WORD           idReserved;   // Reserved (must be 0)
    WORD           idType;       // Resource Type (1 for icons)
    WORD           idCount;      // How many images?
    ICONDIRENTRY   idEntries[1]; // An entry for each image (idCount of 'em)
} ICONDIR, *LPICONDIR;

typedef struct
{
   BYTE   bWidth;               // Width, in pixels, of the image
   BYTE   bHeight;              // Height, in pixels, of the image
   BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
   BYTE   bReserved;            // Reserved
   WORD   wPlanes;              // Color Planes
   WORD   wBitCount;            // Bits per pixel
   DWORD   dwBytesInRes;         // how many bytes in this resource?
   WORD   nID;                  // the ID
} GRPICONDIRENTRY, *LPGRPICONDIRENTRY;

typedef struct 
{
   WORD            idReserved;   // Reserved (must be 0)
   WORD            idType;       // Resource type (1 for icons)
   WORD            idCount;      // How many images?
   GRPICONDIRENTRY   idEntries[1]; // The entries for each image
} GRPICONDIR, *LPGRPICONDIR;
#pragma pack( pop )

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WriteIconResource(HMODULE theModule, FILE *theFile, WORD theId)
{
	HRSRC aRes = FindResource(theModule,MAKEINTRESOURCE(theId),RT_ICON);
	if(aRes==NULL)
		return false;

	HGLOBAL aMem = LoadResource(theModule,aRes);
	if(aMem==NULL)
		return false;

	void *aData = LockResource(aMem);
	if(aData==NULL)
		return false;

	DWORD aDataLen = SizeofResource(theModule,aRes);
	fwrite(aData,1,aDataLen,theFile);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WriteIconGroupResource(HMODULE theModule, FILE *theFile, void *theData, DWORD theDataLen)
{
	GRPICONDIR *aResDir = (GRPICONDIR*)theData;

	DWORD aHeaderSize = sizeof(ICONDIR) + sizeof(ICONDIRENTRY)*(aResDir->idCount-1);
	DWORD anImageOffset = aHeaderSize;

	char *aMem = new char[aHeaderSize];
	std::auto_ptr<char> aDelMem(aMem);

	ICONDIR *aFileDir = (ICONDIR*)aMem;
	aFileDir->idReserved = 0;
	aFileDir->idCount = aResDir->idCount;
	aFileDir->idType = aResDir->idType;

	int i;
	for(i=0; i<aResDir->idCount; i++)
	{
		GRPICONDIRENTRY &aResDirEntry = aResDir->idEntries[i];
		ICONDIRENTRY &aFileDirEntry = aFileDir->idEntries[i];
		aFileDirEntry.bWidth = aResDirEntry.bWidth;
		aFileDirEntry.bHeight = aResDirEntry.bHeight;
		aFileDirEntry.bColorCount = aResDirEntry.bColorCount;
		aFileDirEntry.bReserved = 0;
		aFileDirEntry.wPlanes = aResDirEntry.wPlanes;
		aFileDirEntry.wBitCount = aResDirEntry.wBitCount;
		aFileDirEntry.dwBytesInRes = aResDirEntry.dwBytesInRes;
		aFileDirEntry.dwImageOffset = anImageOffset;
		anImageOffset += aFileDirEntry.dwBytesInRes;
	}

	fwrite(aFileDir,1,aHeaderSize,theFile);
	for(i=0; i<aResDir->idCount; i++)
		WriteIconResource(theModule,theFile,aResDir->idEntries[i].nID);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WriteStretchImage(HMODULE theModule, const char *theResName, FILE *theFile)
{
	HBITMAP aBitmap1 = LoadBitmap(theModule,MAKEINTRESOURCE(1301));
	if(aBitmap1==NULL)
		return false;

	const int NEW_WIDTH = 640;
	const int NEW_HEIGHT = 480;

	BITMAP aBitmapObj;
	GetObject(aBitmap1,sizeof(aBitmapObj),&aBitmapObj);

	HDC aDC1 = CreateCompatibleDC(NULL);
	HDC aDC2 = CreateCompatibleDC(NULL);
	HBITMAP oldBMP1 = (HBITMAP)SelectObject(aDC1,aBitmap1);
	HBITMAP aBitmap2 = CreateCompatibleBitmap(aDC1,NEW_WIDTH,NEW_HEIGHT);
	HBITMAP oldBMP2 = (HBITMAP)SelectObject(aDC2,aBitmap2);
	StretchBlt(aDC2,0,0,640,480,aDC1,0,0,aBitmapObj.bmWidth,aBitmapObj.bmHeight,SRCCOPY);

	SelectObject(aDC1,oldBMP1);
	SelectObject(aDC2,oldBMP2);

	BITMAPINFO anInfo;
	memset(&anInfo,0,sizeof(anInfo));
	anInfo.bmiHeader.biSize = sizeof(anInfo.bmiHeader);
	anInfo.bmiHeader.biBitCount = 24;
	anInfo.bmiHeader.biPlanes = 1;
	anInfo.bmiHeader.biWidth = aBitmapObj.bmWidth;
	anInfo.bmiHeader.biHeight = -aBitmapObj.bmHeight;
	anInfo.bmiHeader.biCompression = BI_RGB;

	GetDIBits(aDC1,aBitmap1,0,aBitmapObj.bmHeight,NULL,&anInfo,DIB_RGB_COLORS);

	int aBytesPerRow = NEW_WIDTH*3;
	if(aBytesPerRow % 4 != 0)
		aBytesPerRow += (4 - aBytesPerRow %4); // make width a multiple of 4

	const int anImageSize = aBytesPerRow*NEW_HEIGHT;
	char *aPixels = new char[anImageSize];
	std::auto_ptr<char> aDelPixels(aPixels);
	anInfo.bmiHeader.biWidth = NEW_WIDTH;
	anInfo.bmiHeader.biHeight = NEW_HEIGHT;
	GetDIBits(aDC1,aBitmap2,0,NEW_HEIGHT,aPixels,&anInfo,DIB_RGB_COLORS);

	DeleteDC(aDC1);
	DeleteDC(aDC2);
	DeleteObject(aBitmap2);

	// Write Bitmap
	BITMAPINFOHEADER *aBitmapHeader = &anInfo.bmiHeader;
	aBitmapHeader->biHeight = abs(aBitmapHeader->biHeight);

	DWORD aColorTableSize = 0;
	switch (aBitmapHeader->biBitCount)
	{
        case 24:                                      // has color table
			aColorTableSize = 0; 
            break;
        case 16:
        case 32:
            aColorTableSize = sizeof(DWORD)*3;
            break;
        default:
			if(aBitmapHeader->biClrUsed>0)
	            aColorTableSize = sizeof(RGBQUAD)*(1<<aBitmapHeader->biClrUsed);
			else
				aColorTableSize = sizeof(RGBQUAD)*(1<<aBitmapHeader->biBitCount);
		    break;
    }


	BITMAPFILEHEADER aFileHeader;
	memset(&aFileHeader,0,sizeof(aFileHeader));
	aFileHeader.bfType = 'B' | ('M' << 8);
	aFileHeader.bfSize = sizeof(aFileHeader) + aBitmapHeader->biSize + aColorTableSize + anImageSize;
	aFileHeader.bfOffBits = sizeof(aFileHeader) + aBitmapHeader->biSize + aColorTableSize;

	fwrite(&aFileHeader,1,sizeof(aFileHeader),theFile);
	fwrite(aBitmapHeader,1,aBitmapHeader->biSize + aColorTableSize,theFile);
	fwrite(aPixels,1,anImageSize,theFile);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WriteResource(HMODULE theModule, const char *theResType, const char *theResName)
{
	HRSRC aRes = FindResource(theModule,theResName,theResType);
	if(aRes==NULL)
		return false;

	HGLOBAL aMem = LoadResource(theModule,aRes);
	if(aMem==NULL)
		return false;

	void *aData = LockResource(aMem);
	if(aData==NULL)
		return false;

	DWORD aDataLen = SizeofResource(theModule,aRes);

	char aFileName[50];
	DWORD aResType = (DWORD)theResType;
	switch(aResType)
	{
		case RT_BITMAP: sprintf(aFileName,"SierraUpRes/%d.bmp",theResName); break;
		case RT_GROUP_ICON: sprintf(aFileName,"SierraUpRes/SierraUp.ico",theResName); break;
	}

	AutoFile aCloseFile = fopen(aFileName,"wb");
	if(aCloseFile.mFile==NULL)
		return false;

	if(gProductIndex==1 && theResType==RT_BITMAP && ((WORD)theResName)==1301) // need to stretch the nascar background image.)
	{
		WriteStretchImage(theModule,theResName,aCloseFile.mFile);
		return true;
	}


	switch(aResType)
	{
		case RT_BITMAP: WriteBitmapResource(aCloseFile.mFile,aData,aDataLen); break;
		case RT_GROUP_ICON: WriteIconGroupResource(theModule,aCloseFile.mFile,aData,aDataLen); break;
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef IS_INTRESOURCE
#define IS_INTRESOURCE(_r) (((ULONG_PTR)(_r) >> 16) == 0)
#endif

BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
	if(!IS_INTRESOURCE(lpszName))
		return TRUE;

	WriteResource(hModule,lpszType,lpszName);
    return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ExportDLL(const char* theDLLFile)
{
	bool aReturnValue = false;
	HMODULE aModule = LoadLibrary(theDLLFile);
	if(aModule == NULL)
		return aReturnValue;

	aReturnValue = (EnumResourceNames(aModule,RT_BITMAP,EnumResNameProc,NULL) == TRUE);
	if(aReturnValue)
		aReturnValue = WriteResource(aModule,RT_GROUP_ICON,MAKEINTRESOURCE(10000));

	FreeLibrary(aModule);
	return aReturnValue;
}

