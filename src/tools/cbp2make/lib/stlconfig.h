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
#ifndef STL_CONFIG_H
#define STL_CONFIG_H
//------------------------------------------------------------------------------
#include "stlvariables.h"
//------------------------------------------------------------------------------

class CConfiguration
{
 protected:
  CVariable m_NullVariable;
  std::vector<CVariable *> m_Variables;
  CString m_DefinedPrefix;
 protected:
  bool ValidIndex(const int Index) const;
 public:
  virtual void Initialize(void) {};
  virtual void Clear(void);
  int GetCount(void) const;
  CString& DefinedPrefix(void) { return m_DefinedPrefix; }
  CString DefinedPrefix(void) const { return m_DefinedPrefix; }
  bool VarDefined(const CString& Name) const;
  void SetDefined(const CString& Name);
  void SetUndefined(const CString& Name);
  CVariable& Variable(const int Index);
  CVariable& VarNamed(const CString& Name);
  int VarIndex(const CString& Name) const;
  int InsertIntegerVariable(const CString& Name, const int Value=0);
  int InsertFloatVariable(const CString& Name, const double Value=0.0);
  int InsertBooleanVariable(const CString& Name, const bool Value=false);
  int InsertStringVariable(const CString& Name, const CString& Value="");
  int InsertCharVariable(const CString& Name, const char Value=char(0));
  void RemoveVariable(const int Index);
  void RemoveVariable(const CString& Name);
  void SetIntegerVariable(const CString& Name, const int Value);
  void SetFloatVariable(const CString& Name, const double Value);
  void SetBooleanVariable(const CString& Name, const bool Value);
  void SetStringVariable(const CString& Name, const CString& Value);
  void SetCharVariable(const CString& Name, const char Value);
  void Print(std::ostream& out);
  void ProcessParameters(int argc, char* argv[]);
  void LoadFromFile(const CString& FileName);
  void SaveToFile(const CString& FileName);
 public:
  CConfiguration(void);
  virtual ~CConfiguration(void);
};

class CParameterString
{
 protected:
  CString m_NullParameter;
  CStringList m_Parameters;
 public:
  void SetParameters(int argc, char* argv[]);
  void SetParameters(const CString& Parameters);
  void SetParameters(const CParameterString& Parameters);
  void AddParameters(const CString& Parameters);
  void AddParameters(const CParameterString& Parameters);
  int GetCount(void) const { return m_Parameters.GetCount(); }
  CString Parameter(const int Index) const;
  virtual void Print(std::ostream& out) { (void)out; };
 public:
  CParameterString(int argc, char* argv[]);
  CParameterString(const CString& Parameters);
  CParameterString(void);
  virtual ~CParameterString(void);
};

class CParameterStringConfiguration: public CConfiguration
{
 protected:
  CString m_DefinedPrefix;
 public:
  CString& DefinedPrefix(void) { return m_DefinedPrefix; }
  bool VarDefined(const CString& Name);
  void SetDefined(const CString& Name);
  void SetUndefined(const CString& Name);
  void ProcessParameters(const CParameterString& Parameters);
  void ProcessParameters(const CString& Parameters);
 public:
  CParameterStringConfiguration(void) { m_DefinedPrefix = '~'; };
  virtual ~CParameterStringConfiguration(void) {};
};

#endif
//------------------------------------------------------------------------------
