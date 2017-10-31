#ifndef SMARTINDENTCPP_H_INCLUDED
#define SMARTINDENTCPP_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <cbplugin.h> // for "class cbSmartIndentPlugin"

class SmartIndentCpp : public cbSmartIndentPlugin
{
    public:
        virtual void OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const;

    private:
        void DoSmartIndent(cbEditor *ed, const wxChar &ch)const;
        bool BraceIndent(cbStyledTextCtrl *control, wxString &indent)const;
        void DoSelectionBraceCompletion(cbStyledTextCtrl *control, const wxChar &ch)const;
        void DoBraceCompletion(cbStyledTextCtrl *control, const wxChar& ch)const;
};

#endif // SMARTINDENTCPP_H_INCLUDED

