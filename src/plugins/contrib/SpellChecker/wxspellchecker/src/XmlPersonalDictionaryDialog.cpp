#include "XmlPersonalDictionaryDialog.h"

#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(XmlPersonalDictionaryDialog, wxDialog)
  EVT_BUTTON(XRCID("ButtonAddToDict"), XmlPersonalDictionaryDialog::AddWordToPersonalDictionary)
  EVT_BUTTON(XRCID("ButtonReplaceInDict"), XmlPersonalDictionaryDialog::ReplaceInPersonalDictionary)
  EVT_BUTTON(XRCID("ButtonRemoveFromDict"), XmlPersonalDictionaryDialog::RemoveFromPersonalDictionary)
  EVT_BUTTON(XRCID("ButtonClose"), XmlPersonalDictionaryDialog::OnClose)
END_EVENT_TABLE()

XmlPersonalDictionaryDialog::XmlPersonalDictionaryDialog(wxWindow* parent, wxString strResourceFile, wxString strResource, wxSpellCheckEngineInterface* pEngine)
{
  m_pSpellCheckEngine = pEngine;
  m_strResourceFile = strResourceFile;
  m_strDialogResource = strResource;

  CreateDialog(parent);
}

XmlPersonalDictionaryDialog::~XmlPersonalDictionaryDialog()
{
}

void XmlPersonalDictionaryDialog::CreateDialog(wxWindow* pParent)
{
  // Load the XML resource
  wxXmlResource::Get()->InitAllHandlers();
  if (wxXmlResource::Get()->Load(m_strResourceFile) == false)
    return;
  
  if (wxXmlResource::Get()->LoadDialog(this, pParent, m_strDialogResource) == false)
    return;
  
  // Verify that the controls we need are present

  // Now present the word list to the user

  PopulatePersonalWordListBox();
}

void XmlPersonalDictionaryDialog::PopulatePersonalWordListBox()
{
  if (m_pSpellCheckEngine != NULL)
  {
    wxListBox* pListBox = XRCCTRL(*this, "ListPersonalWords", wxListBox);

    if (pListBox)
    {
      // Get a list of suggestions to populate the list box
      wxArrayString PersonalWords = m_pSpellCheckEngine->GetWordListAsArray();

      pListBox->Clear();

      // Add each suggestion to the list
      for (unsigned int nCtr = 0; nCtr < PersonalWords.GetCount(); nCtr++)
        pListBox->Append(PersonalWords[nCtr]);

      // If the previous word had no suggestions than the box may be disabled, enable it here to be safe
      pListBox->Enable(TRUE);
    }
  }
}

void XmlPersonalDictionaryDialog::AddWordToPersonalDictionary(wxCommandEvent& event)
{
  if (m_pSpellCheckEngine != NULL)
  {
    TransferDataFromWindow();

    wxTextCtrl* pText = XRCCTRL(*this, "TextNewPersonalWord", wxTextCtrl);

    if (pText != NULL)
    {
      wxString strNewWord = pText->GetValue();

      if (!strNewWord.Trim().IsEmpty())
        m_pSpellCheckEngine->AddWordToDictionary(strNewWord);
      
      // Clear the text control
      pText->Clear();
    }

    PopulatePersonalWordListBox();
  }
}

void XmlPersonalDictionaryDialog::ReplaceInPersonalDictionary(wxCommandEvent& event)
{
  if (m_pSpellCheckEngine != NULL)
  {
    TransferDataFromWindow();
    
    wxString strOldWord = _T("");
    wxString strNewWord = _T("");

    // Find the old word
    wxListBox* pListBox = XRCCTRL(*this, "ListPersonalWords", wxListBox);
    if (pListBox)
      strOldWord = pListBox->GetStringSelection();
    
    // Find the new word
    wxTextCtrl* pText = XRCCTRL(*this, "TextNewPersonalWord", wxTextCtrl);
    if (pText)
    {
      strNewWord = pText->GetValue();
      
      // Clear the text control
      pText->Clear();
    }

    if (!strOldWord.IsEmpty() && !strNewWord.IsEmpty())
    {
      m_pSpellCheckEngine->RemoveWordFromDictionary(strOldWord);
      m_pSpellCheckEngine->AddWordToDictionary(strNewWord);
      PopulatePersonalWordListBox();
    }
  }
}

void XmlPersonalDictionaryDialog::RemoveFromPersonalDictionary(wxCommandEvent& event)
{
  if (m_pSpellCheckEngine != NULL)
  {
    TransferDataFromWindow();
    wxListBox* pListBox = XRCCTRL(*this, "ListPersonalWords", wxListBox);
    if (pListBox)
    {
      wxString strWord = pListBox->GetStringSelection();
      if (!strWord.Trim().IsEmpty())
      {
        if (!(m_pSpellCheckEngine->RemoveWordFromDictionary(strWord)))
        {
          wxMessageOutput* msgOut = wxMessageOutput::Get();
          if (msgOut)
            msgOut->Printf(_T("There was an error removing \"") + strWord + _T("\" to the personal dictionary"));
        }
      }
    }
    PopulatePersonalWordListBox();
  }
}



void XmlPersonalDictionaryDialog::OnClose(wxCommandEvent& event)
{
  EndModal(true);
}
