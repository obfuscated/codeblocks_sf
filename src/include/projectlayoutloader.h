/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PROJECTLAYOUTLOADER_H
#define PROJECTLAYOUTLOADER_H

#include <wx/string.h>

class cbProject;

class DLLIMPORT ProjectLayoutLoader
{
    public:
        ProjectLayoutLoader(cbProject* project);
        virtual ~ProjectLayoutLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& filename);

        ProjectFile* GetTopProjectFile() { return m_TopProjectFile; }
    protected:
    private:
        cbProject* m_pProject;
        ProjectFile* m_TopProjectFile;
};

#endif // PROJECTLAYOUTLOADER_H

