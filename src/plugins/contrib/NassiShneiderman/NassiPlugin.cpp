#include <wx/sstream.h>
#include <wx/txtstrm.h>


#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <cbeditor.h>
#endif
#include <configurationpanel.h>
#include <cbstyledtextctrl.h>
#include <filefilters.h>
#include "NassiPlugin.h"

#include "NassiEditorPanel.h"
#include "NassiView.h"

#include "rc/selecttool16.xpm"
#include "rc/selecttool22.xpm"
#include "rc/instrtool16.xpm"
#include "rc/instrtool22.xpm"
#include "rc/iftool16.xpm"
#include "rc/iftool22.xpm"
#include "rc/switchtool16.xpm"
#include "rc/switchtool22.xpm"
#include "rc/whiletool16.xpm"
#include "rc/whiletool22.xpm"
#include "rc/dowhiletool16.xpm"
#include "rc/dowhiletool22.xpm"
#include "rc/breaktool16.xpm"
#include "rc/breaktool22.xpm"
#include "rc/continuetool16.xpm"
#include "rc/continuetool22.xpm"
#include "rc/returntool16.xpm"
#include "rc/returntool22.xpm"
#include "rc/fortool16.xpm"
#include "rc/fortool22.xpm"
#include "rc/blocktool16.xpm"
#include "rc/blocktool22.xpm"

#include "rc/glassptool16.xpm"
#include "rc/glassptool22.xpm"
#include "rc/glassntool16.xpm"
#include "rc/glassntool22.xpm"


#include "rc/commentToggletool16.xpm"
#include "rc/commentToggletool22.xpm"
#include "rc/sourceToggletool16.xpm"
#include "rc/sourceToggletool22.xpm"


// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    const int MaxInsertMenuEntries = 10;

    PluginRegistrant<NassiPlugin> reg(_T("NassiShneidermanPlugin"));
    const int NASSI_ID_NEW_FILE = wxNewId();
    const int idParseC = wxNewId();
    const int insertCFromDiagram[MaxInsertMenuEntries] = {
        static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()),
        static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId())};
}

namespace {
    const int NASSI_ID_TOGGLE_SOURCE = wxNewId();
    const int NASSI_ID_TOGGLE_COMMENT = wxNewId();

    const int NASSI_ID_ESC = wxNewId();
    const int NASSI_ID_INSTRUCTION = wxNewId();
    const int NASSI_ID_IF = wxNewId();
    const int NASSI_ID_SWITCH = wxNewId();
    const int NASSI_ID_WHILE = wxNewId();
    const int NASSI_ID_DOWHILE = wxNewId();
    const int NASSI_ID_FOR = wxNewId();
    const int NASSI_ID_BLOCK = wxNewId();
    const int NASSI_ID_BREAK = wxNewId();
    const int NASSI_ID_CONTINUE = wxNewId();
    const int NASSI_ID_RETURN = wxNewId();

    const int NASSI_ID_EXPORT_SVG = wxNewId();
    const int NASSI_ID_EXPORT_SOURCE = wxNewId();
    const int NASSI_ID_EXPORT_VHDL = wxNewId();
    const int NASSI_ID_EXPORT_PS = wxNewId();
    const int NASSI_ID_EXPORT_STRUKTEX = wxNewId();
    const int NASSI_ID_EXPORT_BITMAP = wxNewId();

    const int NASSI_ID_GLASS_P = wxNewId();
    const int NASSI_ID_GLASS_N = wxNewId();
}
// TODO (danselmi#1#): Check if export of svg generates not conformant SVG data???

// events handling
BEGIN_EVENT_TABLE(NassiPlugin, cbPlugin)

    EVT_MENU(NASSI_ID_NEW_FILE, NassiPlugin::OnNewDiagram)

    //EVT_MENU(idParseC, NassiPlugin::ParseC)

    EVT_UPDATE_UI_RANGE(NASSI_ID_TOGGLE_SOURCE, NASSI_ID_TOGGLE_COMMENT, NassiPlugin::OnUpdateToggleText)
    EVT_MENU_RANGE(NASSI_ID_TOGGLE_SOURCE, NASSI_ID_TOGGLE_COMMENT , NassiPlugin::OnToggleText)

    EVT_UPDATE_UI_RANGE(NASSI_ID_ESC, NASSI_ID_RETURN, NassiPlugin::OnUpdateTools)
    EVT_MENU_RANGE(NASSI_ID_INSTRUCTION, NASSI_ID_RETURN, NassiPlugin::OnChangeTool)
    EVT_MENU(NASSI_ID_ESC, NassiPlugin::OnToolSelect)

    EVT_UPDATE_UI_RANGE(NASSI_ID_GLASS_P,NASSI_ID_GLASS_N, NassiPlugin::OnUpdateZoom)
    EVT_MENU_RANGE(NASSI_ID_GLASS_P,NASSI_ID_GLASS_N, NassiPlugin::OnZoom)

    //EVT_MENU(wxID_SELECTALL, NassiPlugin::OnSelectAll)
    //EVT_MENU(wxID_DELETE, NassiPlugin::OnDelete)

    /*EVT_MENU(NASSI_ID_NEXT_TOOL, NassiEditView::OnNextTool)
    EVT_MENU(NASSI_ID_PREV_TOOL, NassiEditView::OnPrevTool)*/

    EVT_UPDATE_UI_RANGE(NASSI_ID_EXPORT_SVG, NASSI_ID_EXPORT_BITMAP, NassiPlugin::OnUpdateExport)
    EVT_MENU_RANGE(NASSI_ID_EXPORT_SVG, NASSI_ID_EXPORT_BITMAP,      NassiPlugin::OnExport)

    EVT_UPDATE_UI(idParseC, NassiPlugin::OnUpdateUIMenuItem)
END_EVENT_TABLE()

// constructor
NassiPlugin::NassiPlugin():
    m_pTbar(0)
{
    /*// Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if(!Manager::LoadResource(_T("NassiPlugin.zip")))
    {
        NotifyMissingFile(_T("NassiPlugin.zip"));
    }*/
}

// destructor
NassiPlugin::~NassiPlugin()
{
}

void NassiPlugin::OnAttach()
{
    // do whatever initialization you need for your plugin
    // after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...


    for ( int i = 0 ; i < MaxInsertMenuEntries ; i++ )
        Connect(insertCFromDiagram[i], wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NassiPlugin::OnInsertCFromDiagram), 0, this);
    Connect(idParseC, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NassiPlugin::ParseC), 0, this);

    FileFilters::Add(_("Nassi Shneiderman diagram"), _T("*.nsd") );
}

void NassiPlugin::OnRelease(bool appShutDown)
{
    if ( !appShutDown )
    {
        NassiEditorPanel::CloseAllNassiEditors();
        for ( int i = 0 ; i < MaxInsertMenuEntries ; i++ )
            Disconnect(insertCFromDiagram[i], wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NassiPlugin::OnInsertCFromDiagram), 0, this);
        Disconnect(idParseC, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NassiPlugin::ParseC), 0, this);
    }
}

void NassiPlugin::BuildMenu(wxMenuBar* menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //Be careful in here... The application's menubar is at your disposal.

    int pos = menuBar->FindMenu( _("&File") );
    if ( pos == wxNOT_FOUND ) return;

    wxMenu *filemenu = menuBar->GetMenu(pos);

    wxMenu *exportmenu = 0;
    pos = filemenu->FindItem( _("&Export") );
    if ( pos != wxNOT_FOUND )
    {
        wxMenuItem *menuitm = filemenu->FindItem(pos);
        exportmenu = menuitm->GetSubMenu();
        if (exportmenu) exportmenu->AppendSeparator();
    }
    else
    {
        exportmenu = new wxMenu();
        filemenu->AppendSubMenu(exportmenu, _("&Export"));
    }
    if ( !exportmenu->FindItem(NASSI_ID_EXPORT_SOURCE) )
        exportmenu->Append(NASSI_ID_EXPORT_SOURCE,  _("&Export Source..."), _("Export to C source format"));
    //if ( !exportmenu->FindItem(NASSI_ID_EXPORT_VHDL) )
    //    exportmenu->Append(NASSI_ID_EXPORT_VHDL, _T("VHDL"), _T("export to VHDL format"));
    if ( !exportmenu->FindItem(NASSI_ID_EXPORT_STRUKTEX) )
        exportmenu->Append(NASSI_ID_EXPORT_STRUKTEX, _T("StrukTeX"), _("export to StrukTeX format"));

// TODO (danselmi#1#): check why svg export crashes on wx30; fix and enable feature again
        //    #if wxCHECK_VERSION(3, 0, 0)
//    if ( !exportmenu->FindItem(NASSI_ID_EXPORT_SVG) )
//        exportmenu->Append(NASSI_ID_EXPORT_SVG, _T("SVG"), _("export to SVG format"));
//    #endif

    if ( !exportmenu->FindItem(NASSI_ID_EXPORT_BITMAP) )
        exportmenu->Append(NASSI_ID_EXPORT_BITMAP, _T("PNG"), _("export to PNG format"));

    #if wxUSE_POSTSCRIPT
    if ( !exportmenu->FindItem(NASSI_ID_EXPORT_PS) )
        exportmenu->Append(NASSI_ID_EXPORT_PS, _T("PS"), _("export to PS"));
    #endif


    wxMenu* filenewmenu = 0;
    pos = filemenu->FindItem( _("New") );
    if ( pos != wxNOT_FOUND )
    {
        wxMenuItem *menuitm = filemenu->FindItem(pos);
        filenewmenu = menuitm->GetSubMenu();
    }
    else
    {
        filenewmenu = new wxMenu();
        wxMenuItem *item = new wxMenuItem(filemenu, wxID_ANY, _("New"), _T(""), wxITEM_NORMAL, filenewmenu);
        filemenu->Prepend(item);
    }


    if ( !filenewmenu->FindItem(NASSI_ID_NEW_FILE) )
        filenewmenu->Append(NASSI_ID_NEW_FILE, _("Nassi Shneiderman diagram"), _("Create a new Nassi Shneiderman diagram"));

    pos = menuBar->FindMenu(_("&View"));
    if (pos == wxNOT_FOUND)
        return;

    wxMenu* viewmenu = menuBar->GetMenu(pos);
    viewmenu->Append(idParseC, _("Nassi-Shneiderman diagram"), _("Construct Nassi-Shneiderman diagram from selected text"));
    viewmenu->Enable(idParseC, false);
}

void NassiPlugin::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* /*data*/)
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...
    if ( !IsAttached() || !menu ) return;

    if ( type != mtEditorManager ) return;

    EditorManager* emngr = Manager::Get()->GetEditorManager();
    if ( !emngr ) return;

    EditorBase *edb = emngr->GetActiveEditor();
    if ( !edb || !edb->IsBuiltinEditor() ) return;

    cbStyledTextCtrl* stc = ((cbEditor*)edb)->GetControl();
    if ( !stc ) return;

    wxMenu *NassiMenu = 0;

    // check if user can select to generate a diagram from selection
    if ( stc->GetLexer() == wxSCI_LEX_CPP && stc->GetSelectionEnd() - stc->GetSelectionStart() > 0 )
    {
        if (! NassiMenu ) NassiMenu = new wxMenu();
        NassiMenu->Append(idParseC, _("Create diagram"));
    }

    // check if user can insert an opened diagram
    wxArrayString names;
    for ( int i = 0 ; i < Manager::Get()->GetEditorManager()->GetEditorsCount() ; ++i )
    {
        EditorBase *ed = Manager::Get()->GetEditorManager()->GetEditor( i );
        if ( NassiEditorPanel::IsNassiEditor( ed ) )
            names.Add(ed->GetTitle( ) );
    }
    if ( stc->GetLexer() == wxSCI_LEX_CPP && names.GetCount() > 0 )
    {
        if (! NassiMenu )
        {
            NassiMenu = new wxMenu();
        }
        else
        {
            NassiMenu->AppendSeparator();
        }

        for ( int i = 0; i < static_cast<int>(names.GetCount()) && i < 10 ; ++i )
        {
            NassiMenu->Append(insertCFromDiagram[i], _("insert from ") + names[i] );
        }
    }


    if ( NassiMenu )
    {
        menu->AppendSeparator();
        menu->AppendSubMenu(NassiMenu, _("Nassi Shneiderman"));
    }

}

bool NassiPlugin::BuildToolBar(wxToolBar* toolBar)
{
    //The application is offering its toolbar for your plugin,
    //to add any toolbar items you want...
    //Append any items you need on the toolbar...
    m_pTbar = toolBar;
    if (!IsAttached() || !toolBar)
        return false;
    //wxString my_16x16 = Manager::isToolBar16x16(toolBar) ? _T("_16x16") : _T("");
    //Manager::AddonToolBar(toolBar,wxString(_T("nassi_plugin"))+my_16x16);
    if ( Manager::isToolBar16x16(toolBar) )
        PopulateToolbar16(toolBar);
    else
        PopulateToolbar22(toolBar);
    toolBar->Realize();
    toolBar->SetInitialSize();
    return true;
}

//{ mime handler
bool NassiPlugin::CanHandleFile(const wxString& filename) const
{
    wxFileName fname(filename);
    if (fname.GetExt().Lower() == _T("nsd") )
        return true;
    return false;
}

int NassiPlugin::OpenFile(const wxString& fileName)
{
    EditorBase *alreadopeneditor = Manager::Get()->GetEditorManager()->IsOpen( fileName );
    if ( alreadopeneditor )
    {
        Manager::Get()->GetEditorManager()->SetActiveEditor(alreadopeneditor);
        return 0;
    }

    wxString title = wxFileName( fileName ).GetFullName();
    if ( new NassiEditorPanel( fileName, title ) )
        return 0;

    return -1;
}

void NassiPlugin::OnNewDiagram(wxCommandEvent & /*event*/)
{
    new NassiEditorPanel(_T(""), _T(""));
}
//} end mime handler

void NassiPlugin::ParseC(wxCommandEvent & /*event*/)
{
    EditorManager* emngr = Manager::Get()->GetEditorManager();
    if ( !emngr ) return;

    EditorBase *edb = emngr->GetActiveEditor();
    if ( !edb || !edb->IsBuiltinEditor() ) return;

    cbStyledTextCtrl* stc = ((cbEditor*)edb)->GetControl();
    if ( !stc ) return;


    NassiEditorPanel *panel = new NassiEditorPanel(_T(""), _T(""));

    switch ( stc->GetLexer() )
    {
        case wxSCI_LEX_CPP:
        {
            const wxString str = stc->GetSelectedText();
            if ( !panel->ParseC(str) )
            {
                panel->Close();
                wxMessageBox(_("unable to parse input"), _("Error!"));
            }
            //else stc->SetReadOnly(true);

        }
            break;
        default:
            break;
    }
}

void NassiPlugin::OnInsertCFromDiagram(wxCommandEvent &event)
{
    // check if user can isert an opened diagram
    unsigned idx = 0;
    for ( int i = 0 ; i < Manager::Get()->GetEditorManager()->GetEditorsCount() ; i++ )
    {
        EditorBase *ed = Manager::Get()->GetEditorManager()->GetEditor( i );
        if ( NassiEditorPanel::IsNassiEditor( ed ) )
        {
            NassiEditorPanel *ned = (NassiEditorPanel *)ed;
            if ( event.GetId() == insertCFromDiagram[idx] )
            {
                EditorManager* emngr = Manager::Get()->GetEditorManager();
                if ( !emngr ) return;
                EditorBase *edb = emngr->GetActiveEditor();
                if ( !edb || !edb->IsBuiltinEditor() ) return;
                unsigned int indent = ((cbEditor*)edb)->GetLineIndentInSpaces(); // from current line
                cbStyledTextCtrl *stc = ((cbEditor*)edb)->GetControl();
                if ( !stc ) return;

                wxStringOutputStream ostrstream;
                wxTextOutputStream text_stream(ostrstream);

                if ( !ned ) return;
                ned->GetCSource(text_stream, indent);

                stc->InsertText(wxSCI_INVALID_POSITION, ostrstream.GetString());

            }
            //some comment
            idx++;
        }
    }
}

bool NassiPlugin::IsNassiEditorPanelActive()
{
    EditorBase *ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if ( NassiEditorPanel::IsNassiEditor(ed) )
        return true;
    return false;
}

void NassiPlugin::OnUpdateToggleText(wxUpdateUIEvent &event)
{
    if ( IsNassiEditorPanelActive() )
    {
        NassiEditorPanel *ed = (NassiEditorPanel *)Manager::Get()->GetEditorManager()->GetActiveEditor();

        event.Enable(true);
        if ( event.GetId() == NASSI_ID_TOGGLE_SOURCE )
            event.Check( ed->IsDrawingSource() );
        else
            event.Check( ed->IsDrawingComment() );
    }
    else
        event.Enable(false);
}

void NassiPlugin::OnUpdateTools(wxUpdateUIEvent &event)
{
    event.Enable( IsNassiEditorPanelActive() );
}

void NassiPlugin::PopulateToolbar16(wxToolBar* toolBar)
{
    toolBar->AddTool(NASSI_ID_ESC, _("Select"),  wxBitmap(selecttool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Select"), _("Change the tool for selections"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_INSTRUCTION, _("Instruction"), wxBitmap(instrtool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Instruction"), _("Insert an instruction"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_IF, _("Decision"), wxBitmap(iftool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Decision"), _("Insert a decision"));
    toolBar->AddTool(NASSI_ID_SWITCH, _("Selection"), wxBitmap(switchtool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Selection"), _("Insert a selection"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_WHILE, _("Entry-Condition Loop"), wxBitmap(whiletool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Entry-Condition Loop"), _("Insert an entry-condition loop"));
    toolBar->AddTool(NASSI_ID_DOWHILE, _("Exit-Condition Loop"), wxBitmap(dowhiletool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Exit-Condition Loop"), _("Insert an exit-condition loop"));
    //toolBar->AddTool(NASSI_ID_WHILE1, _("Endless Loop"),  wxBitmap(while1tool_xpm), wxNullBitmap, wxITEM_NORMAL, _("Endless Loop"), _("Insert an endless loop"));
    toolBar->AddTool(NASSI_ID_FOR, _("Counting Loop"), wxBitmap(fortool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Counting Loop"), _("Insert a counting loop"));
    toolBar->AddTool(NASSI_ID_BLOCK, _("Block Instruction"), wxBitmap(blocktool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Block Instruction"), _("Insert a block Instruction {}"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_BREAK, _("Break-Instruction"), wxBitmap(breaktool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Break-Instruction"), _("Insert a break-instruction"));
    toolBar->AddTool(NASSI_ID_CONTINUE, _("Continue-Instruction"), wxBitmap(continuetool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Continue-Instruction"), _("Insert a continue-instruction"));
    toolBar->AddTool(NASSI_ID_RETURN, _("Return-Instruction"), wxBitmap(returntool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Return-Instruction"), _("Insert a return-instruction"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_GLASS_P, _("Zoom in"), wxBitmap(glassptool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Zoom in"), _("zoom in the diagram"));
    toolBar->AddTool(NASSI_ID_GLASS_N, _("Zoom out"), wxBitmap(glassntool16_xpm), wxNullBitmap, wxITEM_NORMAL, _("Zoom out"), _("zoom out the diagram"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_TOGGLE_SOURCE, _("Toggle Source"), wxBitmap( sourceToggletool16_xpm ), wxNullBitmap, wxITEM_CHECK, _("Toggle Source"), _("Show source-code in the diagram") );
    toolBar->AddTool(NASSI_ID_TOGGLE_COMMENT, _("Toggle Comments"), wxBitmap( commentToggletool16_xpm ), wxNullBitmap, wxITEM_CHECK, _("Toggle Comments"), _("Show comments in the diagram") );
}
void NassiPlugin::PopulateToolbar22(wxToolBar* toolBar)
{
    toolBar->AddTool(NASSI_ID_ESC, _("Select"),  wxBitmap(selecttool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Select"), _("Change the tool for selections"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_INSTRUCTION, _("Instruction"), wxBitmap(instrtool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Instruction"), _("Insert an instruction"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_IF, _("Decision"), wxBitmap(iftool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Decision"), _("Insert a decision"));
    toolBar->AddTool(NASSI_ID_SWITCH, _("Selection"), wxBitmap(switchtool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Selection"), _("Insert a selection"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_WHILE, _("Entry-Condition Loop"), wxBitmap(whiletool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Entry-Condition Loop"), _("Insert an entry-condition loop"));
    toolBar->AddTool(NASSI_ID_DOWHILE, _("Exit-Condition Loop"), wxBitmap(dowhiletool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Exit-Condition Loop"), _("Insert an exit-condition loop"));
    //toolBar->AddTool(NASSI_ID_WHILE1, _("Endless Loop"),  wxBitmap(while1tool_xpm), wxNullBitmap, wxITEM_NORMAL, _("Endless Loop"), _("Insert an endless loop"));
    toolBar->AddTool(NASSI_ID_FOR, _("Counting Loop"), wxBitmap(fortool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Counting Loop"), _("Insert a counting loop"));
    toolBar->AddTool(NASSI_ID_BLOCK, _("Block Instruction"), wxBitmap(blocktool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Block Instruction"), _("Insert a block Instruction {}"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_BREAK, _("Break-Instruction"), wxBitmap(breaktool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Break-Instruction"), _("Insert a break-instruction"));
    toolBar->AddTool(NASSI_ID_CONTINUE, _("Continue-Instruction"), wxBitmap(continuetool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Continue-Instruction"), _("Insert a continue-instruction"));
    toolBar->AddTool(NASSI_ID_RETURN, _("Return-Instruction"), wxBitmap(returntool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Return-Instruction"), _("Insert a return-instruction"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_GLASS_P, _("Zoom in"), wxBitmap(glassptool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Zoom in"), _("zoom in the diagram"));
    toolBar->AddTool(NASSI_ID_GLASS_N, _("Zoom out"), wxBitmap(glassntool22_xpm), wxNullBitmap, wxITEM_NORMAL, _("Zoom out"), _("zoom out the diagram"));
    toolBar->AddSeparator();
    toolBar->AddTool(NASSI_ID_TOGGLE_SOURCE, _("Toggle Source"), wxBitmap( sourceToggletool22_xpm ), wxNullBitmap, wxITEM_CHECK, _("Toggle Source"), _("Show source-code in the diagram") );
    toolBar->AddTool(NASSI_ID_TOGGLE_COMMENT, _("Toggle Comments"), wxBitmap( commentToggletool22_xpm ), wxNullBitmap, wxITEM_CHECK, _("Toggle Comments"), _("Show comments in the diagram") );
}
void NassiPlugin::OnToggleText(wxCommandEvent &event)
{
    if ( IsNassiEditorPanelActive() )
    {
        NassiEditorPanel *ed = (NassiEditorPanel *)Manager::Get()->GetEditorManager()->GetActiveEditor();
        if ( event.GetId() == NASSI_ID_TOGGLE_SOURCE )
            ed->EnableDrawSource( event.IsChecked() );
        else
            ed->EnableDrawComment( event.IsChecked() );
    }
}

void NassiPlugin::OnChangeTool(wxCommandEvent &event)
{
    if ( !IsNassiEditorPanelActive() ) return;
    NassiEditorPanel *ed = (NassiEditorPanel *)Manager::Get()->GetEditorManager()->GetActiveEditor();

    int id =  event.GetId();
    if      ( id == NASSI_ID_IF )      ed->ChangeToolTo(NassiView::NASSI_TOOL_IF);
    else if ( id == NASSI_ID_SWITCH )  ed->ChangeToolTo(NassiView::NASSI_TOOL_SWITCH);
    else if ( id == NASSI_ID_WHILE )   ed->ChangeToolTo(NassiView::NASSI_TOOL_WHILE);
    else if ( id == NASSI_ID_DOWHILE ) ed->ChangeToolTo(NassiView::NASSI_TOOL_DOWHILE);
    else if ( id == NASSI_ID_FOR )     ed->ChangeToolTo(NassiView::NASSI_TOOL_FOR);
    else if ( id == NASSI_ID_BLOCK )   ed->ChangeToolTo(NassiView::NASSI_TOOL_BLOCK);
    else if ( id == NASSI_ID_BREAK )   ed->ChangeToolTo(NassiView::NASSI_TOOL_BREAK);
    else if ( id == NASSI_ID_CONTINUE )ed->ChangeToolTo(NassiView::NASSI_TOOL_CONTINUE);
    else if ( id == NASSI_ID_RETURN )  ed->ChangeToolTo(NassiView::NASSI_TOOL_RETURN);
    else /*NASSI_ID_INSTRUCTION*/      ed->ChangeToolTo(NassiView::NASSI_TOOL_INSTRUCTION);
}

void NassiPlugin::OnToolSelect(wxCommandEvent & /*event*/)
{
    if ( !IsNassiEditorPanelActive() ) return;
    NassiEditorPanel *ed = (NassiEditorPanel *)Manager::Get()->GetEditorManager()->GetActiveEditor();

    ed->ToolSelect();
}

//{ zoom
void NassiPlugin::OnUpdateZoom(wxUpdateUIEvent &event)
{
    if ( !IsNassiEditorPanelActive() )
    {
        event.Enable(false);
        return;
    }
    NassiEditorPanel *ed = (NassiEditorPanel *)Manager::Get()->GetEditorManager()->GetActiveEditor();

    if ( event.GetId() == NASSI_ID_GLASS_P )
        event.Enable(ed->CanZoomIn());
    else
        event.Enable(ed->CanZoomOut());

}

void NassiPlugin::OnZoom(wxCommandEvent &event)
{
    if ( !IsNassiEditorPanelActive() ) return;
    NassiEditorPanel *ed = (NassiEditorPanel *)Manager::Get()->GetEditorManager()->GetActiveEditor();

    if ( event.GetId() == NASSI_ID_GLASS_P )
        ed->ZoomIn();
    else
        ed->ZoomOut();
}
//} zoom end

//{ export
void NassiPlugin::OnUpdateExport(wxUpdateUIEvent &event)
{
    if ( !IsNassiEditorPanelActive() )
    {
        event.Enable(false);
        return;
    }

    NassiEditorPanel *ed = (NassiEditorPanel *)Manager::Get()->GetEditorManager()->GetActiveEditor();

    event.Enable(ed->CanExport());
}

void NassiPlugin::OnExport(wxCommandEvent &event)
{
    if ( !IsNassiEditorPanelActive() )return;

    NassiEditorPanel *ed = (NassiEditorPanel *)Manager::Get()->GetEditorManager()->GetActiveEditor();

    int id = event.GetId();
    if( id == NASSI_ID_EXPORT_SOURCE )        ed->ExportCSource();
    #if wxCHECK_VERSION(3, 0, 0)
        else if ( id == NASSI_ID_EXPORT_SVG ) ed->ExportSVG();
    #endif
    else if( id == NASSI_ID_EXPORT_VHDL )     ed->ExportVHDLSource();
    #if wxUSE_POSTSCRIPT
        else if( id == NASSI_ID_EXPORT_PS )   ed->ExportPS();
    #endif
    else if( id == NASSI_ID_EXPORT_STRUKTEX ) ed->ExportStrukTeX();
    else /*NASSI_ID_EXPORT_BITMAP*/           ed->ExportBitmap();
}

//} export end

void NassiPlugin::OnUpdateUIMenuItem(wxUpdateUIEvent &event)
{
    bool enable = false;

    EditorManager* emngr = Manager::Get()->GetEditorManager();
    if (emngr)
    {
        EditorBase *edb = emngr->GetActiveEditor();
        if (edb && edb->IsBuiltinEditor())
        {
            cbStyledTextCtrl* stc = static_cast<cbEditor*>(edb)->GetControl();
            if (stc && stc->GetLexer() == wxSCI_LEX_CPP)
                enable = edb->HasSelection();
        }
    }
    event.Enable(enable);
}
