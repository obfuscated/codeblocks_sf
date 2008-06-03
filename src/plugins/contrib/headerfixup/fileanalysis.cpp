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
#include <wx/tokenzr.h>

#include "cbstyledtextctrl.h"
#include "fileanalysis.h"

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
  m_Verbose = verbose;

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

    File.Write(m_FileContent,wxConvUTF8);
  }
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxString FileAnalysis::GetEOL()
{
  wxString EOL = _T('\n');

  // Detect end-of-line style to prevent inconsistent EOLs
  for ( int i=0; i<m_FileContent.Length(); i++ )
  {
    if (   m_FileContent.GetChar(i)==_T('\n')
        || m_FileContent.GetChar(i)==_T('\r') )
    {
      EOL = m_FileContent.GetChar(i);
      if ( ++i<m_FileContent.Length() )
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
  }

  return EOL;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxArrayString FileAnalysis::ParseForIncludes()
{
  if (m_Verbose)
    Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: Searching in \"")+m_FileName+_T("\" for included headers...")));

  m_IncludedHeaders.Clear();

  for ( size_t i=0; i<m_LinesOfFile.GetCount(); i++ )
  {
    wxString Line = m_LinesOfFile.Item(i); Line.Trim(false);
    if ( !Line.IsEmpty() && Line.GetChar(0)==_T('#') )
    {
      // line with hash in front (e.g. #include, #define, #ifdef...)
      Line.Remove(0,1).Trim(false);
      if ( Line.StartsWith(_T("include")) )
      {
        // line with #include in front -> adding included file into list
        Line.Remove(0,7).Trim(false);
        if ( Line.StartsWith(_T("\"")) || Line.StartsWith(_T("<")) )
        {
          wxChar End = (Line.GetChar(0)==_T('"')) ? _T('"') : _T('>');
          Line.Remove(0,1);
          if ( Line.Find(End) != wxNOT_FOUND )
          {
            Line.Remove(Line.Find(End));
            if ( m_IncludedHeaders.Index(Line) == wxNOT_FOUND )
            {
              m_IncludedHeaders.Add(Line);

              // if it's a source file try to obtain the included header file
              if ( !m_IsHeaderFile )
              {
                wxFileName FileToParseFile(m_FileName);
                wxFileName IncludeFile(Line); // e.g. myheader.h
                if ( FileToParseFile.GetName().IsSameAs(IncludeFile.GetName()) )
                {
                  if (m_Verbose)
                    Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: Recursing into \"")+IncludeFile.GetFullName()+_T("\" for more included headers...")));

                  // "Recursion" -> do another file analysis on the header file
                  FileAnalysis fa(FileToParseFile.GetPath()+
                                  FileToParseFile.GetPathSeparator()+
                                  IncludeFile.GetFullName());
                  fa.LoadFile();
                  wxArrayString MoreIncludedHeaders = fa.ParseForIncludes();

                  // Only add headers that are not included by the source file
                  for ( size_t i=0; i<MoreIncludedHeaders.GetCount(); i++ )
                    if ( m_IncludedHeaders.Index(MoreIncludedHeaders[i]) == wxNOT_FOUND )
                      m_IncludedHeaders.Add(MoreIncludedHeaders[i]);

                  m_HasHeaderFile = true;
                }
              }
            }
          }
        }
      }// definitely line with #include
      // else: line with e.g. #define, #ifdef or similar (skipped)
    }// possibly line with #include
  }

  return m_IncludedHeaders;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void FileAnalysis::Reset()
{
  m_Editor = NULL;
  m_FileName.Empty();
  m_FileContent.Empty();
  m_LinesOfFile.Clear();
  m_IncludedHeaders.Clear();
  m_CurrentLine = 0;
  m_IsHeaderFile  = false;
  m_HasHeaderFile = false;
  m_Verbose = false;
}
