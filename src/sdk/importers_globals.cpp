/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"
#include "importers_globals.h"

// default: ask for compiler to use
bool ImportersGlobals::UseDefaultCompiler = false;

// default: ask which targets to import
bool ImportersGlobals::ImportAllTargets = false;

// Do not forget to update this function, when new flags are added!
void ImportersGlobals::ResetDefaults()
{
    UseDefaultCompiler = false;
    ImportAllTargets = false;
}
