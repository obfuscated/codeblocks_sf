#include "SpellCheckDialogInterface.h"

wxSpellCheckDialogInterface::wxSpellCheckDialogInterface(wxWindow *parent, wxSpellCheckEngineInterface* SpellChecker /*= NULL*/)
  : wxDialog(parent, -1, wxString(_T("Spell-Checker")),wxDefaultPosition, wxSize(330,245), wxDEFAULT_DIALOG_STYLE),
  wxSpellCheckUserInterface(SpellChecker)
{
}

wxSpellCheckDialogInterface::~wxSpellCheckDialogInterface()
{
}

int wxSpellCheckDialogInterface::PresentSpellCheckUserInterface(const wxString& strMisspelling)
{
  // If the dialog is being presented, then default the previous action flag first
  SetMisspelledWord(strMisspelling);
		
	m_nLastAction = ACTION_INITIAL;

  ShowModal();

	return m_nLastAction;
}
