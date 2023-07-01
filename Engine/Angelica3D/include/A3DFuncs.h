/*
 * FILE: A3DFuncs.h
 *
 * DESCRIPTION: some data operation routines for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DFUNCS_H_
#define _A3DFUNCS_H_

#include <math.h>
#include "A3DTypes.h"
#include "A3DMacros.h"

// =====================================
// Unary operators
// =====================================
inline A3DVECTOR3 operator + (const A3DVECTOR3& v) {return A3DVECTOR3(v.x, v.y, v.z);}
inline A3DVECTOR3 operator - (const A3DVECTOR3& v) {return A3DVECTOR3(-v.x, -v.y, -v.z);}

// =====================================
// Binary operators
// =====================================
// Addition and subtraction
inline A3DVECTOR3 operator + (const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return A3DVECTOR3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);}
inline A3DVECTOR3 operator - (const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return A3DVECTOR3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);}

// Scalar multiplication and division
inline A3DVECTOR3 operator * (const A3DVECTOR3& v, FLOAT s) {return A3DVECTOR3(s * v.x, s * v.y, s * v.z);}
inline A3DVECTOR3 operator * (FLOAT s, const A3DVECTOR3& v) {return A3DVECTOR3(s * v.x, s * v.y, s * v.z);}
inline A3DVECTOR3 operator / (const A3DVECTOR3& v, FLOAT s) {return A3DVECTOR3(v.x / s, v.y / s, v.z / s);}

inline bool operator == (A3DCOLORVALUE& c1, A3DCOLORVALUE& c2)	{return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a);	}
inline bool operator != (A3DCOLORVALUE& c1, A3DCOLORVALUE& c2)	{return (c1.r != c2.r || c1.g != c2.g || c1.b != c2.b || c1.a != c2.a);	}

// Length-related functions
inline FLOAT SquareMagnitude(const A3DVECTOR3& v) {return v.x * v.x + v.y * v.y + v.z * v.z;}
inline FLOAT Magnitude(const A3DVECTOR3& v) {return (FLOAT)(sqrt(v.x * v.x + v.y * v.y + v.z * v.z));}
inline FLOAT MagnitudeH(const A3DVECTOR3& v) {return (FLOAT)(sqrt(v.x * v.x + v.z * v.z));}

inline A3DVECTOR3 Vector423(const A3DVECTOR4& v) {return A3DVECTOR3(v.x / v.w, v.y / v.w, v.z / v.w);}

// Returns vector with same direction and unit length
inline A3DVECTOR3 Normalize(const A3DVECTOR3& v)
{
	FLOAT mag = Magnitude(v);
	if( mag < 1e-12	&& mag > -1e-12 )
		return A3DVECTOR3(0.0f);
	else
		return v / mag;
}

inline FLOAT Normalize(const A3DVECTOR3& v, A3DVECTOR3& vOut)
{
	FLOAT mag = Magnitude(v);
	if( mag < 1e-12	&& mag > -1e-12 )
	{
		vOut = A3DVECTOR3(0.0f);
		return 0;
	}
	else
	{
		vOut = v / mag;
		return mag;
	}
}

// Return min/max vector composed with min/max component of the input 2 vector
inline A3DVECTOR3 Min(const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return A3DVECTOR3(min2(v1.x, v2.x), min2(v1.y, v2.y), min2(v1.z, v2.z));}
inline A3DVECTOR3 Max(const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return A3DVECTOR3(max2(v1.x, v2.x), max2(v1.y, v2.y), max2(v1.z, v2.z));}

// Return min/max member component of input vectors
inline FLOAT MinMember(const A3DVECTOR3& v) {return min3(v.x, v.y, v.z);}
inline FLOAT MaxMember(const A3DVECTOR3& v) {return max3(v.x, v.y, v.z);}

// Dot and cross product
inline FLOAT DotProduct(const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;}
inline A3DVECTOR3 CrossProduct(const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return A3DVECTOR3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,	v1.x * v2.y - v1.y * v2.x);}

//	Convert a vector from view coordinates to world coordinates
inline A3DVECTOR3 ViewToWorld(A3DVECTOR3& vIn, A3DMATRIX4& matView)
{
	A3DVECTOR3 vOut;
	vOut.x = vIn.x * matView._11 + vIn.y * matView._12 + vIn.z * matView._13;
	vOut.y = vIn.x * matView._21 + vIn.y * matView._22 + vIn.z * matView._23;
	vOut.z = vIn.x * matView._31 + vIn.y * matView._32 + vIn.z * matView._33;
	return vOut;
}

//Matrix mul with vector3 and vector4;
inline A3DVECTOR3 operator * (const A3DMATRIX4& mat, const A3DVECTOR3& v)
{
	A3DVECTOR3 result;
	FLOAT w;
	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43;
	w = 1.0f / (v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + mat._44);
	result.x *= w; result.y *= w; result.z *= w;
	return result;
}

inline A3DVECTOR3 operator * (const A3DVECTOR3& v, const A3DMATRIX4& mat)
{
	A3DVECTOR3 result;
	FLOAT w;
	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43;
	w = 1.0f / (v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + mat._44);
	result.x *= w; result.y *= w; result.z *= w;
	return result;
}

inline A3DVECTOR4 operator * (const A3DMATRIX4& mat, const A3DVECTOR4& v)
{
	A3DVECTOR4 result;
	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + v.w * mat._41;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + v.w * mat._42;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + v.w * mat._43;
	result.w = v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + v.w * mat._44;
	return result;	
}

inline A3DVECTOR4 operator * (const A3DVECTOR4& v, const A3DMATRIX4& mat)
{
	A3DVECTOR4 result;
	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + v.w * mat._41;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + v.w * mat._42;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + v.w * mat._43;
	result.w = v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + v.w * mat._44;
	return result;	
}

inline A3DVECTOR3 operator * (const A3DMATRIX3& mat, const A3DVECTOR3& v)
{
	A3DVECTOR3 result;
	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33;
	return result;	

}

inline A3DVECTOR3 operator * (const A3DVECTOR3& v, const A3DMATRIX3& mat)
{
	A3DVECTOR3 result;
	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33;
	return result;	
}

//Matrix mul and div a scalar
inline A3DMATRIX4 operator * (const A3DMATRIX4& mat, FLOAT s)
{
	A3DMATRIX4 result;
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			result.m[i][j] = mat.m[i][j] * s;
	return result;
}

inline A3DMATRIX4 operator / (const A3DMATRIX4& mat, FLOAT s)
{
	A3DMATRIX4 result;
	s = 1.0f / s;
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			result.m[i][j] = mat.m[i][j] * s;
	return result;
}

//Matrix add, sub, and mul with matrix;
inline A3DMATRIX4 operator + (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2)
{
	A3DMATRIX4 result;
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			result.m[i][j] = mat1.m[i][j] + mat2.m[i][j];
	return result;
}
inline A3DMATRIX4 operator - (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2)
{
	A3DMATRIX4 result;
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			result.m[i][j] = mat1.m[i][j] - mat2.m[i][j];
	return result;
}
inline A3DMATRIX4 operator * (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2)
{
	A3DMATRIX4	result;

	memset(&result, 0, sizeof(result));
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			for(int k=0; k<4; k++)
				result.m[i][j] += mat1.m[i][k] * mat2.m[k][j];
	return result;
}

//Matrix initialize;
A3DMATRIX3 IdentityMatrix3();
A3DMATRIX4 IdentityMatrix();
A3DMATRIX4 ZeroMatrix();
A3DMATRIX4 ViewMatrix(const A3DVECTOR3& from, const A3DVECTOR3& dir, const A3DVECTOR3& vecUp, FLOAT roll);
A3DMATRIX4 LookAtMatrix(const A3DVECTOR3& from, const A3DVECTOR3& to, const A3DVECTOR3& vecUp, FLOAT roll);
A3DMATRIX4 TransformMatrix(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp, const A3DVECTOR3& vecPos);

//Matrix manipulation;
A3DMATRIX4 RotateX(FLOAT vRad);
A3DMATRIX4 RotateX(const A3DMATRIX4& mat, FLOAT vRad);
A3DMATRIX4 RotateY(FLOAT vRad);
A3DMATRIX4 RotateY(const A3DMATRIX4& mat, FLOAT vRad);
A3DMATRIX4 RotateZ(FLOAT vRad);
A3DMATRIX4 RotateZ(const A3DMATRIX4& mat, FLOAT vRad);
A3DMATRIX4 RotateAxis(A3DVECTOR3 vecAxis, FLOAT vRad);
A3DMATRIX4 RotateAxis(A3DVECTOR3 vecPos, A3DVECTOR3 vecAxis, FLOAT vRad);

A3DMATRIX4 Scaling(FLOAT sx, FLOAT sy, FLOAT sz);
A3DMATRIX4 Scaling(const A3DMATRIX4& mat, FLOAT sx, FLOAT sy, FLOAT sz);
A3DMATRIX4 Translate(FLOAT x, FLOAT y, FLOAT z);

A3DMATRIX4 Transpose(const A3DMATRIX4& mat);

// Get the dir and up of a view within the cube map
// 0 ---- right
// 1 ---- left
// 2 ---- top
// 3 ---- bottom
// 4 ---- front
// 5 ---- back
bool GetCubeMapDirAndUp(int nFaceIndex, A3DVECTOR3 * pDir, A3DVECTOR3 * pUp);

//Color Value and Color RGB Convertion;
A3DCOLOR ColorValueToColorRGBA(A3DCOLORVALUE colorValue);
A3DCOLORVALUE ColorRGBAToColorValue(A3DCOLOR colorRGBA);

A3DMATRIX4 MirrorMatrix(const A3DVECTOR3 &p, const A3DVECTOR3 &n);
A3DMATRIX4 InverseTM(A3DMATRIX4 tm);

//Plane operations
BYTE GetA3DPlaneType(A3DVECTOR3& vNormal);
BYTE GetA3DPlaneSignBits(A3DVECTOR3& vNormal);
void SnapA3DVector(A3DVECTOR3& vNormal);

//AABB operations
void ClearAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs);
void AddVertexToAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, A3DVECTOR3& vPoint);
void ExpandAABB(A3DAABB * pAABB, const A3DOBB& obb);
void ExpandAABB(A3DAABB * pAABB, const A3DAABB& subAABB);
void CompleteAABB(A3DAABB * pAABB);
void ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DOBB& obb);
void ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DAABB& subAABB);

inline bool PointInAABB(A3DVECTOR3& vecPos, A3DAABB& aabb)
{
	for(int i=0; i<3; i++)
	{
		if( vecPos.m[i] > aabb.Maxs.m[i] || vecPos.m[i] < aabb.Mins.m[i] )
			return false;
	}
	return true;
}

//OBB operations;
void CompleteOBB(A3DOBB * pOBB);

A3DVECTOR3 RandDirH();

// Convert a color from HSV to RGB
// h, s, v is in [0.0f, 1.0f]
void HSVToRGB(A3DHSVCOLORVALUE& hsvColor, A3DCOLOR * pRgbColor);
// Convert a color from RGB to HSV
// h, s, v is in [0.0f, 1.0f]
void RGBToHSV(A3DCOLOR rgbColor, A3DHSVCOLORVALUE * pHsvColor);
// Convert a color from RGB to seperate r, g, b value;
void DecompressColor(A3DCOLOR rgbColor, BYTE * pR, BYTE * pG, BYTE * pB, BYTE *pA=NULL);

bool A3DGetFileTitle(char * lpszFile, char * lpszTitle, WORD cbBuf);
bool A3DGetFilePath(char * lpszFile, char * lpszPath, WORD cbBuf);
#endif
