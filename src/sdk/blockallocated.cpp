/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/log.h> // for wxSafeShowMessage()
    #include <wx/regex.h>
#endif
#include "blockallocated.h"

namespace BlkAllc
{

void DebugLog(wxString cn, int blockSize, int poolSize, int max_refs, int total_refs, int ref_count)
        {
            wxString s;
            wxString cn2;

            if(total_refs == 0)
                return; // pointless

            wxRegEx r(_T("^[A-Z]?[0-9]+(.*)"));
            if(r.Matches(cn))
                cn2 = r.GetMatch(cn, 1);

            s.Printf(_T("%s\n\n%d reserved pools of size %d (%d total objects)\n"
            "Maximum number of allocated objects: %d\n"
            "Total number of allocations: %d\n"
            "Number of stale objects: %d %s"),
            cn2.c_str(),
            blockSize, poolSize, blockSize * poolSize,
            max_refs, total_refs, ref_count, (ref_count == 0 ? _T("") : _T("(memory leak)")));

            wxSafeShowMessage(_T("Block Allocator"), s);
		} // end of DebugLog
};

