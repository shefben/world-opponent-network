#include "ResConfig.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void main(int argc, char *argv[])
{
	if(argc!=3)
	{
		printf("Usage: %s <in.cfg> <out.wrs>",argv[0]);
		return;
	}

	ConfigParser aParser;
	if(!aParser.OpenFile(argv[1]))
	{
		printf("Failed to open file %s\n",argv[1]);
		return;
	}

	ResourceCollectionWriter aWriter;
	ResConfigPtr aConfig = new ResConfig(aWriter);
	if(!aConfig->ParseTopLevel(aParser))
	{
		printf("Error parsing file: %s\n",aParser.GetAbortReason().c_str());
		return;
	}

	aWriter.SaveAs(argv[2]);
	printf("Success!\n");
}