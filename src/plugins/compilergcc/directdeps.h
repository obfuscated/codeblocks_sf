/*
* This file is part of Code::Blocks, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*/

#ifndef DIRECTDEPS_H
#define DIRECTDEPS_H

#include <wx/string.h>
#include <wx/dynarray.h>

class cbProject;
class ProjectBuildTarget;

namespace DirectDeps
{
    bool ReadDependencies(const wxString& filename, wxArrayString& deps);
    bool GetDependenciesOf(const wxString& filename, wxArrayString& deps,
                            int pageIndex,
                            cbProject* project,
                            ProjectBuildTarget* target);
};

#endif // DIRECTDEPS_H
