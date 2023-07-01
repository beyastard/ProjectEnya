/*
 * FILE: ALexicalParse.H
 *
 * DESCRIPTION : Implement our C style Lexical Parse 
 *
 * CREATED BY: MengXin
 *
 * HISTORY:	2002 - 9 - 11
 *
 * Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
 */

#ifndef __A_LEXICAL_PARSE_H__
#define __A_LEXICAL_PARSE_H__

#define USING_MODEL_UNIT2
#ifndef USING_MODEL_UNIT2

#include "StlHeader.h"

const unsigned long TOKEN_TYPE_ERROR			=	0;
const unsigned long TOKEN_TYPE_COMMENT			=	1;
const unsigned long TOKEN_TYPE_STRING			=	2;
const unsigned long TOKEN_TYPE_BOOL				=	3;
const unsigned long TOKEN_TYPE_CHAR				=	4;
const unsigned long TOKEN_TYPE_DIGITIAL_HEX		=	5;
const unsigned long TOKEN_TYPE_DIGITIAL_OCT		=	6;
const unsigned long TOKEN_TYPE_DIGITIAL_DEC		=	7;
const unsigned long TOKEN_TYPE_DIGITIAL_FLOAT	=	8;
const unsigned long TOKEN_TYPE_DIGITIAL_SCIENCE	=	9;
const unsigned long TOKEN_TYPE_OPERATOR			=	10;
const unsigned long TOKEN_TYPE_IDENTITY			=	11;
const unsigned long TOKEN_TYPE_BLANK			=	12;
const unsigned long TOKEN_TYPE_EOF				=	13;

const unsigned long ERROR_MISS_END_OF_COMMENT	=	100;
const unsigned long ERROR_MISS_END_OF_CHAR		=	101;
const unsigned long ERROR_INVADATE_ESC_CHAR		=	102;
const unsigned long ERROR_TOO_MANY_CHARS		=	103;
const unsigned long ERROR_MISS_END_OF_STRING	=	104;
const unsigned long ERROR_8_9_IN_OCT_NUMBER		=	105;
const unsigned long ERROR_SCIENCE_NUMBER_ERROR	=	106;
const unsigned long ERROR_E_IN_NUMBER			=	107;
const unsigned long ERROR_CAHR_IN_NUMBER		=	108;

class LexicalParse
{
	string				source_string;
	string				token_in_cache;
	unsigned long		type_of_token_in_cache;
	string::iterator	itor_pointer_to_next;
	string::iterator	itor_begin_of_token_in_cache;
	string::iterator	itor_end_of_token_in_cache;

	bool				isOperator(const string& op);

private:
	
	unsigned long	extractComment(string& str,string::iterator& itor,string& out_token);
	unsigned long	extractIdentity(string& str,string::iterator& itor,unsigned long& type,string& out_token);
	unsigned long	extractChar(string& str,string::iterator& itor,string& out_token);
	unsigned long	extractString(string& str,string::iterator& itor,string& out_token);
	unsigned long	extractDigital(string& str,string::iterator& itor,unsigned long& format,string& out_token);
	unsigned long	extractOperator(string& str,string::iterator& itor, string& out_token);
	unsigned long	extractBlank(string& str,string::iterator& itor,string& out_token);
	unsigned long	extractAllToken(string& str,string::iterator& itor,string& token);

public:

	~LexicalParse();
	LexicalParse();
	void			writeErrorInfo(unsigned long err_code,const string& detail = "");
	void			setSourceString(const string& source_str);
	unsigned long	lookToken(string& token);
	void			goForward();
};

#endif

#endif// __A_LEXICAL_PARSE_H__
