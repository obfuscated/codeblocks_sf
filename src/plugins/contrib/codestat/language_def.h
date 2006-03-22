/***************************************************************
 * Name:      language_def.h
 * Purpose:   Code::Blocks CodeStat plugin: Language definition class
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/

#ifndef LANGUAGE_DEF_H
#define LANGUAGE_DEF_H

#include <wx/string.h>

#define NB_FILETYPES_MAX  50  /**< Max number of languages supported. */

/** Contains the languages characteristics.
 *  @see CodeStat, CodeStatConfigDlg, CodeStatExecDlg
 */
class LanguageDef
{
 	public:
 	   wxString name;                       /**< Name of the language. */
 	   wxArrayString ext;                   /**< File's extensions associated with the language. */
 	   wxString single_line_comment;        /**< Single line comments for the language. */
 	   wxString multiple_line_comment[2];   /**< Multiple line comments for the language. */
 };

#endif
