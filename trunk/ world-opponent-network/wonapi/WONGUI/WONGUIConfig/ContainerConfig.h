#ifndef __WON_CONTAINERCONFIG_H__
#define __WON_CONTAINERCONFIG_H__

#include "ComponentConfig.h"
#include "WONGUI/Container.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ContainerConfig : public ComponentConfig
{
protected:
	Container* mContainer;

	// Last Layout used for chaining
	Component *mLayoutRef;
	int mLayoutInstruction;
	int mLayoutParams[4];
	int mCurLayoutId;


	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool HandleChildGroup(ConfigParser &theParser, const std::string& theGroupType);

	bool HandleAdd();
	bool HandleAddLayoutPoint();
	bool HandleNewLayoutPoint();
	bool HandleLayout(bool chain = false, bool repeat = false);
	bool HandleSameSizeLayout(bool sameWidth, bool sameHeight);
	bool HandleSpaceEvenlyLayout(bool horz);
	bool HandleCenterLayout(bool horz, bool vert);
	bool HandleClearRightLayout();
	bool HandleNoOverlapLayout();
	bool HandleSizeAtLeastLayout();

public:
	ContainerConfig();
	virtual void SetComponent(Component *theComponent);

	static Component* LayoutPointFactory() { return new LayoutPoint; }
	static Component* CompFactory() { return new Container; }
	static ComponentConfig* CfgFactory() { return new ContainerConfig; }

};
typedef SmartPtr<ContainerConfig> ContainerConfigPtr;

}; // namespace WONAPI

#endif