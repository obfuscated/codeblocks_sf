/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef _PARSE_WATCH_VALUE_H_
#define _PARSE_WATCH_VALUE_H_
#endif // _PARSE_WATCH_VALUE_H_

#include "debugger_defs.h"

bool ParseGDBWatchValue(cb::shared_ptr<GDBWatch> watch, wxString const &value);

bool ParseCDBWatchValue(cb::shared_ptr<GDBWatch> watch, wxString const &value);
