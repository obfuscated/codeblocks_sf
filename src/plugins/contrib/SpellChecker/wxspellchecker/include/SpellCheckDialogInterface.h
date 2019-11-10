#ifndef __SPELL_CHECK_DIALOG_INTERFACE__
#define __SPELL_CHECK_DIALOG_INTERFACE__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dialog.h"
#include "wx/string.h"

#include "SpellCheckUserInterface.h"

class wxSpellCheckDialogInterface : public wxDialog, public wxSpellCheckUserInterface
{
public:
  wxSpellCheckDialogInterface(wxWindow *parent, wxSpellCheckEngineInterface* SpellChecker = NULL);
	virtual ~wxSpellCheckDialogInterface();

public:
	virtual int PresentSpellCheckUserInterface(const wxString& strMisspelling);
};

#endif  // __SPELL_CHECK_DIALOG_INTERFACE__
