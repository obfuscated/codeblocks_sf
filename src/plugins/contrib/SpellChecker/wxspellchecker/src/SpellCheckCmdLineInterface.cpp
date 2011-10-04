#include "SpellCheckCmdLineInterface.h"

SpellCheckCmdLineInterface::SpellCheckCmdLineInterface(wxSpellCheckEngineInterface* SpellChecker /*= NULL*/)
  : wxSpellCheckUserInterface(SpellChecker)
{
}

SpellCheckCmdLineInterface::~SpellCheckCmdLineInterface()
{
}

int SpellCheckCmdLineInterface::PresentSpellCheckUserInterface(const wxString& strMisspelling)
{
  SetMisspelledWord(strMisspelling);

	m_nLastAction = ACTION_INITIAL;

  PrintMisspelling();
  PrintSuggestions();
  GetFeedback();

	return m_nLastAction;
}

void SpellCheckCmdLineInterface::PrintMisspelling()
{
  if (m_pSpellCheckEngine)
  {
    wxSpellCheckEngineInterface::MisspellingContext Context = m_pSpellCheckEngine->GetCurrentMisspellingContext();
    wxString strContext = Context.GetContext();
    // Append the closing marker first since the opening marker would shift where the closing marker would have to go
    strContext.insert(Context.GetOffset() + Context.GetLength(), _T("<-**"));
    strContext.insert(Context.GetOffset(), _T("**->"));
    wxCharBuffer contextCharBuffer(wxConvUTF8.cWC2MB(strContext.wc_str(*wxConvCurrent)));
    wxPrintf(_T("%s\n"), (const char*)contextCharBuffer);
  }
}

void SpellCheckCmdLineInterface::PrintSuggestions()
{
  wxPrintf(_("Suggestions: \n"));

  if (m_pSpellCheckEngine)
  {
    wxArrayString SuggestionArray = m_pSpellCheckEngine->GetSuggestions(m_strMisspelledWord);
    if (SuggestionArray.GetCount() > 0)
    {
      // Add each suggestion to the list
			// The (nCtr < 5) clause is to make the list of suggestions manageable to the user
      for (unsigned int nCtr = 0; (nCtr < SuggestionArray.GetCount()) && (nCtr < 5); nCtr++)
      {
        wxCharBuffer suggestionCharBuffer(wxConvUTF8.cWC2MB(SuggestionArray[nCtr].wc_str(*wxConvCurrent)));
        wxPrintf(_T(" '%s'"), (const char*)suggestionCharBuffer);
      }
    }
    else
    {
      wxPrintf(_T(" (no suggestions)\n"));
    }
  }
}

void SpellCheckCmdLineInterface::GetFeedback()
{
  wxChar strReplacement[256];
  wxPrintf(_T("\nReplacement? : \n"));
  if ( !wxFgets(strReplacement, WXSIZEOF(strReplacement), stdin) )
    m_nLastAction = ACTION_IGNORE; /* ignore the current misspelling */
  else
	{
  	strReplacement[wxStrlen(strReplacement) - 1] = '\0';
		if (wxStrlen(strReplacement) == 0)
		{
    	m_nLastAction = ACTION_IGNORE; /* ignore the current misspelling */
		}
		else
		{
	    m_nLastAction = ACTION_REPLACE;
			m_strReplaceWithText = strReplacement;
		}
	}
}

