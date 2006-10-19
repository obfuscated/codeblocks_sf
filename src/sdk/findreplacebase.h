#ifndef FINDREPLACEBASE_H
#define FINDREPLACEBASE_H

#include <wx/dialog.h>
#include <wx/string.h>

class wxWindow;

class FindReplaceBase : public wxDialog
{
	public:
		FindReplaceBase(wxWindow* parent, const wxString& initial, bool hasSelection){}
		virtual ~FindReplaceBase(){}
		virtual wxString GetFindString() const = 0;
		virtual wxString GetReplaceString() const = 0;
		virtual bool IsFindInFiles() const = 0;
		virtual bool GetDeleteOldSearches() const = 0;
		virtual bool GetMatchWord() const = 0;
		virtual bool GetStartWord() const = 0;
		virtual bool GetMatchCase() const = 0;
		virtual bool GetRegEx() const = 0;
		virtual bool GetAutoWrapSearch() const = 0;
		virtual bool GetFindUsesSelectedText() const = 0;
		virtual int GetDirection() const = 0;
		virtual int GetOrigin() const = 0;
		virtual int GetScope() const = 0;
		virtual bool GetRecursive() const = 0; // for find in search path
		virtual bool GetHidden() const = 0; // for find in search path
		virtual wxString GetSearchPath() const = 0; // for find in search path
		virtual wxString GetSearchMask() const = 0; // for find in search path
};

#endif // FINDREPLACEBASE_H
