#ifndef COMPILERERRORS_H
#define COMPILERERRORS_H

#include <settings.h>

struct CompileError
{
    bool isWarning;
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
		
		void AddError(const wxString& filename, long int line, const wxString& error, bool isWarning);
		
        void GotoError(int nr);
		void Next();
		void Previous();
		void Clear();
		bool HasNextError();
		bool HasPreviousError();
		int GetCount(){ return m_Errors.GetCount(); }
		wxString GetErrorString(int index);
		
		unsigned int GetErrorsCount();
		unsigned int GetWarningsCount();
        
        int GetFocusedError(){ return m_ErrorIndex; }
	protected:
		void DoAddError(const CompileError& error);
		void DoGotoError(const CompileError& error);
		void DoClearErrorMarkFromAllEditors();
		int ErrorLineHasMore(const wxString& filename, long int line); // returns the index in the array
		ErrorsArray m_Errors;
		int m_ErrorIndex;
	private:
};

#endif // COMPILERERRORS_H

