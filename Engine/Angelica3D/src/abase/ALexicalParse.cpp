#include "ALexicalParse.h"

#ifndef USING_MODEL_UNIT2

const unsigned long	TOKEN_EXTRACT_ERROR			=	20;
const unsigned long	TOKEN_EXTRACT_HIT			=	21;
const unsigned long	TOKEN_EXTRACT_MISS			=	22;
const unsigned long	TOKEN_EXTRACT_EOF			=	23;

bool LexicalParse::isOperator(const string& op)
{
	return
		op == "+" ||
		op == "-" ||
		op == "*" ||
		op == "/" ||
		op == "%" ||
		op == "==" ||
		op == "!=" ||
		op == ">" ||
		op == "<" ||
		op == ">=" ||
		op == "<=" ||
		op == "~" ||
		op == "^" ||
		op == "|" ||
		op == "&" ||
		op == "||" ||
		op == "&&" ||
		op == "!" ||
		op == "^^" ||
		op == "<<" ||
		op == ">>" ||
		op == ";" ||
		op == "," ||
		op == "=" ||
		op == "(" ||
		op == ")" ||
		op == ":";
}


LexicalParse::LexicalParse():type_of_token_in_cache(0)
{
}

LexicalParse::~LexicalParse()
{
}

void LexicalParse::writeErrorInfo(unsigned long err_code,const string& detail)
{
	
}

void LexicalParse::setSourceString(const string& str)
{
	token_in_cache					= "";
	type_of_token_in_cache			= 0;
	source_string					= str;
	itor_pointer_to_next			= source_string.begin();
	itor_begin_of_token_in_cache	= itor_pointer_to_next;
	itor_end_of_token_in_cache		= itor_pointer_to_next;	
}

unsigned long LexicalParse::lookToken(string& token)
{
	if(!((itor_begin_of_token_in_cache != itor_end_of_token_in_cache) && (itor_pointer_to_next == itor_begin_of_token_in_cache)))
	{
		itor_begin_of_token_in_cache	= itor_end_of_token_in_cache;
		type_of_token_in_cache			= extractAllToken(source_string,itor_pointer_to_next,token_in_cache);
		itor_end_of_token_in_cache		= itor_pointer_to_next;
		itor_pointer_to_next			= itor_begin_of_token_in_cache;
	}
	token= token_in_cache;
	return type_of_token_in_cache;
}

void LexicalParse::goForward()
{
	itor_pointer_to_next = itor_end_of_token_in_cache;
}

unsigned long LexicalParse::extractComment(string& str,string::iterator& itor,string& out_token)
{
	//关于注释的有限自动机
	const string::iterator itor_extract_begin = itor;
	
	string  token		= "";
	char	character	= '\0';
	bool	finished	= false;
	bool    bBlock		= false;

	if(itor == str.end())
	{
		return TOKEN_EXTRACT_EOF;
	}

	for(; itor != str.end() ; ++itor)
	{
		char character	= *itor; 
		if(itor_extract_begin == itor)
		{
			//注释只可能由‘/’开始
			if(character == '/')
			{
				++itor;
				if(itor != str.end())
				{
					if(*itor == '/')
					{
						bBlock = false;
						continue;
					}
					else if(*itor == '*')
					{
						bBlock = true;
						continue;
					}
					else
					{
						goto COMMENT_MISS;
					}
				}
				else
				{
					goto COMMENT_MISS;
				}
			}
			else
			{
				goto COMMENT_MISS;
			}
		}
		//换行号将结束行注释
		else if(character == '\n' && !bBlock)
		{
			out_token = token;
			return TOKEN_EXTRACT_HIT;
		}
		//看看是不是结束块注释
		else if(character == '*' && bBlock)
		{
			++itor;
			if(itor != str.end())
			{
				if(*itor == '/')
				{
					++itor;
					out_token = token;
					return TOKEN_EXTRACT_HIT;
				}
			}
		}
		else
		{
			token += character;
		}
	}
	//单行注释可以以字符串尾作为结束
	if(!bBlock)
	{
		out_token = token;
		return TOKEN_EXTRACT_HIT;
	}

	writeErrorInfo(ERROR_MISS_END_OF_COMMENT);
	return TOKEN_EXTRACT_ERROR;	

COMMENT_MISS:
	
	itor = itor_extract_begin;
	return TOKEN_EXTRACT_MISS;
}

unsigned long LexicalParse::extractIdentity(string& str,string::iterator& itor,unsigned long& type,string& out_token)
{
	const string::iterator itor_extract_begin = itor;
	char character	= '\0';
	string token = "";

	if(itor == str.end())
	{
		return TOKEN_EXTRACT_EOF;
	}

	for(; itor != str.end() ; ++itor)
	{
		char character	= *itor; 
		if(itor == itor_extract_begin)
		{
			//标识符必须是字母或下划线开头
			if(character == '_' || isalpha(character))
			{
				token += character;
				continue;
			}
			else
			{
				itor = itor_extract_begin;
				return TOKEN_EXTRACT_MISS;
			}
		}
		else if(character == '_' || isalnum(character))
		{
			token += character;
			continue;	
		}
		else
		{
			break;
		}
	}
	if(	token == "TRUE" || token == "FALSE" ||
		token == "true" || token == "false" ||
		token == "True" || token == "False")
	{
		type = TOKEN_TYPE_BOOL;
	}
	else
	{
		type = TOKEN_TYPE_IDENTITY;
	}
	out_token = token;
	return TOKEN_EXTRACT_HIT;
}

unsigned long LexicalParse::extractChar(string& str,string::iterator& itor,string& out_token)
{
	//关于字符的有限自动机
	const string::iterator itor_extract_begin = itor;

	char	token		= '\0';
	char	character	= '\0';
	bool	finished	= false;

	if(itor == str.end())
	{
		return TOKEN_EXTRACT_EOF;
	}

	for(; itor != str.end() ; ++itor)
	{
		char character	= *itor; 
		//字符定义开始
		if(itor_extract_begin == itor)
		{
			if(character == '\'')
			{
				continue;
			}
			else
			{
				itor = itor_extract_begin;
				return TOKEN_EXTRACT_MISS;	
			}
		}
		//字符定义结束
		else if(character == '\'')
		{
			++itor;
			out_token = "";
			out_token += token;
			return TOKEN_EXTRACT_HIT;
		}
		//转义字符
		else if(character == '\\')
		{
			string esc_string = "";
			bool   char_esc	  = false;//是否是字符转义如 \t
			//转义序列的第一个字符
			if(itor!=str.end())
			{
				++itor;
				character =	*itor;
			}
			else 
			{
				writeErrorInfo(ERROR_MISS_END_OF_CHAR);
				return TOKEN_EXTRACT_ERROR;
			}
			switch(character)
			{
			//标准转义	
			case	'a':	token = '\a';char_esc=true;goto END_DEF;
			case	'b':	token = '\b';char_esc=true;goto END_DEF;
			case	'f':	token = '\f';char_esc=true;goto END_DEF;
			case	'n':	token = '\n';char_esc=true;goto END_DEF;
			case	'r':	token = '\r';char_esc=true;goto END_DEF;
			case	't':	token = '\t';char_esc=true;goto END_DEF;
			case	'v':	token = '\v';char_esc=true;goto END_DEF;
			case	'\'':	token = '\'';char_esc=true;goto END_DEF;	 
			//数字转义
			case	'0':case	'1':case	'2':case	'3':
			case	'4':case	'5':case	'6':case	'7':
				{
					char_esc	= false;
					esc_string += character;
					break;
				}
			default:
				{
			//		string detail = string("Error char <")+character+">.";
			//		writeErrorInfo(ERROR_INVADATE_ESC_CHAR,detail);
					return TOKEN_EXTRACT_ERROR;
				}
			}
			//转义序列的第二个字符
			if(itor!=str.end())
			{
				++itor;
				character =	*itor;
			}
			else
			{
				writeErrorInfo(ERROR_MISS_END_OF_CHAR);
				return TOKEN_EXTRACT_ERROR;
			}
			if('0'<=character&&character<='7')
			{
				char_esc	= false;
				esc_string += character;
			}
			else
			{
				--itor;
				goto END_DEF;
			}
			//转义序列的第三个字符
			if(itor!=str.end())
			{
				++itor;
				character =	*itor;
			}
			else
			{
				writeErrorInfo(ERROR_MISS_END_OF_CHAR);
				return TOKEN_EXTRACT_ERROR;
			}
			if('0'<=character&&character<='7')
			{
				char_esc	= false;
				esc_string += character;
			}
			else
			{
				--itor;
				goto END_DEF;
			}
	END_DEF:
			if(!char_esc)
			{
				long char_code = 0;
				sscanf(esc_string.c_str(),"%o",&char_code);
				token = static_cast<char>(char_code);
			}
			finished = true;
		}
		else if(finished)
		{
			writeErrorInfo(ERROR_TOO_MANY_CHARS);
			return TOKEN_EXTRACT_ERROR;
		}
		token += character;
		finished = true;
	}
	writeErrorInfo(ERROR_MISS_END_OF_CHAR);
	return TOKEN_EXTRACT_ERROR;
}

unsigned long LexicalParse::extractString(string& str,string::iterator& itor,string& out_token)
{
	//关于字符串的有限自动机
	const string::iterator itor_extract_begin = itor;
	string	token	= "";
	char character	= '\0';

	if(itor == str.end())
	{
		return TOKEN_EXTRACT_EOF;
	}

	for(; itor != str.end() ; ++itor)
	{
		char character	= *itor; 
		//字符串定义开始
		if(itor_extract_begin == itor)
		{
			if(character == '"')
			{
				continue;
			}
			else
			{
				itor = itor_extract_begin;
				return TOKEN_EXTRACT_MISS;
			}
		}
		//字符串定义结束
		else if(character == '"')
		{
			++itor;
			out_token = token;
			return TOKEN_EXTRACT_HIT;
		}
		//转义字符
		else if(character == '\\')
		{
			string esc_string = "";
			bool   char_esc	  = false;//是否是字符转义如 \t
			//转义序列的第一个字符
			if(itor!=str.end())
			{
				++itor;
				character =	*itor;
			}
			else
			{
				writeErrorInfo(ERROR_MISS_END_OF_STRING);
				return TOKEN_EXTRACT_ERROR;
			}
			switch(character)
			{
			//标准转义	
			case	'a':	esc_string += '\a'; char_esc = true; goto END_ESC;
			case	'b':	esc_string += '\b'; char_esc = true; goto END_ESC;
			case	'f':	esc_string += '\f'; char_esc = true; goto END_ESC;
			case	'n':	esc_string += '\n'; char_esc = true; goto END_ESC;
			case	'r':	esc_string += '\r'; char_esc = true; goto END_ESC;
			case	't':	esc_string += '\t'; char_esc = true; goto END_ESC;
			case	'v':	esc_string += '\v'; char_esc = true; goto END_ESC;
			case	'"':	esc_string += '"' ;	char_esc = true; goto END_ESC;
			case	'\\':	esc_string += '\\' ;char_esc = true; goto END_ESC;
			//数字转义
			case	'0':case	'1':case	'2':case	'3':
			case	'4':case	'5':case	'6':case	'7':
				{
					char_esc	= false;
					esc_string += character;
					break;
				}
			default:
				{
//					string detail = string("Error char <")+character+">.";
//					writeErrorInfo(ERROR_INVADATE_ESC_CHAR,detail);
					return TOKEN_EXTRACT_ERROR;
				}
			}
			//转义序列的第二个字符
			if(itor!=str.end())
			{
				++itor;
				character =	*itor;
			}
			else
			{
				writeErrorInfo(ERROR_MISS_END_OF_STRING);
				return TOKEN_EXTRACT_ERROR;
			}
			if('0'<=character&&character<='7')
			{
				char_esc	= false;
				esc_string += character;
			}
			else
			{
				--itor;
				goto END_ESC;
			}
			//转义序列的第三个字符
			if(itor!=str.end())
			{
				++itor;
				character =	*itor;
			}
			else
			{
				goto END_ESC;
			}
			if('0'<=character&&character<='7')
			{
				char_esc	= false;
				esc_string += character;
			}
			else
			{
				--itor;
				goto END_ESC;
			}
	END_ESC:
			if(char_esc)
			{
				token += esc_string;
			}
			else
			{
				long char_code = 0;
				sscanf(esc_string.c_str(),"%o",&char_code);
				token += static_cast<char>(char_code);
			}
		}
		else
		{
			token += character;
		}
	}
	writeErrorInfo(ERROR_MISS_END_OF_STRING);
	return TOKEN_EXTRACT_ERROR;
}

unsigned long LexicalParse::extractDigital(string& str,string::iterator& itor,unsigned long& format,string& out_token)
{
	const string::iterator itor_extract_begin = itor;
	string	token	= "";
	char character	= '\0';
	long digital_format = TOKEN_TYPE_DIGITIAL_DEC;

	if(itor == str.end())
	{
		return TOKEN_EXTRACT_EOF;
	}

	for(; itor != str.end() ; ++itor)
	{
		char character	= *itor; 

		if(itor == itor_extract_begin)
		{
			//开头是小数点就一定是浮点数（以后可能会升级为科学计数数）
			if(character == '.')
			{
				digital_format = TOKEN_TYPE_DIGITIAL_FLOAT;
				token += "0.";
				continue;
			}
			//开头是0那么具体是十六进制数还是八进制数要看下一个字符
			//注意还有单个0的情况
			else if(character == '0')
			{
				if( (++itor) != str.end())
				{
					if(*itor == 'x' || *itor == 'X')
					{
						digital_format = TOKEN_TYPE_DIGITIAL_HEX;
					}
					else if('1' <= *itor && *itor <= '7')
					{
						--itor;
						digital_format = TOKEN_TYPE_DIGITIAL_OCT;
					}
					else if('8'==*itor || *itor=='9')
					{
						writeErrorInfo(ERROR_8_9_IN_OCT_NUMBER);
						return TOKEN_EXTRACT_ERROR;
					}
					else if('.' == *itor)
					{
						token += "0.";
						digital_format = TOKEN_TYPE_DIGITIAL_FLOAT;
					}
					else
					{
						token += "0";
						--itor;
					}
				}
				else
				{
					token += character;
					--itor;
					digital_format = TOKEN_TYPE_DIGITIAL_DEC;
				}
				continue;
			}
			//开头是非零数字就是十进制数了（以后可能会升级为浮点数，科学计数数）
			else if('1'<=character && character<='9')
			{
				digital_format = TOKEN_TYPE_DIGITIAL_DEC;
				token += character;
				continue;
			}
			else
			{
				itor = itor_extract_begin;
				return TOKEN_EXTRACT_MISS;
			}
		}
		else
		{
			//在从前认为的十进制数中发现小数点升级为浮点数
			//其它情况下都是非法的，科学计数法中小数点不可能出现在E后面
			//换句话说确定是科学计数数后再扫描到小数点是非法的
			if(character == '.')
			{
				if(digital_format == TOKEN_TYPE_DIGITIAL_DEC)
				{
					digital_format = TOKEN_TYPE_DIGITIAL_FLOAT;
					token += character;
					continue;
				}
				else
				{
//					string detail = string("Error char <")+character+">.";
//					writeErrorInfo(ERROR_SCIENCE_NUMBER_ERROR,detail);
					return TOKEN_EXTRACT_ERROR;
				}
			}
			else if( character == 'E' || character == 'e' )
			{
				//这个E是科学计数法中的
				if(digital_format == TOKEN_TYPE_DIGITIAL_DEC || digital_format == TOKEN_TYPE_DIGITIAL_FLOAT)
				{
					digital_format = TOKEN_TYPE_DIGITIAL_SCIENCE;
					token += 'E';
					if(++itor != str.end())
					{
						if(*itor == '-')
						{
							token+= '-';
						}
						else
						{
							--itor;
						}
					}
					continue;
				}
				//是十六进制数的
				else if(digital_format == TOKEN_TYPE_DIGITIAL_HEX)
				{
					token += 'E';
					continue;
				}
				else
				{
					writeErrorInfo(ERROR_E_IN_NUMBER);
					return TOKEN_EXTRACT_ERROR;
				}
			}
			else if(	character == 'A' || character == 'a' || 
						character == 'B' || character == 'b' ||
						character == 'C' || character == 'c' ||
						character == 'D' || character == 'd' ||
						character == 'F' || character == 'f' )
			{
				if(digital_format == TOKEN_TYPE_DIGITIAL_HEX)
				{
					token += islower(character)?toupper(character):character;
				}
				else
				{
//					string detail = string("Error char <")+character+">.";
//					writeErrorInfo(ERROR_CAHR_IN_NUMBER,detail);
					return TOKEN_EXTRACT_ERROR;
				}
			}
			else if('0'<=character && character<='9')
			{
				if(('8'==character || character=='9') && digital_format == TOKEN_TYPE_DIGITIAL_OCT)
				{
					writeErrorInfo(ERROR_8_9_IN_OCT_NUMBER);
					return TOKEN_EXTRACT_ERROR;
				}
				token += character;
			}
			else
			{
				break;
			}
		}
	}
	out_token = token;
	format = digital_format;
	return TOKEN_EXTRACT_HIT;
}

unsigned long LexicalParse::extractOperator(string& str,string::iterator& itor, string& out_token)
{
	const string::iterator itor_extract_begin = itor;
	if(itor == str.end())
	{
		return TOKEN_EXTRACT_EOF;
	}
	string op = "";
	op += *itor;
	string op2 = op;
	++itor;
	if(itor != str.end())
	{
		op2 += *itor;
		++itor;
	}
	if(isOperator(op2))
	{
		out_token = op2;
		return TOKEN_EXTRACT_HIT;
	}
	else if(isOperator(op))
	{
		--itor;
		out_token = op;
		return TOKEN_EXTRACT_HIT;
	}
	itor = itor_extract_begin;
	return TOKEN_EXTRACT_MISS;
}

unsigned long LexicalParse::extractBlank(string& str,string::iterator& itor,string& out_token)
{
	const string::iterator itor_extract_begin = itor;
	string token = "";
	if(itor == str.end())
	{
		return TOKEN_EXTRACT_EOF;
	}
	bool bHit = false;
	for(; itor != str.end() ; ++itor)
	{
		char character	= *itor; 
		if(character <= ' ')
		{
			bHit = true;
			token += character;
			continue;			
		}
		else 
		{
			break;
		}
	}
	if(bHit)
	{
		out_token = token;
		return TOKEN_EXTRACT_HIT;
	}

	itor = itor_extract_begin;
	return TOKEN_EXTRACT_MISS;
}

unsigned long LexicalParse::extractAllToken(string& str,string::iterator& itor,string& token)
{
	unsigned long state;
	unsigned long format;
	unsigned long type;
	
	state = extractComment(str,itor,token);
	if(state == TOKEN_EXTRACT_HIT)
	{
		return TOKEN_TYPE_COMMENT;
	}
	if(state == TOKEN_EXTRACT_ERROR)
	{
		return TOKEN_TYPE_ERROR;
	}
	if(state == TOKEN_EXTRACT_EOF)
	{
		return TOKEN_TYPE_EOF;
	}

	state = extractString(str,itor,token);
	if(state == TOKEN_EXTRACT_HIT)
	{
		return TOKEN_TYPE_STRING;
	}
	if(state == TOKEN_EXTRACT_ERROR)
	{
		return TOKEN_TYPE_ERROR;
	}
	if(state == TOKEN_EXTRACT_EOF)
	{
		return TOKEN_TYPE_EOF;
	}

	state = extractChar(str,itor,token);
	if(state == TOKEN_EXTRACT_HIT)
	{
		return TOKEN_TYPE_CHAR;
	}
	if(state == TOKEN_EXTRACT_ERROR)
	{
		return TOKEN_TYPE_ERROR;
	}
	if(state == TOKEN_EXTRACT_EOF)
	{
		return TOKEN_TYPE_EOF;
	}

	state = extractDigital(str,itor,format,token);
	if(state == TOKEN_EXTRACT_HIT)
	{
		return format;
	}
	if(state == TOKEN_EXTRACT_ERROR)
	{
		return TOKEN_TYPE_ERROR;
	}
	if(state == TOKEN_EXTRACT_EOF)
	{
		return TOKEN_TYPE_EOF;
	}

	state = extractOperator(str,itor,token);
	if(state == TOKEN_EXTRACT_HIT)
	{
		return TOKEN_TYPE_OPERATOR;
	}
	if(state == TOKEN_EXTRACT_ERROR)
	{ 
		return TOKEN_TYPE_ERROR;
	}
	if(state == TOKEN_EXTRACT_EOF)
	{ 
		return TOKEN_TYPE_EOF;
	}

	state = extractIdentity(str,itor,type,token);
	if(state == TOKEN_EXTRACT_HIT)
	{
		return type;
	}
	if(state == TOKEN_EXTRACT_ERROR)
	{
		return TOKEN_TYPE_ERROR;
	}
	if(state == TOKEN_EXTRACT_EOF)
	{
		return TOKEN_TYPE_EOF;
	}

	state = extractBlank(str,itor,token);
	if(state == TOKEN_EXTRACT_HIT)
	{ 
		return TOKEN_TYPE_BLANK;
	}
	if(state == TOKEN_EXTRACT_ERROR)
	{ 
		return TOKEN_TYPE_ERROR;
	}
	if(state == TOKEN_EXTRACT_EOF)
	{ 
		return TOKEN_TYPE_EOF;
	}
	return TOKEN_TYPE_ERROR;
}

#endif
