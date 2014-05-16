#include "DisplayContext.h"
#include "XBFont.h"
#include "Graphics.h"
#include <stdio.h>
#include <direct.h>
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Custom vertex type for rendering text
//-----------------------------------------------------------------------------
struct FONT2DVERTEX 
{ 
    D3DXVECTOR4 p;
    DWORD       color;
    FLOAT       tu, tv; 
};

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

inline FONT2DVERTEX InitFont2DVertex( const D3DXVECTOR4& p, D3DCOLOR color,
                                      FLOAT tu, FLOAT tv )
{
    FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
    return v;
}

// Max size for the font class' vertex buffer
#define XBFONT_MAX_VERTICES 500*4



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XBFont::XBFont(DisplayContext *theContext, const FontDescriptor &theDescriptor)
	: Font(theContext, theDescriptor)
{
    m_pd3dDevice           = NULL;
    m_pTexture             = NULL;
    m_pVB                  = NULL;

    m_bIsStateSet          = FALSE;

    m_dwFontHeight         = 36;
    m_dwTexWidth           = 64;
    m_dwTexHeight          = 64;

    m_dwNumGlyphs          = 0L;
    m_Glyphs               = NULL;

	// Create the font
    HRESULT hr;

    // Store the device for use in member functions
    m_pd3dDevice = theContext->mDevice;

    // Find the media files
	CHAR strFontImageFileName[512] = "d:\\Media\\Fonts\\Arial_14.tga";
	CHAR strFontInfoFileName[512] = "d:\\Media\\Fonts\\Arial_14.abc";
	
    // Create a new texture object for the font
    hr = D3DXCreateTextureFromFileEx( m_pd3dDevice, strFontImageFileName, 
                                      D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 
                                      0, D3DFMT_A4R4G4B4, D3DPOOL_DEFAULT, 
                                      D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, 
                                      &m_pTexture );
    if( FAILED(hr) )
        return;

    // Access the font's .abc file
    FILE* file = fopen( strFontInfoFileName, "rb" );
    if( NULL == file )
        return;

    // Read header
    DWORD dwVersion, dwBPP;
    fread( &dwVersion,      sizeof(DWORD), 1, file ); 
    fread( &m_dwFontHeight, sizeof(DWORD), 1, file ); 
    fread( &m_dwTexWidth,   sizeof(DWORD), 1, file ); 
    fread( &m_dwTexHeight,  sizeof(DWORD), 1, file ); 
    fread( &dwBPP,          sizeof(DWORD), 1, file ); 

    // Check version of file (to make sure it matches up with the FontMaker tool)
    if( dwVersion != 0x00000004 )
    {
        OutputDebugString("XBFont: Incorrect version number on font file!\n");
        return;
    }

    // Read the low and high char
    fread( &m_cLowChar,  1, sizeof(WCHAR), file );
    fread( &m_cHighChar, 1, sizeof(WCHAR), file );

    // Read the glyph attributes from the file
    fread( &m_dwNumGlyphs, sizeof(DWORD), 1, file );
    m_Glyphs = new GLYPH_ATTR[m_dwNumGlyphs];
    fread( m_Glyphs, sizeof(GLYPH_ATTR), m_dwNumGlyphs, file ); 

    fclose( file );

    // Create vertex buffer for rendering text strings
    hr = m_pd3dDevice->CreateVertexBuffer( XBFONT_MAX_VERTICES*sizeof(FONT2DVERTEX),
                                         D3DUSAGE_WRITEONLY, 0L,
                                         D3DPOOL_DEFAULT, &m_pVB );
    if( FAILED(hr) )
        return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XBFont::~XBFont()
{
    // Delete objects and allocated memory
	m_pTexture->Release();
    m_pVB->Release();
    delete m_Glyphs;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBFont::PrepareContext(DisplayContext *theContext) 
{
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int XBFont::GetStringWidth(const char *theStr, int theLen)
{
	// Initialize counters that keep track of text extent
	FLOAT sx = 0.0f;

	// Loop through each character and update text extent
	for(int i=0; i<theLen; i++)
	{
		char letter = *theStr++;

		// Ignore unprintable characters
		if( letter<m_cLowChar || letter>m_cHighChar )
			continue;

		// Get text extent for this character's glyph
		GLYPH_ATTR* pGlyph = &m_Glyphs[letter - m_cLowChar];
		sx += pGlyph->wOffset;
		sx += pGlyph->wAdvance;
	}

	return (int)sx;
}



//-----------------------------------------------------------------------------
// Name: SetState()
// Desc: Sets render states before drawing text. Note that, for effeciency,
//       little effort is done to restore state.
//-----------------------------------------------------------------------------
HRESULT XBFont::SetState()
{
    if( NULL == m_pd3dDevice )
        return E_FAIL;
    if( TRUE == m_bIsStateSet )
        return E_FAIL;

    m_pd3dDevice->SetTexture( 0, m_pTexture );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(FONT2DVERTEX) );
    m_pd3dDevice->SetVertexShader( D3DFVF_FONT2DVERTEX );
    m_pd3dDevice->SetPixelShader( NULL );

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
    m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,         D3DFILL_SOLID );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );

    m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

    m_bIsStateSet = TRUE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreState()
// Desc: Restores render state after rendering text. This function currently
//       does little (for performance), but is in place in case a custom app
//       would see reason to restore state after the rendering is complete.
//-----------------------------------------------------------------------------
HRESULT XBFont::RestoreState()
{
    if( NULL == m_pd3dDevice )
        return E_FAIL;
    if( FALSE == m_bIsStateSet )
        return E_FAIL;

    m_bIsStateSet = FALSE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws text as textured polygons.
//-----------------------------------------------------------------------------
void XBFont::DrawString(DisplayContext *theContext, const char *theStr, int theLen, int x, int y)
{
//void XBFont::DrawString(FLOAT fOriginX, FLOAT fOriginY, DWORD dwColor,
                           //const TCHAR* strText, DWORD dwFlags )
//{
	float fOriginX = x;
	float fOriginY = y;
	DWORD dwColor = theContext->mGraphics->GetColor();

    if( NULL == m_pd3dDevice )
        return;

    // Set the necessary renderstates if they are not already set
    BOOL bStateNeedsToBeSet = !m_bIsStateSet;
    if( bStateNeedsToBeSet )
        SetState();

    // Set the starting screen position
    FLOAT sx = fOriginX;
    FLOAT sy = fOriginY;


    // Fill vertex buffer
    FONT2DVERTEX* pVertices;
    DWORD         dwNumQuads = 0;
    m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0);

    // Set a flag so we can determine initial justification effects
    BOOL bStartingNewLine = TRUE;

    for(int i=0; i<theLen; i++)
    {
        // Get the current letter in the string
        TCHAR letter = *theStr++;

        // Skip invalid characters
        if( letter<m_cLowChar || letter>m_cHighChar )
            continue;

        // Get the glyph for this character
        GLYPH_ATTR* pGlyph = &m_Glyphs[letter-m_cLowChar];

        // Setup the screen coordinates (note the 0.5f shift value which is to
        // align texel centers with pixel centers)
        sx += pGlyph->wOffset;
        FLOAT sx1 = sx - 0.5f;
        FLOAT sx2 = sx - 0.5f + ((FLOAT)pGlyph->wWidth + 1);
        FLOAT sy1 = sy - 0.5f;
        FLOAT sy2 = sy - 0.5f + ((FLOAT)m_dwFontHeight + 1);
        sx += pGlyph->wAdvance;

        // Setup the texture coordinates (note the fudge factor for converting
        // from integer texel values to floating point texture coords).
        FLOAT tx1 = ( pGlyph->left   * ( m_dwTexWidth-1) ) / m_dwTexWidth;
        FLOAT ty1 = ( pGlyph->top    * (m_dwTexHeight-1) ) / m_dwTexHeight;
        FLOAT tx2 = ( pGlyph->right  * ( m_dwTexWidth-1) ) / m_dwTexWidth;
        FLOAT ty2 = ( pGlyph->bottom * (m_dwTexHeight-1) ) / m_dwTexHeight;

        // Set up the vertices (1 quad = 2 triangles = 6 vertices)
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx1,sy2,0.0f,0.0f), dwColor, tx1, ty2 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx1,sy1,0.0f,0.0f), dwColor, tx1, ty1 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx2,sy1,0.0f,0.0f), dwColor, tx2, ty1 );
        *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx2,sy2,0.0f,0.0f), dwColor, tx2, ty2 );
        dwNumQuads++;

        // If the vertex buffer is full, render it
        if( dwNumQuads*4 >= XBFONT_MAX_VERTICES )
        {
            // Unlock, render, and relock the vertex buffer
            m_pVB->Unlock();
            m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, dwNumQuads );
            m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0L );
            dwNumQuads = 0L;
        }
    }

    // If there's any vertices left in the vertex buffer, render it
    m_pVB->Unlock();
    if( dwNumQuads > 0 )
        m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, dwNumQuads );

    // Restore state if we explicity set it earlier in this function
    if( bStateNeedsToBeSet )
        RestoreState();
}
