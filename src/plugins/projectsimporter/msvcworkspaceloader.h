/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef MSVCWORKSPACELOADER_H
#define MSVCWORKSPACELOADER_H

#include "ibaseworkspaceloader.h"
#include "msvcworkspacebase.h"

class MSVCWorkspaceLoader : public IBaseWorkspaceLoader, public MSVCWorkspaceBase
{
    public:
		MSVCWorkspaceLoader();
		virtual ~MSVCWorkspaceLoader();

        bool Open(const wxString& filename, wxString& Title);
        bool Save(const wxString& title, const wxString& filename);
};

#endif // MSVCWORKSPACELOADER_H
