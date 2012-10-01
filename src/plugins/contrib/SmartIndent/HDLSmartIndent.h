#ifndef HDLSMARTINDENT_H_INCLUDED
#define HDLSMARTINDENT_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/string.h>
#endif

#include <cbplugin.h> // for "class cbSmartIndentPlugin"

class cbEditor;
class wxScintillaEvent;

class HDLSmartIndent : public cbSmartIndentPlugin
{
    public:
        virtual void OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const;

    private:
        int FindBlockStartVHDL(cbEditor* ed, int position, wxString block) const;

        void DoIndent  (cbEditor* ed, const wxString& langname) const;
        void DoUnIndent(cbEditor* ed, const wxString& langname) const;
};

#endif // HDLSMARTINDENT_H_INCLUDED
