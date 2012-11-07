/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PROJECTSIMPORTER_H
#define PROJECTSIMPORTER_H

#include "cbplugin.h" // the base class we 're inheriting

class ProjectsImporter : public cbMimePlugin
{
    public:
        ProjectsImporter();
        ~ProjectsImporter();
        bool HandlesEverything() const { return false; }
        bool CanHandleFile(const wxString& filename) const;
        int OpenFile(const wxString& filename);
        void BuildMenu(wxMenuBar* menuBar);
    private:
        int LoadProject(const wxString& filename);
        int LoadWorkspace(const wxString& filename);

        wxMenu* m_Menu;
};

#endif // PROJECTSIMPORTER_H

