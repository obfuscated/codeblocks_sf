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
#ifndef STL_GPM_H
#define STL_GPM_H
//------------------------------------------------------------------------------
#include "stlconfig.h"
//------------------------------------------------------------------------------

class CGenericProcessingMachine
{
 protected:
  CConfiguration m_FileConfiguration;
  CParameterStringConfiguration m_ParameterStringConfiguration;
  CParameterString m_ParameterString;
  CStringList m_InputFileList;
  CStringList m_OutputFileList;
  int m_FileIndex;
  bool m_AliasesEnabled;
  int m_FileNameLength;
  bool m_VerboseMode;
  bool m_QuietMode;
  bool m_HelpMode;
 protected:
  virtual void Initialize(void);
 public:
  virtual void Initialize(int argc, char * argv[]);
  virtual void Initialize(const CParameterString& Parameters);
  virtual void DisplayHelpMessage(void);
  virtual void CreateConfiguration(void);
  CString DefaultConfigurationName(void);
  virtual CString ConfigurationName(void);
  virtual bool Configure(const CString& FileName);
  int GetFileNameLength(void) const { return m_FileNameLength; }
  int SetFileNameLength(const int FileNameLength);
  int FileIndex(void) const { return m_FileIndex; }
  virtual CString TargetName(const int FileIndex, const CString& SourceFileName);
  virtual bool ProcessFile(const CString& SourceFileName, CString& TargetFileName);
  virtual bool PreProcess(void);
  virtual bool Run(void);
  virtual bool PostProcess(void);
  virtual void Finalize(void);
  virtual void Print(std::ostream& out) { };
 public:
  bool& AliasesEnabled(void) { return m_AliasesEnabled; }
  bool& BeVerbose(void) { return m_VerboseMode; }
  bool& BeQuiet(void) { return m_QuietMode; }
  bool& DoShowHelp(void) { return m_HelpMode; }
  CConfiguration& CFG(void) { return m_FileConfiguration; }
  CParameterStringConfiguration& PSC(void) { return m_ParameterStringConfiguration; }
  CParameterString& PS(void) { return m_ParameterString; }
  CStringList& ILST(void) { return m_InputFileList; };
  CStringList& OLST(void) { return m_OutputFileList; };
 public:
  CGenericProcessingMachine(void);
  virtual ~CGenericProcessingMachine(void);
};

const CString GPM_VAR_NAME_IN   = "-in";
const CString GPM_VAR_NAME_OUT  = "-out";
const CString GPM_VAR_NAME_LIST = "-list";
const CString GPM_VAR_NAME_CFG  = "-cfg";
const CString GPM_VAR_NAME_INDIR   = "-indir";
const CString GPM_VAR_NAME_OUTDIR  = "-outdir";
const CString GPM_VAR_NAME_HELP   = "--help";
const CString GPM_VAR_NAME_VERBOSE = "--verbose";
const CString GPM_VAR_NAME_QUIET = "--quiet";
const CString GPM_VAR_NAME_HELP_ALIAS   = "-h";
const CString GPM_VAR_NAME_VERBOSE_ALIAS = "-v";
const CString GPM_VAR_NAME_QUIET_ALIAS = "-q";

#endif
//------------------------------------------------------------------------------
