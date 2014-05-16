#ifndef __WON_COMPONENTCONFIG_H__
#define __WON_COMPONENTCONFIG_H__

#include "WONCommon/StringUtil.h"
#include "WONConfig/ConfigObject.h"
#include "WONGUI/Component.h"
#include "WONGUI/Background.h"
#include "WONGUI/SelectionColor.h"
#include "WONGUI/Align.h"
#include "WONGUI/RawImage.h"
#include "WONGUI/GUIConfig.h"

#include <exception>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WON_PTR_FORWARD(ComponentConfig);
WON_PTR_FORWARD(ResourceConfig);
WON_PTR_FORWARD(ResourceConfigTable);
WON_PTR_FORWARD(Sound);

enum StandardCursor;	

class ComponentConfig : public ConfigObject
{
protected:
	virtual ~ComponentConfig();

protected:
	ComponentPtr mComponent;
	ConfigParser *mConfigParser;
	StringParser *mStringParser;
	GUIString mConfigError;

	bool mHasSetWidth, mHasSetHeight;
	bool mHaveInitedComponents;

	typedef std::map<std::string,ComponentConfigPtr,StringLessNoCase> ConfigMap;
	ConfigMap mConfigMap;

	typedef std::vector<int> IntVector;

	static ResourceConfigTablePtr mResourceConfigTable;
	ResourceConfigPtr mCurResourceConfig;

	static std::string mResourceFolder;

	virtual bool PreParse(ConfigParser &theParser);
	virtual bool HandleKeyVal(ConfigParser &theParser, const std::string& theKey, StringParser &theVal);
	virtual bool HandleChildGroup(ConfigParser &theParser, const std::string& theGroupType);
	virtual bool PostParse(ConfigParser &theParser);

	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent);
	virtual bool GetNamedIntValue(const std::string &theName, int &theValue);
	virtual bool GetNamedIntFunc(const std::string &theName, const IntVector &theParams, int &theResult);

	int GetIntValue();
	int ParseExpression(int theLevel);
	void SetDesiredSizeIfNecessary();

	void InitComponents();

public:
	void SkipComma();
	void EnsureComma();
	bool EndOfString();
	int ReadInt();
	int ReadChar();
	bool ReadBool();
	GUIString ReadFlag(bool *on = NULL);
	GUIString ReadString(bool stopAtWhitepsace = false);
	GUIString ReadRawString(bool stopAtWhitespace = false);
	FontPtr ReadFont();
	SoundPtr ReadSound();
	ImagePtr ReadImage(bool getRaw = false);
	RawImagePtr ReadRawImage();
	int ReadColor();
	Direction ReadDirection();
	VertAlign ReadVertAlign();
	HorzAlign ReadHorzAlign();
	SelectionColor ReadSelectionColor();
	Background ReadBackground();
	Component* ReadComponent();
	ComponentConfig* ReadComponentConfig();
	CursorPtr ReadCursor();
	StandardCursor ReadStandardCursor();

	ComponentConfig* GetComponentConfig(const GUIString &theName, bool throwExceptionOnFailure = true);
	Component* GetComponentByName(const GUIString &theName);
	ImagePtr GetImageFromStr(const std::string &theImageSpec, bool getRaw = false);
	RawImagePtr GetRawImageFromStr(const std::string &theImageSpec);

	GUIString GetStringResource(const std::string &theKey);
	Image* GetImageResource(const std::string &theKey);
	Font* GetFontResource(const std::string &theKey);
	DWORD GetColorResource(const std::string &theKey);
	void GetBackgroundResource(const std::string &theKey, Background &theBackground);
	int GetIntResource(const std::string &theKey);
	SoundPtr GetSoundResource(const std::string &theKey);

	bool HandleMapComponent();
	bool HandleModify();
	bool HandleModifyGroup(ConfigParser &theParser);
	bool HandleResourceGroup(ConfigParser &theParser);

public:
	bool BaseHandleInstruction(ConfigParser &theParser, const std::string& theKey, StringParser &theVal);
	void AddChildToConfigMap(const GUIString &theName, ComponentConfig *theConfig);

public:
	static Component* CompFactory() { return new Component; }
	static ComponentConfig* CfgFactory() { return new ComponentConfig; }

public:
	ComponentConfig();
	virtual void SetComponent(Component *theComponent);

	// ReadConfig will show an error dialog and exit if there's a problem.
	// ReadConfig_Unsafe will return false if something is wrong with the config
	void ReadConfig(Component *theComponent, const char *theFileName, ResourceConfigTable *theTable = NULL, ConfigParser *theParser = NULL);
	bool ReadConfig_Unsafe(Component *theComponent, const char *theFileName, ResourceConfigTable *theTable = NULL, ConfigParser *theParser = NULL); 

	Component* GetChildComponent(const std::string &theName);
	ComponentConfig* GetChildConfig(const std::string &theName);

	Component* GetComponent() { return mComponent; }
	const GUIString& GetConfigError() { return mConfigError; }

	static void SetResourceFolder(const std::string &theFolder);
	static std::string GetResourceFile(const std::string &theFileName);
}; 	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class T>
T SafeConfigGetComponent(Component *theComponent, const char *theName)
{
	T aComp = dynamic_cast<T>(theComponent);
	if(aComp==NULL)
		throw ConfigObjectException(std::string("Invalid ") + theName);

	return aComp;
}



}; // namespace WONAPI


#endif