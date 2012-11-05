#ifndef XMLSMARTINDENT_H_INCLUDED
#define XMLSMARTINDENT_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/string.h>
#endif

#include <cbplugin.h> // for "class cbSmartIndentPlugin"

class cbEditor;
class wxScintillaEvent;

class XMLSmartIndent : public cbSmartIndentPlugin
{
    public:
        virtual void OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const;
};

#endif // XMLSMARTINDENT_H_INCLUDED
