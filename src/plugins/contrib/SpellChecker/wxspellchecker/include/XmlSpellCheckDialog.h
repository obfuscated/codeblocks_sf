#ifndef __XML_SPELL_CHECK_DIALOG__
#define __XML_SPELL_CHECK_DIALOG__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "SpellCheckUserInterface.h"
#include "SpellCheckEngineInterface.h"

// modelless SpellChecker dialog
class XmlSpellCheckDialog : public wxDialog, public wxSpellCheckUserInterface
{
public:
    XmlSpellCheckDialog(wxWindow *parent, wxString strResourceFile, wxString strDialogResource, wxString strWordListResource, wxSpellCheckEngineInterface* SpellChecker = NULL);
		~XmlSpellCheckDialog();

		// Code handling the interface
		void OnRecheckPage(wxCommandEvent& event);
		void OnCheckWord(wxCommandEvent& event);
		void OnReplaceWord(wxCommandEvent& event);
		void OnIgnoreWord(wxCommandEvent& event);
		void OnReplaceAll(wxCommandEvent& event);
		void OnIgnoreAll(wxCommandEvent& event);
		void OnAddWordToCustomDictionary(wxCommandEvent& event);
		void OnEditCustomDictionary(wxCommandEvent& event);
		void OnChangeLanguage(wxCommandEvent& event);
		void OnChangeSuggestionSelection(wxCommandEvent& event);
		void OnDblClkSuggestionSelection(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);

    virtual void SetMisspelledWord(const wxString& strMisspelling);

    virtual int PresentSpellCheckUserInterface(const wxString& strMisspelling);
 		void CreateDialog(wxWindow* pParent);
    void OnOptions(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()

    wxString m_strResourceFile;
    wxString m_strDialogResource;
    wxString m_strWordListResource;
};

#endif  // __XML_SPELL_CHECK_DIALOG__

