#ifndef __WON_XBFONT_H__
#define __WON_XBFONT_H__

#include "GUISystem.h"
#include "Font.h"

namespace WONAPI
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class XBFont : public Font
{
protected:
//-----------------------------------------------------------------------------
// Name: struct GLYPH_ATTR
// Desc: Structure to hold information about one glyph (font character image)
//-----------------------------------------------------------------------------
struct GLYPH_ATTR
{
    FLOAT left, top, right, bottom; // Texture coordinates for the image
    SHORT wOffset;                  // Pixel offset for glyph start
    SHORT wWidth;                   // Pixel width of the glyph
    SHORT wAdvance;                 // Pixels to advance after the glyph
};

protected:
    // Font and texture dimensions
    DWORD        m_dwFontHeight;
    DWORD        m_dwTexWidth;
    DWORD        m_dwTexHeight;

    // Unicode ranges
    WCHAR        m_cLowChar;
    WCHAR        m_cHighChar;

    // Glyph data for the font
    DWORD        m_dwNumGlyphs;
    GLYPH_ATTR*  m_Glyphs;

    // D3D rendering objects
    LPDIRECT3DDEVICE8       m_pd3dDevice;
    LPDIRECT3DTEXTURE8      m_pTexture;
    LPDIRECT3DVERTEXBUFFER8 m_pVB;
    BOOL         m_bIsStateSet;

  	virtual ~XBFont();
	HRESULT SetState();
	HRESULT RestoreState();


public:
	virtual void PrepareContext(DisplayContext *theContext);
	virtual void DrawString(DisplayContext *theContext, const char *theStr, int theLen, int x, int y);

public:
	XBFont(DisplayContext *theContext, const FontDescriptor &theDescriptor);

	virtual int GetStringWidth(const char *theStr, int theLen);
	virtual int GetCharWidth(char theChar) { return GetStringWidth(&theChar,1); }

	virtual int GetHeight()		{ return m_dwFontHeight; }
	virtual int GetAscent()		{ return GetHeight();	 }
	virtual int GetDescent()	{ return 0;				 }
};
typedef WONAPI::SmartPtr<XBFont> XBFontPtr;

};

#endif
