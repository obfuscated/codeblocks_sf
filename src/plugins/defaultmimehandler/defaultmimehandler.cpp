/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/filename.h>
    #include <wx/fs_zip.h>
    #include "globals.h"
    #include <wx/intl.h>
    #include <wx/utils.h>
    #include <wx/xrc/xmlres.h>
    #include "cbeditor.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "logmanager.h"
    #include "manager.h"
#endif

#include <wx/choicdlg.h>
#include <wx/filedlg.h>

#include "EmbeddedHtmlPanel.h"
#include "defaultmimehandler.h"
#include "editmimetypesdlg.h"
#include "filefilters.h"

// this auto-registers the plugin
namespace
{
    PluginRegistrant<DefaultMimeHandler> reg(_T("FilesExtensionHandler"));

    const int idHtml = wxNewId();
}

DefaultMimeHandler::DefaultMimeHandler()
{
    //ctor
    if(!Manager::LoadResource(_T("defaultmimehandler.zip")))
    {
        NotifyMissingFile(_T("defaultmimehandler.zip"));
    }
}

DefaultMimeHandler::~DefaultMimeHandler()
{
    //dtor
}

void DefaultMimeHandler::OnAttach()
{
    // load configuration
    WX_CLEAR_ARRAY(m_MimeTypes);

    ConfigManager* conf = Manager::Get()->GetConfigManager(_T("mime_types"));
    wxArrayString list = conf->EnumerateKeys(_T("/"));
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        wxArrayString array = GetArrayFromString(conf->Read(list[i]), _T(";"), false);
        if (array.GetCount() < 3)
            continue;

        cbMimeType* mt = new cbMimeType;

        // older formats:
        // array.GetCount() == 3 or 4 (3 no ext. program, 4 yes)
        bool isOld = array.GetCount() == 3 || array.GetCount() == 4;
        if (isOld)
        {
            mt->useEditor = array[0] == _T("true");
            mt->useAssoc = false;
            mt->programIsModal = array[1] == _T("true");
            mt->wildcard = array[2];
            mt->program = array.GetCount() == 4 ? array[3] : _T("");
        }
        else
        {
            mt->useEditor = array[0] == _T("true");
            mt->useAssoc = array[1] == _T("true");
            mt->programIsModal = array[2] == _T("true");
            mt->wildcard = array[3];
            mt->program = array.GetCount() == 5 ? array[4] : _T("");
        }
        mt->program.Trim();

        if (!mt->useEditor && !mt->useAssoc && mt->program.IsEmpty())
            delete mt;
        else
            m_MimeTypes.Add(mt);
    }

    m_Html = new EmbeddedHtmlPanel(Manager::Get()->GetAppWindow());

    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.pWindow = m_Html;
    evt.name = _T("DefMimeHandler_HTMLViewer");
    evt.title = _("HTML viewer");
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 250);
    evt.floatingSize.Set(350, 250);
    evt.minimumSize.Set(150, 150);
    evt.shown = false;
    Manager::Get()->ProcessEvent(evt);
}


void DefaultMimeHandler::OnRelease(bool appShutDown)
{
    CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
    evt.pWindow = m_Html;
    Manager::Get()->ProcessEvent(evt);
    m_Html->Destroy();
    m_Html = 0;

    // save configuration
    ConfigManager* conf = Manager::Get()->GetConfigManager(_T("mime_types"));
    wxArrayString list = conf->EnumerateKeys(_T("/"));
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        conf->UnSet(list[i]);
    }
    for (unsigned int i = 0; i < m_MimeTypes.GetCount(); ++i)
    {
        cbMimeType* mt = m_MimeTypes[i];
        wxString txt;
        txt << (mt->useEditor ? _T("true") : _T("false")) << _T(";");
        txt << (mt->useAssoc ? _T("true") : _T("false")) << _T(";");
        txt << (mt->programIsModal ? _T("true") : _T("false")) << _T(";");
        txt << mt->wildcard << _T(";");
        txt << mt->program << _T(' ');
        wxString key;
        key.Printf(_T("MimeType%u"), i);
        conf->Write(key, txt);
    }
    WX_CLEAR_ARRAY(m_MimeTypes);
}

cbConfigurationPanel* DefaultMimeHandler::GetConfigurationPanel(wxWindow* parent)
{
    EditMimeTypesDlg* dlg = new EditMimeTypesDlg(parent, m_MimeTypes);
    return dlg;
}

bool DefaultMimeHandler::CanHandleFile(const wxString& filename) const
{
    // always return true
    // even if we don't know how to handle the file,
    // we 'll ask the user what to do when we are requested to open it...
    return true;
}

int DefaultMimeHandler::OpenFile(const wxString& filename)
{
    wxFileName the_file(filename);

    // don't check for existence because URLs can't be checked this way
//    if (!the_file.FileExists())
//        return -1;

    cbMimeType* mt = FindMimeTypeFor(filename);
    if (mt)
        return DoOpenFile(mt, filename);
    else if (the_file.GetExt().CmpNoCase(_T("htm")) == 0 ||
            the_file.GetExt().CmpNoCase(_T("html")) == 0)
    {
        // embedded help viewer (unless the user has added an explicit association manually)
        m_Html->Open(filename);
        CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
        evt.pWindow = m_Html;
        Manager::Get()->ProcessEvent(evt);
        return 0;
    }
    else
    {
        // not yet supported. ask the user how to open it.
        wxString choices[3] = {_("Select an external program to open it"),
                               _("Open it with the associated application"),
                               _("Open it inside the Code::Blocks editor")};
        wxSingleChoiceDialog dlg(Manager::Get()->GetAppWindow(),
                                _("Code::Blocks does not yet know how to open this kind of file.\n"
                                  "Please select what you want to do with it:"),
                                _("What to do?"),
                                sizeof(choices) / sizeof(choices[0]),
                                choices);
        dlg.SetSelection(0);
        PlaceWindow(&dlg);
        int answer = dlg.ShowModal();

        if (answer == wxID_OK)
        {
            wxString ext = the_file.GetExt().Lower();
            wxString wild = ext.IsEmpty()
                            ? the_file.GetName().Lower()
                            : wxString(_T("*.")) + ext;
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
                        mt->useAssoc = false;
                        mt->program = prg;
                        mt->programIsModal = cbMessageBox(_("Do you want Code::Blocks to be disabled while the external program is running?"), _("Question"), wxICON_QUESTION | wxYES_NO) == wxID_YES;
                        m_MimeTypes.Add(mt);
                        return DoOpenFile(mt, filename);
                    }
                    break;
                }
                case 1: // open with associated app
                    mt = new cbMimeType;
                    mt->wildcard = wild;
                    mt->useEditor = false;
                    mt->useAssoc = true;
                    m_MimeTypes.Add(mt);
                    return DoOpenFile(mt, filename);
                    break;
                case 2: // open in editor
                {
                    mt = new cbMimeType;
                    mt->wildcard = wild;
                    mt->useEditor = true;
                    mt->useAssoc = false;
                    m_MimeTypes.Add(mt);
                    return DoOpenFile(mt, filename);
                    break;
                }
                default: break;
            }
        }
        else if (answer == wxID_CANCEL)
        {
            return 0; // Cancel is interpreted as success, too
        }
        else
        {
            return -1;
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
    wxFileDialog dlg(0,
                     _("Select program"),
                    wxEmptyString,
                    wxEmptyString,
                    FileFilters::GetFilterAll(),
                    wxFD_OPEN | compatibility::wxHideReadonly);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
        return dlg.GetPath();
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
//            ed->SetProjectFile(pf);
            ed->Show(true);
            return 0;
        }
    }
    else if (mt->useAssoc)
    {
        // easy too. use associated app
        #ifdef __WXMSW__
        ShellExecute(0, wxString(_T("open")).c_str(), filename.c_str(), 0, 0, SW_SHOW);
        #endif
        #ifdef __WXGTK__
        wxExecute(wxString::Format(_T("xdg-open \"%s\""), filename.c_str()));
        #endif
        #ifdef __WXMAC__
        wxExecute(wxString::Format(_T("open \"%s\""), filename.c_str()));
        #endif
        return 0;
    }
    else
    {
        // call external program

        // create command line
        wxString external = mt->program;
        if (external.Find(_T("$(FILE)")) != -1)
            external.Replace(_T("$(FILE)"), filename);
        else
            external << _T(" \"") << filename << _T("\""); // file args wrapped in quotes (bug #1187231)

        Manager::Get()->GetLogManager()->Log(_T("Launching command: ") + external);

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
