#include "A3DMoxMan.h"
#include "A3DErrLog.h"
#include "A3DFuncs.h"
#include "A3DEngine.h"

A3DMoxMan::A3DMoxMan()
{
	m_pA3DDevice = NULL;
}

A3DMoxMan::~A3DMoxMan()
{
}

bool A3DMoxMan::Init(A3DDevice * pDevice)
{
	strcpy(m_szFolderName, "Models");
	m_pA3DDevice = pDevice;
 	m_ListMox.Init();

	return true;
}

bool A3DMoxMan::Release()
{
	ALISTELEMENT * pThisMoxElement = m_ListMox.GetFirst();

	while( pThisMoxElement != m_ListMox.GetTail() )
	{
		MOXRECORD * pMox = (MOXRECORD *) pThisMoxElement->pData;
		
		A3DFrame * pFrame = pMox->pFrame;
		pFrame->Release();
		delete pFrame;
		pFrame = NULL;

		free(pMox);
		pThisMoxElement = pThisMoxElement->pNext;
	}

	m_ListMox.Release();
	return true;
}

bool A3DMoxMan::FindMoxFile(char * szFilename, A3DFrame ** ppFrame)
{
	ALISTELEMENT * pThisMoxElement = m_ListMox.GetFirst();

	while( pThisMoxElement != m_ListMox.GetTail() )
	{
		MOXRECORD * pMox = (MOXRECORD *) pThisMoxElement->pData;
		if( 0 == _stricmp(szFilename, pMox->szFilename) )
		{
			*ppFrame = pMox->pFrame;
			return true;
		}
		pThisMoxElement = pThisMoxElement->pNext;
	}
	return true;
}

bool A3DMoxMan::LoadMoxFile(char * szFilename, A3DFrame **ppFrame)
{
	A3DFrame * pOrgFrame = NULL;
	*ppFrame = NULL;

	if( !FindMoxFile(szFilename, &pOrgFrame) )
		return false;

	if( pOrgFrame )
		return DuplicateFrame(pOrgFrame, ppFrame);
	
	//Now open the file;
	AFileImage aFile;
	A3DFrame *  pNewFrame = NULL;

	pNewFrame = new A3DFrame();
	if( NULL == pNewFrame )
	{
		g_pA3DErrLog->ErrLog("A3DMoxMan::LoadMoxFile Not enough memory!");
		return false;
	}
	if( !aFile.Open(m_szFolderName, szFilename, AFILE_OPENEXIST) )
	{
		g_pA3DErrLog->ErrLog("A3DMoxMan::LoadMoxFile [%s] can not be found!", szFilename);
		if( pNewFrame )	delete pNewFrame;
		return false;
	}
	if( !pNewFrame->Load(m_pA3DDevice, &aFile) )
	{								
		g_pA3DErrLog->ErrLog("A3DMoxMan::LoadMoxFile() File [%s] parsed error!", szFilename);
		if( pNewFrame ) delete pNewFrame;
		aFile.Close();
		return false;
	}
	aFile.Close();

	pNewFrame->SetName(szFilename);

	// Calculate automatic aabbs;
	// But this calculation is only applied to these that has no user defined obbs
	if( !pNewFrame->BuildAutoAABB() )
		return false;

	// If the obb is need to build?
	if( m_pA3DDevice->GetA3DEngine()->GetUseOBBFlag() && !pNewFrame->BuildAutoOBB() )
		return false;

	MOXRECORD * pNewMox = (MOXRECORD *) malloc(sizeof(MOXRECORD));
	if( NULL == pNewMox ) 
	{
		g_pA3DErrLog->ErrLog("A3DMoxMan::LoadMoxFile Not enough Memory!");
		if( pNewFrame )
		{
			pNewFrame->Release();
			delete pNewFrame;
		}
		return false;
	}
	strncpy(pNewMox->szFilename, szFilename, MAX_PATH);//AGetFileTitle(szFilename, pNewMox->szFilename, MAX_PATH);
	pNewMox->pFrame = pNewFrame;
	
	if( !m_ListMox.Append((LPVOID)pNewMox) )
		return false;

	return DuplicateFrame(pNewFrame, ppFrame);
}

bool A3DMoxMan::Reset()
{
	ALISTELEMENT * pThisMoxElement = m_ListMox.GetFirst();

	while( pThisMoxElement != m_ListMox.GetTail() )
	{
		MOXRECORD * pMox = (MOXRECORD *) pThisMoxElement->pData;
		
		A3DFrame * pFrame = pMox->pFrame;
		pFrame->Release();
		delete pFrame;
		pFrame = NULL;

		free(pMox);
		pThisMoxElement = pThisMoxElement->pNext;
	}

	m_ListMox.Reset();

	return true;
}

bool A3DMoxMan::ReleaseFrame(A3DFrame *& pFrame)
{
	pFrame->Release();
	delete pFrame;

	pFrame = NULL;
	return true;
}

bool A3DMoxMan::DuplicateFrame(A3DFrame * pOrgFrame, A3DFrame ** ppNewFrame)
{
	A3DFrame * pNewFrame = new A3DFrame();
	if( NULL == pNewFrame )
	{
		g_pA3DErrLog->ErrLog("A3DMoxMan::DuplicateFrame() Not enough memory!");
		return false;
	}

	if( !pNewFrame->Duplicate(m_pA3DDevice, pOrgFrame) )
	{
		g_pA3DErrLog->ErrLog("A3DMoxMan::DuplicateFrame(), Duplicate child frame fail!");
		return false;
	}

	*ppNewFrame = pNewFrame;
	return true;
}