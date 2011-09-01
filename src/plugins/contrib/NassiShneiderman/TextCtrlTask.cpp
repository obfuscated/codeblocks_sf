#include "TextCtrlTask.h"
#include "TextGraph.h"
#include "NassiView.h"
#include "NassiFileContent.h"
#include "commands.h"
#include "TextCtrl.h"


TextCtrlTask::TextCtrlTask(NassiView *view, NassiFileContent *nfc, TextCtrl *textctrl, TextGraph *textgraph, const wxPoint &pos):
    m_done(false),
    m_textctrl(textctrl),
    m_view(view),
    m_nfc(nfc),
    m_textgraph(textgraph)
{
    //ctor
    if ( m_textctrl && m_textgraph )
    {
        m_textgraph->SetEditTask(this);

        m_textctrl->Clear();
        UpdateSize();

        wxTextAttr attr(*wxBLACK, wxNullColour,
            m_textgraph->m_nmbr%2 ?
                m_view->GetSourceFont() : m_view->GetCommentFont()
        );

        m_textctrl->SetDefaultStyle( attr );
        m_textctrl->WriteText( *(m_textgraph->m_str) );
        m_textctrl->SetStyle(0, m_textctrl->GetLastPosition(), attr);

        TextCtrlTask::EditPosition xypos = GetEditPosition(pos);
        long p = m_textctrl->XYToPosition(xypos.column, xypos.line);
        m_textctrl->SetInsertionPoint(p);
        m_textctrl->ShowPosition(p);
        m_textctrl->SetOrigSize( wxSize(m_textgraph->GetWidth(), m_textgraph->GetTotalHeight()) );

        if ( !m_textctrl->IsShown() )
            m_textctrl->Show(true);
        m_textctrl->SetFocus();
    }
    else
        CloseTask();
}

TextCtrlTask::~TextCtrlTask()
{
    //dtor
    if ( m_textgraph )
        m_textgraph->ClearEditTask();
    m_textgraph = 0;

    if ( m_textctrl && m_textctrl->IsShown())
        m_textctrl->Show(false);
}
wxCursor TextCtrlTask::Start()
{
    wxCursor newcursor(wxCURSOR_ARROW);
    return newcursor;
}

// events from window:
void TextCtrlTask::OnMouseLeftDown(wxMouseEvent & /*event*/, const wxPoint & /*position*/)
{
    //LoggerSingleton::exemplar()->Log( _T("Left Down"));

    m_nfc->GetCommandProcessor()->Submit(
        new NassiEditTextCommand(
            m_nfc,
            m_textgraph->m_brick,
            m_textctrl->GetValue(),
            m_textgraph->m_nmbr
        )
    );
    CloseTask();
}
void TextCtrlTask::OnMouseRightDown(wxMouseEvent& /*event*/, const wxPoint& /*position*/)
{
    CloseTask();
}

void TextCtrlTask::OnKeyDown(wxKeyEvent & /*event*/){}
void TextCtrlTask::OnChar(wxKeyEvent & /*event*/){}

bool TextCtrlTask::HasSelection()const
{
    if ( !Done() && m_textctrl )
    {
        long from, to;
        m_textctrl->GetSelection(&from, &to);
        if ( from != to )
            return true;
    }
    return false;
}
bool TextCtrlTask::CanPaste()const
{
    if ( !Done() && m_textctrl )
        return m_textctrl->CanPaste();
    return false;
}
void TextCtrlTask::DeleteSelection()
{
    if ( !Done() && m_textctrl )
    {
        long from, to;
        m_textctrl->GetSelection(&from, &to);
        if ( from != to )
            m_textctrl->Replace(from, to, _T(""));
    }
}
void TextCtrlTask::Copy()
{
    if ( !Done() && m_textctrl )
        m_textctrl->Copy();
}
void TextCtrlTask::Paste()
{
    if ( !Done() && m_textctrl )
        m_textctrl->Paste();
}
void TextCtrlTask::Cut()
{
    if ( !Done() && m_textctrl )
        m_textctrl->Cut();
}

TextCtrlTask::EditPosition TextCtrlTask::GetEditPosition(const wxPoint &pos)
{
    TextCtrlTask::EditPosition epos;
    for ( wxUint32 n = 0 ; n < m_textgraph->linesizes.size() ; n++ )
    {
        wxPoint offset = m_textgraph->lineoffsets[n] + m_textgraph->offset;
        wxPoint size = m_textgraph->linesizes[n];
        if ( pos.x > offset.x && pos.x < offset.x + size.x &&
             pos.y > offset.y && pos.y < offset.y + size.y )
        {
            wxArrayInt widths = m_textgraph->linewidths[n];
            wxUint32 k;
            for ( k = 0 ; k < widths.GetCount()-1; ++k )
                if ( pos.x <= offset.x + (widths[k] + widths[k+1] )/2 )
                    break;
            epos.line = n;
            epos.column = k;
        }
    }
    return epos;
}

void TextCtrlTask::UpdateSize()
{
    if ( Done() || !m_textgraph ) return;

    m_view->MoveTextCtrl( m_textgraph->offset );
}

/// /////////////////////////////////


void TextCtrlTask::OnMouseRightUp(wxMouseEvent& /*event*/, const wxPoint& /*position*/){}
void TextCtrlTask::OnMouseLeftUp(wxMouseEvent& /*event*/, const wxPoint& /*position*/){}
HooverDrawlet *TextCtrlTask::OnMouseMove(wxMouseEvent& /*event*/, const wxPoint& /*position*/)
{
    return 0;
}

void TextCtrlTask::UnlinkTextGraph(){m_textgraph = 0;}
void TextCtrlTask::CloseTask()
{
    m_done = true;
    if ( m_textctrl )
        m_textctrl->Show(false);
}

