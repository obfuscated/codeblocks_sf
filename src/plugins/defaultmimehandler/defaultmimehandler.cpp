/***************************************************************
 * Name:      defaultmimehandler.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis An. Mandravellos<mandrav@codeblocks.org>
 * Created:   03/13/05 14:08:51
 * Copyright: (c) Yiannis An. Mandravellos
 * License:   GPL
 **************************************************************/

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "defaultmimehandler.h"
#endif

#include "defaultmimehandler.h"
#include <manager.h>
#include <editormanager.h>
#include <configmanager.h>
#include <cbeditor.h>
#include <licenses.h> // defines some common licenses (like the GPL)

#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/choicdlg.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include "editmimetypesdlg.h"

#define CONF_GROUP "/mime_types"

cbPlugin* GetPlugin()
{
	return new DefaultMimeHandler;
}

DefaultMimeHandler::DefaultMimeHandler()
{
	//ctor
	m_PluginInfo.name = "DefaultMimeHandler";
	m_PluginInfo.title = "Default MIME handler";
	m_PluginInfo.version = "1.0";
	m_PluginInfo.description = "This is the default MIME handler for Code::Blocks";
	m_PluginInfo.author = "Yiannis An. Mandravellos";
	m_PluginInfo.authorEmail = "mandrav@codeblocks.org";
	m_PluginInfo.authorWebsite = "http://www.codeblocks.org";
	m_PluginInfo.thanksTo = "Code::Blocks";
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = true;

    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource::Get()->Load(resPath + "/defaultmimehandler.zip#zip:*.xrc");

	ConfigManager::AddConfiguration(_("MIME types handling"), CONF_GROUP);
}

DefaultMimeHandler::~DefaultMimeHandler()
{
	//dtor
}

void DefaultMimeHandler::OnAttach()
{
    // load configuration
    WX_CLEAR_ARRAY(m_MimeTypes);
	long cookie;
	wxString entry;
	wxConfigBase* conf = ConfigManager::Get();
	wxString oldPath = conf->GetPath();
	conf->SetPath(CONF_GROUP);
	bool cont = conf->GetFirstEntry(entry, cookie);
	while (cont)
	{
        wxArrayString array = GetArrayFromString(conf->Read(entry));
        if (array.GetCount() == 3 || array.GetCount() == 4)
        {
            cbMimeType* mt = new cbMimeType;
            mt->useEditor = array[0] == "true";
            mt->programIsModal = array[1] == "true";
            mt->wildcard = array[2];
            mt->program = array.GetCount() == 4 ? array[3] : "";
            
            if (!mt->useEditor && mt->program.IsEmpty())
                delete mt;
            else
                m_MimeTypes.Add(mt);
        }
		cont = conf->GetNextEntry(entry, cookie);
	}
	conf->SetPath(oldPath);
}


void DefaultMimeHandler::OnRelease(bool appShutDown)
{
    // save configuration
	wxConfigBase* conf = ConfigManager::Get();
	conf->DeleteGroup(CONF_GROUP);
	wxString oldPath = conf->GetPath();
	conf->SetPath(CONF_GROUP);
	for (unsigned int i = 0; i < m_MimeTypes.GetCount(); ++i)
	{
        cbMimeType* mt = m_MimeTypes[i];
        wxString txt;
        txt << (mt->useEditor ? "true" : "false") << ";";
        txt << (mt->programIsModal ? "true" : "false") << ";";
        txt << mt->wildcard << ";";
        txt << mt->program;
        wxString key;
        key.Printf("MimeType%d", i);
		conf->Write(key, txt);
	}
	conf->SetPath(oldPath);
    WX_CLEAR_ARRAY(m_MimeTypes);
}

int DefaultMimeHandler::Configure()
{
    EditMimeTypesDlg dlg(0, m_MimeTypes);
    dlg.ShowModal();
    return 0;
}

bool DefaultMimeHandler::CanHandleFile(const wxString& filename)
{
    // always return true
    // even if we don't know how to handle the file,
    // we 'll ask the user what to do when we are requested to open it...
	return true;
}

int DefaultMimeHandler::OpenFile(const wxString& filename)
{
	cbMimeType* mt = FindMimeTypeFor(filename);
	if (mt)
        return DoOpenFile(mt, filename);
    else
    {
        // not yet supported. ask the user how to open it.
        wxString choices[2] = {_("Select an external program to open it."),
                               _("Open it inside the Code::Blocks editor.")};
        wxSingleChoiceDialog dlg(0,
                                _("Code::Blocks does not yet know how to open this kind of file.\n"
                                "Please select what you want to do with it:"),
                                _("What to do?"),
                                sizeof(choices) / sizeof(choices[0]),
                                choices);
        dlg.SetSelection(0);
        if (dlg.ShowModal() == wxID_OK)
        {
            wxString ext = wxFileName(filename).GetExt().Lower();
            wxString wild = ext.IsEmpty()
                            ? wxFileName(filename).GetName().Lower()
                            : "*." + ext;
            switch (dlg.GetSelection())
            {
                case 0: // choose external program
                {
                    wxString prg = ChooseExternalProgram();
                    if (!prg.IsEmpty())
                    {
                        mt = new cbMimeType;
                        mt->wildcard = wild;
                        mt->useEditor = false;
                        mt->program = prg;
                        mt->programIsModal = wxMessageBox(_("Do you want Code::Blocks to be disabled while the external program is running?"), _("Question"), wxICON_QUESTION | wxYES_NO) == wxYES;
                        m_MimeTypes.Add(mt);
                        return DoOpenFile(mt, filename);
                    }
                    break;
                }
                case 1: // open in editor
                {
                    mt = new cbMimeType;
                    mt->wildcard = wild;
                    mt->useEditor = true;
                    m_MimeTypes.Add(mt);
                    return DoOpenFile(mt, filename);
                    break;
                }
                default: break;
            }
        }
    }
	return -1;
}

cbMimeType* DefaultMimeHandler::FindMimeTypeFor(const wxString& filename)
{
    wxString tmp = wxFileName(filename).GetFullName().Lower();

    // look for a registered type
    for (size_t i = 0; i < m_MimeTypes.GetCount(); ++i)
    {
        cbMimeType* mt = m_MimeTypes[i];
        if (tmp.Matches(mt->wildcard))
            return mt;
    }
    return 0;
}

wxString DefaultMimeHandler::ChooseExternalProgram()
{
    wxFileDialog* dlg = new wxFileDialog(0,
                            _("Select program"),
                            wxEmptyString,
                            wxEmptyString,
                            ALL_FILES_FILTER,
                            wxOPEN);
    if (dlg->ShowModal() == wxID_OK)
        return dlg->GetPath();
    return wxEmptyString;
}

int DefaultMimeHandler::DoOpenFile(cbMimeType* mt, const wxString& filename)
{
    if (!mt)
        return -1;

    if (mt->useEditor)
    {
        // easy. use internal editor.
		cbEditor* ed = Manager::Get()->GetEditorManager()->Open(filename);
		if (ed)
		{
//			ed->SetProjectFile(pf);
			ed->Show(true);
			return 0;
		}
    }
    else
    {
        // call external program

        // create command line
        wxString external = mt->program;
        if (external.Find("$(FILE)") != -1)
            external.Replace("$(FILE)", filename);
        else
            external << " \"" << filename << "\""; // file args wrapped in quotes (bug #1187231)

        // launch external program
        int ret = 0;
        if (mt->programIsModal)
        {
            wxEnableTopLevelWindows(false);
            ret = wxExecute(external, wxEXEC_SYNC);
            wxEnableTopLevelWindows(true);
        }
        else
            wxExecute(external, wxEXEC_ASYNC);
        return ret;
    }
    
    // failed...
    return -1;
}
