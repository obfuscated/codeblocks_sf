#ifndef COMPILEROPTIONS_H
#define COMPILEROPTIONS_H

#include "settings.h"
#include <wx/intl.h>

struct CompOption
{
	// following comments are an example of an option
	wxString name;			// "Profile code"
	wxString option;		// "-pg"
	wxString additionalLibs;// "-lgmon"
	bool enabled;			// true/false
	wxString category;		// "Profiling"
	bool doChecks;			// true/false
	wxString checkAgainst;	// "-O -O1 -O2 -O3 -Os" (will check for these options and if any of them is found, will display the following message)
	wxString checkMessage;	// "You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."
};

WX_DEFINE_ARRAY(CompOption*, OptionsArray);

class DLLIMPORT CompilerOptions
{
	public:
		CompilerOptions();
		CompilerOptions(const CompilerOptions& other);
		CompilerOptions& operator=(const CompilerOptions& other);
		~CompilerOptions();
		void ClearOptions();
		void AddOption(CompOption* coption);
		void AddOption(const wxString& name,
						const wxString& option,
						const wxString& category = _("General"),
						const wxString& additionalLibs = wxEmptyString,
						bool doChecks = false,
						const wxString& checkAgainst = wxEmptyString,
						const wxString& checkMessage = wxEmptyString);
		unsigned int GetCount(){ return m_Options.GetCount(); }
		CompOption* GetOption(int index){ return m_Options[index]; }
		CompOption* GetOptionByName(const wxString& name);
		CompOption* GetOptionByOption(const wxString& option);
	protected:
	private:
		OptionsArray m_Options;
};

#endif // COMPILEROPTIONS_H
