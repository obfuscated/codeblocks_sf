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
#include "stlgpm.h"
#include "stlfutils.h"
#include <iostream>
//------------------------------------------------------------------------------

CGenericProcessingMachine::CGenericProcessingMachine(void)
{
 m_FileConfiguration.Clear();
 m_ParameterStringConfiguration.Clear();
 //m_ParameterString.Clear();
 m_InputFileList.Clear();
 m_OutputFileList.Clear();
 m_FileIndex = 0;
 m_AliasesEnabled = false;
 m_FileNameLength = 0;
 m_VerboseMode = false;
 m_QuietMode = false;
 m_HelpMode = false;
}

CGenericProcessingMachine::~CGenericProcessingMachine(void)
{
 //
}

void CGenericProcessingMachine::Initialize(void)
{
 CreateConfiguration();
 PSC().ProcessParameters(m_ParameterString);
 Configure(ConfigurationName());
 m_FileNameLength = 12;
}

void CGenericProcessingMachine::Initialize(int argc, char * argv[])
{
 PS().SetParameters(argc,argv);
 Initialize();
}

void CGenericProcessingMachine::Initialize(const CParameterString& Parameters)
{
 PS().SetParameters(Parameters);
 Initialize();
}

void CGenericProcessingMachine::DisplayHelpMessage(void)
{
 // do nothing //
}

void CGenericProcessingMachine::CreateConfiguration(void)
{
 PSC().InsertStringVariable(GPM_VAR_NAME_IN);
 PSC().InsertStringVariable(GPM_VAR_NAME_OUT);
 PSC().InsertBooleanVariable(GPM_VAR_NAME_LIST);
 PSC().InsertStringVariable(GPM_VAR_NAME_CFG);
 PSC().InsertStringVariable(GPM_VAR_NAME_INDIR);
 PSC().InsertStringVariable(GPM_VAR_NAME_OUTDIR);
 PSC().InsertBooleanVariable(GPM_VAR_NAME_HELP);
 PSC().InsertBooleanVariable(GPM_VAR_NAME_VERBOSE);
 PSC().InsertBooleanVariable(GPM_VAR_NAME_QUIET);
 if (m_AliasesEnabled)
 {
  PSC().InsertBooleanVariable(GPM_VAR_NAME_HELP_ALIAS);
  PSC().InsertBooleanVariable(GPM_VAR_NAME_VERBOSE_ALIAS);
  PSC().InsertBooleanVariable(GPM_VAR_NAME_QUIET_ALIAS);
 }
}

CString CGenericProcessingMachine::DefaultConfigurationName(void)
{
 return ChangeFileExt(ExtractFileName(PS().Parameter(0)),".cfg");
}

CString CGenericProcessingMachine::ConfigurationName(void)
{
 if (PSC().VarDefined(GPM_VAR_NAME_CFG))
 {
  return PSC().VarNamed(GPM_VAR_NAME_CFG).GetString();
 }
 else
 {
  return DefaultConfigurationName();
 }
}

bool CGenericProcessingMachine::Configure(const CString& FileName)
{
 bool result = true;
 //
 if (m_AliasesEnabled)
 {
  m_VerboseMode = PSC().VarDefined(GPM_VAR_NAME_VERBOSE_ALIAS);
  m_QuietMode = PSC().VarDefined(GPM_VAR_NAME_QUIET_ALIAS);
  m_HelpMode = PSC().VarDefined(GPM_VAR_NAME_HELP_ALIAS);
 }
 else
 {
  m_VerboseMode = PSC().VarDefined(GPM_VAR_NAME_VERBOSE);
  m_QuietMode = PSC().VarDefined(GPM_VAR_NAME_QUIET);
  m_HelpMode = PSC().VarDefined(GPM_VAR_NAME_HELP);
 }
 //PSC().ProcessParameters(m_ParameterString);
 if ((FileName.GetLength()>0)&&(FileExists(FileName.GetString())))
 {
  CFG().LoadFromFile(FileName);
 }
 else
 {
  result = false;
 }
 if (PSC().VarDefined(GPM_VAR_NAME_LIST))
 {
  ILST().Clear();
  ILST().LoadFromFile(PSC().VarNamed(GPM_VAR_NAME_IN).GetString());
 }
 else if (PSC().VarDefined(GPM_VAR_NAME_IN))
 {
  ILST().Clear();
  ILST().Insert(PSC().VarNamed(GPM_VAR_NAME_IN).GetString());
 }
 return result;
}

int CGenericProcessingMachine::SetFileNameLength(const int FileNameLength)
{
 m_FileNameLength = std::max(FileNameLength,1);
 return m_FileNameLength;
}

CString CGenericProcessingMachine::TargetName(const int FileIndex, const CString& SourceFileName)
{
 (void)SourceFileName; return EnumStr("",FileIndex,".out",m_FileNameLength);
}

bool CGenericProcessingMachine::ProcessFile(const CString& SourceFileName, CString& TargetFileName)
{
 (void)SourceFileName;(void)TargetFileName;
 // override this function //
 return true;
}

bool CGenericProcessingMachine::PreProcess(void)
{
 // override this function //
 return true;
}

bool CGenericProcessingMachine::Run(void)
{
 if (!PreProcess()) return false;
 //
 bool result = true;
 CString source_directory; CString target_directory;
 if (PSC().VarDefined(GPM_VAR_NAME_INDIR))
 {
  source_directory = CheckLastChar(PSC().VarNamed(GPM_VAR_NAME_INDIR).GetString(),'/');
  PSC().VarNamed(GPM_VAR_NAME_INDIR).SetString(source_directory);
 }
 if (PSC().VarDefined(GPM_VAR_NAME_OUTDIR))
 {
  target_directory = CheckLastChar(PSC().VarNamed(GPM_VAR_NAME_OUTDIR).GetString(),'/');
  PSC().VarNamed(GPM_VAR_NAME_OUTDIR).SetString(target_directory);
 }
 bool do_rename_outfile = ((!PSC().VarDefined(GPM_VAR_NAME_LIST))&&(PSC().VarDefined(GPM_VAR_NAME_OUT)));
 bool do_create_outlist = ((PSC().VarDefined(GPM_VAR_NAME_LIST))&&(PSC().VarDefined(GPM_VAR_NAME_OUT)));
 //if ((ILST().GetCount()>1)||(!PSC().VarDefined(GPM_VAR_NAME_OUT)))
 for (int i = 0; i < ILST().GetCount(); i++)
 {
  m_FileIndex = i;
  if (ILST().GetString(i).GetLength()==0) continue;
  CString source_file_name = source_directory + ILST().GetString(i);
  CString target_name = TargetName(i,ILST().GetString(i));
  if (do_rename_outfile)
  {
   target_name = PSC().VarNamed(GPM_VAR_NAME_OUT).GetString();
  }
  CString target_file_name = target_directory + target_name;
  result = result && ProcessFile(source_file_name,target_file_name);
  if (!target_file_name.IsEmpty())
  {
   OLST().Insert(target_name);
  }
 }
 if (do_create_outlist)
 {
  OLST().SaveToFile(PSC().VarNamed(GPM_VAR_NAME_OUT).GetString());
 }
 //
 result = result && PostProcess();
 return result;
}

bool CGenericProcessingMachine::PostProcess(void)
{
 // override this function //
 return true;
}

void CGenericProcessingMachine::Finalize(void)
{
}

/*
CGenericProcessingMachine::CGenericProcessingMachine(void)
{
}

CGenericProcessingMachine::~CGenericProcessingMachine(void)
{
}
*/

//------------------------------------------------------------------------------
