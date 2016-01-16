#ifndef __MYSPELL_CHECK_INTERFACE__
#define __MYSPELL_CHECK_INTERFACE__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/string.h"

// spell checker/thingie
#include "wx/process.h"
#include "wx/txtstrm.h"
#include "wx/file.h"

#include "SpellCheckUserInterface.h"
#include "PersonalDictionary.h"

#include "MySpell/myspell.hxx"

// Get rid of the warning about identifiers being truncated in the debugger.  Using the STL collections
// will produce this everywhere.  Must disable at beginning of stdafx.h because it doesn't work if
// placed elsewhere.
#ifdef __VISUALC__
  #pragma warning(disable:4786)
#endif

class MySpellInterface : public wxSpellCheckEngineInterface
{
public:
  MySpellInterface(wxSpellCheckUserInterface* pDlg = NULL);
  ~MySpellInterface();

	// Spell Checker functions
  virtual wxString GetSpellCheckEngineName() { return _T("MySpell"); }
	virtual int InitializeSpellCheckEngine();
	virtual int UninitializeSpellCheckEngine();
  virtual int SetOption(SpellCheckEngineOption& Option);
  virtual void UpdatePossibleValues(SpellCheckEngineOption& OptionDependency, SpellCheckEngineOption& OptionToUpdate);
	virtual wxString CheckSpelling(wxString strText);
  wxArrayString GetSuggestions(const wxString& strMisspelledWord);

	virtual bool IsWordInDictionary(const wxString& strWord);
  virtual int AddWordToDictionary(const wxString& strWord);
  virtual int RemoveWordFromDictionary(const wxString& strWord);
  virtual wxArrayString GetWordListAsArray();
  void OpenPersonalDictionary(const wxString& strPersonalDictionaryFile);
  PersonalDictionary* GetPersonalDictionary() { return &m_PersonalDictionary; }
  void AddCustomMySpellDictionary(const wxString& strDictionaryName, const wxString& strDictionaryFileRoot);
  void CleanCustomMySpellDictionaries() { m_CustomMySpellDictionaryMap.clear(); }

  virtual wxString GetCharacterEncoding();

private:  
  void PopulateDictionaryMap(StringToStringMap* pLookupMap, const wxString& strDictionaryPath);
  void AddDictionaryElement(StringToStringMap* pLookupMap, const wxString& strDictionaryPath, const wxString& strDictionaryName, const wxString& strDictionaryFileRoot);
  wxString GetSelectedLanguage();
  wxString GetAffixFileName();
  wxString GetAffixFileName(const wxString& strDictionaryName);
  wxString GetDictionaryFileName();
  wxString GetDictionaryFileName(const wxString& strDictionaryName);

	MySpell* m_pMySpell;
  
  StringToStringMap m_DictionaryLookupMap;
  StringToStringMap m_CustomMySpellDictionaryMap;
  wxString m_strDictionaryPath;
  
  PersonalDictionary m_PersonalDictionary;
};

#endif  // __MYSPELL_CHECK_INTERFACE__
