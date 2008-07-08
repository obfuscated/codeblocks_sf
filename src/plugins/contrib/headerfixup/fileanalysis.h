/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef FILEANALYSIS_H
#define FILEANALYSIS_H

#include <wx/string.h>
#include <wx/arrstr.h>

class cbEditor;

class FileAnalysis
{
public:
           /// Default constructor (no initialisation -> call ReInit after this)
           FileAnalysis();
           /// Initialiasing constructor (calls ReInit)
           FileAnalysis(const wxString& FileName);
  virtual ~FileAnalysis();

  /// Has usually to be called first -> initialises the class and reads the file
  void ReInit(const wxString& FileName, bool verbose = false);

  /// Returns whether the file is a header file (upon file extension)
  bool IsHeaderFile()
  { return m_IsHeaderFile; }

  /// Load the file from cbEditor or disk and splits it up into lines
  void LoadFile();

  /// Saves the file to cbEditor or disk and prepends a string (the includes)
  void SaveFile(const wxString& Prepend);

  /// Returns whether there is another line to parse for iterators
  bool HasMoreLines()
  { return (m_CurrentLine<m_LinesOfFile.GetCount()); }

  /// "Rewinds" the file and re-starts from the first line for iterators
  void Rewind()
  { m_CurrentLine = 0; }

  /// Returns the nest line in a file for iterators
  wxString GetNextLine();

  /// Detects and returns the end-of-line type of the file
  wxString GetEOL();

  /// Parses the file for includes and takes included header file into account
  wxArrayString ParseForIncludes();

  /// Returns the include files (parsed using ParseForIncludes before)
  wxArrayString GetIncludes()
  { return m_IncludedHeaders; }

  /// Parses the file for forward declarations (only if it's a header file)
  wxArrayString ParseForFwdDecls();

  /// Returns the forward declararions (parsed using ParseForForwardDecls before)
  wxArrayString GetFwdDecls()
  { return m_ForwardDecls; }

  /// Returns the protocol (log) of the last operation
  wxString GetLog()
  { return m_Log; }

protected:
private:
  /// Internal function to reset all states (for re-initialisation).
  void          Reset();

  cbEditor*     m_Editor;          //!< The editor that is being use to parse for contents
  wxString      m_Log;             //!< The protocol (log) of the current file analysis
  wxString      m_FileName;        //!< The filename of the currently operated file
  wxString      m_FileContent;     //!< The content of the currently operated file
  wxArrayString m_LinesOfFile;     //!< The content (line-by-line) of the currently operated file
  wxArrayString m_IncludedHeaders; //!< A list of included header files of the currently operated file
  wxArrayString m_ForwardDecls;    //!< A list of forward declarations of the currently operated file
  size_t        m_CurrentLine;     //!< Current line in the file (used for iterators)
  bool          m_Verbose;         //!< Be verbose in debug messages (be silent otherwise)
  bool          m_IsHeaderFile;    //!< Flag whether the currently operated file is a header file (upon file extension)
  bool          m_HasHeaderFile;   //!< Flag whether the currently operated has a header file
};

#endif // FILEANALYSIS_H
