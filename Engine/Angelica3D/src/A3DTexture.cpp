#include "A3DPlatform.h"
#include "A3DTexture.h"
#include "A3DErrLog.h"
#include "A3DFuncs.h"
#include "A3DSurface.h"
#include "A3DTextureMan.h"
#include "A3DEngine.h"
#include "A3DConfig.h"

int GetMaxMipLevel(int nWidth, int nHeight)
{
	int			nMaxLevel = 1;
	int			nMipSize = 1;

	while(nMipSize < nWidth && nMipSize < nHeight)
	{
		nMaxLevel ++;
		nMipSize *= 2;
	}

	return nMaxLevel;
}

A3DTexture::A3DTexture()
{
	m_pA3DDevice	= NULL;
	m_bHWITexture	= false;

	m_pDXTexture	= NULL;
	m_szMapFile[0]	= '\0';

	m_bAlphaTexture = false;
	m_bShaderTexture = false;
	m_bDetailTexture = false;
	m_bLightMap = false;

	m_nDesiredMipLevel = 0; // Complete mip map chain will be created;
}
	
A3DTexture::~A3DTexture()
{
}

bool A3DTexture::Init(A3DDevice * pDevice)
{
	m_pA3DDevice	= pDevice;

	m_szMapFile[0]	= '\0';
	m_pDXTexture 	= NULL;
	m_bHWITexture	= false;

	m_bAlphaTexture = false;
	m_bShaderTexture = false;
	m_bDetailTexture = false;
	m_bLightMap = false;

	m_nDesiredMipLevel = 0; // Complete mip map chain will be created;
	return true;
}

bool A3DTexture::LoadFromFile(A3DDevice * pA3DDevice, char * szFullpath, char * szRelativePath)
{
	m_pA3DDevice = pA3DDevice;

	strncpy(m_szMapFile, szRelativePath, MAX_PATH);

	if( strstr(m_szMapFile, ".tga") || strstr(m_szMapFile, ".TGA") )
		m_bAlphaTexture = true;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER || g_pA3DConfig->GetFlagNoTextures() )
	{
		m_bHWITexture = true;
		return true;
	}

	if( !m_pA3DDevice )
		return true;

	if( m_bDetailTexture )
	{
		if( !LoadDetailTexture(pA3DDevice, szFullpath, szRelativePath) )
		{
			g_pA3DErrLog->ErrLog("A3DTexture::LoadFromFile(), call LoadDetailTexture() fail!");
			return false;
		}
	}
	else if( m_bLightMap )
	{
		if( !LoadLightMapTexture(pA3DDevice, szFullpath, szRelativePath) )
		{
			g_pA3DErrLog->ErrLog("A3DTexture::LoadFromFile(), call LoadLightMapTexture() fail!");
			return false;
		}
	}
	else
	{
		if( !LoadNormalTexture(pA3DDevice, szFullpath, szRelativePath) )
		{
			g_pA3DErrLog->ErrLog("A3DTexture::LoadFromFile(), call LoadNormalTexture() fail!");
			return false;
		}
	}

	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DENGINE_OBJECT_TEXTURE);
	return true;
}

bool A3DTexture::Release()
{
	if( m_pDXTexture )
	{
		if( m_pA3DDevice )
			m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DENGINE_OBJECT_TEXTURE);

		m_pDXTexture->Release();
		m_pDXTexture = NULL;
	}

	m_pA3DDevice = NULL;
	return true;
}

bool A3DTexture::Appear(int nLayer)
{
	//If this object is created outside D3D;
	if( m_pA3DDevice == NULL || m_bHWITexture )
		return true;

	if( m_pDXTexture )
		m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, m_pDXTexture);
	else
		m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, NULL);

	return true;
}

bool A3DTexture::Disappear(int nLayer)
{
	//If this object is created outside D3D;
	if( m_pA3DDevice == NULL || m_bHWITexture )
		return true;

	if( m_pDXTexture )
		m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, NULL);

	return true;
}

bool A3DTexture::Create(A3DDevice* pDevice, int iWidth, int iHeight, A3DFORMAT Format, int nMipLevel)
{
	if( m_pDXTexture )
	{
		g_pA3DErrLog->ErrLog("A3DTexture::Create This texture has been created, can not recreated!");
		return false;
	}

	if (!Init(pDevice))
		return false;

	m_szMapFile[0]  = '\0';
	m_nMipLevel = nMipLevel;

	int nMaxMipLevel = GetMaxMipLevel(iWidth, iHeight);
	if( m_nMipLevel <= 0 || m_nMipLevel > nMaxMipLevel ) m_nMipLevel = nMaxMipLevel;
	
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWITexture = true;
		return true;
	}

	if( !CreateDXTexture(iWidth, iHeight, Format, m_nMipLevel) )
		return false;

	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DENGINE_OBJECT_TEXTURE);
	return true;
}

bool A3DTexture::LockRect(RECT* pRect, void** ppData, int* iPitch, DWORD dwFlags, int nMipLevel)
{
	if (!m_pDXTexture || m_bHWITexture)
		return false;

	if( nMipLevel >= m_nMipLevel )
	{
		g_pA3DErrLog->ErrLog("A3DTexture::LockRect(), mip level [%d] larger than m_nMipLevel [%d]", nMipLevel, m_nMipLevel);
		return false;
	}

	D3DLOCKED_RECT LockedRect;

	if FAILED(m_pDXTexture->LockRect(nMipLevel, &LockedRect, pRect, 0))
		return false;

	*ppData = LockedRect.pBits;
	*iPitch = LockedRect.Pitch;

	return true;
}

bool A3DTexture::UnlockRect()
{
	if (!m_pDXTexture || m_bHWITexture)
		return false;

	if (FAILED(m_pDXTexture->UnlockRect(0)))
		return false;

	return true;
}

bool A3DTexture::UpdateFromSurface(A3DSurface * pA3DSurface, A3DRECT & rect)
{
	HRESULT hval;
	IDirect3DSurface8 * pDXSurface;
	hval = m_pDXTexture->GetSurfaceLevel(0, &pDXSurface);
	if FAILED(hval)
	{
		g_pA3DErrLog->ErrLog("A3DTexture::UpdateFromSurface(), GetSurfaceLevel Fail!");
		return false;
	}

	POINT ptDest;
	ptDest.x = ptDest.y = 0;

	hval = m_pA3DDevice->GetD3DDevice()->CopyRects(pA3DSurface->GetDXSurface(), (RECT *)&rect,
		1, pDXSurface, &ptDest);
	if FAILED(hval) 
	{
		g_pA3DErrLog->ErrLog("A3DTexture::UpdateFromSurface(), CopyRects Fail!");
		return false;
	}

	pDXSurface->Release();
	pDXSurface = NULL;

	return true;
}

bool A3DTexture::CreateFromSurface(A3DDevice * pA3DDevice, A3DSurface * pA3DSurface, A3DRECT& rect, int * pTexWidth, int * pTexHeight, A3DFORMAT fmtSurface)
{
	int nTexWidth = rect.right - rect.left;
	int nTexHeight = rect.bottom - rect.top;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWITexture = true;
		*pTexWidth = nTexWidth;
		*pTexHeight = nTexHeight;
		return true;
	}

	A3DFORMAT format;
	// For we can create a texture not from a specified surface, so we must check it here;
	if( pA3DSurface )
		format = pA3DSurface->GetFormat();
	else
		format = fmtSurface;

	//Now determine the texture's width and height;
	int nPowerOf2, i;
	nPowerOf2 = 4;
	for(i=2; i<=8; i++)
	{
		if( nTexWidth <= nPowerOf2 )
		{
			nTexWidth = nPowerOf2;
			break;
		}
		nPowerOf2 *= 2;
	}
	nPowerOf2 = 4;
	for(i=2; i<=8; i++)
	{
		if( nTexHeight <= nPowerOf2 )
		{
			nTexHeight = nPowerOf2;
			break;
		}
		nPowerOf2 *= 2;
	}

	bool bval;
	bval = Create(pA3DDevice, nTexWidth, nTexHeight, format, 1);
	if( !bval ) return false;

	if( pA3DSurface )
	{
		// Copy from the A3DSurface;
		HRESULT hval;
		IDirect3DSurface8 * pDXSurface;
		hval = m_pDXTexture->GetSurfaceLevel(0, &pDXSurface);
		if FAILED(hval)
		{
			g_pA3DErrLog->ErrLog("A3DTexture::CreateFromSurface GetSurfaceLevel Fail!");
			return false;
		}

		POINT ptDest;
		ptDest.x = ptDest.y = 0;

		hval = m_pA3DDevice->GetD3DDevice()->CopyRects(pA3DSurface->GetDXSurface(), (RECT *)&rect,
			1, pDXSurface, &ptDest);
		if FAILED(hval) 
		{
			g_pA3DErrLog->ErrLog("A3DTexture::CreateFromSurface CopyRects Fail!");
			return false;
		}

		pDXSurface->Release();
		pDXSurface = NULL;
	}

	*pTexWidth = nTexWidth;
	*pTexHeight = nTexHeight;

	return true;
}

bool A3DTexture::TickAnimation()
{
	return true;
}

bool A3DTexture::ExportToFile(char * szFullpath)
{
	if( m_bHWITexture ) return true;

	HRESULT hval;
	// Require DX 8.1
	hval = D3DXSaveTextureToFile(szFullpath, D3DXIFF_BMP, m_pDXTexture, NULL);
	if FAILED(hval) 
	{
		g_pA3DErrLog->ErrLog("A3DTexture::ExportToFile(), Fail to export to file [%s]", szFullpath);
		return false;
	}
	return true;
}

// This is only the wrapper function for creating a DX texture
bool A3DTexture::CreateDXTexture(int nWidth, int nHeight, A3DFORMAT format, int nMipLevel)
{
	if (FAILED(m_pA3DDevice->GetD3DDevice()->CreateTexture(nWidth, nHeight, nMipLevel,
              0, (D3DFORMAT)format, D3DPOOL_MANAGED, &m_pDXTexture)))
	{
		g_pA3DErrLog->ErrLog("A3DTexture::CreateDXTExture(), Fail in creating format[%d], [%d]x[%d], [%d]mip texture", format, nWidth, nHeight, nMipLevel);
		return false;
	}

	return true;
}

// This function load the texture as a normal texture;
// NOTE: we haven't set the m_nMipLevel variable correctly.
bool A3DTexture::LoadNormalTexture(A3DDevice * pA3DDevice, char * szFullpath, char  *szRelativePath)
{
	D3DXIMAGE_INFO  image_info;
	HRESULT			hval;

	AFileImage fileImage;

	if( !fileImage.Open(szFullpath, AFILE_OPENEXIST) )
	{
		g_pA3DErrLog->ErrLog("A3DTexture::LoadNormalTexture(), Can not init the file to be a fileimage [%s]!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}
	
	A3DFORMAT fmtTarget;
	
	if( strstr(szRelativePath, ".dds") )
	{
		fmtTarget = (A3DFORMAT) D3DFMT_UNKNOWN;
	}
	else
	{
		if( !m_bAlphaTexture )
			fmtTarget = m_pA3DDevice->GetNormalTextureFormat();
		else
			fmtTarget = m_pA3DDevice->GetAlphaTextureFormat();
	}

	hval = D3DXCreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), fileImage.GetFileBuffer(), fileImage.GetFileLength(), D3DX_DEFAULT, D3DX_DEFAULT, m_nDesiredMipLevel, 0, (D3DFORMAT) fmtTarget,
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &image_info, NULL, &m_pDXTexture);
	if( D3D_OK != hval )
	{
		fileImage.Close();
		g_pA3DErrLog->ErrLog("A3DTexture::LoadNormalTexture() Can't load %s!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}

	// alpha flag of .dds file must be determined from the file
	if( strstr(szRelativePath, ".dds") )
	{
		if( image_info.Format == D3DFMT_A8R8G8B8 || image_info.Format == D3DFMT_A4R4G4B4 ||
			image_info.Format == D3DFMT_DXT3 || image_info.Format == D3DFMT_DXT5 )
			m_bAlphaTexture = true;
	}
	m_nMipLevel = image_info.MipLevels;
	DWORD dwDownrate = 1;
	
	if( g_pA3DConfig->GetTextureQuality() == A3DTEXTURE_QUALITY_NULL )
	{
		if( !g_pA3DConfig->GetFlagHQTexture() )
			dwDownrate = 2;
	}
	else
	{
		if( g_pA3DConfig->GetTextureQuality() == A3DTEXTURE_QUALITY_MEDIUM )
			dwDownrate = 2;
		else if( g_pA3DConfig->GetTextureQuality() == A3DTEXTURE_QUALITY_LOW )
			dwDownrate = 4;
	}
	
	if( dwDownrate > 1 && image_info.Width > dwDownrate && image_info.Height > dwDownrate )
	{
		 // Now we should load half size textures;
		m_pDXTexture->Release();
		m_pDXTexture = NULL;

		UINT width, height;
		width = image_info.Width / dwDownrate;
		height = image_info.Height / dwDownrate;

		hval = D3DXCreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), fileImage.GetFileBuffer(), fileImage.GetFileLength(), width, height, m_nDesiredMipLevel, 0, (D3DFORMAT) fmtTarget,
			D3DPOOL_MANAGED, D3DX_FILTER_BOX, D3DX_DEFAULT, 0, &image_info, NULL, &m_pDXTexture);
		if( D3D_OK != hval )
		{
			fileImage.Close();
			g_pA3DErrLog->ErrLog("A3DTexture::LoadNormalTexture(), Can't load %s!", szRelativePath);
			m_pDXTexture = NULL;
			return true;
		}
		m_nMipLevel = image_info.MipLevels;
	}

	fileImage.Close();
	return true;
}

bool A3DTexture::LoadDetailTexture(A3DDevice * pA3DDevice, char * szFullpath, char  *szRelativePath)
{
	IDirect3DTexture8	*pDXTexture;
	D3DXIMAGE_INFO		image_info;
	HRESULT				hval;

	AFileImage fileImage;

	if( !fileImage.Open(szFullpath, AFILE_OPENEXIST) )
	{
		g_pA3DErrLog->ErrLog("A3DTexture::LoadDetailTexture(), Can not init the file to be a fileimage [%s]!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}
	
	// For detail texture, we always create A3DFMT_X8R8G8B8 (or A3DFMT_A8R8G8B8 for voodoo5) format texture;
	hval = D3DXCreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), fileImage.GetFileBuffer(), fileImage.GetFileLength(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &image_info, NULL, &pDXTexture);
	if( D3D_OK != hval )
	{
		fileImage.Close();
		g_pA3DErrLog->ErrLog("A3DTexture::LoadDetailTexture() Can't load %s!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}

	fileImage.Close();
	
	D3DSURFACE_DESC	desc;
	pDXTexture->GetLevelDesc(0, &desc);

	if( desc.Format != D3DFMT_X8R8G8B8 && desc.Format != D3DFMT_A8R8G8B8 && desc.Format != D3DFMT_R5G6B5 &&
		desc.Format != D3DFMT_X1R5G5B5 )
	{
		g_pA3DErrLog->ErrLog("A3DTexture::LoadDetailTexture(), Texture format unsupported!");
		return false;
	}

	UINT width, height;
	width = desc.Width;
	height = desc.Height;

	// Create a mip map chain, the more deep the level are, the more far the clear range is
	// this max depth should be determined by texture size and the desired max clear distance
	int nMipLevel = GetMaxMipLevel(width, height);

	if( nMipLevel > 7 ) nMipLevel = 7;

	m_nMipLevel = nMipLevel;

	A3DFORMAT texFormat;
	if( desc.Format == D3DFMT_A8R8G8B8 ) // it must be voodoo5
		texFormat = A3DFMT_A8R8G8B8;
	else
		texFormat = A3DFMT_X8R8G8B8;
	if( !CreateDXTexture(width, height, texFormat, nMipLevel) )
	{
		pDXTexture->Release();
		g_pA3DErrLog->ErrLog("A3DTexture::LoadDetailTexture(), Can not create mip map chain!");
		return false;
	}
	
	D3DLOCKED_RECT  LockedRectOrg;
	RECT			rectOrg;
	LPBYTE			pDataOrg;
	int				nPitchOrg;
	SetRect(&rectOrg, 0, 0, width, height);

	if FAILED(pDXTexture->LockRect(0, &LockedRectOrg, &rectOrg, 0))
	{
		pDXTexture->Release();
		g_pA3DErrLog->ErrLog("A3DTexture::LoadDetailTexture(), Lock original texture fail!");
		return false;
	}
	pDataOrg = (LPBYTE) LockedRectOrg.pBits;
	nPitchOrg = LockedRectOrg.Pitch;

	D3DLOCKED_RECT  LockedRectMap;
	RECT			rectMap;
	LPBYTE			pDataMap;
	int				nPitchMap;
	SetRect(&rectMap, 0, 0, width, height);

	int nSubSize = 1;
	for(int idLevel=0; idLevel<m_nMipLevel; idLevel ++)
	{
		if FAILED(m_pDXTexture->LockRect(idLevel, &LockedRectMap, &rectMap, 0))
		{
			pDXTexture->Release();
			g_pA3DErrLog->ErrLog("A3DTexture::LoadDetailTexture(), Lock mipmap texture fail!");
			return false;
		}
		pDataMap = (LPBYTE) LockedRectMap.pBits;
		nPitchMap = LockedRectMap.Pitch;
	
		for(int x=0; x<rectMap.right; x++)
		{
			for(int y=0; y<rectMap.bottom; y++)
			{
				DWORD * pColor = (DWORD *) (pDataMap + nPitchMap * y + x * 4);
				int		nOX, nOY;
				int		r=0, g=0, b=0;

				if( idLevel == m_nMipLevel - 1 )
				{
					*pColor = A3DCOLORRGBA(128, 128, 128, 255);
					//*pColor = A3DCOLORRGBA(0, 0, 0, 255);
				}
				else
				{
					// we use a box resample algorithm;
					nOY = y * nSubSize;
					for(int sy=0; sy<nSubSize; sy++)
					{
						nOX = x * nSubSize;
						for(int sx=0; sx<nSubSize; sx++)
						{
							switch(desc.Format)
							{
							case D3DFMT_X1R5G5B5:
								WORD colorOrg1;
								colorOrg1 = *(WORD *) (pDataOrg + nPitchOrg * nOY + nOX * 2);
								r += (colorOrg1 & 0x7c00) >> 10 << 3;
								g += (colorOrg1 & 0x03e0) >> 5 << 3;
								b += (colorOrg1 & 0x1f) << 3;
								break;

							case D3DFMT_R5G6B5:
								WORD colorOrg2;
								colorOrg2 = *(WORD *) (pDataOrg + nPitchOrg * nOY + nOX * 2);
								r += (colorOrg2 & 0x8f00) >> 11 << 3;
								g += (colorOrg2 & 0x07e0) >> 5 << 3;
								b += (colorOrg2 & 0x1f) << 3;
								break;

							case D3DFMT_X8R8G8B8:
							case D3DFMT_A8R8G8B8:
								DWORD colorOrg3;
								colorOrg3 = *(DWORD *) (pDataOrg + nPitchOrg * nOY + nOX * 4);
								r += (colorOrg3 & 0x00ff0000) >> 16;
								g += (colorOrg3 & 0xff00) >> 8;
								b += colorOrg3 & 0xff;
								break;
							default:
								break;
							}
							nOX ++;
						}
						nOY ++;
					}
					r /= (nSubSize * nSubSize);
					g /= (nSubSize * nSubSize);
					b /= (nSubSize * nSubSize);
					*pColor = A3DCOLORRGBA(r, g, b, 255);
				}
			}
		}

		m_pDXTexture->UnlockRect(idLevel);

		rectMap.right /= 2;
		rectMap.bottom /= 2;
		nSubSize *= 2;
	}
	
	pDXTexture->UnlockRect(0);
	pDXTexture->Release();
	return true;
}

// When loading a lightmap, we did not do any mip-maping
bool A3DTexture::LoadLightMapTexture(A3DDevice * pA3DDevice, char * szFullpath, char * szRelativePath)
{
	D3DXIMAGE_INFO  image_info;
	HRESULT			hval;

	AFileImage fileImage;

	if( !fileImage.Open(szFullpath, AFILE_OPENEXIST) )
	{
		g_pA3DErrLog->ErrLog("A3DTexture::LoadNormalTexture(), Can not init the file to be a fileimage [%s]!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}
	
	m_nMipLevel = 1;
	A3DFORMAT fmtTarget;
	fmtTarget = m_pA3DDevice->GetNormalTextureFormat();
	
	hval = D3DXCreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), fileImage.GetFileBuffer(), fileImage.GetFileLength(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &image_info, NULL, &m_pDXTexture);
	if( D3D_OK != hval )
	{
		fileImage.Close();
		g_pA3DErrLog->ErrLog("A3DTexture::LoadNormalTexture() Can't load %s!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}

	fileImage.Close();
	return true;
}