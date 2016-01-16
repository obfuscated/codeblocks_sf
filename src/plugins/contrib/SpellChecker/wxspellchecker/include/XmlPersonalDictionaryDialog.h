#ifndef __XML_PERSONAL_DICTIONARY_DIALOG__
#define __XML_PERSONAL_DICTIONARY_DIALOG__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "SpellCheckEngineInterface.h"

class XmlPersonalDictionaryDialog : public wxDialog
{
public:
  XmlPersonalDictionaryDialog(wxWindow* parent, wxString strResourceFile, wxString strResource, wxSpellCheckEngineInterface* pEngine);
  ~XmlPersonalDictionaryDialog();

  void CreateDialog(wxWindow* pParent);
  void PopulatePersonalWordListBox();
  void AddWordToPersonalDictionary(wxCommandEvent& event);
  void ReplaceInPersonalDictionary(wxCommandEvent& event);
  void RemoveFromPersonalDictionary(wxCommandEvent& event);
  void OnClose(wxCommandEvent& event);

protected:
  wxSpellCheckEngineInterface* m_pSpellCheckEngine;
  wxString m_strResourceFile;
  wxString m_strDialogResource;

private:
    DECLARE_EVENT_TABLE()
};

#endif  // __XML_PERSONAL_DICTIONARY_DIALOG__

