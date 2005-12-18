/** xtra_classes
  * Use this to add your own "helper" classes to use in the program.
  * Panels, windows, toolbars, etc.
  */

#include "sdk_precomp.h"
#include "xtra_classes.h"
#include "configmanager.h"

/** A simple panel with included wxSplitterWindow and sizer. Nothing more
  *
  */

IMPLEMENT_DYNAMIC_CLASS(wxSplitPanel, wxPanel)
BEGIN_EVENT_TABLE(wxSplitPanel,wxPanel)
// We connect the events dynamically. Look in Create().
//    EVT_UPDATE_UI(-1,wxSplitPanel::OnUpdateUI)
END_EVENT_TABLE()


bool wxSplitPanel::Create(wxWindow* parent, wxWindowID id,
                          const wxPoint& pos, const wxSize& size,
                          long style, const wxString& name,const wxString configname,int defaultsashposition)
{
    if(!wxPanel::Create(parent,id,pos,size,style,name))
        return false;
    m_splitter = new wxSplitterWindow(this,-1);
    m_splitter->SetMinimumPaneSize(20);
    m_SplitterConfig = configname;
    m_defaultsashposition = defaultsashposition;
    m_IniSashPos = m_SplitterConfig.IsEmpty() ? -1 : Manager::Get()->GetConfigManager(_T("splitpanel"))->ReadInt(configname, m_defaultsashposition);
    m_lastsashposition = 0;
    m_sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(m_sizer);
    m_sizer->Add( m_splitter, 1, wxGROW, 0 );

    Connect(id, -1, wxEVT_UPDATE_UI,
            (wxObjectEventFunction)(wxEventFunction)(wxUpdateUIEventFunction)
            &wxSplitPanel::OnUpdateUI);

    return true;
}

void wxSplitPanel::RefreshSplitter(int idtop,int idbottom)
{
    if(!this)
        return;
    if(!m_splitter)
        return;
    int sashPosition = 0;
    wxWindow *thewin = 0, *w1 = 0, *w2 = 0;
    wxWindow *topwin = m_splitter->FindWindowById(idtop);
    wxWindow *bottomwin = m_splitter->FindWindowById(idbottom);

    m_splitter->Freeze();
    {
        wxUpdateUIEvent tmpevent;
        this->OnUpdateUI(tmpevent); // Refresh the recorded splitter position
    }

    if(topwin && topwin->IsShown() && bottomwin && bottomwin->IsShown())
    {
        w1 = m_splitter->GetWindow1();
        w2 = 0;
        if(m_splitter->IsSplit())
            w2 = m_splitter->GetWindow2();

        sashPosition = m_lastsashposition;
        if(sashPosition <= 0)
        {
            m_IniSashPos = m_SplitterConfig.IsEmpty() ? -1 : Manager::Get()->GetConfigManager(_T("splitpanel"))->ReadInt(m_SplitterConfig, m_defaultsashposition);
            sashPosition =  (m_IniSashPos > 0) ? m_IniSashPos : m_lastsashposition;
        }
        if(w1!=topwin && w2!=bottomwin)
            m_splitter->SplitHorizontally(topwin,bottomwin);
        if(sashPosition > 0)
            m_splitter->SetSashPosition(sashPosition);
    }
    else
    {
        if (topwin && topwin->IsShown())
            thewin = topwin;
        else if (bottomwin && bottomwin->IsShown())
            thewin = bottomwin;
        m_splitter->Initialize(thewin);
        m_splitter->SetSashPosition(0);
    }

    m_splitter->Thaw();
}

void wxSplitPanel::OnUpdateUI(wxUpdateUIEvent& event)
{
    if(!this || !m_splitter)
    {
        event.Skip();
        return;
    }
    if(!m_splitter->IsSplit())
    {
        event.Skip();
        return;
    }
    int sashpos = m_splitter->GetSashPosition();
    if(sashpos > 0)
    {
        if (sashpos > 20)
            m_lastsashposition = sashpos;
        else
            m_lastsashposition = 20;
    }
    event.Skip();
}

wxSplitPanel::~wxSplitPanel()
{
	if (!m_SplitterConfig.IsEmpty() && m_lastsashposition > 20)
        Manager::Get()->GetConfigManager(_T("splitpanel"))->Write(m_SplitterConfig, m_lastsashposition);
    m_splitter=0;
    SetSizer(0L);
}
