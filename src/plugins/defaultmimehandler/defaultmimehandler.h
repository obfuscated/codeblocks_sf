/***************************************************************
 * Name:      defaultmimehandler.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis An. Mandravellos<mandrav@codeblocks.org>
 * Created:   03/13/05 14:08:51
 * Copyright: (c) Yiannis An. Mandravellos
 * License:   GPL
 **************************************************************/

#ifndef DEFAULTMIMEHANDLER_H
#define DEFAULTMIMEHANDLER_H

#include "cbplugin.h" // the base class we 're inheriting
#include "mimetypesarray.h"

class DefaultMimeHandler : public cbMimePlugin
{
    public:
        DefaultMimeHandler();
        ~DefaultMimeHandler();
        int Configure();
        int GetConfigurationGroup() const { return cgCorePlugin; }
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
        bool HandlesEverything() const { return true; }
        bool CanHandleFile(const wxString& filename) const;
        int OpenFile(const wxString& filename);
        void OnAttach(); // fires when the plugin is attached to the application
        void OnRelease(bool appShutDown); // fires when the plugin is released from the application
    private:
        cbMimeType* FindMimeTypeFor(const wxString& filename);
        wxString ChooseExternalProgram();
        int DoOpenFile(cbMimeType* mt, const wxString& filename);
        MimeTypesArray m_MimeTypes;
};

#endif // DEFAULTMIMEHANDLER_H

