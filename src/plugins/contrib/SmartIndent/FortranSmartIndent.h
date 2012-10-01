#ifndef FORTRANSMARTINDENT_H_INCLUDED
#define FORTRANSMARTINDENT_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/string.h>
#endif

#include <cbplugin.h> // for "class cbSmartIndentPlugin"

class cbEditor;
class wxScintillaEvent;

class FortranSmartIndent : public cbSmartIndentPlugin
{
    public:
        virtual void OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const;
};

#endif // FORTRANSMARTINDENT_H_INCLUDED
