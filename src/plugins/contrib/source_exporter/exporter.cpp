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

static wxMenu* FindOrInsertExportMenu(wxMenuBar *menuBar)
{
    // find "File" menu position
    const int fileMenuPos = menuBar->FindMenu(_("&File"));
    if (fileMenuPos == -1)
        return nullptr;
    // find actual "File" menu
    wxMenu *fileMenu = menuBar->GetMenu(fileMenuPos);
    if (!fileMenu)
        return nullptr;

    // decide where to insert in "File" menu
    size_t printPos = fileMenu->GetMenuItemCount() - 4; // the default location
    const int printID = fileMenu->FindItem(_("Print..."));
    if (printID != wxNOT_FOUND)
    {
        fileMenu->FindChildItem(printID, &printPos);
        ++printPos; // after "Print"
    }
    wxMenu *exportMenu = nullptr;
    const int pos = fileMenu->FindItem(_("&Export"));
    if (pos != wxNOT_FOUND)
    {
        wxMenuItem *menuItem = fileMenu->FindItem(pos);
        exportMenu = menuItem->GetSubMenu();
        if (exportMenu)
            exportMenu->AppendSeparator();
    }
    else
    {
        exportMenu = new wxMenu();
        fileMenu->Insert(printPos, wxID_ANY, _("&Export"), exportMenu);
    }
    return exportMenu;
}

void Exporter::BuildMenu(wxMenuBar *menuBar)
{
    wxMenu *exportMenu = FindOrInsertExportMenu(menuBar);
    if (!exportMenu)
        return;

    if (!exportMenu->FindItem(idFileExportHTML))
        exportMenu->Append(idFileExportHTML, _("As &HTML..."), _("Exports the current file to HTML"));
    if (!exportMenu->FindItem(idFileExportRTF))
        exportMenu->Append(idFileExportRTF, _("As &RTF..."), _("Exports the current file to RTF"));
    if (!exportMenu->FindItem(idFileExportODT))
        exportMenu->Append(idFileExportODT, _("As &ODT..."), _("Exports the current file to ODT"));
    if (!exportMenu->FindItem(idFileExportPDF))
        exportMenu->Append(idFileExportPDF, _("As &PDF..."), _("Exports the current file to PDF"));
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

  EditorManager* em = Manager::Get()->GetEditorManager();
  cbEditor*      cb = em->GetBuiltinActiveEditor();

  wxString filename = wxFileSelector(_("Choose the filename"), _T(""), wxFileName(cb->GetFilename()).GetName() + _T(".") + default_extension, default_extension, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  if (filename.IsEmpty())
  {
    return;
  }

  cbStyledTextCtrl* stc = cb->GetControl();
  if (!stc)
      return;

  int lineCount = -1;
  if (wxMessageBox(_("Would you like to have the line numbers printed in the exported file?"), _("Export line numbers"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxYES)
  {
    lineCount = stc->GetLineCount();
  }

  exp->Export(filename, cb->GetFilename(), stc->GetStyledText(0, stc->GetLength() - 1), cb->GetColourSet(), lineCount, stc->GetTabWidth());
}
