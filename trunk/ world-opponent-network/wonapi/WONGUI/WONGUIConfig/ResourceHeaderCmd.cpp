#include "ResourceHeader.h"

using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void main(int argc, char *argv[])
{
	if(argc!=3)
	{
		printf("Usage: %s <resource file> <header file>\n",argv[0]);
		return;
	}

	ConfigParser aParser;
	aParser.SetWarningsAsErrors(true);
	if(!aParser.OpenFile(argv[1]))
	{
		printf("Failed to open resource file.\n");
		return;
	}

	ResourceHeaderPtr aResource = new ResourceHeader;
	aResource->SetFileName(argv[2]);

	if(aResource->IsFileReadOnly())
	{
		printf("*** Error: Source file is readonly. ***\n\n");
		return;
	}

	printf("Parsing config files.\n");
	if(!aResource->ParseTopLevel(aParser))
	{
		printf("Parse Error: %s\n",aParser.GetAbortReason().c_str());
		return;
	}
	
	printf("Writing source file.\n");
	if(!aResource->WriteSourceFiles())
	{
		printf("Error writing source files.\n");
		return;
	}

	printf("Success!\n");
}

