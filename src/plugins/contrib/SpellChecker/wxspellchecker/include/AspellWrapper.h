#ifndef __ASPELL_WRAPPER_H__
#define __ASPELL_WRAPPER_H__
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dynlib.h"

#include <stdio.h>
#include <stdlib.h>
#ifndef __VISUALC__
  #include <unistd.h>
#endif

#include "aspell.h"


/************************** mutable container **************************/
typedef int (*aspell_mutable_container_add_type) (struct AspellMutableContainer * ths, const char * to_add);
typedef int (*aspell_mutable_container_remove_type) (struct AspellMutableContainer * ths, const char * to_rem);
typedef void (*aspell_mutable_container_clear_type) (struct AspellMutableContainer * ths);
typedef struct AspellMutableContainer* (*aspell_mutable_container_to_mutable_container_type) (struct AspellMutableContainer * ths);

/******************************** config ********************************/
typedef int (*aspell_key_info_enumeration_at_end_type) (const struct AspellKeyInfoEnumeration * ths);
typedef const struct AspellKeyInfo* (*aspell_key_info_enumeration_next_type) (struct AspellKeyInfoEnumeration * ths);
typedef void (*delete_aspell_key_info_enumeration_type) (struct AspellKeyInfoEnumeration * ths);
typedef struct AspellKeyInfoEnumeration* (*aspell_key_info_enumeration_clone_type) (const struct AspellKeyInfoEnumeration * ths);
typedef void (*aspell_key_info_enumeration_assign_type) (struct AspellKeyInfoEnumeration * ths, const struct AspellKeyInfoEnumeration * other);
typedef struct AspellConfig* (*new_aspell_config_type) ();
typedef void (*delete_aspell_config_type) (struct AspellConfig * ths);
typedef struct AspellConfig* (*aspell_config_clone_type) (const struct AspellConfig * ths);
typedef void (*aspell_config_assign_type) (struct AspellConfig * ths, const struct AspellConfig * other);
typedef unsigned int (*aspell_config_error_number_type) (const struct AspellConfig * ths);
typedef const char* (*aspell_config_error_message_type) (const struct AspellConfig * ths);
typedef const struct AspellError* (*aspell_config_error_type) (const struct AspellConfig * ths);
typedef void (*aspell_config_set_extra_type) (struct AspellConfig * ths, const struct AspellKeyInfo * begin, const struct AspellKeyInfo * end);
typedef const struct AspellKeyInfo* (*aspell_config_keyinfo_type) (struct AspellConfig * ths, const char * key);
typedef struct AspellKeyInfoEnumeration* (*aspell_config_possible_elements_type) (struct AspellConfig * ths, int include_extra);
typedef const char* (*aspell_config_get_default_type) (struct AspellConfig * ths, const char * key);
typedef struct AspellStringPairEnumeration* (*aspell_config_elements_type) (struct AspellConfig * ths);
typedef int (*aspell_config_replace_type) (struct AspellConfig * ths, const char * key, const char * value);
typedef int (*aspell_config_remove_type) (struct AspellConfig * ths, const char * key);
typedef int (*aspell_config_have_type) (const struct AspellConfig * ths, const char * key);
typedef const char* (*aspell_config_retrieve_type) (struct AspellConfig * ths, const char * key);
typedef int (*aspell_config_retrieve_list_type) (struct AspellConfig * ths, const char * key, struct AspellMutableContainer * lst);
typedef int (*aspell_config_retrieve_bool_type) (struct AspellConfig * ths, const char * key);
typedef int (*aspell_config_retrieve_int_type) (struct AspellConfig * ths, const char * key);

/******************************** error ********************************/
typedef int (*aspell_error_is_a_type) (const struct AspellError * ths, const struct AspellErrorInfo * e);

/**************************** can have error ****************************/
typedef unsigned int (*aspell_error_number_type) (const struct AspellCanHaveError * ths);
typedef const char* (*aspell_error_message_type) (const struct AspellCanHaveError * ths);
typedef const struct AspellError* (*aspell_error_type) (const struct AspellCanHaveError * ths);
typedef void (*delete_aspell_can_have_error_type) (struct AspellCanHaveError * ths);

/******************************* speller *******************************/
typedef struct AspellCanHaveError* (*new_aspell_speller_type) (struct AspellConfig * config);
typedef struct AspellSpeller* (*to_aspell_speller_type) (struct AspellCanHaveError * obj);
typedef void (*delete_aspell_speller_type) (struct AspellSpeller * ths);
typedef unsigned int (*aspell_speller_error_number_type) (const struct AspellSpeller * ths);
typedef const char* (*aspell_speller_error_message_type) (const struct AspellSpeller * ths);
typedef const struct AspellError* (*aspell_speller_error_type) (const struct AspellSpeller * ths);
typedef struct AspellConfig* (*aspell_speller_config_type) (struct AspellSpeller * ths);
typedef int (*aspell_speller_check_type) (struct AspellSpeller * ths, const char * word, int word_size);
typedef int (*aspell_speller_add_to_personal_type) (struct AspellSpeller * ths, const char * word, int word_size);
typedef int (*aspell_speller_add_to_session_type) (struct AspellSpeller * ths, const char * word, int word_size);
typedef const struct AspellWordList* (*aspell_speller_personal_word_list_type) (struct AspellSpeller * ths);
typedef const struct AspellWordList* (*aspell_speller_session_word_list_type) (struct AspellSpeller * ths);
typedef const struct AspellWordList* (*aspell_speller_main_word_list_type) (struct AspellSpeller * ths);
typedef int (*aspell_speller_save_all_word_lists_type) (struct AspellSpeller * ths);
typedef int (*aspell_speller_clear_session_type) (struct AspellSpeller * ths);
typedef const struct AspellWordList* (*aspell_speller_suggest_type) (struct AspellSpeller * ths, const char * word, int word_size);
typedef int (*aspell_speller_store_replacement_type) (struct AspellSpeller * ths, const char * mis, int mis_size, const char * cor, int cor_size);

/******************************** filter ********************************/
typedef void (*delete_aspell_filter_type) (struct AspellFilter * ths);
typedef unsigned int (*aspell_filter_error_number_type) (const struct AspellFilter * ths);
typedef const char* (*aspell_filter_error_message_type) (const struct AspellFilter * ths);
typedef const struct AspellError* (*aspell_filter_error_type) (const struct AspellFilter * ths);
typedef struct AspellFilter* (*to_aspell_filter_type) (struct AspellCanHaveError * obj);

/*************************** document checker ***************************/
typedef void (*delete_aspell_document_checker_type) (struct AspellDocumentChecker * ths);
typedef unsigned int (*aspell_document_checker_error_number_type) (const struct AspellDocumentChecker * ths);
typedef const char* (*aspell_document_checker_error_message_type) (const struct AspellDocumentChecker * ths);
typedef const struct AspellError* (*aspell_document_checker_error_type) (const struct AspellDocumentChecker * ths);
typedef struct AspellCanHaveError* (*new_aspell_document_checker_type) (struct AspellSpeller * speller);
typedef struct AspellDocumentChecker* (*to_aspell_document_checker_type) (struct AspellCanHaveError * obj);
typedef void (*aspell_document_checker_reset_type) (struct AspellDocumentChecker * ths);
typedef void (*aspell_document_checker_process_type) (struct AspellDocumentChecker * ths, const char * str, int size);
typedef struct AspellToken (*aspell_document_checker_next_misspelling_type) (struct AspellDocumentChecker * ths);
typedef struct AspellFilter* (*aspell_document_checker_filter_type) (struct AspellDocumentChecker * ths);

/****************************** word list ******************************/
typedef int (*aspell_word_list_empty_type) (const struct AspellWordList * ths);
typedef unsigned int (*aspell_word_list_size_type) (const struct AspellWordList * ths);
typedef struct AspellStringEnumeration* (*aspell_word_list_elements_type) (const struct AspellWordList * ths);

/************************** string enumeration **************************/
typedef void (*delete_aspell_string_enumeration_type) (struct AspellStringEnumeration * ths);
typedef struct AspellStringEnumeration* (*aspell_string_enumeration_clone_type) (const struct AspellStringEnumeration * ths);
typedef void (*aspell_string_enumeration_assign_type) (struct AspellStringEnumeration * ths, const struct AspellStringEnumeration * other);
typedef int (*aspell_string_enumeration_at_end_type) (const struct AspellStringEnumeration * ths);
typedef const char* (*aspell_string_enumeration_next_type) (struct AspellStringEnumeration * ths);

/********************************* info *********************************/
typedef struct AspellModuleInfoList* (*get_aspell_module_info_list_type) (struct AspellConfig * config);
typedef int (*aspell_module_info_list_empty_type) (const struct AspellModuleInfoList * ths);
typedef unsigned int (*aspell_module_info_list_size_type) (const struct AspellModuleInfoList * ths);
typedef struct AspellModuleInfoEnumeration* (*aspell_module_info_list_elements_type) (const struct AspellModuleInfoList * ths);
typedef struct AspellDictInfoList* (*get_aspell_dict_info_list_type) (struct AspellConfig * config);
typedef int (*aspell_dict_info_list_empty_type) (const struct AspellDictInfoList * ths);
typedef unsigned int (*aspell_dict_info_list_size_type) (const struct AspellDictInfoList * ths);
typedef struct AspellDictInfoEnumeration* (*aspell_dict_info_list_elements_type) (const struct AspellDictInfoList * ths);
typedef int (*aspell_module_info_enumeration_at_end_type) (const struct AspellModuleInfoEnumeration * ths);
typedef const struct AspellModuleInfo* (*aspell_module_info_enumeration_next_type) (struct AspellModuleInfoEnumeration * ths);
typedef void (*delete_aspell_module_info_enumeration_type) (struct AspellModuleInfoEnumeration * ths);
typedef struct AspellModuleInfoEnumeration* (*aspell_module_info_enumeration_clone_type) (const struct AspellModuleInfoEnumeration * ths);
typedef void (*aspell_module_info_enumeration_assign_type) (struct AspellModuleInfoEnumeration * ths, const struct AspellModuleInfoEnumeration * other);
typedef int (*aspell_dict_info_enumeration_at_end_type) (const struct AspellDictInfoEnumeration * ths);
typedef const struct AspellDictInfo* (*aspell_dict_info_enumeration_next_type) (struct AspellDictInfoEnumeration * ths);
typedef void (*delete_aspell_dict_info_enumeration_type) (struct AspellDictInfoEnumeration * ths);
typedef struct AspellDictInfoEnumeration* (*aspell_dict_info_enumeration_clone_type) (const struct AspellDictInfoEnumeration * ths);
typedef void (*aspell_dict_info_enumeration_assign_type) (struct AspellDictInfoEnumeration * ths, const struct AspellDictInfoEnumeration * other);

/***************************** string list *****************************/
typedef struct AspellStringList* (*new_aspell_string_list_type) ();
typedef int (*aspell_string_list_empty_type) (const struct AspellStringList * ths);
typedef unsigned int (*aspell_string_list_size_type) (const struct AspellStringList * ths);
typedef struct AspellStringEnumeration* (*aspell_string_list_elements_type) (const struct AspellStringList * ths);
typedef int (*aspell_string_list_add_type) (struct AspellStringList * ths, const char * to_add);
typedef int (*aspell_string_list_remove_type) (struct AspellStringList * ths, const char * to_rem);
typedef void (*aspell_string_list_clear_type) (struct AspellStringList * ths);
typedef struct AspellMutableContainer* (*aspell_string_list_to_mutable_container_type) (struct AspellStringList * ths);
typedef void (*delete_aspell_string_list_type) (struct AspellStringList * ths);
typedef struct AspellStringList* (*aspell_string_list_clone_type) (const struct AspellStringList * ths);
typedef void (*aspell_string_list_assign_type) (struct AspellStringList * ths, const struct AspellStringList * other);

/****************************** string map ******************************/
typedef struct AspellStringMap* (*new_aspell_string_map_type) ();
typedef int (*aspell_string_map_add_type) (struct AspellStringMap * ths, const char * to_add);
typedef int (*aspell_string_map_remove_type) (struct AspellStringMap * ths, const char * to_rem);
typedef void (*aspell_string_map_clear_type) (struct AspellStringMap * ths);
typedef struct AspellMutableContainer* (*aspell_string_map_to_mutable_container_type) (struct AspellStringMap * ths);
typedef void (*delete_aspell_string_map_type) (struct AspellStringMap * ths);
typedef struct AspellStringMap* (*aspell_string_map_clone_type) (const struct AspellStringMap * ths);
typedef void (*aspell_string_map_assign_type) (struct AspellStringMap * ths, const struct AspellStringMap * other);
typedef int (*aspell_string_map_empty_type) (const struct AspellStringMap * ths);
typedef unsigned int (*aspell_string_map_size_type) (const struct AspellStringMap * ths);
typedef struct AspellStringPairEnumeration* (*aspell_string_map_elements_type) (const struct AspellStringMap * ths);
typedef int (*aspell_string_map_insert_type) (struct AspellStringMap * ths, const char * key, const char * value);
typedef int (*aspell_string_map_replace_type) (struct AspellStringMap * ths, const char * key, const char * value);
typedef const char* (*aspell_string_map_lookup_type) (const struct AspellStringMap * ths, const char * key);

/*********************** string pair enumeration ***********************/
typedef int (*aspell_string_pair_enumeration_at_end_type) (const struct AspellStringPairEnumeration * ths);
typedef struct AspellStringPair (*aspell_string_pair_enumeration_next_type) (struct AspellStringPairEnumeration * ths);
typedef void (*delete_aspell_string_pair_enumeration_type) (struct AspellStringPairEnumeration * ths);
typedef struct AspellStringPairEnumeration* (*aspell_string_pair_enumeration_clone_type) (const struct AspellStringPairEnumeration * ths);
typedef void (*aspell_string_pair_enumeration_assign_type) (struct AspellStringPairEnumeration * ths, const struct AspellStringPairEnumeration * other);


class AspellWrapper
{
public:
  AspellWrapper();
  virtual ~AspellWrapper();
  
  bool LoadFunctions();
  void Unload();
  
  aspell_speller_save_all_word_lists_type AspellSpellerSaveAllWordLists;
  delete_aspell_document_checker_type DeleteAspellDocumentChecker;
  delete_aspell_config_type DeleteAspellConfig;
  delete_aspell_speller_type DeleteAspellSpeller;
  delete_aspell_can_have_error_type DeleteAspellCanHaveError;
  to_aspell_speller_type ToAspellSpeller;
  new_aspell_config_type NewAspellConfig;
  aspell_config_replace_type AspellConfigReplace;
  aspell_speller_check_type AspellSpellerCheck;
  new_aspell_document_checker_type NewAspellDocumentChecker;
  aspell_error_type AspellError;
  to_aspell_document_checker_type ToAspellDocumentChecker;
  aspell_document_checker_process_type AspellDocumentCheckerProcess;
  aspell_document_checker_next_misspelling_type AspellDocumentCheckerNextMisspelling;
  aspell_speller_suggest_type AspellSpellerSuggest;
  aspell_word_list_elements_type AspellWordListElements;
  aspell_string_enumeration_next_type AspellStringEnumerationNext;
  delete_aspell_string_enumeration_type DeleteAspellStringEnumeration;
  aspell_speller_add_to_personal_type AspellSpellerAddToPersonal;
  aspell_speller_personal_word_list_type AspellSpellerPersonalWordList;
  aspell_config_possible_elements_type AspellConfigPossibleElements;
  aspell_key_info_enumeration_next_type AspellKeyInfoEnumerationNext;
  delete_aspell_key_info_enumeration_type DeleteAspellKeyInfoEnumeration;
  new_aspell_speller_type NewAspellSpeller;
  aspell_error_message_type AspellErrorMessage;
  aspell_speller_store_replacement_type AspellSpellerStoreReplacement;
  aspell_config_retrieve_type AspellConfigRetrieve;
  get_aspell_dict_info_list_type GetAspellDictInfoList;
  aspell_dict_info_list_elements_type AspellDictInfoListElements;
  aspell_dict_info_enumeration_next_type AspellDictInfoEnumerationNext;
  delete_aspell_dict_info_enumeration_type DeleteAspellDictInfoEnumeration;
  aspell_config_remove_type AspellConfigRemove;
  
  void SetLibraryPath(wxString& strPath) { m_strLibraryPath = strPath; }
  wxString GetLibraryPath() { return m_strLibraryPath; }
  bool IsLoaded();
  
private:
  wxDynamicLibrary m_AspellLibrary;
  wxString m_strLibraryPath;
};

#endif // __ASPELL_WRAPPER_H__
