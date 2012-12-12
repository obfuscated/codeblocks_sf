/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef COMPILEROPTIONS_H
#define COMPILEROPTIONS_H

#include <wx/dynarray.h>
#include <wx/intl.h>
#include <wx/string.h>
#include "settings.h"

struct CompOption
{
    // following comments are an example of an option
    wxString name;          // "Profile code"
    wxString option;        // "-pg"
    wxString additionalLibs;// "-lgmon"
    bool     enabled;       // true/false
    wxString category;      // "Profiling"
    wxString checkAgainst;  // "-O -O1 -O2 -O3 -Os" (will check for these options and if any of them is found, will display the following message)
    wxString checkMessage;  // "You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."
    wxString supersedes;    // "-O -O1 -O2" (will check for these options and disable any of them that are found)
    bool     exclusive;     // true/false (will ensure that only one item in this category is ever selected)
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
        void AddOption(CompOption* coption, int index = -1);
        void AddOption(const wxString& name,
                       const wxString& option,
                       const wxString& category       = _("General"),
                       const wxString& additionalLibs = wxEmptyString,
                       const wxString& checkAgainst   = wxEmptyString,
                       const wxString& checkMessage   = wxEmptyString,
                       const wxString& supersedes     = wxEmptyString,
                       bool            exclusive      = false,
                       int index = -1);
        void RemoveOption(int index);
        unsigned int GetCount() const { return m_Options.GetCount(); }
        CompOption* GetOption(int index){ return m_Options[index]; }
        CompOption* GetOptionByName(const wxString& name);
        CompOption* GetOptionByOption(const wxString& option);
        CompOption* GetOptionByAdditionalLibs(const wxString& libs);
    protected:
    private:
        OptionsArray m_Options;
};

#endif // COMPILEROPTIONS_H
