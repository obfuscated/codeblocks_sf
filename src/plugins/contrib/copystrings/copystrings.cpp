/***************************************************************
 * Name:      copystrings.cpp
 * Purpose:   Code::Blocks plugin - copies all literal strings to the clipboard
 * Author:    Ricardo Garcia
 * Copyright: (c) 2005 Ricardo Garcia
 * License:   wxWindows License
 **************************************************************/

#include "copystrings.h"
#include <licenses.h> // defines some common licenses (like the GPL)
#include <wx/clipbrd.h>

#include <sdk.h>
#include <manager.h>
#include <editormanager.h>
#include <cbeditor.h>
#include <wx/msgdlg.h>
#include <map>
using namespace std;

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(copystrings, "Copy Strings to clipboard");

copystrings::copystrings()
{
	//ctor
	m_PluginInfo.name = _T("copystrings");
	m_PluginInfo.title = _("Copy Strings to clipboard");
	m_PluginInfo.version = _T("1.00");
	m_PluginInfo.description = _("This plugin copies all the strings in the current editor, into the clipboard.\n" \
	 "Duplicated strings are removed.");
	m_PluginInfo.author = _T("Ricardo Garcia");
	m_PluginInfo.authorEmail = _T("rick_g22 <at> yahoo <dot> com");
	m_PluginInfo.authorWebsite = _T("http://www.codeblocks.org/");
	m_PluginInfo.thanksTo = _("The Code::Blocks team");
	m_PluginInfo.license = LICENSE_WXWINDOWS;
}

copystrings::~copystrings()
{
	//dtor
}

void copystrings::OnAttach()
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...
}

void copystrings::OnRelease(bool appShutDown)
{
	// do de-initialization for your plugin
	// if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
}

int copystrings::Execute()
{
	//do your magic ;)

//	NotImplemented(_T("copystrings::Execute()"));
	EditorManager* man = Manager::Get()->GetEditorManager();
	if(!man)
        return -1;
	cbEditor* myeditor = man->GetBuiltinActiveEditor();
	if(!myeditor)
        return -1;
	cbStyledTextCtrl* ctrl = myeditor->GetControl();
	if(ctrl)
	{
	    wxString result(_T(""));
	    wxString input(_T(""));
	    input = ctrl->GetText();
	    GetStrings(input,result);
        if (wxTheClipboard->Open())
        {
            wxTheClipboard->SetData( new wxTextDataObject(result));
            wxTheClipboard->Close();
        }
        wxMessageBox(_T("Literal strings copied to clipboard."));
	}
	return -1;
}

typedef map<wxString, bool, less<wxString> > mymaptype;

void copystrings::GetStrings(const wxString& buffer,wxString& result)
{
    size_t i = 0;
    i = 0;
    int mode = 0;
    mymaptype mymap;
    wxString curstr;
    curstr.Clear();
    for(i = 0; i < buffer.Length(); i++)
    {
        wxChar ch = buffer[i];
        switch(mode)
        {
            case 0: // Normal
                if(ch==_T('\''))
                    mode = 1;
                else if(ch==_T('"'))
                {
                    mode = 2;
                    curstr.Clear();
                    curstr << ch;
                }
                else if(ch==_T('\\'))
                    mode = 3;
                else if(ch==_T('/'))
                    mode = 6;
            break;
            case 1: // Single quotes mode
                if(ch==_T('\''))
                    mode = 0;
                else if(ch==_T('\\'))
                    mode = 4;
            break;
            case 2: // Double quotes mode
                curstr << ch;
                if(ch==_T('"'))
                {
                    mymap[curstr] = true;
                    mode = 0;
                }
                else if(ch==_T('\\'))
                    mode = 5;
            break;
            case 3: // Escaped
                mode = 0;
            break;

            case 4: // Single quotes, escaped
                mode = 1;
            break;
            case 5: // Double quotes, escaped
                curstr << ch;
                mode = 2;
            break;
            case 6: // Possibly opening comment
                if(ch == _T('/'))
                    mode = 7;
                else if(ch == _T('*'))
                    mode = 8;
                else
                    mode = 0;
            break;
            case 7: // C++ style comment
                if(ch == _T('\n') || ch == _T('\r'))
                    mode = 0;
            break;
            case 8: // C-style comment
                if(ch == _T('*'))
                    mode = 9;
            break;
            case 9: // Possibly closing C-style comment
                if(ch == _T('/'))
                    mode = 0;
                else if(ch == _T('*'))
                    mode = 9;
                else
                    mode = 8;
            break;
        }
    }
    result.Clear();
    mymaptype::iterator it;
    for(it = mymap.begin();it != mymap.end(); it++)
    {
        result << it->first;
#ifdef __WXMSW__
         result << _T("\r\n");
#else
         result << _T("\n");
#endif
    }
    return;
}
