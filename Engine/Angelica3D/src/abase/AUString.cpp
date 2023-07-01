/*
 * FILE: AUString.h
 *
 * DESCRIPTION:字符处理类
 *
 * CREATED BY: 杨智盈, 2002/8/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#include <assert.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "AUString.h"

#define	DEL_PDATA(pData)	\
	if( pData )				\
	{						\
		delete	[] (pData);	\
		(pData)=NULL;		\
	}							
//end define

#define NEW_PDATA(pData,type,size,result)	\
	if(pData) return (result);				\
	(pData) = new type[(size)];				\
	if(NULL ==(pData)) return (result);
//end define

AUString::AUString()
{
	m_buff = NULL;
	GetBufferSetLength(0);
}

AUString::AUString(char ch,int repeat)
{
	m_buff = NULL;

	if(repeat < 0)
	{
		return;
	}

	char * buff = GetBufferSetLength(repeat);
	if(buff)
	{
		memset(buff,ch,repeat);
	}
}

AUString::AUString(const char *string)
{
	m_buff = NULL;

	char * buff = GetBufferSetLength(string != NULL ? strlen(string)+1 : 0);
	if(buff && string)
	{
		strcpy(buff,string);
	}
}

AUString::AUString(const AUString & right)
{
	m_buff = NULL;

	char * buff = GetBufferSetLength(right.GetLength());
	if(buff && right.m_buff)
	{
		strcpy(buff,right.m_buff);
	}
}

AUString::~AUString()
{
	Free();
}

char * AUString::GetBufferSetLength(int length)
{
	if(length < 0)
	{
		assert(false);
		Free();
		return NULL;
	}

/*	char * buff = new char[length + 1];
	memset(buff,0,length + 1);
	if(m_buff)
	{
		memcpy(buff,m_buff,(int)strlen(m_buff) > length ? length : strlen(m_buff));
	}

	Free();
	m_buff = buff;*/
	m_buff = (char *)realloc(m_buff, length + 1);
	assert(m_buff);
	m_buff[length] = 0;
	return m_buff;
}

void AUString::Free()
{
	if(m_buff)
	{
		//delete []m_buff;
		free(m_buff);
		m_buff = NULL;
	}
}

//设置字符
bool AUString::SetAt(int index,char ch)
{
	if(index < 0 || index > GetLength())
	{
		return false;
	}
	m_buff[index] = ch;
	return true;
}

bool AUString::GetAt(int index,char & ch)
{
	if(index < 0 || index > GetLength())
	{
		return false;
	}
	ch = m_buff[index];
	return true;
}

//移除字串中的指定字符，返回删除或遇到的个数
int AUString::FilterChar(char ch,bool remove)
{
	if(!GetBuffer())
	{
		return 0;
	}

	int count = 0;
	int length = GetLength();
	for(int a = length;a >= 0;a--)
	{
		if(GetBuffer()[a] == ch)
		{
			if(remove)
			{
				RemoveAt(a);
			}
			count++;
		}
	}

	return count;
}

//移除和添加字符
bool AUString::RemoveAt(int index)
{
	int length = GetLength();

	if(index < 0 || index > length)
	{
		return false;
	}

	for(int a = index;a < length;a++)
	{
		m_buff[a] = m_buff[a + 1];
	}

	return true;
}

//置为大写
void AUString::MakeUpper()
{
	char * buff = GetBuffer();
	if(buff)
	{
		int length = GetLength();
		for(int a = 0;a < length;a ++)
		{
			buff[a] = toupper(buff[a]);
		}
	}
}

//置为小写
void AUString::MakeLower()
{
	char * buff = GetBuffer();
	if(buff)
	{
		int length = GetLength();
		for(int a = 0;a < length;a ++)
		{
			buff[a] = tolower(buff[a]);
		}
	}
}

bool AUString::InsertAt(int index,char ch)
{
	
	//	在 0 ~ (缓冲区长度 - 1)之间插入字符，如果长度大于字串，
	//	则在插入位置与字符串尾部之间用' '（空格）进行填充
	
	int length = GetLength();

	if(index < 0)
	{
		return false;
	}

	if(!GetBufferSetLength((index < length ? length : index) + 1))
	{
		return false;
	}

	if(index <= length)
	{
		for(int a = length;a > index;a--)
		{
			m_buff[a] = m_buff[a - 1];
		}
	}
	else
	{
		// > length
		memset(m_buff + length,' ',index - length);
	}
	m_buff[index] = ch;
	return true;
}

void AUString::Left(AUString & str,int count)
{
	Mid(str,0,count);
}

void AUString::Right(AUString & str,int count)
{
	Mid(str,GetLength() - count,GetLength());
}

void AUString::Mid(AUString & str,int start,int end)
{
	if(end > GetLength())
	{
		end = GetLength();
	}
	int midlength = end - start;
	if(midlength < 0)
	{
		return;
	}

	char * buff = str.GetBufferSetLength(midlength);
	if(buff)
	{
		memcpy(buff,m_buff + start,midlength);
	}
}

const AUString & AUString::operator = (const AUString & right)
{
	if(&right == this)
	{
		return right;
	}

	char * buff = GetBufferSetLength(right.GetLength());
	if(buff && !right.IsEmpty())
	{
		strcpy(buff,right.GetBuffer());
	}

	return right;
}

const char * AUString::operator = (const char * string)
{
	if(!string)
	{
		Empty();
		return string;
	}

	if(strlen(string) == 0)
	{
		Empty();
		return string;
	}

	char * buff = GetBufferSetLength(strlen(string));
	if(m_buff)
	{
		strcpy(m_buff,string);
	}

	return string;
}

AUString AUString::operator + (AUString right)
{
	AUString value = *this;
	if(right.m_buff)
	{
		value.GetBufferSetLength(value.GetLength() + right.GetLength());
		if(value.m_buff)
		{
			strcat(value.m_buff,right);
		}
	}

	return value;
}

char AUString::operator [] (int index)
{
	char ch = 0;
	if(GetAt(index,ch))
	{
		return ch;
	}
	
	return 0;
}

const char AUString::operator [](int index) const
{
	if(index < 0 || index > GetLength())
	{
		return '\0';
	}
	return m_buff[index];
}

char * AUString::GetBuffer() const
{
	return m_buff;
}

bool AUString::IsEmpty() const
{
	if(!GetBuffer())
	{
		return true;
	}

	if(GetLength() == 0 || m_buff[0] == 0)
	{
		return true;
	}

	return false;
}

int AUString::GetLength() const
{
	if(!m_buff)
	{
		return 0;
	}
	return strlen(m_buff);
}

int AUString::FindChar(char ch,int start)
{
	int length = GetLength();
	if(start >= length)
	{
		return -1;
	}

	for(int a = start;a < length;a++)
	{
		if(m_buff[a] == ch) return a;
	}

	return -1;
}

int AUString::ReverseFind(char ch)
{
	int length = GetLength();
	for(int a = length - 1;a >= 0;a --)
	{
		if(m_buff[a] == ch) return a;
	}

	return -1;
}

void AUString::Empty()
{
	if(IsEmpty())
	{
		return;
	}

	GetBufferSetLength(0);
}

int AUString::FindString(AUString strP, int nStart)
{
	char * str = GetBuffer() ;

	if(nStart>=(int)strlen(str) || nStart <0 || strlen(str)==0)
	{
		return -1;
	}
	
	char * pFailLink=NULL;

	NEW_PDATA(pFailLink,char,strlen(strP)+2,-1)

	memset(pFailLink,0,strlen(strP)+2);

	FailLink(strlen(strP),pFailLink);

	int i=nStart+1,j=1,flag=0;
	int m=strlen(strP);

	while(i<=(int)strlen(str) &&  flag == 0)
	{
		while(j != 0 && strP[j-1] != str[i-1] )
			j = pFailLink[j];
		if( j==m) flag= 1;
		else
		{
			i++;
			j++;
		}
	}
	i=i-m;

	DEL_PDATA(pFailLink)
		
	if( flag ) return i;
	
	return -1;
}

//构造失败链接
//失败链接是“开始读取节点”+“匹配节点”+“结束节点”
bool AUString::FailLink(int iLen, char *pFailLink)
{
	pFailLink[1]=0;
	char * str = GetBuffer() ;

	int j=0;
	for(int i=2;i<=iLen;i++)
	{
		j=pFailLink[i-1];
		while( j!=0 && str[j-1] != str[i-2] )
		{
			j = pFailLink[j];
			pFailLink[i] = j+1 ;
		}
	}
	return true;
}

bool operator == (AUString v1, AUString v2)
{
	if(v1.IsEmpty() && v2.IsEmpty())
	{
		return true;
	}

	if(!v1.IsEmpty() && v2.IsEmpty())
	{
		return false;
	}

	if(v1.IsEmpty() && !v2.IsEmpty())
	{
		return false;
	}

	if(_stricmp(v1.GetBuffer(),v2.GetBuffer()))
	{
		return false;
	}

	return true;
}

bool operator != (AUString v1, AUString v2)
{
	return !(v1 == v2);
}