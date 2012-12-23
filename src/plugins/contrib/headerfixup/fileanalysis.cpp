/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#ifndef CB_PRECOMP
  #include "manager.h"
  #include "editormanager.h"
  #include "cbeditor.h"
  #include "logmanager.h"
#endif

#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/regex.h>

#include "cbstyledtextctrl.h"
#include "fileanalysis.h"

const wxString reInclude = _T("^[ \t]*#[ \t]*include[ \t]+[\"<]([^\">]+)[\">]");
const wxString reFwdDecl = _T("class[ \\t]*([A-Za-z]+[A-Za-z0-9_]*);");

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

FileAnalysis::FileAnalysis()
{
  Reset();
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

FileAnalysis::FileAnalysis(const wxString& FileName)
{
  ReInit(FileName);
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

FileAnalysis::~FileAnalysis()
{
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void FileAnalysis::ReInit(const wxString& FileName, bool verbose)
{
  Reset();
  m_FileName = FileName;
  m_Verbose  = verbose;

  wxFileName FileNameObj(m_FileName);
  if ( FileNameObj.GetExt().Lower() == _T("h")   ||
       FileNameObj.GetExt().Lower() == _T("hh")  ||
       FileNameObj.GetExt().Lower() == _T("hpp") ||
       FileNameObj.GetExt().Lower() == _T("h++") ||
       FileNameObj.GetExt().Lower() == _T("hxx") )
  {
    m_IsHeaderFile = true;
  }
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void FileAnalysis::LoadFile()
{
  // reading content of the file
  m_Editor = Manager::Get()->GetEditorManager()->GetBuiltinEditor(m_FileName);
  if ( m_Editor )
    m_FileContent = m_Editor->GetControl()->GetText();
  else
  {
    wxFFile File(m_FileName,_T("rb"));
    if ( !File.IsOpened() )
      return;

    File.ReadAll(&m_FileContent,wxConvUTF8);
  }

  // tokenise the file upon linefeeds
  wxStringTokenizer Tknz(m_FileContent,_T("\n\r"));
  while (Tknz.HasMoreTokens())
    m_LinesOfFile.Add(Tknz.GetNextToken());
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void FileAnalysis::SaveFile(const wxString& Prepend)
{
  if ( m_Editor )
  {
    m_Editor->GetControl()->SetTargetStart(0);
    m_Editor->GetControl()->SetTargetEnd(0);
    m_Editor->GetControl()->ReplaceTarget(Prepend);
  }
  else
  {
    m_FileContent = Prepend + m_FileContent;

    wxFFile File;
    if ( !File.Open(m_FileName,_T("wb")) )
    {
      Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: ")+m_FileName+_T("\" could not be updated (opened).")));
      return;
    }

    if ( !File.Write(m_FileContent,wxConvUTF8) )
    {
      Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: ")+m_FileName+_T("\" could not be updated (written).")));
      return;
    }

    if ( !File.Close() )
    {
      Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: ")+m_FileName+_T("\" could not be closed.")));
      return;
    }
  }
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxString FileAnalysis::GetNextLine()
{
  if ( HasMoreLines() )
  {
    wxString LineOfFile = m_LinesOfFile.Item(m_CurrentLine);
    m_CurrentLine++;
    return LineOfFile;
  }

  return wxEmptyString;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxString FileAnalysis::GetEOL()
{
  wxString EOL = _T('\n');

  // Detect end-of-line style to prevent inconsistent EOLs
  for ( size_t i=0; i<m_FileContent.Len(); i++ )
  {
    if (   m_FileContent.GetChar(i)!=_T('\n')
        && m_FileContent.GetChar(i)!=_T('\r') )
    {
      continue; // no EOL
    }

    EOL = m_FileContent.GetChar(i);
    if ( ++i<m_FileContent.Len() )
    {
      if (   m_FileContent.GetChar(i)==_T('\n')
          || m_FileContent.GetChar(i)==_T('\r') )
      {
        if ( m_FileContent.GetChar(i) != EOL.GetChar(0) )
          EOL << m_FileContent.GetChar(i);
      }
    }
    break;
  }

  return EOL;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxArrayString FileAnalysis::ParseForIncludes()
{
  if (m_Verbose)
    m_Log << _T("- Searching in \"") << m_FileName << _T("\" for included headers.\n");

  m_IncludedHeaders.Clear();

  for (size_t LineIdx = 0; LineIdx < m_LinesOfFile.GetCount(); ++LineIdx )
  {
    const wxString Line = m_LinesOfFile.Item(LineIdx);
    const wxRegEx RegEx(reInclude);

    wxString Include;
    if (RegEx.Matches(Line))
      Include = RegEx.GetMatch(Line, 1);

    // Include is empty if the RegEx did *not* match.
    if (Include.IsEmpty()) continue; // nothing else to do...

    if (m_Verbose)
      m_Log << _T("- Include detected via RegEx: \"") << Include << _T("\".\n");
    m_IncludedHeaders.Add(Include);

    if (m_IsHeaderFile) continue; // nothing else to do...

    // if it's a source file try to obtain the included header file
    wxFileName FileToParseFile(m_FileName);
    wxFileName IncludeFile(Include); // e.g. myheader.h
    if ( !FileToParseFile.GetName().IsSameAs(IncludeFile.GetName()) ) continue;

    if (m_Verbose)
      m_Log << _T("- Recursing into \"") << IncludeFile.GetFullName() << _T("\" for more included headers.\n");

    // "Recursion" -> do another file analysis on the header file
    FileAnalysis fa(FileToParseFile.GetPath()+
                    FileToParseFile.GetPathSeparator()+
                    IncludeFile.GetFullName());
    fa.LoadFile();
    wxArrayString MoreIncludedHeaders = fa.ParseForIncludes();

    // Only add headers that are not included by the source file
    for ( size_t i = 0; i < MoreIncludedHeaders.GetCount(); ++i )
    {
      if ( m_IncludedHeaders.Index(MoreIncludedHeaders[i]) == wxNOT_FOUND )
        m_IncludedHeaders.Add(MoreIncludedHeaders[i]);
    }

    m_Log << fa.GetLog();
    m_HasHeaderFile = true;
  }

  return m_IncludedHeaders;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxArrayString FileAnalysis::ParseForFwdDecls()
{
  if (m_Verbose)
    m_Log << _T("- Searching in \"") << m_FileName << _T("\" for forward decls.\n");

  m_ForwardDecls.Clear();

  if (!m_IsHeaderFile)
    return m_ForwardDecls;

  for ( size_t i=0; i<m_LinesOfFile.GetCount(); i++ )
  {
    wxString Line = m_LinesOfFile.Item(i);
    const wxRegEx RegEx(reFwdDecl);
    wxString FdwDecl;
    if (RegEx.Matches(Line))
      FdwDecl = RegEx.GetMatch(Line, 1);

    // Include is empty if the RegEx did *not* match.
    if (!FdwDecl.IsEmpty())
    {
      if (m_Verbose)
        m_Log << _T("- Forward decl detected via RegEx: \"") << FdwDecl << _T("\".");
      m_ForwardDecls.Add(FdwDecl);
    }
  }

  return m_ForwardDecls;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void FileAnalysis::Reset()
{
  m_Editor = NULL;
  m_Log.Clear();
  m_FileName.Empty();
  m_FileContent.Empty();
  m_LinesOfFile.Clear();
  m_IncludedHeaders.Clear();
  m_ForwardDecls.Clear();
  m_CurrentLine = 0;
  m_IsHeaderFile  = false;
  m_HasHeaderFile = false;
  m_Verbose = false;
}
