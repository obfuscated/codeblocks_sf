/*
    cbp2make : Makefile generation tool for the Code::Blocks IDE
    Copyright (C) 2010-2013 Mirai Computing (mirai.computing@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//------------------------------------------------------------------------------
#ifndef STL_CONVERT_H
#define STL_CONVERT_H
//------------------------------------------------------------------------------
#include <cstdio>
#include <sstream>
//------------------------------------------------------------------------------
#include "stlstrings.h"
//------------------------------------------------------------------------------

inline double IntToFloat(const int Value)
{
 return (double)Value;
}

inline bool IntToBool(const int Value)
{
 return (0!=Value);
}

inline std::string IntToStr(const int Value)
{
 std::string result;
 std::stringstream tmp;
 tmp<<Value;
 tmp>>result;
 return result;
}

inline char IntToChar(const int Value)
{
 return (Value & 0xff);
}

inline int FloatToInt(const double Value)
{
 return (int)Value;
}

inline bool FloatToBool(const double Value)
{
 return (0.0!=Value);
}

inline std::string FloatToStr(const double Value)
{
 std::string result;
 std::stringstream tmp;
 tmp<<Value;
 tmp>>result;
 return result;
}

inline char FloatToChar(const double Value)
{
 return IntToChar(FloatToInt(Value));
}

inline double BoolToFloat(const bool Value)
{
 return (double)Value;
}

inline int BoolToInt(const bool Value)
{
 return (int)Value;
}

inline std::string BoolToStr(const bool Value)
{
 if (Value) return "true"; else return "false";
}

inline char BoolToChar(const bool Value)
{
 if (Value) return '1'; else return '0';
}

inline double StrToFloat(const std::string& Value)
{
 double result;
 std::stringstream tmp;
 tmp<<Value;
 tmp>>result;
 return result;
}

inline int StrToInt(const std::string& Value)
{
 int result;
 std::stringstream tmp;
 tmp<<Value;
 tmp>>result;
 return result;
}

inline bool StrToBool(const std::string& Value)
{
 return (UpperCase(Value).GetString().compare("TRUE")==0);
}

inline char StrToChar(const std::string& Value)
{
 char result = 0;
 if (Value.size()) result = Value[0];
 return result;
}

inline double CharToFloat(const char Value)
{
 return (double)Value;
}

inline int CharToInt(const char Value)
{
 return (int)Value;
}

inline bool CharToBool(const char Value)
{
 return ('0'!=Value);
}

inline std::string CharToStr(const char Value)
{
 std::string result;
 result.resize(1);
 result[0] = Value;
 return result;
}

//------------------------------------------------------------------------------

inline CString IntegerToString(const int Value)
{
 CString result;
 result = (int)Value;
 return result;
}

inline CString Int64ToString(const long long int Value)
{
 CString result;
 result = (long long int)Value;
 return result;
}

inline CString FloatToString(const double Value)
{
 CString result;
 result = Value;
 return result;
}

inline CString FloatToString(const double Value, const CString& Format)
{
 CString result; result.SetLength(MAX_SHORTSTRING_LENGTH);
 sprintf(result.GetCString(),Format.GetCString(),Value);
 result.SetLength();
 return result;
}

inline CString BooleanToString(const bool Value)
{
 if (Value) return CString("true"); else return CString("false");
}

inline CString BooleanToYesNoString(const bool Value)
{
 if (Value) return CString("yes"); else return CString("no");
}

inline double StringToFloat(const CString& Value)
{
 return Value.GetFloat();
}

inline int StringToInteger(const CString& Value)
{
 return Value.GetInteger();
}

inline bool StringToBoolean(const CString& Value)
{
 return ((CString(Value)=="1")||(UpperCase(Value)=="TRUE")||(UpperCase(Value)=="YES"));
}

inline char StringToChar(const CString& Value)
{
 return Value.GetChar(0);
}

inline CString CharToString(const char Value)
{
 CString result;
 result.Assign((int)Value);
 return result;
}

#endif
//------------------------------------------------------------------------------
