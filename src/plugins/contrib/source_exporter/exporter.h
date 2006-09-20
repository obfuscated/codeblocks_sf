/***************************************************************
 * Name:      exporter.h
 * Purpose:   Code::Blocks plugin
 * Author:    Ceniza<ceniza@gda.utp.edu.co>
 * Copyright: (c) Ceniza
 * License:   GPL
 **************************************************************/

#ifndef EXPORTER_H
#define EXPORTER_H

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

#include "BaseExporter.h"

class Exporter : public cbPlugin
{
	public:
		Exporter();
		~Exporter();
    void BuildMenu(wxMenuBar *menuBar);
    void RemoveMenu(wxMenuBar *menuBar);
//		int Configure() { return 0; }
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
    void OnExportHTML(wxCommandEvent &event);
    void OnExportRTF(wxCommandEvent &event);
    void OnExportODT(wxCommandEvent &event);
    void OnExportPDF(wxCommandEvent &event);
    void ExportFile(BaseExporter *exp, const wxString &default_extension, const wxString &wildcard);
    void OnUpdateUI(wxUpdateUIEvent &event);
  private:
    void BuildModuleMenu(const ModuleType type, wxMenu *menu, const FileTreeData* data = 0) {}
    bool BuildToolBar(wxToolBar *toolBar) { return false; }
    void RemoveToolBar(wxToolBar *toolBar) {}

    DECLARE_EVENT_TABLE();
};

#endif // EXPORTER_H
