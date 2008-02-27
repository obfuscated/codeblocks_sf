/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILERERRORS_H
#define COMPILERERRORS_H

#include <wx/arrstr.h>
#include <wx/dynarray.h>
#include <wx/string.h>
#include "compiler.h" // CompilerLineType

class cbProject;

struct CompileError
{
    CompilerLineType lineType;
    cbProject* project;
	wxString filename;
	long int line;
	wxArrayString errors;
};
WX_DECLARE_OBJARRAY(CompileError, ErrorsArray);

class CompilerErrors
{
	public:
		CompilerErrors();
		virtual ~CompilerErrors();

		void AddError(CompilerLineType lt, cbProject* project, const wxString& filename, long int line, const wxString& error);

        void GotoError(int nr);
		void Next();
		void Previous();
		void Clear();
		bool HasNextError() const;
		bool HasPreviousError() const;
		int GetCount() const { return m_Errors.GetCount(); }
		wxString GetErrorString(int index);

		unsigned int GetCount(CompilerLineType lt) const;

        int GetFirstError() const;
        int GetFocusedError() const { return m_ErrorIndex; }
	private:
		void DoAddError(const CompileError& error);
		void DoGotoError(const CompileError& error);
		void DoClearErrorMarkFromAllEditors();
		int ErrorLineHasMore(const wxString& filename, long int line) const; // returns the index in the array
		ErrorsArray m_Errors;
		int m_ErrorIndex;
};

#endif // COMPILERERRORS_H

