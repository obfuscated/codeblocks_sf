/** xtra_classes
  * Use this to add your own "helper" classes to use in the program. 
  * Panels, windows, toolbars, etc.
  */

#include "xtra_classes.h"
#include "configmanager.h"

/** A simple panel with included wxSplitterWindow and sizer. Nothing more
  *
  */

IMPLEMENT_DYNAMIC_CLASS(wxSplitPanel, wxPanel)

bool wxSplitPanel::Create(wxWindow* parent, wxWindowID id,
                          const wxPoint& pos, const wxSize& size,
                          long style, const wxString& name)
{
    if(!wxPanel::Create(parent,id,pos,size,style,name))
        return false;
    m_splitter = new wxSplitterWindow(this,-1);
    m_splitter->SetMinimumPaneSize(20);
    m_sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(m_sizer);
    m_sizer->Add( m_splitter, 1, wxGROW, 0 );
    return true;
}                          

void wxSplitPanel::RefreshSplitter(int idtop,int idbottom,int sashPosition)
{
    wxWindow *topwin = m_splitter->FindWindowById(idtop);
    wxWindow *bottomwin = m_splitter->FindWindowById(idbottom);
    int oldsash = m_splitter->GetSashPosition();
    m_splitter->Freeze();
    if(topwin && topwin->IsShown() && bottomwin && bottomwin->IsShown())
    {
        m_splitter->SplitHorizontally(topwin,bottomwin);
    }
    else
    {
        wxWindow* thewin = 0;
        if (topwin && topwin->IsShown())
            thewin = topwin;
        else if (bottomwin && bottomwin->IsShown())
            thewin = bottomwin;
        m_splitter->Initialize(thewin);
    }
    if (sashPosition)
    {
        m_splitter->SetSashPosition(sashPosition);
    }
    else
    {
        if (!m_SplitterConfig.IsEmpty())
            m_splitter->SetSashPosition(ConfigManager::Get()->Read(m_SplitterConfig, (long int)150));
        else
            m_splitter->SetSashPosition(oldsash);
    }
    m_splitter->Thaw();
}

wxSplitPanel::~wxSplitPanel()
{
	if (!m_SplitterConfig.IsEmpty())
        ConfigManager::Get()->Write(m_SplitterConfig, m_splitter->GetSashPosition());
    delete m_splitter;
    m_splitter=0;
    SetSizer(0L);
}

