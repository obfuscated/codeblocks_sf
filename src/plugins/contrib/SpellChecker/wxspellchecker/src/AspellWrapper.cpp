#include "AspellWrapper.h"

#include "wx/filename.h"

AspellWrapper::AspellWrapper()
{
  // "CanonicalizeName" is only available in the 2.5.x wxWidgets branch
  //wxString strLibrary = ::wxGetCwd() + wxFileName::GetPathSeparator() + m_AspellLibrary.CanonicalizeName(_("aspell"));
  // The following is the 2.4.x equivalent
  #ifdef __WIN32__
    m_strLibraryPath = ::wxGetCwd() + wxFileName::GetPathSeparator() + _("aspell.dll");
  #else
    m_strLibraryPath = ::wxGetCwd() + wxFileName::GetPathSeparator() + _("libaspell.so");
  #endif
}

AspellWrapper::~AspellWrapper()
{
  if (m_AspellLibrary.IsLoaded())
  {
    m_AspellLibrary.Unload();
  }
}

bool AspellWrapper::LoadFunctions()
{
  if (m_AspellLibrary.IsLoaded())
  {
    return true;
  }
  
  if (::wxFileExists(m_strLibraryPath) == false)
  {
    wxMessageOutput* msgOut = wxMessageOutput::Get();
    if (msgOut)
      msgOut->Printf(_T("Aspell library not found.  Aspell spell checking engine unavailable\n"));
    return false;
  }
  
  if (m_AspellLibrary.Load(m_strLibraryPath))
  {
    AspellSpellerSaveAllWordLists =  (aspell_speller_save_all_word_lists_type)m_AspellLibrary.GetSymbol(_("aspell_speller_save_all_word_lists"));
    DeleteAspellDocumentChecker =  (delete_aspell_document_checker_type)m_AspellLibrary.GetSymbol(_("delete_aspell_document_checker"));
    DeleteAspellConfig =  (delete_aspell_config_type)m_AspellLibrary.GetSymbol(_("delete_aspell_config"));
    DeleteAspellSpeller =  (delete_aspell_speller_type)m_AspellLibrary.GetSymbol(_("delete_aspell_speller"));
    DeleteAspellCanHaveError =  (delete_aspell_can_have_error_type)m_AspellLibrary.GetSymbol(_("delete_aspell_can_have_error"));    ToAspellSpeller =  (to_aspell_speller_type)m_AspellLibrary.GetSymbol(_("to_aspell_speller"));
    NewAspellConfig =  (new_aspell_config_type)m_AspellLibrary.GetSymbol(_("new_aspell_config"));
    AspellConfigReplace =  (aspell_config_replace_type)m_AspellLibrary.GetSymbol(_("aspell_config_replace"));
    AspellSpellerCheck =  (aspell_speller_check_type)m_AspellLibrary.GetSymbol(_("aspell_speller_check"));
    NewAspellDocumentChecker =  (new_aspell_document_checker_type)m_AspellLibrary.GetSymbol(_("new_aspell_document_checker"));
    AspellError =  (aspell_error_type)m_AspellLibrary.GetSymbol(_("aspell_error"));
    ToAspellDocumentChecker =  (to_aspell_document_checker_type)m_AspellLibrary.GetSymbol(_("to_aspell_document_checker"));
    AspellDocumentCheckerProcess =  (aspell_document_checker_process_type)m_AspellLibrary.GetSymbol(_("aspell_document_checker_process"));
    AspellDocumentCheckerNextMisspelling =  (aspell_document_checker_next_misspelling_type)m_AspellLibrary.GetSymbol(_("aspell_document_checker_next_misspelling"));
    AspellSpellerSuggest =  (aspell_speller_suggest_type)m_AspellLibrary.GetSymbol(_("aspell_speller_suggest"));    AspellWordListElements =  (aspell_word_list_elements_type)m_AspellLibrary.GetSymbol(_("aspell_word_list_elements"));
    AspellStringEnumerationNext =  (aspell_string_enumeration_next_type)m_AspellLibrary.GetSymbol(_("aspell_string_enumeration_next"));
    DeleteAspellStringEnumeration =  (delete_aspell_string_enumeration_type)m_AspellLibrary.GetSymbol(_("delete_aspell_string_enumeration"));
    AspellSpellerAddToPersonal =  (aspell_speller_add_to_personal_type)m_AspellLibrary.GetSymbol(_("aspell_speller_add_to_personal"));
    AspellSpellerPersonalWordList =  (aspell_speller_personal_word_list_type)m_AspellLibrary.GetSymbol(_("aspell_speller_personal_word_list"));
    AspellConfigPossibleElements =  (aspell_config_possible_elements_type)m_AspellLibrary.GetSymbol(_("aspell_config_possible_elements"));
    AspellKeyInfoEnumerationNext =  (aspell_key_info_enumeration_next_type)m_AspellLibrary.GetSymbol(_("aspell_key_info_enumeration_next"));
    DeleteAspellKeyInfoEnumeration =  (delete_aspell_key_info_enumeration_type)m_AspellLibrary.GetSymbol(_("delete_aspell_key_info_enumeration"));
    NewAspellSpeller = (new_aspell_speller_type)m_AspellLibrary.GetSymbol(_("new_aspell_speller"));
    AspellErrorMessage = (aspell_error_message_type)m_AspellLibrary.GetSymbol(_("aspell_error_message"));
    AspellSpellerStoreReplacement = (aspell_speller_store_replacement_type)m_AspellLibrary.GetSymbol(_("aspell_speller_store_replacement"));
    AspellConfigRetrieve = (aspell_config_retrieve_type)m_AspellLibrary.GetSymbol(_("aspell_config_retrieve"));
    GetAspellDictInfoList = (get_aspell_dict_info_list_type)m_AspellLibrary.GetSymbol(_("get_aspell_dict_info_list"));
    AspellDictInfoListElements = (aspell_dict_info_list_elements_type)m_AspellLibrary.GetSymbol(_("aspell_dict_info_list_elements"));
    AspellDictInfoEnumerationNext = (aspell_dict_info_enumeration_next_type)m_AspellLibrary.GetSymbol(_("aspell_dict_info_enumeration_next"));
    DeleteAspellDictInfoEnumeration = (delete_aspell_dict_info_enumeration_type)m_AspellLibrary.GetSymbol(_("delete_aspell_dict_info_enumeration"));
    AspellConfigRemove = (aspell_config_remove_type)m_AspellLibrary.GetSymbol(_("aspell_config_remove"));
    

    return true;
  }
  else
  {
    return false;
  }
}

void AspellWrapper::Unload()
{
  if (m_AspellLibrary.IsLoaded())
  {
    m_AspellLibrary.Unload();
  }
}

bool AspellWrapper::IsLoaded()
{
  return m_AspellLibrary.IsLoaded();
}
