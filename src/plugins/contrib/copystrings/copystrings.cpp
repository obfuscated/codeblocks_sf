/***************************************************************
 * Name:      copystrings.cpp
 * Purpose:   Code::Blocks plugin - copies all literal strings to the clipboard
 * Author:    Ricardo Garcia
 * Copyright: (c) 2005 Ricardo Garcia
 * License:   wxWindows License
 **************************************************************/

#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/intl.h>
#include <wx/string.h>
#include "cbeditor.h"
#include "editormanager.h"
#include "pluginmanager.h"
#include "globals.h"
#include "manager.h"
#endif
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include "copystrings.h"
#include <map>
#include <iterator>

using namespace std;

// Register the plugin
namespace
{
    PluginRegistrant<copystrings> reg(_T("copystrings"));
};

copystrings::copystrings()
{
	//ctor
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

void GetStrings(const wxString& buffer,wxString& result)
{
    typedef map<wxString, bool, less<wxString> > mymaptype;
    int mode = 0;
    mymaptype mymap;
    wxString curstr;
    curstr.Clear();
    for(size_t i = 0; i < buffer.Length(); ++i)
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
    } // end for : idx : i
    result.Clear();
    for(mymaptype::iterator it = mymap.begin();it != mymap.end(); ++it)
    {
        result << it->first;
#ifdef __WXMSW__
         result << _T("\r\n");
#else
         result << _T("\n");
#endif
    }
    return;
} // end of GetStrings

int copystrings::Execute()
{
	//do your magic ;)

	EditorManager* man = Manager::Get()->GetEditorManager();
	if(!man)
        return -1;
	cbEditor* myeditor = man->GetBuiltinActiveEditor();
	if(!myeditor)
        return -1;
	if(cbStyledTextCtrl* ctrl = myeditor->GetControl())
	{
	    wxString result(_T(""));
	    wxString input(_T(""));
	    input = ctrl->GetText();
	    GetStrings(input, result);
        if (wxTheClipboard->Open())
        {
            wxTheClipboard->SetData( new wxTextDataObject(result));
            wxTheClipboard->Close();
        }
        cbMessageBox(_T("Literal strings copied to clipboard."));
	}
	return -1;
}
