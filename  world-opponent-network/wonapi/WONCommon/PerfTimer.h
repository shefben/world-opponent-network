#ifndef __WON_PERFTIMER_H__
#define __WON_PERFTIMER_H__
#include "WONShared.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PerfTimer
{
protected:
	bool mStarted;
	const char *mName;

public:
	PerfTimer(const char *theName = NULL);
	~PerfTimer();

	void Start();
	void Stop();

	static void GlobalStart(const char *theName = NULL);
	static void GlobalStop(const char *theName = NULL);
	static double GetCountedTime(const char *theName = NULL);
	static double GetElapsedTime(const char *theName = NULL);
	static double GetPercentTime(const char *theName = NULL);

	static void DumpTimesToStr(std::string &theStr);
	static void DumpTimesToFile(const char *theFileName);
};


} // namespace WONAPI


#endif