#ifndef __WON_RESOURCECONFIG_H__
#define __WON_RESOURCECONFIG_H__
#include "WONCommon/StringUtil.h"
#include "WONConfig/ConfigObject.h"
#include "WONGUI/GUIString.h"
#include "WONGUI/Image.h"
#include "WONGUI/Background.h"
#include "WONGUI/Sound.h"

#include <set>

namespace WONAPI
{

WON_PTR_FORWARD(Component);
WON_PTR_FORWARD(ComponentConfig);
class ResourceConfigTable;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ResourceConfig : public ConfigObject
{
protected:
	template <class T> class ResMap : public std::map<std::string,std::pair<T,int>,StringLessNoCase> { };
	typedef ResMap<GUIString> StringMap;
	typedef ResMap<ImagePtr> ImageMap;
	typedef ResMap<FontPtr> FontMap;
	typedef ResMap<DWORD> ColorMap;
	typedef ResMap<Background> BackgroundMap;
	typedef ResMap<int> IntMap;
	typedef ResMap<SoundPtr> SoundMap;

	bool mAllowOverwrite;
	int mParseCount; 
	StringMap mStringMap;
	ImageMap mImageMap;
	FontMap mFontMap;
	ColorMap mColorMap;
	BackgroundMap mBackgroundMap;
	IntMap mIntMap; 
	SoundMap mSoundMap;
	ResourceConfigTable *mConfigTable;

	bool HandleString(StringParser &theVal);
	bool HandleImage(StringParser &theVal);
	bool HandleBackground(StringParser &theVal);
	bool HandleColor(StringParser &theVal);
	bool HandleFont(StringParser &theVal);
	bool HandleSound(StringParser &theVal);
	bool HandleInt(StringParser &theVal);
	static void EnsureComma(StringParser &theVal);
	static void EnsureCloseParen(StringParser &theVal);
	ResourceConfig* CheckResourceReference(StringParser &theVal, std::string &theResName);

	virtual ~ResourceConfig();

public:
	ResourceConfig(ResourceConfigTable *theTable = NULL);
	void SetAllowOverwrite(bool allow) { mAllowOverwrite = allow; }

	virtual bool HandleKeyVal(ConfigParser &theParser, const std::string& theKey, StringParser &theVal);
	virtual bool PreParse(ConfigParser &theParser);

	void AddString(const std::string &theKey, const GUIString &theStr);
	void AddImage(const std::string &theKey, Image *theImage);
	void AddFont(const std::string &theKey, Font *theFont);
	void AddColor(const std::string &theKey, DWORD theColor);
	void AddBackground(const std::string &theKey, const Background &theBackground);
	void AddInt(const std::string &theKey, int theInt);
	void AddSound(const std::string &theKey, Sound *theSound);

	bool GetString(const std::string &theKey, GUIString &theStr); 
	bool GetImage(const std::string &theKey, Image* &theImage);
	bool GetFont(const std::string &theKey, Font* &theFont);
	bool GetColor(const std::string &theKey, DWORD &theColor);
	bool GetBackground(const std::string &theKey, Background &theBackground);
	bool GetInt(const std::string &theKey, int &theInt);
	bool GetSound(const std::string &theKey, Sound* &theSound);

	const GUIString& SafeGetString(const std::string &theKey); 
	Image* SafeGetImage(const std::string &theKey);
	Font* SafeGetFont(const std::string &theKey);
	DWORD SafeGetColor(const std::string &theKey);
	const Background& SafeGetBackground(const std::string &theKey);
	int SafeGetInt(const std::string &theKey);
	Sound* SafeGetSound(const std::string &theKey);

	static int SafeReadColor(StringParser &theVal);
	static int SafeReadFontStyle(StringParser &theVal);
	static FontPtr SafeReadFont(StringParser &theVal);
	static SoundPtr SafeReadSound(StringParser &theVal);
	static Background SafeReadBackground(StringParser &theVal);
	static ImagePtr SafeReadImage(StringParser &theVal);
};
typedef SmartPtr<ResourceConfig> ResourceConfigPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ResourceConfigTable : public RefCount
{
protected:
	typedef std::map<std::string,ResourceConfigPtr,StringLessNoCase> ResourceMap;
	ResourceMap mResourceMap;

	typedef std::map<std::string,int,StringLessNoCase> ControlIdMap;
	static ControlIdMap mControlIdMap;

	typedef Component*(*ComponentFactory)();
	typedef ComponentConfig*(*ComponentConfigFactory)();
	struct ConfigAllocator
	{
		ComponentConfigFactory mConfigFactory;
		ComponentFactory mComponentFactory;

		ConfigAllocator()  { }
		ConfigAllocator(ComponentConfigFactory a, ComponentFactory b) : mConfigFactory(a), mComponentFactory(b) { }
	};

	typedef std::map<std::string,ConfigAllocator,StringLessNoCase> ConfigAllocatorMap;
	static ConfigAllocatorMap mConfigAllocatorMap;


	typedef void(*GlueResourceFunc)(ResourceConfigTable *theTable);
	typedef std::set<GlueResourceFunc> GlueResourceFuncSet;
	GlueResourceFuncSet mGlueResourceFuncSet;

	virtual ~ResourceConfigTable();

public:
	ResourceConfigTable();

	void AddAllocator(const std::string &theName, ComponentConfigFactory theConfigFactory, ComponentFactory theComponentFactory);
	void RegisterControlId(const std::string &theName, int theVal);
	bool AllocateConfig(const std::string &theName, ComponentConfigPtr &theConfig, ComponentPtr &theComponent);
	bool GetControlId(const std::string &theName, int &theVal);

	ResourceConfig* AddConfig(const std::string &theName, bool* alreadyExists = NULL);
	void RegisterGlueResourceFunc(GlueResourceFunc theFunc);
	void SafeGlueResources(); // will throw a ConfigObjectException if a resource isn't found

	void ClearGlueResourceFuncs();
	void ClearAllocators();
	void ClearControlIds();
	void ClearResources();
	void ClearAll();
	
	ResourceConfig* GetConfig(const std::string &theName);
	ResourceConfig* SafeGetConfig(const std::string &theName); // throws config object exception if the config isn't there
};
typedef SmartPtr<ResourceConfigTable> ResourceConfigTablePtr;



} // namespace WONAPI

#endif