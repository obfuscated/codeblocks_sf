#ifndef __SPELL_CHECK_CMDLINE_INTERFACE__
#define __SPELL_CHECK_CMDLINE_INTERFACE__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/string.h"

#include "SpellCheckUserInterface.h"

class SpellCheckCmdLineInterface : public wxSpellCheckUserInterface
{
public:
  SpellCheckCmdLineInterface(wxSpellCheckEngineInterface* SpellChecker = NULL);
	virtual ~SpellCheckCmdLineInterface();

public:
	virtual int PresentSpellCheckUserInterface(const wxString& strMisspelling);
  void PrintMisspelling();
  void PrintSuggestions();
  void GetFeedback();
};

#endif  // __SPELL_CHECK_CMDLINE_INTERFACE__
