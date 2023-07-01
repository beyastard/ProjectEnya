#include "ADarray.h"

int ADArray::FindElementByData(LPVOID pData)
{
	LPVOID * fp;
	for(fp = _array.begin();fp != _array.end();fp++)
	{
		if(*fp == pData) return fp - _array.begin();
	}
	return -1;
}
