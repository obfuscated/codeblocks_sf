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
#ifndef STL_VARIABLES_H
#define STL_VARIABLES_H
//------------------------------------------------------------------------------
#include "stlconvert.h"
//------------------------------------------------------------------------------

const int VARIABLE_TYPE_NONE    = 0;
const int VARIABLE_TYPE_INTEGER = 1;
const int VARIABLE_TYPE_FLOAT   = 2;
const int VARIABLE_TYPE_BOOLEAN = 1;
const int VARIABLE_TYPE_STRING  = 4;
const int VARIABLE_TYPE_CHAR    = 5;
const int VARIABLE_TYPE_COUNT   = 6;

class CVariable
{
 protected:
  CString m_Name;
 public:
  virtual int GetType(void) const;
  virtual CString GetTypeName(void) const;
  CString& GetName(void);
  void SetName(const CString& Name);
  virtual double GetFloat(void) const;
  virtual void SetFloat(const double Value);
  virtual int GetInteger(void) const;
  virtual void SetInteger(const int Value);
  virtual bool GetBoolean(void) const;
  virtual void SetBoolean(const bool Value);
  virtual CString GetString(void) const;
  virtual void SetString(const CString& Value);
  virtual char GetChar(void) const;
  virtual void SetChar(const char Value);
  void Print(std::ostream& out);
 public:
  CVariable(void);
  CVariable(const CString& Name);
  virtual ~CVariable(void);
};

class CIntegerVariable: public CVariable
{
 protected:
  int m_Value;
 public:
  virtual int GetType(void) const;
  virtual CString GetTypeName(void) const;
  virtual double GetFloat(void) const;
  virtual void SetFloat(const double Value);
  virtual int GetInteger(void) const;
  virtual void SetInteger(const int Value);
  virtual bool GetBoolean(void) const;
  virtual void SetBoolean(const bool Value);
  virtual CString GetString(void) const;
  virtual void SetString(const CString& Value);
  virtual char GetChar(void) const;
  virtual void SetChar(const char Value);
  int operator=(const CIntegerVariable& Variable);
  CIntegerVariable& operator =(const double Value);
  CIntegerVariable& operator =(const int Value);
  CIntegerVariable& operator =(const bool Value);
  CIntegerVariable& operator =(const CString& Value);
  CIntegerVariable& operator =(const char Value);
 public:
  CIntegerVariable(const CString& Name,const int Value=0);
  virtual ~CIntegerVariable(void) {};
};

class CFloatVariable: public CVariable
{
 protected:
  double m_Value;
 public:
  virtual int GetType(void) const;
  virtual CString GetTypeName(void) const;
  virtual double GetFloat(void) const;
  virtual void SetFloat(const double Value);
  virtual int GetInteger(void) const;
  virtual void SetInteger(const int Value);
  virtual bool GetBoolean(void) const;
  virtual void SetBoolean(const bool Value);
  virtual CString GetString(void) const;
  virtual void SetString(const CString& Value);
  virtual char GetChar(void) const;
  virtual void SetChar(const char Value);
  double operator=(const CFloatVariable& Variable);
  CFloatVariable& operator =(const double Value);
  CFloatVariable& operator =(const int Value);
  CFloatVariable& operator =(const bool Value);
  CFloatVariable& operator =(const CString& Value);
  CFloatVariable& operator =(const char Value);
 public:
  CFloatVariable(const CString& Name,const double Value=0.0);
  virtual ~CFloatVariable(void) {};
};

class CBooleanVariable: public CVariable
{
 protected:
  bool m_Value;
 public:
  virtual int GetType(void) const;
  virtual CString GetTypeName(void) const;
  virtual double GetFloat(void) const;
  virtual void SetFloat(const double Value);
  virtual int GetInteger(void) const;
  virtual void SetInteger(const int Value);
  virtual bool GetBoolean(void) const;
  virtual void SetBoolean(const bool Value);
  virtual CString GetString(void) const;
  virtual void SetString(const CString& Value);
  virtual char GetChar(void) const;
  virtual void SetChar(const char Value);
  bool operator=(const CBooleanVariable& Variable);
  CBooleanVariable& operator =(const double Value);
  CBooleanVariable& operator =(const int Value);
  CBooleanVariable& operator =(const bool Value);
  CBooleanVariable& operator =(const CString& Value);
  CBooleanVariable& operator =(const char Value);
 public:
  CBooleanVariable(const CString& Name,const bool Value=false);
  virtual ~CBooleanVariable(void) {};
};

class CStringVariable: public CVariable
{
 protected:
  CString m_Value;
 public:
  virtual int GetType(void) const;
  virtual CString GetTypeName(void) const;
  virtual double GetFloat(void) const;
  virtual void SetFloat(const double Value);
  virtual int GetInteger(void) const;
  virtual void SetInteger(const int Value);
  virtual bool GetBoolean(void) const;
  virtual void SetBoolean(const bool Value);
  virtual CString GetString(void) const;
  virtual void SetString(const CString& Value);
  virtual char GetChar(void) const;
  virtual void SetChar(const char Value);
  CString operator=(const CStringVariable& Variable);
  CStringVariable& operator =(const double Value);
  CStringVariable& operator =(const int Value);
  CStringVariable& operator =(const bool Value);
  CStringVariable& operator =(const CString& Value);
  CStringVariable& operator =(const char Value);
 public:
  CStringVariable(const CString& Name,const CString Value="");
  virtual ~CStringVariable(void) {};
};

class CCharVariable: public CVariable
{
 protected:
  char m_Value;
 public:
  virtual int GetType(void) const;
  virtual CString GetTypeName(void) const;
  virtual double GetFloat(void) const;
  virtual void SetFloat(const double Value);
  virtual int GetInteger(void) const;
  virtual void SetInteger(const int Value);
  virtual bool GetBoolean(void) const;
  virtual void SetBoolean(const bool Value);
  virtual CString GetString(void) const;
  virtual void SetString(const CString& Value);
  virtual char GetChar(void) const;
  virtual void SetChar(const char Value);
  char operator=(const CCharVariable& Variable);
  CCharVariable& operator =(const double Value);
  CCharVariable& operator =(const int Value);
  CCharVariable& operator =(const bool Value);
  CCharVariable& operator =(const CString& Value);
  CCharVariable& operator =(const char Value);
 public:
  CCharVariable(const CString& Name,const char Value=char(0));
  virtual ~CCharVariable(void) {};
};

#endif
//------------------------------------------------------------------------------
