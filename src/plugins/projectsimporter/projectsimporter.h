/***************************************************************
 * Name:      projectsimporter.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis An. Mandravellos<mandrav@codeblocks.org>
 * Created:   03/13/05 14:08:51
 * Copyright: (c) Yiannis An. Mandravellos
 * License:   GPL
 **************************************************************/

#ifndef PROJECTSIMPORTER_H
#define PROJECTSIMPORTER_H

#include "cbplugin.h" // the base class we 're inheriting

class ProjectsImporter : public cbMimePlugin
{
    public:
        ProjectsImporter();
        ~ProjectsImporter();
        int Configure();
        int GetConfigurationGroup() const { return cgCorePlugin; }
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
        bool HandlesEverything() const { return false; }
        bool CanHandleFile(const wxString& filename) const;
        int OpenFile(const wxString& filename);
        void OnAttach(); // fires when the plugin is attached to the application
        void OnRelease(bool appShutDown); // fires when the plugin is released from the application
        void BuildMenu(wxMenuBar* menuBar);
    private:
        int LoadProject(const wxString& filename);
        int LoadWorkspace(const wxString& filename);

        wxMenu* m_Menu;
};

#endif // PROJECTSIMPORTER_H

