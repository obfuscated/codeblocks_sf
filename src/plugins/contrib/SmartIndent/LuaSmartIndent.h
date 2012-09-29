#ifndef LUASMARTINDENT_H_INCLUDED
#define LUASMARTINDENT_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/string.h>
#endif

#include <cbplugin.h> // for "class cbSmartIndentPlugin"

class cbEditor;
class wxScintillaEvent;

class LuaSmartIndent : public cbSmartIndentPlugin
{
    public:
        virtual void OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const;

    private:
        virtual bool InComment(const wxString& LanguageName, int style) const;
        bool BraceIndent(cbStyledTextCtrl *stc, wxString &indent)const;
};

#endif // LUASMARTINDENT_H_INCLUDED

