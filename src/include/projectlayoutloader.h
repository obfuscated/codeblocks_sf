/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PROJECTLAYOUTLOADER_H
#define PROJECTLAYOUTLOADER_H

#ifndef CB_PRECOMP
    #include <wx/string.h>
    #include "cbauibook.h"
#endif

class cbProject;

class DLLIMPORT ProjectLayoutLoader
{
    public:
        ProjectLayoutLoader(cbProject* project);
        virtual ~ProjectLayoutLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& filename);

        bool LoadNotebookLayout() {
            return Manager::Get()->GetEditorManager()->GetNotebook()->LoadPerspective( m_NotebookLayout,
                                                                                      !Manager::Get()->GetProjectManager()->IsLoadingWorkspace());

        };

        ProjectFile* GetTopProjectFile() { return m_TopProjectFile; }
    protected:
    private:
        cbProject* m_pProject;
        ProjectFile* m_TopProjectFile;
        wxString m_NotebookLayout;
};

#endif // PROJECTLAYOUTLOADER_H

