#ifndef FINDREPLACEBASE_H
#define FINDREPLACEBASE_H

#include <wx/dialog.h>
#include "settings.h"

class FindReplaceBase : public wxDialog
{
	public:
		FindReplaceBase(wxWindow* parent, const wxString& initial, bool hasSelection){}
		virtual ~FindReplaceBase(){}
		virtual wxString GetFindString() = 0;
		virtual wxString GetReplaceString() = 0;
		virtual bool IsFindInFiles() = 0;
		virtual bool GetMatchWord() = 0;
		virtual bool GetStartWord() = 0;
		virtual bool GetMatchCase() = 0;
		virtual bool GetRegEx() = 0;
		virtual int GetDirection() = 0;
		virtual int GetOrigin() = 0;
		virtual int GetScope() = 0;
		virtual bool GetRecursive() = 0; // for find in search path
		virtual bool GetHidden() = 0; // for find in search path
		virtual wxString GetSearchPath() = 0; // for find in search path
		virtual wxString GetSearchMask() = 0; // for find in search path
};

#endif // FINDREPLACEBASE_H
