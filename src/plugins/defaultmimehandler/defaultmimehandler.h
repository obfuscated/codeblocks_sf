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

#ifdef __WXMSW__
    #if defined(__GNUG__) && !defined(__APPLE__)
        #pragma interface "defaultmimehandler.h"
    #endif
#endif
// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK
#include "mimetypesarray.h"

class DefaultMimeHandler : public cbMimePlugin
{
	public:
		DefaultMimeHandler();
		~DefaultMimeHandler();
		int Configure();
        bool HandlesEverything(){ return true; }
		bool CanHandleFile(const wxString& filename);
		int OpenFile(const wxString& filename);
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
	protected:
        cbMimeType* FindMimeTypeFor(const wxString& filename);
        wxString ChooseExternalProgram();
		int DoOpenFile(cbMimeType* mt, const wxString& filename);
        MimeTypesArray m_MimeTypes;
	private:
};

#ifdef __cplusplus
extern "C" {
#endif
	PLUGIN_EXPORT cbPlugin* GetPlugin();
#ifdef __cplusplus
};
#endif

#endif // DEFAULTMIMEHANDLER_H

