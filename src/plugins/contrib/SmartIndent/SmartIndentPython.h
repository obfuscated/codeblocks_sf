#ifndef SMARTINDENTPYTHON_H_INCLUDED
#define SMARTINDENTPYTHON_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/string.h>
#endif

#include <cbplugin.h> // for "class cbSmartIndentPlugin"

class cbEditor;
class wxScintillaEvent;

class SmartIndentPython : public cbSmartIndentPlugin
{
    public:
        virtual void OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const;
};

#endif // SMARTINDENTPYTHON_H_INCLUDED
