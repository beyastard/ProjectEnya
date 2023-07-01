/*
 * FILE: AWorldInit.H
 *
 * DESCRIPTION : helper classes to link database to the world compent
 *				 initial struct	
 *
 * CREATED BY: MengXin
 *
 * HISTORY:	2002 - 9 - 16
 *
 * Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
 */

#ifndef __AWORLDINIT_H__
#define __AWORLDINIT_H__

#include "ABaseA3D.h"
#include "ADebug.h"
#include "AData.h"

// 游戏中出现的天空类型
EXTERN_DEFINESET(int, global_skytypes)

typedef struct tagAWORLD_INIT_ENVIRONMENT
{
	A3DWORLD_WEATHER	EnvironmentType;
	A3DCOLOR			fogColor;
	FLOAT				vFogStart;
	FLOAT				vFogEnd;
	FLOAT				vFogDensity;
	A3DCOLOR			colorAmbient;
	A3DCOLOR			colorDirectional;
	A3DVECTOR3			vecLightDir;

	tagAWORLD_INIT_ENVIRONMENT()
	{
		memset(this,0,sizeof(*this));
	}

} AWORLD_INIT_ENVIRONMENT,*PAWORLD_INIT_ENVIRONMENT;

typedef	struct tagAWORLD_INIT_TERRAIN  
{
	TERRAINPARAM	terrainParam;
	char			szTexBase[MAX_PATH];
	char			szHeightMap[MAX_PATH];
	char			szTexMap[MAX_PATH];
	
	tagAWORLD_INIT_TERRAIN()
	{
		memset(this,0,sizeof(*this));
	}

} AWORLD_INIT_TERRAIN,*PAWORLD_INIT_TERRAIN;

typedef	struct tagAWORLD_INIT_SCENE 
{
	char			szESPFile[MAX_PATH];
	char			szBSPFile[MAX_PATH];
	char			szModFile[MAX_PATH];

	tagAWORLD_INIT_SCENE()
	{
		memset(this,0,sizeof(*this));
	}

} AWORLD_INIT_SCENE,*PAWORLD_INIT_SCENE;

typedef	struct tagAWORLD_INIT_SKY 
{
	char	szSkySurround0[MAX_PATH];
	char	szSkySurround1[MAX_PATH];
	char	szSkySurround2[MAX_PATH];
	char	szSkySurround3[MAX_PATH];
	char	szSkySurround4[MAX_PATH];
	char	szSkySurround5[MAX_PATH];
	
	char	szSkyFront[MAX_PATH];
	char	szSkyBack[MAX_PATH];
	char	szSkyCap[MAX_PATH];

	char	szSkyCloud[MAX_PATH];
	char	szSkyGradient[MAX_PATH];

	int		skyType;

	tagAWORLD_INIT_SKY()
	{
		memset(this,0,sizeof(*this));
	}

} AWORLD_INIT_SKY,*PAWORLD_INIT_SKY;

class AEnvironmentInit : public AData
{
public:

	DECLARE_ASERIAL(AEnvironmentInit)

protected:

	long				Weather;
	unsigned long		FogColor;
	float				FogStart;
	float				FogEnd;
	float				FogDensity;
	unsigned long		Ambient;
	unsigned long		Directional;
	A3DVECTOR3			LightDir;


	STATICPROPERIES_BEGIN(AData, AEnvironmentInit)
 		STATICPROPERTY(AEnvironmentInit, "天气类型", long, Weather, NULL, NULL, WAY_DEFAULT, NULL)
		STATICPROPERTY(AEnvironmentInit, "雾颜色", unsigned long, FogColor, NULL, NULL, WAY_COLOR, NULL)
		STATICPROPERTY(AEnvironmentInit, "雾起始距离",float, FogStart, NULL, NULL, WAY_DEFAULT, NULL)
		STATICPROPERTY(AEnvironmentInit, "雾终止距离",float, FogEnd, NULL, NULL, WAY_DEFAULT, NULL)
		STATICPROPERTY(AEnvironmentInit, "雾浓度",float, FogDensity, NULL, NULL, WAY_DEFAULT, NULL)
		STATICPROPERTY(AEnvironmentInit, "环境光",unsigned long, Ambient, NULL, NULL, WAY_COLOR, NULL)
		STATICPROPERTY(AEnvironmentInit, "直射光",unsigned long, Directional, NULL, NULL, WAY_COLOR, NULL)
		STATICPROPERTY(AEnvironmentInit, "直射光方向",A3DVECTOR3, LightDir, NULL, NULL, WAY_DEFAULT, NULL)
	STATICPROPERIES_END(AEnvironmentInit)

public:

	AEnvironmentInit()
	{
		INITIALIZE_STATICPROPERIES(AEnvironmentInit);
		Weather		=	0;
		FogColor	=	A3DCOLORRGBA(192,192,192,255);
		FogStart	=	800.0f;
		FogEnd		=	850.0f;
		FogDensity	=	1.0;
		Ambient		=	A3DCOLORRGBA(255,255,255,255);
		Directional	=	0;
		LightDir	=	A3DVECTOR3(1,1,0);
	}
	void ToKernelDatum(AWORLD_INIT_ENVIRONMENT* __pDatum)
	{
		if(!__pDatum) return;
		__pDatum->EnvironmentType = static_cast<A3DWORLD_WEATHER>(Weather);
		__pDatum->fogColor = FogColor;
		__pDatum->vFogStart = FogStart;
		__pDatum->vFogEnd = FogEnd;
		__pDatum->vFogDensity = FogDensity;
		__pDatum->colorAmbient = Ambient;
		__pDatum->colorDirectional = Directional;
		__pDatum->vecLightDir = LightDir;
	}
	void FromKernelDatum(const AWORLD_INIT_ENVIRONMENT* __pDatum)
	{
		if(!__pDatum) return;
		Weather=__pDatum->EnvironmentType;
		FogColor=__pDatum->fogColor;
		FogStart=__pDatum->vFogStart;
		FogEnd=__pDatum->vFogEnd;
		FogDensity=__pDatum->vFogDensity;
		Ambient=__pDatum->colorAmbient;
		Directional=__pDatum->colorDirectional;
		LightDir=__pDatum->vecLightDir;
	}
	bool Save(AArchive& ar)
	{
		if(ar.Write(&Weather,sizeof(Weather))==false) return false;
		if(ar.Write(&FogColor,sizeof(FogColor))==false) return false;
		if(ar.Write(&FogStart,sizeof(FogStart))==false) return false;
		if(ar.Write(&FogEnd,sizeof(FogEnd))==false) return false;
		if(ar.Write(&FogDensity,sizeof(FogDensity))==false) return false;
		if(ar.Write(&Ambient,sizeof(Ambient))==false) return false;
		if(ar.Write(&Directional,sizeof(Directional))==false) return false;
		if(ar.Write(&LightDir,sizeof(LightDir))==false) return false;
		int nVersion;
		nVersion = AEnvironmentInit::GetClassInfo()->GetVersion();
		if(ar.Write(&nVersion,sizeof(nVersion))==false)
		{
			assert(0);
			return false;
		}

		return true;
	}
	bool Load(AArchive& ar)
	{
		if(ar.Read(&Weather,sizeof(Weather))==false) return false;
		if(ar.Read(&FogColor,sizeof(FogColor))==false) return false;
		if(ar.Read(&FogStart,sizeof(FogStart))==false) return false;
		if(ar.Read(&FogEnd,sizeof(FogEnd))==false) return false;
		if(ar.Read(&FogDensity,sizeof(FogDensity))==false) return false;
		if(ar.Read(&Ambient,sizeof(Ambient))==false) return false;
		if(ar.Read(&Directional,sizeof(Directional))==false) return false;
		if(ar.Read(&LightDir,sizeof(LightDir))==false) return false;
		int nVersion;
		if(ar.Read(&nVersion,sizeof(nVersion))==false)
		{
			assert(0);
			return false;
		}
		//利用这个版本号进行版本控制
		return true;
	}
	virtual ~AEnvironmentInit(){}
};

class ASkyInit : public AData
{
public:

	enum
	{
		SKY_BOX,		// 盒状天空
		SKY_SPHERE,		// 球状天空
		SKY_GRADIENT	// 渐变天空
	};

	DECLARE_ASERIAL(ASkyInit)

protected:

	AUString m_strSurround0;
	AUString m_strSurround1;
	AUString m_strSurround2;
	AUString m_strSurround3;
	AUString m_strSurround4;
	AUString m_strSurround5;

	int		m_int_skytype;		// 天空类型

	AUString	m_str_textfront;	// 前方纹理
	AUString	m_str_textback;		// 后方纹理
	AUString	m_str_textflat;		// 浮动云图
	AUString	m_str_textcap;		// 顶部云图

	AUString	m_str_cloud;		// 浮动云图
	AUString	m_str_gradient;		// 梯度云图

	STATICPROPERIES_BEGIN(AData, ASkyInit)
		
		STATICPROPERTY(ASkyInit, "天空类型", int, m_int_skytype, &global_skytypes, NULL, WAY_DEFAULT, NULL)
		//STATICPROPERTY(ASkyInit, "盒状天空文件0", AUString, m_strSurround0, NULL, NULL, WAY_FILENAME, "textures\\sky")
		//STATICPROPERTY(ASkyInit, "盒状天空文件1", AUString, m_strSurround1, NULL, NULL, WAY_FILENAME, "textures\\sky")
		//STATICPROPERTY(ASkyInit, "盒状天空文件2", AUString, m_strSurround2, NULL, NULL, WAY_FILENAME, "textures\\sky")
		//STATICPROPERTY(ASkyInit, "盒状天空文件3", AUString, m_strSurround3, NULL, NULL, WAY_FILENAME, "textures\\sky")
		//STATICPROPERTY(ASkyInit, "盒状天空文件4", AUString, m_strSurround4, NULL, NULL, WAY_FILENAME, "textures\\sky")
		//STATICPROPERTY(ASkyInit, "盒状天空文件5", AUString, m_strSurround5, NULL, NULL, WAY_FILENAME, "textures\\sky")
		STATICPROPERTY(ASkyInit, "球形天空-前方云图", AUString, m_str_textfront, NULL, NULL, WAY_FILENAME, "textures\\sky")
		STATICPROPERTY(ASkyInit, "球形天空-后方云图", AUString, m_str_textback, NULL, NULL, WAY_FILENAME, "textures\\sky")
		//STATICPROPERTY(ASkyInit, "球形天空-浮动云图(已废弃)", AUString, m_str_textflat, NULL, NULL, WAY_FILENAME, "textures\\sky")
		STATICPROPERTY(ASkyInit, "球形天空-顶部云图", AUString, m_str_textcap, NULL, NULL, WAY_FILENAME, "textures\\sky")
		//STATICPROPERTY(ASkyInit, "渐变天空-云图", AUString, m_str_cloud, NULL, NULL, WAY_FILENAME, "textures\\sky")
		//STATICPROPERTY(ASkyInit, "渐变天空-梯度图", AUString, m_str_gradient, NULL, NULL, WAY_FILENAME, "textures\\sky")

	STATICPROPERIES_END(ASkyInit)

public:

	ASkyInit()
	{
		INITIALIZE_STATICPROPERIES(ASkyInit);
		m_int_skytype = SKY_BOX;	// 默认状态下为盒状天空
	}

	void ToKernelDatum(AWORLD_INIT_SKY* __pDatum)
	{
		if(!__pDatum) return;
		
		strcpy(__pDatum->szSkySurround0,m_strSurround0);
		strcpy(__pDatum->szSkySurround1,m_strSurround1);
		strcpy(__pDatum->szSkySurround2,m_strSurround2);
		strcpy(__pDatum->szSkySurround3,m_strSurround3);
		strcpy(__pDatum->szSkySurround4,m_strSurround4);
		strcpy(__pDatum->szSkySurround5,m_strSurround5);

		strcpy(__pDatum->szSkyFront,m_str_textfront);
		strcpy(__pDatum->szSkyBack,m_str_textback);
		strcpy(__pDatum->szSkyCap,m_str_textcap);

		strcpy(__pDatum->szSkyCloud,m_str_cloud);
		strcpy(__pDatum->szSkyGradient,m_str_gradient);

		__pDatum->skyType = m_int_skytype;
	}

	void FromKernelDatum(const AWORLD_INIT_SKY* __pDatum)
	{
		if(!__pDatum) return;
		
		m_strSurround0=__pDatum->szSkySurround0;
		m_strSurround1=__pDatum->szSkySurround1;
		m_strSurround2=__pDatum->szSkySurround2;
		m_strSurround3=__pDatum->szSkySurround3;
		m_strSurround4=__pDatum->szSkySurround4;
		m_strSurround5=__pDatum->szSkySurround5;

		m_str_textfront=__pDatum->szSkyFront;
		m_str_textback=__pDatum->szSkyBack;
		m_str_textcap=__pDatum->szSkyCap;

		m_str_cloud=__pDatum->szSkyCloud;
		m_str_gradient=__pDatum->szSkyGradient;

		m_int_skytype = __pDatum->skyType;
	}
	bool Load(AArchive& ar)
	{
		if(ar.ReadString(m_strSurround0)==false) return false;
		if(ar.ReadString(m_strSurround1)==false) return false;
		if(ar.ReadString(m_strSurround2)==false) return false;
		if(ar.ReadString(m_strSurround3)==false) return false;
		if(ar.ReadString(m_strSurround4)==false) return false;
		if(ar.ReadString(m_strSurround5)==false) return false;
		int nVersion;
		if(ar.Read(&nVersion,sizeof(nVersion))==false)
		{
			assert(0);
			return false;
		}
		//利用这个版本号进行版本控制
		if(nVersion > 0x0100)
		{
			ar.Read(&m_int_skytype, sizeof(m_int_skytype));
			ar.ReadString(m_str_textfront);
			ar.ReadString(m_str_textback);	
			ar.ReadString(m_str_textflat);	
			ar.ReadString(m_str_textcap);	
		}
		//利用这个版本号进行版本控制
		if(nVersion > 0x0200)
		{
			ar.ReadString(m_str_cloud);
			ar.ReadString(m_str_gradient);	
		}
		return true;
	}
	bool Save(AArchive& ar)
	{
		if(ar.WriteString(m_strSurround0)==false) return false;
		if(ar.WriteString(m_strSurround1)==false) return false;
		if(ar.WriteString(m_strSurround2)==false) return false;
		if(ar.WriteString(m_strSurround3)==false) return false;
		if(ar.WriteString(m_strSurround4)==false) return false;
		if(ar.WriteString(m_strSurround5)==false) return false;

		int nVersion;
		nVersion = ASkyInit::GetClassInfo()->GetVersion();
		if(ar.Write(&nVersion,sizeof(nVersion))==false)
		{
			assert(0);
			return false;
		}

		ar.Write(&m_int_skytype, sizeof(m_int_skytype));
		ar.WriteString(m_str_textfront);
		ar.WriteString(m_str_textback);	
		ar.WriteString(m_str_textflat);	
		ar.WriteString(m_str_textcap);	
		ar.WriteString(m_str_cloud);
		ar.WriteString(m_str_gradient);	
		return true;
	}
	virtual ~ASkyInit(){}
};

class ASceneInit : public AData
{
public:

	DECLARE_ASERIAL(ASceneInit)
	
protected:

	AUString m_strESPName;
	AUString m_strBSPName;
	AUString m_strModelName;


	STATICPROPERIES_BEGIN(AData, ASceneInit)
		STATICPROPERTY(ASceneInit, "ESP 文件", AUString, m_strESPName, NULL, NULL, WAY_FILENAME, "")
		STATICPROPERTY(ASceneInit, "BSP 文件", AUString, m_strBSPName, NULL, NULL, WAY_FILENAME, "")
		STATICPROPERTY(ASceneInit, "模型文件", AUString, m_strModelName, NULL, NULL, WAY_FILENAME, "")
	STATICPROPERIES_END(ASceneInit)

public:

	ASceneInit()
	{
		INITIALIZE_STATICPROPERIES(ASceneInit);
		//m_strModelName	= "models\\maya.mox";
		//m_strESPName	= "models\\maya.esp";
	}
	void ToKernelDatum(AWORLD_INIT_SCENE* __pDatum)
	{
		if(!__pDatum) return;
		strcpy(__pDatum->szESPFile,m_strESPName);
		strcpy(__pDatum->szBSPFile,m_strBSPName);
		strcpy(__pDatum->szModFile,m_strModelName);
	}
	void FromKernelDatum(const AWORLD_INIT_SCENE* __pDatum)
	{
		if(!__pDatum) return;
		m_strESPName = __pDatum->szESPFile;
		m_strBSPName = __pDatum->szBSPFile;
		m_strModelName = __pDatum->szModFile;
	}
	bool Load(AArchive & ar)
	{
		if(ar.ReadString(m_strESPName)==false) return false;
		if(ar.ReadString(m_strBSPName)==false) return false;
		if(ar.ReadString(m_strModelName)==false) return false;
		int nVersion;
		if(ar.Read(&nVersion,sizeof(nVersion))==false)
		{
			assert(0);
			return false;
		}
		//利用这个版本号进行版本控制

		return true;
	}
	bool Save(AArchive& ar)
	{
		if(ar.WriteString(m_strESPName)==false) return false;
		if(ar.WriteString(m_strBSPName)==false) return false;
		if(ar.WriteString(m_strModelName)==false) return false;

		int nVersion;
		nVersion = ASceneInit::GetClassInfo()->GetVersion();
		if(ar.Write(&nVersion,sizeof(nVersion))==false)
		{
			assert(0);
			return false;
		}
		return true;
	}
	inline AUString GetMoxName() { return m_strModelName;}
	virtual ~ASceneInit(){}
};
class ATerrainInit : public AData
{
	int			Width;			
	int			Height;		
	int			SightRange;	
	int			TextureCover;	
	int			NumTexture;	
	float		CellSize;		
	float		Offset;		
	float		Scale;	
	
	AUString	m_strBase;
	AUString	m_strHeightMap;
	AUString	m_strTexMap;

	DECLARE_ASERIAL(ATerrainInit)

	STATICPROPERIES_BEGIN(AData, ATerrainInit)

		STATICPROPERTY(ATerrainInit, "宽度",int,Width, NULL, NULL, WAY_DEFAULT, NULL)			
		STATICPROPERTY(ATerrainInit, "高度",int,Height, NULL, NULL, WAY_DEFAULT, NULL)		
		STATICPROPERTY(ATerrainInit, "可见范围",int,SightRange, NULL, NULL, WAY_DEFAULT, NULL)	
		STATICPROPERTY(ATerrainInit, "纹理覆盖度",int,TextureCover, NULL, NULL, WAY_DEFAULT, NULL)	
		STATICPROPERTY(ATerrainInit, "纹理数",int,NumTexture, NULL, NULL, WAY_DEFAULT, NULL)	
		STATICPROPERTY(ATerrainInit, "单元尺寸",float,CellSize, NULL, NULL, WAY_DEFAULT, NULL)		
		STATICPROPERTY(ATerrainInit, "偏移",float,Offset, NULL, NULL, WAY_DEFAULT, NULL)		
		STATICPROPERTY(ATerrainInit, "比例",float,Scale, NULL, NULL, WAY_DEFAULT, NULL)	
		STATICPROPERTY(ATerrainInit, "基本文件", AUString, m_strBase, NULL, NULL, WAY_DEFAULT, NULL)
		STATICPROPERTY(ATerrainInit, "高度场文件", AUString, m_strHeightMap, NULL, NULL, WAY_DEFAULT, NULL)
		STATICPROPERTY(ATerrainInit, "纹理文件", AUString, m_strTexMap, NULL, NULL, WAY_DEFAULT, NULL)

	STATICPROPERIES_END(ATerrainInit)

public:

	ATerrainInit()
	{
		INITIALIZE_STATICPROPERIES(ATerrainInit);
		Width			= 0;			
		Height			= 0;		
		SightRange		= 0;	
		TextureCover	= 0;	
		NumTexture		= 0;	
		CellSize		= 0;		
		Offset			= 0;		
		Scale			= 0;		
	}
	void ToKernelDatum(AWORLD_INIT_TERRAIN* __pDatum)
	{
		if(!__pDatum) return;
		__pDatum->terrainParam.nWidth=Width;		
		__pDatum->terrainParam.nHeight=Height;		
		__pDatum->terrainParam.nSightRange=SightRange;	
		__pDatum->terrainParam.nTextureCover=TextureCover;
		__pDatum->terrainParam.nNumTexture=NumTexture;	
		__pDatum->terrainParam.vCellSize=CellSize;	
		__pDatum->terrainParam.vOffset=Offset;		
		__pDatum->terrainParam.vScale=Scale;	
		strcpy(__pDatum->szTexBase,m_strBase);
		strcpy(__pDatum->szHeightMap,m_strHeightMap);
		strcpy(__pDatum->szTexMap,m_strTexMap);
	}

	void FromKernelDatum(const AWORLD_INIT_TERRAIN* __pDatum)
	{
		if(!__pDatum) return;
		Width=__pDatum->terrainParam.nWidth;		
		Height=__pDatum->terrainParam.nHeight;		
		SightRange=__pDatum->terrainParam.nSightRange;	
		TextureCover=__pDatum->terrainParam.nTextureCover;
		NumTexture=__pDatum->terrainParam.nNumTexture;	
		CellSize=__pDatum->terrainParam.vCellSize;	
		Offset=__pDatum->terrainParam.vOffset;		
		Scale=__pDatum->terrainParam.vScale;	
		m_strBase=__pDatum->szTexBase;
		m_strHeightMap=__pDatum->szHeightMap;
		m_strTexMap=__pDatum->szTexMap;
	}
	bool Save(AArchive& ar)
	{
		if(ar.Write(&Width,sizeof(Width))==false) return false;
		if(ar.Write(&Height,sizeof(Height))==false) return false;
		if(ar.Write(&SightRange,sizeof(SightRange))==false) return false;
		if(ar.Write(&TextureCover,sizeof(TextureCover))==false) return false;
		if(ar.Write(&NumTexture,sizeof(NumTexture))==false) return false;
		if(ar.Write(&CellSize,sizeof(CellSize))==false) return false;
		if(ar.Write(&Offset,sizeof(Offset))==false) return false;
		if(ar.Write(&Scale,sizeof(Scale))==false) return false;
		if(ar.WriteString(m_strBase)==false) return false;
		if(ar.WriteString(m_strHeightMap)==false) return false;
		if(ar.WriteString(m_strTexMap)==false) return false;
		int nVersion;
		nVersion = ATerrainInit::GetClassInfo()->GetVersion();
		if(ar.Write(&nVersion,sizeof(nVersion))==false)
		{
			assert(0);
			return false;
		}

		return true;
	}
	bool Load(AArchive& ar)
	{
		if(ar.Read(&Width,sizeof(Width))==false) return false;
		if(ar.Read(&Height,sizeof(Height))==false) return false;
		if(ar.Read(&SightRange,sizeof(SightRange))==false) return false;
		if(ar.Read(&TextureCover,sizeof(TextureCover))==false) return false;
		if(ar.Read(&NumTexture,sizeof(NumTexture))==false) return false;
		if(ar.Read(&CellSize,sizeof(CellSize))==false) return false;
		if(ar.Read(&Offset,sizeof(Offset))==false) return false;
		if(ar.Read(&Scale,sizeof(Scale))==false) return false;
		if(ar.ReadString(m_strBase)==false) return false;
		if(ar.ReadString(m_strHeightMap)==false) return false;
		if(ar.ReadString(m_strTexMap)==false) return false;
		int nVersion;
		if(ar.Read(&nVersion,sizeof(nVersion))==false)
		{
			assert(0);
			return false;
		}
		//利用这个版本号进行版本控制

		return true;
	}
	virtual ~ATerrainInit(){};
};
#endif