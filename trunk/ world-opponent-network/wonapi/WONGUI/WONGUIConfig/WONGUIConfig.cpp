#include "WONGUIConfig.h"
#include "ResourceConfig.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::WONGUIConfig_Init(ResourceConfigTable *theTable)
{
	theTable->AddAllocator("Component",ComponentConfig::CfgFactory,ComponentConfig::CompFactory);
	theTable->AddAllocator("LayoutPoint",ComponentConfig::CfgFactory,ContainerConfig::LayoutPointFactory);
	theTable->AddAllocator("Container",ContainerConfig::CfgFactory,ContainerConfig::CompFactory);
	theTable->AddAllocator("Dialog",DialogConfig::CfgFactory,DialogConfig::CompFactory);
	theTable->AddAllocator("ImageButton",ImageButtonConfig::CfgFactory,ImageButtonConfig::CompFactory);
	theTable->AddAllocator("Label",LabelConfig::CfgFactory,LabelConfig::CompFactory);
	theTable->AddAllocator("ListArea",ListAreaConfig::CfgFactory,ListAreaConfig::CompFactory);
	theTable->AddAllocator("MultiListArea",MultiListAreaConfig::CfgFactory,MultiListAreaConfig::CompFactory);
	theTable->AddAllocator("MultiListHeader",MultiListHeaderConfig::CfgFactory,MultiListHeaderConfig::CompFactory);
	theTable->AddAllocator("InputBox",InputBoxConfig::CfgFactory,InputBoxConfig::CompFactory);
	theTable->AddAllocator("Splitter",SplitterConfig::CfgFactory,SplitterConfig::CompFactory);
	theTable->AddAllocator("TextArea",TextAreaConfig::CfgFactory,TextAreaConfig::CompFactory);
	theTable->AddAllocator("TabCtrl",TabCtrlConfig::CfgFactory,TabCtrlConfig::CompFactory);
	theTable->AddAllocator("TreeArea",TreeAreaConfig::CfgFactory,TreeAreaConfig::CompFactory);

	theTable->AddAllocator("RectangleComponent",RectangleComponentConfig::CfgFactory,RectangleComponentConfig::CompFactory);
	theTable->AddAllocator("RectangleComponent3D",RectangleComponent3DConfig::CfgFactory,RectangleComponent3DConfig::CompFactory);
	theTable->AddAllocator("ImageComponent",ImageComponentConfig::CfgFactory,ImageComponentConfig::CompFactory);
	theTable->AddAllocator("AnimationComponent",AnimationComponentConfig::CfgFactory,AnimationComponentConfig::CompFactory);
	theTable->AddAllocator("ScreenContainer",ScreenContainerConfig::CfgFactory,ScreenContainerConfig::CompFactory);
	MSComponentConfig_Init(theTable);
	ImageConfig_Init(theTable);
	FontConfig_Init(theTable);
	WONControlsConfig_Init(theTable);

	theTable->RegisterControlId("Ok",ControlId_Ok);
	theTable->RegisterControlId("Cancel",ControlId_Cancel);
/*	theTable->RegisterControlId("Splitter_UnMax",Splitter::ControlId_Unmax);
	theTable->RegisterControlId("Splitter_MaxLeft",Splitter::ControlId_MaxLeft);
	theTable->RegisterControlId("Splitter_MaxRight",Splitter::ControlId_MaxRight);
	theTable->RegisterControlId("Splitter_MaxTop",Splitter::ControlId_MaxTop);
	theTable->RegisterControlId("Splitter_MaxBottom",Splitter::ControlId_MaxBottom);
	theTable->RegisterControlId("Splitter_MaxTopLeft",Splitter::ControlId_MaxTopLeft);
	theTable->RegisterControlId("Splitter_MaxTopRight",Splitter::ControlId_MaxTopRight);
	theTable->RegisterControlId("Splitter_MaxBottomLeft",Splitter::ControlId_MaxBottomLeft);
	theTable->RegisterControlId("Splitter_MaxBottomRight",Splitter::ControlId_MaxBottomRight);*/
}
