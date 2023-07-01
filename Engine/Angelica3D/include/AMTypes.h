/*
 * FILE: AMTypes.h
 *
 * DESCRIPTION: Some basic types for Angelica Media engine
 *
 * CREATED BY: Hedi, 2002/1/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMTYPES_H_
#define _AMTYPES_H_

#include "A3DTypes.h"
#include "A3DPlatform.h"
#include "A3DFuncs.h"
#include "AMPlatform.h"

#define AMSOUND_MAXVOLUME		0
#define AMSOUND_MINVOLUME		-3000

enum AMSOUND3DALGORITHM
{
	AMS3DALG_DEFAULT = 0,
	AMS3DALG_NO_VIRTUALIZATION,
	AMS3DALG_HRTF_FULL,
	AMS3DALG_HRTF_LIGHT 
};

enum AMSOUNDCOLEVEL
{
	AMSCL_EXCLUSIVE = 0,
	AMSCL_NORMAL,
	AMSCL_PRIORITY,
	AMSCL_WRITEPRIMARY
};

enum AMSBSTATE
{
	AMSBSTATE_STOPPED = 0,
	AMSBSTATE_PLAYING,
	AMSBSTATE_PAUSED
};

enum AMSOUNDFILETYPE
{
	AMS_UNKOWNFILE = -1,
	AMS_WAVEFILE = 0,
	AMS_MP3FILE
};

enum AMSS_STATE
{
	AMSS_STOPPED = 0,
	AMSS_PLAYING,
	AMSS_PAUSED,
	AMSS_ENDTRACK,
	AMSS_BEGINTRACK // the time waiting to play
};

typedef struct _AMSOUNDDEVFORMAT
{
    WORD				nChannels; 
    DWORD				nSamplesPerSec; 
    WORD				wBitsPerSample; 

	AMSOUNDCOLEVEL		soundCL;
	AMSOUND3DALGORITHM	algorithm;
} AMSOUNDDEVFORMAT, * PAMSOUNDDEVFORMAT;

typedef struct _AMSSTREAMDESC
{
	WAVEFORMATEX		wfxFormat;
	DWORD				dwBufferLen;
	DWORD				dwBufferNum;
}AMSSTREAMDESC, * PAMSSTREAMDESC;

typedef struct _AMSBUFFERDESC
{ 
	DWORD				dwFlags; // Now we use these flags came from dsound directly
	DWORD				dwBufferBytes; 

	WAVEFORMATEX		wfxFormat; 
	AMSOUND3DALGORITHM	algorithm; 
} AMSBUFFERDESC, *PAMSBUFFERDESC; 

typedef struct _AMS3DBUFFERDESC
{ 
    A3DVECTOR3 vecPos; 
    A3DVECTOR3 vecVel; 
    DWORD      dwInsideConeAngle; 
    DWORD      dwOutsideConeAngle; 
    A3DVECTOR3 vecConeOrientation; 
    LONG       lConeOutsideVolume; 
    FLOAT	   vMinDistance; 
    FLOAT	   vMaxDistance; 
    DWORD      dwMode; // Now we use these flags came from dsound directly
} AMS3DBUFFERDESC, *PAMS3DBUFFERDESC;
  
typedef struct _AMS3DLISTENERDESC
{ 	
    A3DVECTOR3	vecPos;
    A3DVECTOR3  vecVel;
    A3DVECTOR3  vecOrientFront;
    A3DVECTOR3  vecOrientTop;
    FLOAT		vDistanceFactor; 
    FLOAT		vRolloffFactor; 
    FLOAT		vDopplerFactor; 
} AMS3DLISTENERDESC, *PAMS3DLISTENERDESC;
 
#endif//_AMTYPES_H_
