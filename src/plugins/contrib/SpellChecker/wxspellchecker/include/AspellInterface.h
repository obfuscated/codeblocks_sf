#ifndef __ASPELL_CHECK_INTERFACE__
#define __ASPELL_CHECK_INTERFACE__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/string.h"

#include "SpellCheckEngineInterface.h"

#include "AspellWrapper.h"
#include "PersonalDictionary.h"

class AspellInterface : public wxSpellCheckEngineInterface
{
public:
  AspellInterface(wxSpellCheckUserInterface* pDlg = NULL);
  ~AspellInterface();

	// Spell Checker functions
  virtual wxString GetSpellCheckEngineName() { return _T("Aspell"); }
	virtual int InitializeSpellCheckEngine();
	virtual int UninitializeSpellCheckEngine();
  virtual int SetOption(SpellCheckEngineOption& Option);
  virtual void UpdatePossibleValues(SpellCheckEngineOption& OptionDependency, SpellCheckEngineOption& OptionToUpdate);
	virtual bool IsWordInDictionary(const wxString& strWord);
	virtual wxString CheckSpelling(wxString strText);
  virtual wxArrayString GetSuggestions(const wxString& strMisspelledWord);
  virtual int AddWordToDictionary(const wxString& strWord);
  virtual int RemoveWordFromDictionary(const wxString& strWord);
  virtual wxArrayString GetWordListAsArray();
  void OpenPersonalDictionary(const wxString& strPersonalDictionaryFile);
  PersonalDictionary* GetPersonalDictionary() { return &m_PersonalDictionary; }

  bool LoadLibrary();
  bool UnloadLibrary();
  virtual void ApplyOptions();  // Go through all the options in the options map and apply them to the spell check engine

  virtual wxString GetCharacterEncoding();

private:  
  // Aspell data structures
  AspellWrapper m_AspellWrapper;
	AspellConfig* m_AspellConfig;
	AspellSpeller* m_AspellSpeller;
  AspellDocumentChecker* m_AspellChecker;
  
  // Aspell has personal dictionary support, but it lacks the ability to remove words from the
  //  personal dictionary.  So for now, we'll just use the generic PersonalDictionary
  PersonalDictionary m_PersonalDictionary;

	// Use new_aspell_config and new_aspell_speller for initialization
	// Use aspell_config_replace to set options (language, data location, case-sensitive, ...)
	// Use aspell_speller_check to run the actual spell check (return value of 0 means to match found)
	// Use aspell_document_checker_process to spell check more than one word at a time
	// Use aspell_speller_suggest and aspell_word_list_elements to get a list of suggestions
	//	Use delete_aspell_string_manage to clean up the memory allocated by aspell_word_list_elements
	// Optional: Use aspell_speller_store_repl so that aspell can learn from the correct word
	// Use aspell_speller_add_to_personal to add words to the personal dictionary
	// Use delete_aspell_speller to clean up during uninitialization
};

#endif  // __ASPELL_CHECK_INTERFACE__

