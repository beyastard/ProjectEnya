#include "A3DMaterial.h"
#include "A3DMacros.h"

A3DMaterial::A3DMaterial()
{
	ZeroMemory(&m_MaterialParam, sizeof(A3DMATERIALPARAM));
	m_b2Sided = false;
	m_bAlphaMaterial = false;
	m_pA3DDevice = NULL;
}

A3DMaterial::~A3DMaterial()
{
}
							
bool A3DMaterial::Init(A3DDevice * pDevice)
{
	m_pA3DDevice = pDevice;

	//Set to a default material;
	m_MaterialParam.Ambient			= A3DCOLORVALUE(0.2f, 0.2f, 0.2f, 1.0f);
	m_MaterialParam.Diffuse			= A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	m_MaterialParam.Emissive		= A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 0.0f);
	m_MaterialParam.Specular		= A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	m_MaterialParam.Power			= 0.0f;
	m_b2Sided = false;
	m_bAlphaMaterial = false;

	return true;
}

bool A3DMaterial::Init(A3DDevice * pDevice, A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShininess)
{
	m_pA3DDevice = pDevice;

	return SetValues(clAmbient, clDiffuse, clEmissive, clSpecular, fShininess);
}

bool A3DMaterial::Init(A3DDevice * pDevice, A3DMATERIALPARAM& Params)
{
	m_pA3DDevice	= pDevice;
	m_MaterialParam	= Params;

	return true;
}

bool A3DMaterial::SetValues(A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShininess, bool b2Sided)
{
	m_MaterialParam.Ambient			= clAmbient;
	m_MaterialParam.Diffuse			= clDiffuse;
	m_MaterialParam.Emissive		= clEmissive;
	m_MaterialParam.Specular		= clSpecular;
	m_MaterialParam.Power			= fShininess;

	if( m_MaterialParam.Diffuse.a < 1.0f )
		m_bAlphaMaterial = true;
	else
		m_bAlphaMaterial = false;

	m_b2Sided = b2Sided;
	return true;
}

bool A3DMaterial::Release()
{
	return true;
}

bool A3DMaterial::Appear()
{
	//If this object is created outside D3D;
	if( m_pA3DDevice == NULL )
		return true;

	m_pA3DDevice->SetMaterial(&m_MaterialParam, m_b2Sided);
	return true;
}

bool A3DMaterial::SetDiffuse(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
{
	m_MaterialParam.Diffuse = A3DCOLORVALUE(r, g, b, a);
	return true;
}

bool A3DMaterial::Save(AFile * pFileToSave)
{
	if( pFileToSave->IsBinary() )
	{
		DWORD		dwWriteLength;
		char		szLineBuffer[2048];

		sprintf(szLineBuffer, "MATERIAL: %s", GetName());
		pFileToSave->Write(szLineBuffer, strlen(szLineBuffer) + 1, &dwWriteLength);

		pFileToSave->Write(&m_MaterialParam.Ambient, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_MaterialParam.Diffuse, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_MaterialParam.Emissive, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_MaterialParam.Specular, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_MaterialParam.Power, sizeof(FLOAT), &dwWriteLength);
		pFileToSave->Write(&m_b2Sided, sizeof(bool), &dwWriteLength);
	}
	else
	{			 
		char szLineBuffer[2048];
		sprintf(szLineBuffer, "MATERIAL: %s\n{", GetName());
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "Ambient:(%f, %f, %f, %f)", m_MaterialParam.Ambient.r, m_MaterialParam.Ambient.g, m_MaterialParam.Ambient.b, m_MaterialParam.Ambient.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Diffuse:(%f, %f, %f, %f)", m_MaterialParam.Diffuse.r, m_MaterialParam.Diffuse.g, m_MaterialParam.Diffuse.b, m_MaterialParam.Diffuse.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Emissive:(%f, %f, %f, %f)", m_MaterialParam.Emissive.r, m_MaterialParam.Emissive.g, m_MaterialParam.Emissive.b, m_MaterialParam.Emissive.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Specular:(%f, %f, %f, %f)", m_MaterialParam.Specular.r, m_MaterialParam.Specular.g, m_MaterialParam.Specular.b, m_MaterialParam.Specular.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Power:(%f)", m_MaterialParam.Power);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "2Sided:(%d)", m_b2Sided);
		pFileToSave->WriteLine(szLineBuffer);

		//<== "MATERIAL: {"
		pFileToSave->WriteLine("}");
	}
	return true;
}

bool A3DMaterial::Load(A3DDevice * pDevice, AFile * pFileToLoad)
{
	if( !Init(pDevice) )
		return false;

	if( pFileToLoad->IsBinary() )
	{
		DWORD		dwReadLength;
		char		szLineBuffer[2048];
		char		szResult[2048];

		pFileToLoad->ReadString(szLineBuffer, 2048, &dwReadLength);
		pFileToLoad->GetStringAfter(szLineBuffer, "MATERIAL: ", szResult);
		if( strlen(szResult) >= MAX_NAME )
			return false;
		SetName(szResult);

		pFileToLoad->Read(&m_MaterialParam.Ambient, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_MaterialParam.Diffuse, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_MaterialParam.Emissive, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_MaterialParam.Specular, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_MaterialParam.Power, sizeof(FLOAT), &dwReadLength);
		pFileToLoad->Read(&m_b2Sided, sizeof(bool), &dwReadLength);
	}
	else
	{
		char szLineBuffer[AFILE_LINEMAXLEN];
		char szResult[AFILE_LINEMAXLEN];
		int  nValue;
		DWORD dwReadLen;

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		pFileToLoad->GetStringAfter(szLineBuffer, "MATERIAL: ", szResult);
		SetName(szResult);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") )
			return false;

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Ambient:(%f, %f, %f, %f)", &m_MaterialParam.Ambient.r, &m_MaterialParam.Ambient.g, &m_MaterialParam.Ambient.b, &m_MaterialParam.Ambient.a);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Diffuse:(%f, %f, %f, %f)", &m_MaterialParam.Diffuse.r, &m_MaterialParam.Diffuse.g, &m_MaterialParam.Diffuse.b, &m_MaterialParam.Diffuse.a);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Emissive:(%f, %f, %f, %f)", &m_MaterialParam.Emissive.r, &m_MaterialParam.Emissive.g, &m_MaterialParam.Emissive.b, &m_MaterialParam.Emissive.a);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Specular:(%f, %f, %f, %f)", &m_MaterialParam.Specular.r, &m_MaterialParam.Specular.g, &m_MaterialParam.Specular.b, &m_MaterialParam.Specular.a);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Power:(%f)", &m_MaterialParam.Power);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);

		sscanf(szLineBuffer, "2Sided:(%d)", &nValue);
		m_b2Sided = nValue ? true : false;
		
		//<== "MATERIAL: {"
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;
	}

	return true;
}
