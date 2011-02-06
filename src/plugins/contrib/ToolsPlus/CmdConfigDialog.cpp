///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  2 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "ToolsPlus.h"
#include "CmdConfigDialog.h"

///////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(CmdConfigDialog, wxDialog)
  EVT_BUTTON(ID_NEW, CmdConfigDialog::New)
  EVT_BUTTON(ID_COPY, CmdConfigDialog::Copy)
  EVT_BUTTON(ID_DELETE, CmdConfigDialog::Delete)
  EVT_BUTTON(ID_UP, CmdConfigDialog::OnUp)
  EVT_BUTTON(ID_DOWN, CmdConfigDialog::OnDown)
  EVT_LISTBOX(ID_COMMANDLIST, CmdConfigDialog::ChangeSelection)
  EVT_TEXT(ID_COMMANDNAME, CmdConfigDialog::NameChange)
  EVT_BUTTON(ID_EXPORT, CmdConfigDialog::OnExport)
  EVT_BUTTON(ID_IMPORT, CmdConfigDialog::OnImport)
END_EVENT_TABLE()


CmdConfigDialog::CmdConfigDialog( wxWindow* parent, ToolsPlus* plugin) : wxDialog(parent,wxID_ANY,GetTitle(),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
    m_plugin=plugin;
    m_icperm=&(plugin->m_ic);
    m_ic.interps=plugin->m_ic.interps; //holds temporary interpreter properties for editing in the dialog (actual properties are not overwritten until user presses APPLY/OK)

    wxSizer *button_sizer=CreateSeparatedButtonSizer(wxOK|wxCANCEL);

    //SetSizeHints(600, 500);

	wxBoxSizer* main_sizer;
	main_sizer = new wxBoxSizer( wxVERTICAL );

    wxNotebook *nb=new wxNotebook(this,wxID_ANY);
    wxPanel *tools_panel=new wxPanel(nb);
    wxPanel *settings_panel=new wxPanel(nb);
    nb->AddPage(tools_panel,_("Tools"),true);
    nb->AddPage(settings_panel,_("Plugin Settings"),false);

    wxBoxSizer *tools_sizer=new wxBoxSizer( wxHORIZONTAL);
    wxBoxSizer *settings_sizer=new wxBoxSizer( wxHORIZONTAL);
    wxBoxSizer *list_sizer=new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *p_sizer=new wxBoxSizer( wxVERTICAL );
	tools_sizer->Add(list_sizer,1,wxALL|wxEXPAND,20);
	tools_sizer->Add(p_sizer,1,wxALL|wxEXPAND,20);
    tools_panel->SetSizer(tools_sizer);
    settings_panel->SetSizer(settings_sizer);
    main_sizer->Add(nb,1,wxALL|wxEXPAND,5);

    m_replace_tools=new wxCheckBox(settings_panel,wxID_ANY,_("Replace Tools menu with Tools Plus"));
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("ShellExtensions"));
    m_replace_tools->SetValue(cfg->ReadBool(_T("HideToolsMenu"),false));
    settings_sizer->Add(m_replace_tools);

    m_prop_panel=new wxPanel(tools_panel);
	wxBoxSizer *prop_sizer=new wxBoxSizer( wxVERTICAL );
    m_prop_panel->SetSizer(prop_sizer);
	p_sizer->Add(m_prop_panel,1,wxEXPAND);

	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer_toprow;
	bSizer_toprow = new wxBoxSizer( wxHORIZONTAL );

	m_staticText27 = new wxStaticText( tools_panel, wxID_ANY, wxT("Known Tools"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43->Add( m_staticText27, 0, wxALL, 5 );

	m_commandlist = new wxListBox( tools_panel, ID_COMMANDLIST, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer43->Add( m_commandlist, 1, wxALL|wxEXPAND, 1 );

	bSizer40->Add( bSizer43, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	m_butnew = new wxButton( tools_panel, ID_NEW, wxT("New"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_butnew, 0, wxLEFT|wxRIGHT, 5 );

	m_butcopy = new wxButton( tools_panel, ID_COPY, wxT("Copy"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_butcopy, 0, wxLEFT|wxRIGHT, 5 );

	m_butdelete = new wxButton( tools_panel, ID_DELETE, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_butdelete, 0, wxRIGHT|wxLEFT, 5 );

	bSizer11->Add( 0, 10, 1, wxALL, 5 );

	m_butup = new wxButton( tools_panel, ID_UP, wxT("Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_butup, 0, wxRIGHT|wxLEFT, 5 );

	m_butdown = new wxButton( tools_panel, ID_DOWN, wxT("Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_butdown, 0, wxRIGHT|wxLEFT, 5 );

	bSizer11->Add( 0, 10, 1, wxALL, 5 );

	wxButton *m_butimport = new wxButton( tools_panel, ID_IMPORT, wxT("Import..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_butimport, 0, wxRIGHT|wxLEFT, 5 );

	wxButton *m_butexport = new wxButton( tools_panel, ID_EXPORT, wxT("Export..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_butexport, 0, wxRIGHT|wxLEFT, 5 );

	bSizer40->Add( bSizer11, 0, wxEXPAND, 5 );

	list_sizer->Add( bSizer40, 1, wxEXPAND, 5 );

	wxBoxSizer* name_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_staticText11 = new wxStaticText( m_prop_panel, wxID_ANY, wxT("Tool Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	name_sizer->Add( m_staticText11, 0, wxALIGN_CENTER|wxALL, 5 );
	m_commandname = new wxTextCtrl( m_prop_panel, ID_COMMANDNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	name_sizer->Add( m_commandname, 2, wxALL, 1 );
	m_commandname->SetToolTip(_T("Set a name to easily identify the command in m_prop_panel list. For tools that have output redirect to the Tools output window, the name will also be displayed in the tab after the command is executed"));
	prop_sizer->Add( name_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* commandtext_sizer = new wxBoxSizer( wxVERTICAL);
	m_staticText12 = new wxStaticText( m_prop_panel, wxID_ANY, wxT("Command Line:"), wxDefaultPosition, wxDefaultSize, 0 );
	commandtext_sizer->Add( m_staticText12, 0, 0, 5 );
	prop_sizer->Add( commandtext_sizer, 0, wxALIGN_LEFT|wxALL, 5 );

    wxBoxSizer *command_sizer=new wxBoxSizer(wxHORIZONTAL);
	m_command = new wxTextCtrl( m_prop_panel, ID_COMMAND, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_command->SetToolTip(_T("The command line to be executed. The following variable substitions are accepted:\n\n$relfile,$file: respectively the relative and absolute name of a selected file\n$reldir, $dir: respectively the relative and absolute name of a selected directory\n$relpath,$path: the relative and absolute name of the selected file or directory\n$mpaths: a list of selected files or directories (absolute paths only)\n$fname,$fext: the name without extension and the extension without name of a selected file\n$inputstr{prompt}: prompts the user to enter a string of text which is subsituted into the command line\n\nRight clicking on a file, directory or multiple paths in the Project Tree, File Explorer or Editor Pane will only populate if m_prop_panel command handles that type of object.\nTo use relative path names make sure you set the working directory appropriately (typically use $parentdir)\nYou can also use global, project and codeblocks special variables"));
	command_sizer->Add(m_command,1,wxEXPAND);
	prop_sizer->Add( command_sizer, 0, wxALL|wxEXPAND, 1 );

	wxBoxSizer* wild_sizer=new wxBoxSizer( wxHORIZONTAL);
	m_staticText28 = new wxStaticText( m_prop_panel, wxID_ANY, wxT("File Types:"), wxDefaultPosition, wxDefaultSize, 0 );
	wild_sizer->Add( m_staticText28, 0, wxALIGN_LEFT|wxALL, 5 );
	m_wildcards = new wxTextCtrl( m_prop_panel, ID_WILDCARDS, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_wildcards->SetToolTip(_T("The context menu will only be populated with the tool if the file or directory selected matches the semi-colon separated list of wildcard expressions. For example \"*.cpp;*.h;makefile.*;Makefile.*\" to handle C++ sources, headers and makefiles. Leave blank to handle all file/directory types"));
	wild_sizer->Add( m_wildcards, 1, wxALL, 1 );
	prop_sizer->Add( wild_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* wdir_sizer=new wxBoxSizer( wxHORIZONTAL);
	m_staticText112 = new wxStaticText( m_prop_panel, wxID_ANY, wxT("Working Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	wdir_sizer->Add( m_staticText112, 0, wxALIGN_LEFT|wxALL, 5 );
	m_workdir = new wxTextCtrl( m_prop_panel, ID_WORKDIR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_workdir->SetToolTip(_T("m_prop_panel is the working directory for executing the command. Two context specific variables may be available:\n1. If you have specified $dir in the command line then you can use $dir here also.\n2. $parentdir is available for $relfile, $file, $reldir, $dir, $relpath, $path, $fname, $fext and is the absolute path of the directory containing the item.\nYou can also use codeblocks variables, project variables and global variables"));
	wdir_sizer->Add( m_workdir, 1, wxEXPAND, 1 );
	prop_sizer->Add( wdir_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* menu_sizer = new wxBoxSizer( wxHORIZONTAL);
	m_staticText13 = new wxStaticText( m_prop_panel, wxID_ANY, wxT("Tools Menu Path"), wxDefaultPosition, wxDefaultSize, 0 );
	menu_sizer->Add( m_staticText13, 0, wxALIGN_LEFT|wxALL, 5 );
	m_menuloc = new wxTextCtrl( m_prop_panel, ID_MENULOC, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_menuloc->SetToolTip(_T("m_prop_panel field controls the appearance of the command in the \"ToolsPlus\" menu.\nSpecify the nested structure as a path: for example submenu1/submenu2/itemname\nIf you leave itemname blank the command name will be used. If you specify a period as the first character of the field, the command will not be shown in the Extensions menu."));
	menu_sizer->Add( m_menuloc, 1, wxALL, 1 );
	prop_sizer->Add( menu_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* menupriority_sizer = new wxBoxSizer( wxHORIZONTAL);
	m_staticText16 = new wxStaticText( m_prop_panel, wxID_ANY, wxT("Priority"), wxDefaultPosition, wxDefaultSize, 0 );
	menupriority_sizer->Add( m_staticText16, 0, wxALIGN_CENTER|wxALL, 5 );
	m_menulocpriority = new wxSpinCtrl( m_prop_panel, ID_MENULOCPRIORITY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0);
	menupriority_sizer->Add( m_menulocpriority, 0, wxALL, 1 );
	prop_sizer->Add( menupriority_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* contextmenu_sizer = new wxBoxSizer( wxHORIZONTAL);
	m_staticText131 = new wxStaticText( m_prop_panel, wxID_ANY, wxT("Context Menu Path"), wxDefaultPosition, wxDefaultSize, 0 );
	contextmenu_sizer->Add( m_staticText131, 0, wxALIGN_CENTER|wxALL, 5 );
	m_cmenuloc = new wxTextCtrl( m_prop_panel, ID_CMENULOC, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_cmenuloc->SetToolTip(_T("m_prop_panel field controls the appearance of the command in context menus offered when you right click files or directories in the File Explorer, files in the Project Manager and file in the Editor pane.\nSpecify the nested structure as a path: for example submenu1/submenu2/itemname\nIf you leave itemname blank the command name will be used. If you specify a period as the first character of the field, the command will not be shown in any context menu."));
	contextmenu_sizer->Add( m_cmenuloc, 1, wxALL, 1 );
	prop_sizer->Add( contextmenu_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* contextmenupriority_sizer = new wxBoxSizer( wxHORIZONTAL);
	m_staticText161 = new wxStaticText( m_prop_panel, wxID_ANY, wxT("Priority:"), wxDefaultPosition, wxDefaultSize, 0 );
	contextmenupriority_sizer->Add( m_staticText161, 0, wxALIGN_LEFT|wxALL, 5 );
	m_cmenulocpriority = new wxSpinCtrl( m_prop_panel, ID_CMENULOCPRIORITY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0);
	contextmenupriority_sizer->Add( m_cmenulocpriority, 0, wxALL, 1 );
	prop_sizer->Add( contextmenupriority_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* output_sizer = new wxBoxSizer( wxVERTICAL);
	m_staticText111 = new wxStaticText( m_prop_panel, wxID_ANY, wxT("Output to:"), wxDefaultPosition, wxDefaultSize, 0 );
	output_sizer->Add( m_staticText111, 0, wxALIGN_LEFT|wxALL, 5 );
	wxString m_modeChoices[] = { wxT("Tools Output Window"), wxT("Code::Blocks Console"), wxT("Standard Shell") };
	int m_modeNChoices = sizeof( m_modeChoices ) / sizeof( wxString );
	m_mode = new wxChoice( m_prop_panel, ID_MODE, wxDefaultPosition, wxDefaultSize, m_modeNChoices, m_modeChoices, 0 );
	m_mode->SetToolTip(_T("Select how the command is spawned:\n1. Windowed Console Notepage: redirects input and output to the Shell Extensions Dockable Notebook\n2. Code::Blocks Console: Runs as an external app in a terminal window, reports elapsed time and pause after execution.\n3. Standard Shell execution: will either spawn the command in a standard terminal window (win32), or spawn the command hidden (linux)."));
	output_sizer->Add( m_mode, 0, wxALL|wxEXPAND, 1 );
	prop_sizer->Add( output_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* env_sizer = new wxBoxSizer( wxHORIZONTAL);
	m_staticText1111 = new wxStaticText( m_prop_panel, wxID_ANY, wxT("Environment Vars:"), wxDefaultPosition, wxDefaultSize, 0 );
	env_sizer->Add( m_staticText1111, 0, wxALIGN_LEFT|wxALL, 5 );
	wxString m_envvarsChoices[] = {  };
	int m_envvarsNChoices = sizeof( m_envvarsChoices ) / sizeof( wxString );
	m_envvars = new wxChoice( m_prop_panel, ID_ENVVARS, wxDefaultPosition, wxDefaultSize, m_envvarsNChoices, m_envvarsChoices, 0 );
	env_sizer->Add( m_envvars, 0, wxALL|wxEXPAND, 1 );
	prop_sizer->Add( env_sizer, 0, wxEXPAND, 5 );

	
	main_sizer->Add(button_sizer,0,wxALIGN_CENTER);
	
	this->SetSizer( main_sizer );
	this->SetSizeHints(500,400);
	this->Layout();
	this->Fit();
    m_activeinterp=0;
    for(unsigned int i=0;i<m_ic.interps.GetCount();i++)
        m_commandlist->Append(m_ic.interps[i].name);
    SetDialogItems();

}


void CmdConfigDialog::OnApply()
{
    GetDialogItems();
    m_icperm->interps=m_ic.interps;
    m_icperm->WriteConfig();
    m_plugin->UpdateMenu(m_replace_tools->IsChecked());
}

void CmdConfigDialog::NameChange(wxCommandEvent& event)
{
    if(m_ic.interps.GetCount()>0)
        m_commandlist->SetString(m_activeinterp, m_commandname->GetValue());
}

void CmdConfigDialog::ChangeSelection(wxCommandEvent& event)
{
    if(m_commandlist->GetSelection()>=0)
    {
        GetDialogItems();
        m_activeinterp=m_commandlist->GetSelection();
        SetDialogItems();
    }
}

// Updates the Dialog controls to the stored values for the current interpreter
void CmdConfigDialog::SetDialogItems()
{
    if(m_ic.interps.GetCount()>0&&m_activeinterp>=0&&m_activeinterp<static_cast<int>(m_ic.interps.GetCount()))
    {
        m_commandname->Enable();
        m_command->Enable();
        m_wildcards->Enable();
        m_workdir->Enable();
        m_menuloc->Enable();
        m_menulocpriority->Enable();
        m_cmenuloc->Enable();
        m_cmenulocpriority->Enable();
        m_mode->Enable();
        m_envvars->Enable();

        ShellCommand &interp=m_ic.interps[m_activeinterp];
        m_commandname->SetValue(interp.name);
        m_command->SetValue(interp.command);
        m_wildcards->SetValue(interp.wildcards);
        m_workdir->SetValue(interp.wdir);
        m_menuloc->SetValue(interp.menu);
        m_menulocpriority->SetValue(interp.menupriority);
        m_cmenuloc->SetValue(interp.cmenu);
        m_cmenulocpriority->SetValue(interp.cmenupriority);
        if(interp.mode==_T("W"))
            m_mode->SetSelection(0);
        else if(interp.mode==_T("C"))
            m_mode->SetSelection(1);
        else
            m_mode->SetSelection(2);
        m_envvars->SetSelection(m_envvars->FindString(interp.envvarset));
    } else
    {
        m_commandname->SetValue(_T(""));
        m_command->SetValue(_T(""));
        m_wildcards->SetValue(_T(""));
        m_workdir->SetValue(_T(""));
        m_menuloc->SetValue(_T(""));
        m_menulocpriority->SetValue(0);
        m_cmenuloc->SetValue(_T(""));
        m_cmenulocpriority->SetValue(0);
        m_mode->SetSelection(0);
        m_envvars->SetSelection(0);

        m_commandname->Disable();
        m_command->Disable();
        m_wildcards->Disable();
        m_workdir->Disable();
        m_menuloc->Disable();
        m_menulocpriority->Disable();
        m_cmenuloc->Disable();
        m_cmenulocpriority->Disable();
        m_mode->Disable();
        m_envvars->Disable();
    }
}

// Retrieve configuration values from the dialog widgets and store them appropriately
void CmdConfigDialog::GetDialogItems()
{
    if(!m_ic.interps.GetCount()||m_activeinterp<0||m_activeinterp>=static_cast<int>(m_ic.interps.GetCount()))
        return;
    ShellCommand &interp=m_ic.interps[m_activeinterp];
    interp.name=m_commandname->GetValue();
    interp.command=m_command->GetValue();
    interp.wildcards=m_wildcards->GetValue();
    interp.wdir=m_workdir->GetValue();
    interp.menu=m_menuloc->GetValue();
    interp.menupriority=m_menulocpriority->GetValue();
    interp.cmenu=m_cmenuloc->GetValue();
    interp.cmenupriority=m_cmenulocpriority->GetValue();
    switch(m_mode->GetSelection())
    {
        case 0:
            interp.mode=_T("W");
            break;
        case 1:
            interp.mode=_T("C");
            break;
        case 2:
            interp.mode=_T("");
            break;
    }
    interp.envvarset=m_envvars->GetStringSelection();
}

void CmdConfigDialog::New(wxCommandEvent &event)
{
    GetDialogItems();
    ShellCommand interp;
    interp.name=_T("New Tool");
    m_ic.interps.Add(interp);

    m_activeinterp=m_ic.interps.GetCount()-1;

    m_commandlist->Insert(m_ic.interps[m_activeinterp].name,m_activeinterp);

    m_commandlist->SetSelection(m_activeinterp);
    SetDialogItems();
}

void CmdConfigDialog::Copy(wxCommandEvent &event)
{
    GetDialogItems();
    if(m_ic.interps.GetCount()>0)
    {
        ShellCommand interp=m_ic.interps[m_activeinterp];
        interp.name+=_T(" (Copy)");
        m_ic.interps.Add(interp);

        m_activeinterp=m_ic.interps.GetCount()-1;

        m_commandlist->Insert(m_ic.interps[m_activeinterp].name,m_activeinterp);

        m_commandlist->SetSelection(m_activeinterp);
        SetDialogItems();
    }
}

void CmdConfigDialog::Delete(wxCommandEvent &event)
{
    if(m_activeinterp>=0 && m_ic.interps.GetCount()>0)
//        if (cbMessageBox(_("Are you sure you want to remove this command?"), _("Remove"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
        {
            m_ic.interps.RemoveAt(m_activeinterp);
            m_commandlist->Delete(m_activeinterp);
            if(m_activeinterp>=static_cast<int>(m_ic.interps.GetCount()))
                m_activeinterp=m_ic.interps.GetCount()-1;
            SetDialogItems();
            if(m_activeinterp>=0)
                m_commandlist->SetSelection(m_activeinterp);
        }
}

void CmdConfigDialog::OnUp(wxCommandEvent &event)
{
    if(m_activeinterp>0 && m_ic.interps.GetCount()>1)
    {
        GetDialogItems();
        ShellCommand interp=m_ic.interps[m_activeinterp];
        m_ic.interps.RemoveAt(m_activeinterp);
        m_commandlist->Delete(m_activeinterp);
        m_activeinterp--;
        m_ic.interps.Insert(interp,m_activeinterp);
        m_commandlist->Insert(interp.name,m_activeinterp);
        m_commandlist->Select(m_activeinterp);
    }
}

void CmdConfigDialog::OnDown(wxCommandEvent &event)
{
    if(m_activeinterp+1<static_cast<int>(m_ic.interps.GetCount()) && m_ic.interps.GetCount()>1)
    {
        GetDialogItems();
        ShellCommand interp=m_ic.interps[m_activeinterp];
        m_ic.interps.RemoveAt(m_activeinterp);
        m_commandlist->Delete(m_activeinterp);
        m_activeinterp++;
        m_ic.interps.Insert(interp,m_activeinterp);
        m_commandlist->Insert(interp.name,m_activeinterp);
        m_commandlist->Select(m_activeinterp);
    }
}

void CmdConfigDialog::OnImport(wxCommandEvent &event)
{
    #ifdef __WXMSW__
    wxFileDialog fd(NULL, _T("Import: Select File"),_T(""),_T(""),_T("*.*"),wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    #else
    wxFileDialog fd(NULL, _T("Import: Select File"),_T(""),_T(""),_T("*"),wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    #endif
    int prevlistsize=m_ic.interps.GetCount();
    if(fd.ShowModal()!=wxID_OK)
        return;
    m_ic.ImportConfig(fd.GetPath());
    for(unsigned int i=prevlistsize;i<m_ic.interps.GetCount();i++)
        m_commandlist->Append(m_ic.interps[i].name);
}

void CmdConfigDialog::OnExport(wxCommandEvent &event)
{
    #ifdef __WXMSW__
    wxFileDialog fd(NULL, _T("Export: Choose a Filename"),_T(""),_T(""),_T("*.*"),wxFD_SAVE);
    #else
    wxFileDialog fd(NULL, _T("Export: Choose a Filename"),_T(""),_T(""),_T("*"),wxFD_SAVE);
    #endif
    if(fd.ShowModal()!=wxID_OK)
        return;
    m_ic.ExportConfig(fd.GetPath());
}

