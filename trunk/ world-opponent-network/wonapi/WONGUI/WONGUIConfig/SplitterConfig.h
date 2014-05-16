#ifndef __WON_SPLITTERCONFIG_H__
#define __WON_SPLITTERCONFIG_H__

#include "ContainerConfig.h"
#include "WONGUI/Splitter.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SplitterConfig : public ContainerConfig
{
protected:
	Splitter* mSplitter;

	virtual bool HandleInstruction(const std::string &theInstruction);
	Splitter::QuadrantPosition ReadQuadrant();
	Splitter::SplitterType ReadSplitterType();

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new Splitter; }
	static ComponentConfig* CfgFactory() { return new SplitterConfig; }
};
typedef SmartPtr<SplitterConfig> SplitterConfigPtr;

}; // namespace WONAPI

#endif