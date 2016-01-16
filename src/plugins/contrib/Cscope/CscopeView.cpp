#include "CscopeView.h"
#include "CscopeTab.h"

CscopeView::CscopeView(CscopeConfig *cfg)
:Logger(),
m_pPanel(0),
m_cfg(cfg)
{
}

CscopeView::~CscopeView()
{}

wxWindow* CscopeView::CreateControl(wxWindow* parent)
{
    if ( m_pPanel)
        return m_pPanel;

    m_pPanel = new CscopeTab( parent, m_cfg );
    return m_pPanel;
}


CscopeTab* CscopeView::GetWindow(){ return m_pPanel; }

void CscopeView::Append( const wxString &  /*msg*/,  Logger::level  /*lv*/)
{}
void CscopeView::Clear()
{
    if ( m_pPanel )
        m_pPanel->Clear();
}
void CscopeView::CopyContentsToClipboard(bool selectionOnly)
{
    if ( m_pPanel )
        m_pPanel->CopyContentsToClipboard(selectionOnly);
}
