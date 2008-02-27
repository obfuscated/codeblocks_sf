/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef WORKSPACELOADER_H
#define WORKSPACELOADER_H

#include "ibaseworkspaceloader.h"

class WorkspaceLoader : public IBaseWorkspaceLoader
{
	public:
		WorkspaceLoader();
		virtual ~WorkspaceLoader();

        bool Open(const wxString& filename, wxString& Title);
        bool Save(const wxString& title, const wxString& filename);
};

#endif // WORKSPACELOADER_H
