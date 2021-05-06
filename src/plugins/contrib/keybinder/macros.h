//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : macros.h
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//
#ifndef MACROS_H
#define MACROS_H

//-#include "cl_standard_paths.h"
#include <wx/stdpaths.h>
#include <wx/intl.h>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>

// Useful macros
#define CHECK_PTR_RET(p) \
    if(!p) return
#define CHECK_PTR_RET_FALSE(p) \
    if(!p) return false
#define CHECK_PTR_RET_NULL(p) \
    if(!p) return NULL
#define CHECK_PTR_RET_EMPTY_STRING(p) \
    if(!p) return wxEmptyString

// ----------------------------- usefule typedefs -------------------------------
typedef std::unordered_map<wxString, bool> wxStringBoolMap_t;
typedef std::unordered_map<wxString, wxString> wxStringTable_t;
typedef std::unordered_set<wxString> wxStringSet_t;
typedef wxStringTable_t wxStringMap_t; // aliases

#endif // MACROS_H
