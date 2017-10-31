#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/regex.h>
#include "PipedProcessCtrl.h"
#include <globals.h>
#include <cbeditor.h>

////////////////////////////////////// PipedProcessCtrl /////////////////////////////////////////////
#define PP_ERROR_STYLE 1
#define PP_LINK_STYLE 2


int ID_PROC=wxNewId();

BEGIN_EVENT_TABLE(PipedTextCtrl, wxScintilla)
    EVT_LEFT_DCLICK(PipedTextCtrl::OnDClick)
    EVT_KEY_DOWN(PipedTextCtrl::OnUserInput)
END_EVENT_TABLE()

PipedTextCtrl::PipedTextCtrl(wxWindow *parent, PipedProcessCtrl *pp) : wxScintilla(parent, wxID_ANY)
{
    m_pp = pp;

    wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("editor"));
    wxString fontstring = mgr->Read(_T("/font"), wxEmptyString);
    if (!fontstring.IsEmpty())
    {
        wxNativeFontInfo nfi;
        nfi.FromString(fontstring);
        font.SetNativeFontInfo(nfi);
    }

    StyleSetFont(wxSCI_STYLE_DEFAULT, font);
    StyleSetForeground(PP_ERROR_STYLE,wxColor(200,0,0));
    StyleSetForeground(PP_LINK_STYLE,wxColor(0,0,200));
    StyleSetUnderline(PP_LINK_STYLE,true);
}


void PipedTextCtrl::OnDClick(wxMouseEvent &e)
{
    m_pp->OnDClick(e);
}


void PipedTextCtrl::OnUserInput(wxKeyEvent &e)
{
    m_pp->OnUserInput(e);
}


BEGIN_EVENT_TABLE(PipedProcessCtrl, wxPanel)
    EVT_CHAR(PipedProcessCtrl::OnUserInput)
    EVT_END_PROCESS(ID_PROC, PipedProcessCtrl::OnEndProcess)
    EVT_SIZE    (PipedProcessCtrl::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(PipedProcessCtrl, wxPanel)


PipedProcessCtrl::PipedProcessCtrl(wxWindow* parent, int id, const wxString &name, ShellManager *shellmgr) : ShellCtrlBase(parent, id, name, shellmgr)
{
    m_shellmgr=shellmgr;
    m_name=name;
    m_dead=true;
    m_proc=NULL;
    m_killlevel=0;
    m_linkclicks=true;
    m_parselinks=true;
    m_textctrl=new PipedTextCtrl(this,this);//(this, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH|wxTE_MULTILINE|wxTE_READONLY|wxTE_PROCESS_ENTER|wxEXPAND);
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
    bs->Add(m_textctrl, 1, wxEXPAND | wxALL);
    SetAutoLayout(TRUE);
    SetSizer(bs);
}


void PipedProcessCtrl::OnEndProcess(wxProcessEvent &event)
{
    m_exitcode=event.GetExitCode();
    SyncOutput(-1); //read any left over output TODO: while loop to handle extremely large amount of output
    m_dead=true;
    delete m_proc;
    m_proc=NULL;
    m_killlevel=0;
    if(m_shellmgr)
        m_shellmgr->OnShellTerminate(this);
}

long PipedProcessCtrl::LaunchProcess(const wxString &processcmd, const wxArrayString &/*options*/) // bool ParseLinks, bool LinkClicks, const wxString &LinkRegex
{
    if(!m_dead)
        return -1;
    if(m_proc) //this should never happen
        m_proc->Detach(); //self cleanup
    m_proc=new wxProcess(this,ID_PROC);
    m_proc->Redirect();
    m_procid=wxExecute(processcmd,wxEXEC_ASYNC,m_proc);
    m_parselinks=true;//ParseLinks;
    m_linkclicks=true;//LinkClicks;
    m_linkregex=LinkRegexDefault; //LinkRegex;
    if(m_procid>0)
    {
        m_ostream=m_proc->GetOutputStream();
        m_istream=m_proc->GetInputStream();
        m_estream=m_proc->GetErrorStream();
        m_dead=false;
        m_killlevel=0;
    }
    return m_procid;
}

void PipedProcessCtrl::KillProcess()
{
    if(m_dead)
        return;
//    if(m_killlevel==0) //some process will complete if we send EOF. TODO: make sending EOF a separate option
//    {
//        m_proc->CloseOutput();
//        m_killlevel=1;
//        return;
//    }
    long pid=GetPid();
#ifdef __WXGTK__
    if(m_killlevel==0)
    {
        m_killlevel=1;
        if(wxProcess::Exists(pid))
            wxProcess::Kill(pid,wxSIGTERM);
        return;
    }
    if(m_killlevel==1)
    {
        if(wxProcess::Exists(pid))
        {
            wxProcess::Kill(pid,wxSIGKILL);
        }
    }
#else
    if(wxProcess::Exists(pid))
    {
        wxProcess::Kill(pid,wxSIGKILL);
    }
#endif //__WXGTK__
}

wxString PipedProcessCtrl::LinkRegexDefault=
_T("[\"']?((?:\\w\\:)?[^'\",\\s:;*?]+?)[\"']?[\\s]*(\\:|\\(|\\[|\\,?\\s*[Ll]ine)?\\s*(\\d*)");
//           a:         \path\to\file              line 300
void PipedProcessCtrl::SyncOutput(int maxchars)
{
    if(!m_proc)
        return;
    bool oneshot=true;
    if(maxchars<=0)
    {
        maxchars=20000;
        oneshot=false;
    }
    int lineno=m_textctrl->GetLineCount()-1;
    while(m_proc->IsInputAvailable())
    {
        char buf0[maxchars+1];
        for(int i=0;i<maxchars+1;i++)
            buf0[i]=0;
        m_istream->Read(buf0,maxchars);
        wxString m_latest=wxString::FromAscii(buf0);
        long start,end;
        start=m_textctrl->GetSelectionStart();
        end=m_textctrl->GetSelectionEnd();
        int pos=start>end?start:end;
        bool move_caret=(pos>=m_textctrl->PositionFromLine(m_textctrl->GetLineCount())&&
                         (start==end));
        m_textctrl->AppendText(m_latest);
        if(move_caret)
        {
                m_textctrl->GotoLine(m_textctrl->GetLineCount());
//                m_textctrl->SetSelectionStart(-1);
//                m_textctrl->SetSelectionEnd(-1);
        }
        if(oneshot)
            break;
    }
    if(m_proc->IsErrorAvailable())
    {
        while(m_proc->IsErrorAvailable())
        {
            char buf0[maxchars+1];
            for(int i=0;i<maxchars+1;i++)
                buf0[i]=0;
            m_estream->Read(buf0,maxchars);
            wxString m_latest=wxString::FromAscii(buf0);
            long start,end;
            start=m_textctrl->GetSelectionStart();
            end=m_textctrl->GetSelectionEnd();
            int pos=start>end?start:end;
            bool move_caret=(pos>=m_textctrl->PositionFromLine(m_textctrl->GetLineCount())&&
                             (start==end));
            int style_start=m_textctrl->PositionFromLine(m_textctrl->GetLineCount());
            m_textctrl->AppendText(m_latest);
            if(move_caret)
            {
                m_textctrl->GotoLine(m_textctrl->GetLineCount());
//                m_textctrl->SetSelectionStart(-1);
//                m_textctrl->SetSelectionEnd(-1);
            }

            m_textctrl->StartStyling(style_start,0x1F);
            m_textctrl->SetStyling(m_textctrl->PositionFromLine(m_textctrl->GetLineCount())-style_start,PP_ERROR_STYLE);
            if(oneshot)
                break;
        }
    }
    if(m_parselinks)
        ParseLinks(lineno,m_textctrl->GetLineCount());
}

void PipedProcessCtrl::ParseLinks(int lineno, int lastline)
{
    wxRegEx re(m_linkregex,wxRE_ADVANCED|wxRE_NEWLINE);
    while(lineno<lastline)
    {
        int col=0;
        wxString text=m_textctrl->GetLine(lineno);
        wxString file;
        while(re.Matches(text))
        {
            size_t start,len;
            if(re.GetMatch(&start,&len,0))
            {
                size_t fstart, flen;
                if(re.GetMatch(&fstart,&flen,1))
                    file=text.Mid(fstart,flen);
                wxFileName f(file);
                if(f.FileExists())
                {
                    int pos=m_textctrl->PositionFromLine(lineno)+col+start;
                    m_textctrl->StartStyling(pos,0x1F);
                    m_textctrl->SetStyling(len,PP_LINK_STYLE);

                }
            }
            col+=start+len;
            text=text.Mid(start+len);
        }
        lineno++;
    }
}

void PipedProcessCtrl::OnSize(wxSizeEvent& event)
{
    m_textctrl->SetSize(event.GetSize());
}


void PipedProcessCtrl::OnUserInput(wxKeyEvent& ke)
{
    if(m_dead)
    {
        ke.Skip();
        return;
    }
    //todo: if user presses navigational keys accept them as navigational (also copy/paste shortcuts?)
    char kc1[2];
    kc1[0]=ke.GetKeyCode()%256;
    kc1[1]=0;
    if(kc1[0]=='\r')
        kc1[0]='\n';
    wxChar kc2=ke.GetUnicodeKey();
    wxString buf(kc2);
    if (!ke.ControlDown() && !ke.AltDown())
        if (ke.GetKeyCode()<WXK_START ||
           ke.GetKeyCode()>WXK_COMMAND)
        {
            m_ostream->Write(&kc1,1);
            m_textctrl->AppendText(kc2);
            m_textctrl->GotoPos(m_textctrl->GetLength());
            return;
        }

    ke.Skip();
}



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// HyperLinking ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



void PipedProcessCtrl::OnDClick(wxMouseEvent &e)
{
    //First retrieve the link text
    if(!m_linkclicks)
        return;
    long pos=m_textctrl->PositionFromPoint(e.GetPosition());
    int style=m_textctrl->GetStyleAt(pos);
    if((style&PP_LINK_STYLE)!=PP_LINK_STYLE)
        return; //didn't click a link
    long start=pos;
    while(start>0)
    {
        style=m_textctrl->GetStyleAt(start-1);
        if((style&PP_LINK_STYLE)!=PP_LINK_STYLE)
            break;
        start--;
    }
    long end=pos;
    while(end<m_textctrl->PositionFromLine(m_textctrl->GetLineCount()-1))
    {
        style=m_textctrl->GetStyleAt(end+1);
        if((style&PP_LINK_STYLE)!=PP_LINK_STYLE)
            break;
        end++;
    }
    wxString text=m_textctrl->GetTextRange(start,end+1);

    //retrieve the file and line number parts of the link
    wxRegEx re(m_linkregex,wxRE_ADVANCED|wxRE_NEWLINE);
    wxString file;
    long line;
    if(!re.Matches(text))
        return;
    size_t ind,len;
    re.GetMatch(&ind,&len,0);
    if(re.GetMatch(&ind,&len,1))
        file=text.Mid(ind,len);
    else
        file=wxEmptyString;
    if(re.GetMatch(&ind,&len,3))
        text.Mid(ind,len).ToLong(&line);
    else
        line=0;

    //open the file in the editor
    wxFileName f(file);
    if(f.FileExists())
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->Open(f.GetFullPath());
        if (ed)
        {
            ed->Show(true);
//            if (!ed->GetProjectFile())
//                ed->SetProjectFile(f.GetFullPath());
            ed->GotoLine(line - 1, false);
            if(line>0)
                if(!ed->HasBookmark(line - 1))
                    ed->ToggleBookmark(line -1);
        }
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
