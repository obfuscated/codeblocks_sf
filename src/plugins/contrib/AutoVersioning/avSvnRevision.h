/*
* autorevision - a tool to incorporate Subversion revisions into binary builds
* Copyright (C) 2005 Thomas Denk
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
*Modified by JGM for adaptation to the AutoVersioning Plugin
*/

#ifndef AVSVNREVISION_H
#define AVSVNREVISION_H

#include <wx/string.h>

bool QuerySvn(const wxString& workingDir, wxString& revision, wxString& date);
#endif //AVSVNREVISION_H
