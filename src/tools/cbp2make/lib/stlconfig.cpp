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
#include <cstdio>
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "stlconfig.h"
//------------------------------------------------------------------------------

CConfiguration::CConfiguration(void)
{
 Clear();
}

CConfiguration::~CConfiguration(void)
{
 Clear();
}

void CConfiguration::Clear(void)
{
 for (size_t i = 0; i < m_Variables.size(); i++) delete m_Variables[i];
 m_Variables.clear();
 m_NullVariable.SetName("NULL");
 m_DefinedPrefix = "~";
}

int CConfiguration::GetCount(void) const
{
 return m_Variables.size();
}

bool CConfiguration::ValidIndex(const int Index) const
{
 return ((Index>=0)&&(Index<GetCount()));
}

bool CConfiguration::VarDefined(const CString& Name) const
{
 int index = VarIndex(DefinedPrefix()+Name);
 return (INVALID_INDEX!=index);
}

void CConfiguration::SetDefined(const CString& Name)
{
 int index = VarIndex(DefinedPrefix()+Name);
 if (INVALID_INDEX==index) InsertBooleanVariable(DefinedPrefix()+Name,true);
}

void CConfiguration::SetUndefined(const CString& Name)
{
 RemoveVariable(DefinedPrefix()+Name);
}

CVariable& CConfiguration::Variable(const int Index)
{
 if (ValidIndex(Index)) return *m_Variables[Index]; else return m_NullVariable;
}

CVariable& CConfiguration::VarNamed(const CString& Name)
{
 return Variable(VarIndex(Name));
}

int CConfiguration::VarIndex(const CString& Name) const
{
 for (size_t i = 0; i < m_Variables.size(); i++)
 {
  CVariable *variable = m_Variables[i];
  if (variable->GetName()==Name) return i;
 }
 return INVALID_INDEX;
}

int CConfiguration::InsertIntegerVariable(const CString& Name, const int Value)
{
 m_Variables.push_back(new CIntegerVariable(Name,Value));
 return GetCount()-1;
}

int CConfiguration::InsertFloatVariable(const CString& Name, const double Value)
{
 m_Variables.push_back(new CFloatVariable(Name,Value));
 return GetCount()-1;
}

int CConfiguration::InsertBooleanVariable(const CString& Name, const bool Value)
{
 m_Variables.push_back(new CBooleanVariable(Name,Value));
 return GetCount()-1;
}

int CConfiguration::InsertStringVariable(const CString& Name, const CString& Value)
{
 m_Variables.push_back(new CStringVariable(Name,Value));
 return GetCount()-1;
}

int CConfiguration::InsertCharVariable(const CString& Name, const char Value)
{
 m_Variables.push_back(new CCharVariable(Name,Value));
 return GetCount()-1;
}

void CConfiguration::RemoveVariable(const int Index)
{
 if (ValidIndex(Index))
 {
  CVariable *variable = m_Variables[Index];
  m_Variables.erase(m_Variables.begin()+Index);
  delete variable;
 }
}

void CConfiguration::RemoveVariable(const CString& Name)
{
 RemoveVariable(VarIndex(Name));
}

void CConfiguration::SetIntegerVariable(const CString& Name, const int Value)
{
 int index = VarIndex(Name);
 if (ValidIndex(index))
 {
  CVariable *variable = m_Variables[index];
  variable->SetInteger(Value);
 }
 else
 {
  InsertIntegerVariable(Name,Value);
 }
}

void CConfiguration::SetFloatVariable(const CString& Name, const double Value)
{
 int index = VarIndex(Name);
 if (ValidIndex(index))
 {
  CVariable *variable = m_Variables[index];
  variable->SetFloat(Value);
 }
 else
 {
  InsertFloatVariable(Name,Value);
 }
}

void CConfiguration::SetBooleanVariable(const CString& Name, const bool Value)
{
 int index = VarIndex(Name);
 if (ValidIndex(index))
 {
  CVariable *variable = m_Variables[index];
  variable->SetBoolean(Value);
 }
 else
 {
  InsertBooleanVariable(Name,Value);
 }
}

void CConfiguration::SetStringVariable(const CString& Name, const CString& Value)
{
 int index = VarIndex(Name);
 if (ValidIndex(index))
 {
  CVariable *variable = m_Variables[index];
  variable->SetString(Value);
 }
 else
 {
  InsertStringVariable(Name,Value);
 }
}

void CConfiguration::SetCharVariable(const CString& Name, const char Value)
{
 int index = VarIndex(Name);
 if (ValidIndex(index))
 {
  CVariable *variable = m_Variables[index];
  variable->SetChar(Value);
 }
 else
 {
  InsertCharVariable(Name,Value);
 }
}

void CConfiguration::Print(std::ostream& out)
{
 for (size_t i = 0; i < m_Variables.size(); i++)
 {
  CVariable *variable = m_Variables[i];
  out<<variable->GetTypeName().GetCString()<<" "<<variable->GetName().GetCString()<<"=";
  if (VARIABLE_TYPE_STRING == variable->GetType())
  {
   out<<"\""<<variable->GetString().GetCString()<<"\";"<<std::endl;
  }
  else if (VARIABLE_TYPE_CHAR == variable->GetType())
  {
   out<<"'"<<variable->GetString().GetCString()<<"';"<<std::endl;
  }
  else
  {
   out<<variable->GetString().GetCString()<<";"<<std::endl;
 }}
}

void CConfiguration::LoadFromFile(const CString& FileName)
{
 CStringList configuration, sub_strings;
 CStringListIterator iterator(&configuration);
 configuration.LoadFromFile(FileName);
 //
 for (int i = 0; i < configuration.GetCount(); i++)
 {
  ParseStr(configuration[i],"=;",sub_strings);
  CVariable variable = VarNamed(sub_strings[0]);
  if (variable.GetType()==VARIABLE_TYPE_CHAR)
  {
   CStringList sub_strings2;
   ParseStr(sub_strings[1],"''",sub_strings2);
   variable.SetString(sub_strings2[1]);
  }
  else if (variable.GetType()==VARIABLE_TYPE_STRING)
  {
   CStringList sub_strings2;
   ParseStr(sub_strings[1],"\"\"",sub_strings2);
   variable.SetString(sub_strings2[1]);
  }
  sub_strings.Clear();
 }
}

void CConfiguration::SaveToFile(const CString& FileName)
{
 FILE *stream = fopen(FileName.GetCString(),"wt");
 for (size_t i = 0; i < m_Variables.size(); i++)
 {
  CVariable *variable = m_Variables[i];
  CString string = variable->GetName()+"=";
  if ((variable->GetType()==VARIABLE_TYPE_STRING)||
      (variable->GetType()==VARIABLE_TYPE_CHAR))
  { string+="\""+variable->GetString()+"\""; }
  else
  { string+=variable->GetString(); }
  string.Append(';').AppendEOL();
  fwrite(string.GetCString(),string.GetLength(),1,stream);
 }
 fclose(stream);
}

CParameterString::CParameterString(int argc, char* argv[])
{
 SetParameters(argc,argv);
}

CParameterString::CParameterString(const CString& Parameters)
{
 SetParameters(Parameters);
}

CParameterString::CParameterString(void)
{
 m_NullParameter.Clear();
 m_Parameters.Clear();
}

CParameterString::~CParameterString(void)
{
}

void CParameterString::SetParameters(int argc, char* argv[])
{
 m_Parameters.Clear();
 for (int i = 0; i < argc; i++) m_Parameters.Insert(CString(argv[i]));
}

void CParameterString::SetParameters(const CString& Parameters)
{
 m_Parameters.Clear();
 ParseStr(Parameters,' ',m_Parameters);
}

void CParameterString::SetParameters(const CParameterString& Parameters)
{
 m_Parameters = Parameters.m_Parameters;
}

void CParameterString::AddParameters(const CString& Parameters)
{
 CStringList l_Parameters;
 ParseStr(Parameters,' ',l_Parameters);
 m_Parameters.Insert(l_Parameters);
}

void CParameterString::AddParameters(const CParameterString& Parameters)
{
 m_Parameters.Insert(Parameters.m_Parameters);
}

CString CParameterString::Parameter(const int Index) const
{
 return m_Parameters[Index];
}

bool CParameterStringConfiguration::VarDefined(const CString& Name)
{
 int index = VarIndex(DefinedPrefix()+Name);
 return (INVALID_INDEX!=index);
}

void CParameterStringConfiguration::SetDefined(const CString& Name)
{
 int index = VarIndex(DefinedPrefix()+Name);
 if (INVALID_INDEX==index) InsertBooleanVariable(DefinedPrefix()+Name,true);
}

void CParameterStringConfiguration::SetUndefined(const CString& Name)
{
 RemoveVariable(DefinedPrefix()+Name);
}

void CParameterStringConfiguration::ProcessParameters(const CParameterString& Parameters)
{
 int j = 1; // parameterIndex
 while (j < Parameters.GetCount())
 {
  CString parameter = Parameters.Parameter(j);
  for (size_t i = 0; i < m_Variables.size(); i++)
  {
   CVariable& variable = Variable(i);
   if (variable.GetName()==parameter)
   {
    if (variable.GetType()==VARIABLE_TYPE_BOOLEAN) variable.SetBoolean(true);
    else { variable.SetString(Parameters.Parameter(++j)); }
    m_Variables.push_back(new CBooleanVariable(m_DefinedPrefix+variable.GetName(),true));
    break;
  }}
  j++;
 }
}

void CParameterStringConfiguration::ProcessParameters(const CString& Parameters)
{
 CParameterString parameterString(Parameters);
 ProcessParameters(parameterString);
}

//------------------------------------------------------------------------------
