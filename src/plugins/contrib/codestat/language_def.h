/***************************************************************
 * Name:      language_def.h
 * Purpose:   Code::Blocks Profiler plugin: main window
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/

#ifndef LANGUAGE_DEF_H
#define LANGUAGE_DEF_H

#include <wx/string.h>

#define NB_FILETYPES  7  // Number of languages supported

class LanguageDef
{
 	public:
 	   wxString name;
 	   wxArrayString ext;
 	   wxString single_line_comment;
 	   wxString multiple_line_comment[2];
 };

#endif
