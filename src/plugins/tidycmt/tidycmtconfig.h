/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TIDYCMTCONFIG_H
#define TIDYCMTCONFIG_H

#include <wx/string.h>

struct TidyCmtConfig
{
    TidyCmtConfig() :
      enabled(false),
      length(80),
      single_line_cmt(_T("//--")),
      multi_line_cmt(_T("/*--")) { ; }
    bool         enabled;
    unsigned int length;
    wxString     single_line_cmt;
    wxString     multi_line_cmt;
};

#endif // TIDYCMTCONFIG_H
