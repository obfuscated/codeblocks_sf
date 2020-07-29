/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DEFAULTMIMEHANDLER_H
#define DEFAULTMIMEHANDLER_H

#include "cbplugin.h" // the base class we 're inheriting
#include "mimetypesarray.h"

class EmbeddedHtmlPanel;

class DefaultMimeHandler : public cbMimePlugin
{
    public:
        DefaultMimeHandler();
        ~DefaultMimeHandler() override;
        int GetConfigurationGroup() const override { return cgCorePlugin; }
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent) override;
        bool HandlesEverything() const override { return true; }
        bool CanHandleFile(const wxString& filename) const override;
        int OpenFile(const wxString& filename) override;
        void OnAttach() override; // fires when the plugin is attached to the application
        void OnRelease(bool appShutDown) override; // fires when the plugin is released from the application
    private:
        cbMimeType* FindMimeTypeFor(const wxString& filename);
        wxString ChooseExternalProgram();
        int DoOpenFile(cbMimeType* mt, const wxString& filename);
        MimeTypesArray m_MimeTypes;

        EmbeddedHtmlPanel* m_Html;
};

#endif // DEFAULTMIMEHANDLER_H

