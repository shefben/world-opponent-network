//----------------------------------------------------------------------------------
// ResourceConversion.h
//----------------------------------------------------------------------------------
#ifndef __ResourceConversion_H__
#define __ResourceConversion_H__

#ifndef _STRING_
#include <string>
#endif

namespace WONAPI
{

struct ResourceMaps : public RefCount
{
	typedef std::map<int, ImagePtr>    IMAGE_MAP;
	typedef IMAGE_MAP::iterator        IMAGE_MAP_ITR;
	typedef IMAGE_MAP::const_iterator  IMAGE_MAP_CITR;
	typedef std::map<int, GUIString>   STRING_MAP;
	typedef STRING_MAP::iterator       STRING_MAP_ITR;
	typedef STRING_MAP::const_iterator STRING_MAP_CITR;

	int GetNumResources() { return (mImageMap.size() + mStringMap.size()); }
	GUIString GetString(int theId);
	ImagePtr GetImage(int theId);

	IMAGE_MAP  mImageMap;
	STRING_MAP mStringMap;
};
typedef SmartPtr<ResourceMaps> ResourceMapsPtr;

//----------------------------------------------------------------------------------
// ResourceConversion.
//----------------------------------------------------------------------------------
class ResourceConversion
{
protected:
	std::string              m_sDllFile;     // DLL to convert.
	HMODULE                  m_hDll;         // Handle the the DLL (once opened);
	bool                     m_bAvP2;        // Are we converting an Aliens Vs Predator 2 DLL?
	bool                     m_bEmpireEarth; // Are we converting an Empire Earth DLL?
	bool                     m_bHalfLife;    // Are we converting a Half-Life DLL?
	bool                     m_bNolf;        // Are we converting a demo Nolf DLL?
	bool                     m_bNR2002;      // Are we converting an NR2002 DLL?
	ResourceMapsPtr          mResourceMaps;

	bool ConvertIcon(int nOldID, int nNewID, const std::string& sNewID);
	bool ConvertImage(int nOldID, int nNewID, const std::string& sNewID);
	bool ConvertString(int nOldID, int nNewID, const std::string& sNewID);
	bool CreateGrayTextColor(int nOldID, int nNewID, const std::string& sNewID);
	void ConvertImages(void);
	void ConvertStrings(void);
	void ConvertColors(void);
	void ConvertOthers(void);

public:
	ResourceConversion(const std::string& sDll, ResourceMapsPtr theResourceCollection);

	bool Convert(void);

	inline std::string GetDllFile(void) const { return m_sDllFile; }
	inline void SetDllFile(const std::string& sFile);
};

} // namespace

#endif