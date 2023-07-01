#include "A3DSurfaceMan.h"
#include "A3DErrLog.h"
#include "A3DConfig.h"

A3DSurfaceMan::A3DSurfaceMan()
{
	m_pA3DDevice = NULL;
}

A3DSurfaceMan::~A3DSurfaceMan()
{
}

bool A3DSurfaceMan::Init(A3DDevice * pA3DDevice)
{	
	strcpy(m_szFolderName, "Surfaces");
	m_pA3DDevice = pA3DDevice;
	return true;
}

bool A3DSurfaceMan::Release()
{
	return true;
}

bool A3DSurfaceMan::Reset()
{
	return true;
}

bool A3DSurfaceMan::LoadCursorSurfaceFromFile(int nWidth, int nHeight, char * szFileName, A3DCOLOR colorKey, A3DSurface ** ppSurface)
{
	IDirect3DSurface8 * pDXSurface = NULL;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
	}
	else
	{
		HRESULT hval;
		
		if( strlen(szFileName) < 4 )
		{
			g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadCursorSurfaceFromFile(), Filename [%s] not valid", szFileName);
			return false;
		}

		D3DFORMAT format = D3DFMT_A8R8G8B8;
		hval = m_pA3DDevice->GetD3DDevice()->CreateImageSurface(nWidth, nHeight, format, &pDXSurface);
		if FAILED(hval) 
		{
			g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile CreateImageSurface Fail!");
			return false;
		}

		D3DXIMAGE_INFO image_info;

		AFileImage fileImage;
				   
		if( !fileImage.Open(m_szFolderName, szFileName, AFILE_OPENEXIST) )
		{
			pDXSurface->Release();
			g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile(), Can not locate [%s] in package!", szFileName);
			return false;
		}
		
		hval = D3DXLoadSurfaceFromFileInMemory(pDXSurface, NULL, NULL, fileImage.GetFileBuffer(), fileImage.GetFileLength(), NULL, D3DX_FILTER_TRIANGLE, colorKey, &image_info);
		if FAILED(hval) 
		{
			fileImage.Close();
			pDXSurface->Release();
			g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile create surface from file [%s] fail!", szFileName);
			return false;
		}
		fileImage.Close();
	}

	A3DSurface * pA3DSurface;
	pA3DSurface = new A3DSurface();
	if( NULL == pA3DSurface )
	{
		g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile Not enough memory!");
		return false;
	}

	if( !pA3DSurface->Init(m_pA3DDevice, pDXSurface, szFileName) )
	{
		pDXSurface->Release();
		g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile Init A3DSurface Fail!");
		return false;
	}

	*ppSurface = pA3DSurface;
	return true;
}

bool A3DSurfaceMan::LoadSurfaceFromFile(int nWidth, int nHeight, char * szFileName, A3DCOLOR colorKey, A3DSurface ** ppSurface)
{
	IDirect3DSurface8 * pDXSurface = NULL;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
	}
	else
	{
		HRESULT hval;
		char	szExt[5];

		if( strlen(szFileName) < 4 )
		{
			g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile Filename [%s] not valid", szFileName);
			return false;
		}

		D3DFORMAT format;
		strcpy(szExt, szFileName + strlen(szFileName) - 4);
		if( 0 == _stricmp(szExt, ".bmp") )
		{
			if( colorKey == 0 )
			{
				format = (D3DFORMAT) m_pA3DDevice->GetNormalTextureFormat();
			}
			else
			{
				//Has color key, so use A1R5G5B5 or A8R8G8B8
				format = (D3DFORMAT) m_pA3DDevice->GetColorKeyTextureFormat();
			}
		}
		else if( 0 == _stricmp(szExt, ".tga") )
		{
			//Use A4R4G4B4, or A8R8G8B8
			format = (D3DFORMAT) m_pA3DDevice->GetAlphaTextureFormat();
		}
		
		hval = m_pA3DDevice->GetD3DDevice()->CreateImageSurface(nWidth, nHeight, format, &pDXSurface);
		if FAILED(hval) 
		{
			g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile CreateImageSurface Fail!");
			return false;
		}

		D3DXIMAGE_INFO image_info;

		AFileImage fileImage;
				   
		if( !fileImage.Open(m_szFolderName, szFileName, AFILE_OPENEXIST) )
		{
			pDXSurface->Release();
			g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile(), Can not locate [%s] in package!", szFileName);
			return false;
		}
		
		hval = D3DXLoadSurfaceFromFileInMemory(pDXSurface, NULL, NULL, fileImage.GetFileBuffer(), fileImage.GetFileLength(), NULL, D3DX_FILTER_TRIANGLE, colorKey, &image_info);
		if FAILED(hval) 
		{
			fileImage.Close();
			pDXSurface->Release();
			g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile create surface from file [%s] fail!", szFileName);
			return false;
		}
		fileImage.Close();
	}

	A3DSurface * pA3DSurface;
	pA3DSurface = new A3DSurface();
	if( NULL == pA3DSurface )
	{
		g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile Not enough memory!");
		return false;
	}

	if( !pA3DSurface->Init(m_pA3DDevice, pDXSurface, szFileName) )
	{
		pDXSurface->Release();
		g_pA3DErrLog->ErrLog("A3DSurfaceMan::LoadSurfaceFromFile Init A3DSurface Fail!");
		return false;
	}

	*ppSurface = pA3DSurface;
	return true;
}

bool A3DSurfaceMan::ReleaseSurface(PA3DSurface& pSurface)
{
	pSurface->Release();
	delete pSurface;

	pSurface = NULL;
	return true;
}