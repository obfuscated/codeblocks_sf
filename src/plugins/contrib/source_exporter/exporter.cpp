/***************************************************************
 * Name:      exporter.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Ceniza<ceniza@gda.utp.edu.co>
 * Copyright: (c) Ceniza
 * License:   GPL
 **************************************************************/

#include <wx/frame.h> // GetMenuBar
#include "exporter.h"
#include <configmanager.h>
#include <manager.h>
#include <cbeditor.h>
#include <editormanager.h>
#include <editorcolourset.h>
#include <logmanager.h>
#include <cbexception.h>
#include "HTMLExporter.h"
#include "RTFExporter.h"
#include "ODTExporter.h"
#include "PDFExporter.h"
#include "cbstyledtextctrl.h"

static int idFileExport = wxNewId();
static int idFileExportHTML = wxNewId();
static int idFileExportRTF = wxNewId();
static int idFileExportODT = wxNewId();
static int idFileExportPDF = wxNewId();

// Register the plugin
namespace
{
    PluginRegistrant<Exporter> reg(_T("Exporter"));
};

BEGIN_EVENT_TABLE(Exporter, cbPlugin)
  EVT_MENU(idFileExportHTML, Exporter::OnExportHTML)
  EVT_MENU(idFileExportRTF, Exporter::OnExportRTF)
  EVT_MENU(idFileExportODT, Exporter::OnExportODT)
  EVT_MENU(idFileExportPDF, Exporter::OnExportPDF)
  EVT_UPDATE_UI(idFileExportHTML, Exporter::OnUpdateUI)
  EVT_UPDATE_UI(idFileExportRTF, Exporter::OnUpdateUI)
  EVT_UPDATE_UI(idFileExportODT, Exporter::OnUpdateUI)
END_EVENT_TABLE()

Exporter::Exporter()
{
  //ctor
}

Exporter::~Exporter()
{
  //dtor
}

void Exporter::OnAttach()
{
  // do whatever initialization you need for your plugin
  // NOTE: after this function, the inherited member variable
  // IsAttached() will be TRUE...
  // You should check for it in other functions, because if it
  // is FALSE, it means that the application did *not* "load"
  // (see: does not need) this plugin...
}

void Exporter::OnRelease(bool /*appShutDown*/)
{
  // do de-initialization for your plugin
  // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
  // which means you must not use any of the SDK Managers
  // NOTE: after this function, the inherited member variable
  // IsAttached() will be FALSE...
}

void Exporter::BuildMenu(wxMenuBar *menuBar)
{
  // find "File" menu position
  int fileMenuPos = menuBar->FindMenu(_("&File"));

  if (fileMenuPos == -1)
  {
    //cbThrow(_T("Can't find \"File\" menu position?!?"));
    return;
  }

  // find actual "File" menu
  wxMenu *file = menuBar->GetMenu(fileMenuPos);

  if (!file)
  {
    //cbThrow(_T("Can't find \"File\" menu?!?"));
    return;
  }

  // decide where to insert in "File" menu
  size_t printPos = file->GetMenuItemCount() - 4; // the default location
  int printID = file->FindItem(_("Print..."));

  if (printID != wxNOT_FOUND)
  {
    file->FindChildItem(printID, &printPos);
    ++printPos; // after "Print"
  }

  // insert menu items
  wxMenu *export_submenu = new wxMenu;
  export_submenu->Append(idFileExportHTML, _("As &HTML..."), _("Exports the current file to HTML"));
  export_submenu->Append(idFileExportRTF, _("As &RTF..."), _("Exports the current file to RTF"));
  export_submenu->Append(idFileExportODT, _("As &ODT..."), _("Exports the current file to ODT"));
  export_submenu->Append(idFileExportPDF, _("As &PDF..."), _("Exports the current file to PDF"));

  wxMenuItem *export_menu = new wxMenuItem(0, idFileExport, _("&Export"), _T(""), wxITEM_NORMAL);
  export_menu->SetSubMenu(export_submenu);

  file->Insert(printPos, export_menu);
}

void Exporter::RemoveMenu(wxMenuBar *menuBar)
{
  wxMenu *menu = 0;
  wxMenuItem *item = menuBar->FindItem(idFileExport, &menu);

  if (menu && item)
  {
    menu->Remove(item);
  }
}

void Exporter::OnUpdateUI(wxUpdateUIEvent &event)
{
  if (Manager::IsAppShuttingDown())
  {
    event.Skip();
    return;
  }

  wxMenuBar *mbar = Manager::Get()->GetAppFrame()->GetMenuBar();

  if (mbar)
  {
    EditorManager *em = Manager::Get()->GetEditorManager();

    // Enabled if there's a source file opened (be sure it isn't the "Start here" page)
    bool disable = !em || !em->GetActiveEditor() || !em->GetBuiltinActiveEditor();
    mbar->Enable(idFileExportHTML, !disable);
    mbar->Enable(idFileExportRTF, !disable);
    mbar->Enable(idFileExportODT, !disable);
    mbar->Enable(idFileExportPDF, !disable);
  }

  event.Skip();
}

void Exporter::OnExportHTML(wxCommandEvent & /*event*/)
{
  HTMLExporter exp;
  ExportFile(&exp, _T("html"), _("HTML files|*.html;*.htm"));
}

void Exporter::OnExportRTF(wxCommandEvent & /*event*/)
{
  RTFExporter exp;
  ExportFile(&exp, _T("rtf"), _("RTF files|*.rtf"));
}


void Exporter::OnExportODT(wxCommandEvent & /*event*/)
{
  ODTExporter exp;
  ExportFile(&exp, _T("odt"), _("ODT files|*.odt"));
}

void Exporter::OnExportPDF(wxCommandEvent & /*event*/)
{
  PDFExporter exp;
  ExportFile(&exp, _T("pdf"), _("PDF files|*.pdf"));
}

void Exporter::ExportFile(BaseExporter *exp, const wxString &default_extension, const wxString &wildcard)
{
  if (!IsAttached())
  {
    return;
  }

  EditorManager *em = Manager::Get()->GetEditorManager();
  cbEditor *cb = em->GetBuiltinActiveEditor();
  wxString filename = wxFileSelector(_("Choose the filename"), _T(""), wxFileName(cb->GetFilename()).GetName() + _T(".") + default_extension, default_extension, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

  if (filename.IsEmpty())
  {
    return;
  }

  cbStyledTextCtrl* ed = cb->GetControl();
  wxMemoryBuffer mb = ed->GetStyledText(0, ed->GetLength() - 1);

  int lineCount = -1;

  if (wxMessageBox(_("Would you like to have the line numbers printed in the exported file?"), _("Export line numbers"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxYES)
  {
    lineCount = cb->GetControl()->GetLineCount();
  }

  int tabWidth = cb->GetControl()->GetTabWidth();

  EditorColourSet *ecs = cb->GetColourSet();

  exp->Export(filename, cb->GetFilename(), mb, ecs, lineCount, tabWidth);
}
