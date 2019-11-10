#ifndef __SPELL_CHECKER_OPTIONS_DIALOG__
#define __SPELL_CHECKER_OPTIONS_DIALOG__

#include "SpellCheckEngineInterface.h"

class wxSpellCheckEngineInterface;

struct DependencyStruct
{
  wxString strDependency;
  wxString strLastValue;
};

WX_DECLARE_STRING_HASH_MAP(DependencyStruct, StringToDependencyMap);

class SpellCheckerOptionsDialog : public wxDialog
{
public:
  SpellCheckerOptionsDialog(wxWindow* pParent, const wxString& strCaption, wxSpellCheckEngineInterface* pEngineInterface);
  
  void OnOK(wxCommandEvent& event);
  void OnBrowseForDir(wxCommandEvent& event);
  void OnBrowseForFile(wxCommandEvent& event);
  OptionsMap* GetModifiedOptions() { return &m_ModifiedOptions; }

  virtual bool TransferDataFromWindow();
  virtual bool TransferDataToWindow();
  
  virtual void UpdateControlPossibleValues(wxFocusEvent& event);

  private:
  void CreateControls();
  void PopulateOptionsSizer(wxSizer* pSizer);
  
  wxSpellCheckEngineInterface* m_pEngineInterface;
  OptionsMap m_ModifiedOptions;
  
  // Keep a map of all the option dependencies and add event handlers to update the
  //  dependent option controls when they receive focus
  StringToDependencyMap m_OptionDependenciesMap;

  DECLARE_EVENT_TABLE()
};

#endif // __SPELL_CHECKER_OPTIONS_DIALOG__
