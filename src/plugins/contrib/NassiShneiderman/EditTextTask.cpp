#include "EditTextTask.h"
#include "GraphBricks.h"
#include "NassiFileContent.h"
#include "commands.h"
#include "NassiView.h"


#include <wx/clipbrd.h>


#include "logger.h"


class TextCommandCollector: public wxCommand
{
public:
    TextCommandCollector();
    ~TextCommandCollector(void)
    {
        while ( cmds.size() )
        {
            delete cmds.back();
            cmds.pop_back();
        }
    }

    bool Do(void)
    {
        for ( wxUint32 n = 0 ; n < cmds.size() ; n++)
            if ( ! cmds[n]->Do() ) return false;
        return true;
    }
    bool Undo(void)
    {
        for ( wxUint32 n = 0 ; n < cmds.size() ; n++)
            if ( ! cmds[n]->Undo() ) return false;
        return true;
    }
    void Add(wxCommand *cmd){cmds.push_back(cmd);}
protected:
    std::vector<wxCommand *> cmds;
};



EditTextTask::EditTextTask(NassiView *view, NassiFileContent *nfc, TextGraph *textgraph, const wxPoint &pos):
    Task(),
    m_view(view),
    m_nfc(nfc),
    m_textgraph(textgraph)
{
    m_textgraph->SetEditTask(this);

    m_posa = m_posb = GetEditPosition(pos);
    this->MoveCaret();
    this->ShowCaret();
}

EditTextTask::~EditTextTask()
{
    if ( m_textgraph )
        m_textgraph->ClearEditTask();
    m_textgraph = NULL;
}
wxCursor EditTextTask::Start()
{
    m_done = false;
    wxCursor newcursor(wxCURSOR_IBEAM);
    return newcursor;
}
bool EditTextTask::Done()
{
    return m_done;
}
void EditTextTask::OnMouseLeftDown(wxMouseEvent &event, const wxPoint &position)
{
    if ( m_textgraph && m_textgraph->HasPoint(position) )
    {
        if ( event.ShiftDown() )
            m_posa = GetEditPosition(position);
        else
            m_posa = m_posb = GetEditPosition(position);
        this->MoveCaret();
        this->ShowCaret();
    }
    else
    {
//        GraphNassiBrick *gbrick = m_view->GetBrickAtPosition(position);
//        if ( !gbrick )
//        {
//            CloseTask();
//            return;
//        }
//        TextGraph *textgraph = gbrick->IsOverText(position);
//        if ( textgraph )
//        {
//            if ( m_textgraph ) m_textgraph->ClearEditTask();
//            m_textgraph = textgraph;
//            m_textgraph->SetEditTask(this); // to be sure he can remove itselfe if it gets deleted
//            m_posa = m_posb = GetEditPosition(position);
//            this->MoveCaret();
//            this->ShowCaret();
//        }
//        else
            CloseTask();
    }
}
HooverDrawlet *EditTextTask::OnMouseMove(wxMouseEvent &event, const wxPoint &position)
{

    if ( event.LeftIsDown() && m_textgraph && m_textgraph->HasPoint(position) )
    {
        m_posa = GetEditPosition(position);
        this->MoveCaret();
    }



    return NULL; // ok
}
void EditTextTask::OnKeyDown(wxKeyEvent &event)
{
    if ( m_done ) return;

    int code = event.GetKeyCode();
    event.Skip(true);

    if ( code == WXK_ESCAPE )
    {
        CloseTask();
        event.Skip(false);
    }
    if ( code == WXK_DELETE || code == WXK_NUMPAD_DELETE || code == WXK_BACK )
    {
        if ( HasSelection() )
            DeleteSelection();
        else
            if ( code == WXK_BACK )
                DeleteBack();
            else
                Delete();
        event.Skip(false);
    }
    if ( code == WXK_RETURN || code == WXK_NUMPAD_ENTER )
    {
        InsertText(_T("\n"));
        event.Skip(false);
    }
    if ( code == WXK_TAB )
    {
        InsertText(_T("\t"));
        event.Skip(false);
    }
}
void EditTextTask::OnChar(wxKeyEvent &event)
{
    if ( m_done ) return;
}
void EditTextTask::DeleteBack()
{
    m_posb = m_posa;
    DecrementPosition(m_posa);
    if(HasSelection())DeleteSelection();

}
void EditTextTask::Delete()
{
    m_posb = m_posa;
    IncrementPosition(m_posb);
    if(HasSelection())DeleteSelection();
}
void EditTextTask::IncrementPosition(EditTextTask::EditPosition &pos)
{
    if ( !m_textgraph ) return;
    // check end of line
    if ( pos.column > (m_textgraph->linewidths[pos.line]).size() )
    {
        //check last line
        if ( pos.line + 1 < m_textgraph->lineoffsets.size()  )
        {
            pos.line++;
            pos.column = 0;
        }
    }
    else
        pos.column ++;
}
void EditTextTask::DecrementPosition(EditTextTask::EditPosition &pos)
{
    if (!m_textgraph) return;

    if ( pos.column == 0 )
    {
        if ( pos.line > 0 )
        {
            pos.line--;
            pos.column = (m_textgraph->linewidths[pos.line]).size() -1;
        }
    }
    else
        pos.column--;
}
void EditTextTask::OnMouseRightDown(wxMouseEvent &event, const wxPoint &position)
{
    CloseTask();
}
void EditTextTask::OnMouseRightUp(wxMouseEvent& event, const wxPoint &position)
{
    CloseTask();
}
void EditTextTask::OnMouseLeftUp(wxMouseEvent &event, const wxPoint &position){}
bool EditTextTask::HasSelection()
{
    return m_textgraph && m_posa != m_posb;
}
bool EditTextTask::CanPaste()
{
    bool res = false;
    if ( wxTheClipboard->Open() )
    {
        res = wxTheClipboard->IsSupported( wxDF_TEXT );
        wxTheClipboard->Close();
    }
    return res;
}
void EditTextTask::DeleteSelection()
{
    if ( !m_textgraph || !HasSelection() ) return;

    wxString s( *( m_textgraph->m_brick->GetTextByNumber(m_textgraph->m_nmbr) ) );
    wxString res;

    wxInt32 pos;
    wxUint32 line = 0;
    EditTextTask::EditPosition posa, posb;
    posa = m_posa;
    posb = m_posb;

    if ( posa.line > m_posb.line || ( posa.line == posb.line && posb.column < posa.column ))
    {
        posa = m_posb;
        posb = m_posa;
    }
    do{
        pos = s.Find('\n');
        wxString te = s;
        if ( pos != -1 )
        {
            te = s.SubString(0, pos);
            s = s.SubString(pos+1, s.Length() );
        }
        if ( line < posa.line || line > posb.line )
            res += te;
        else if (line == posa.line && line == posb.line )
            res += te.SubString( 0, posa.column -1) + te.SubString( posb.column, te.Length() );
        else if ( line == posa.line )
            res += te.SubString(0, posa.column);
        else if ( line == posb.line )
            res += te.SubString( posb.column+1, te.Length() );
    }
    while ( pos != -1);
    m_posb = m_posa = posa;

    m_nfc->GetCommandProcessor()->Submit(
        new NassiEditTextCommand(
            m_nfc,
            m_textgraph->m_brick,
            res,
            m_textgraph->m_nmbr
        )
    );

    // update position:
    if ( m_posa.line == m_posb.line )
    {
        if ( m_posa.column < m_posb.column )
            m_posb = m_posa;
        else
            m_posa = m_posb;
    }
    else
    {
        if ( m_posa.line < m_posb.line )
            m_posb = m_posa;
        else
            m_posa = m_posb;
    }

    this->MoveCaret();
}
wxString EditTextTask::GetSelectedText()
{
    if ( !m_textgraph || !HasSelection() ) return wxEmptyString;

    wxString s( *( m_textgraph->m_brick->GetTextByNumber(m_textgraph->m_nmbr) ) );
    wxString res;

    wxInt32 pos;
    wxUint32 line = 0;
    do{
        pos = s.Find('\n');
        wxString te = s;
        if ( pos != -1 )
        {
            te = s.SubString(0, pos-1);
            s = s.SubString(pos+1, s.Length() );
        }
        if ( line >= m_posa.line && line <= m_posb.line )
        {
            if ( m_posa.line == m_posb.line )
            {
                res = te.SubString(m_posa.column, m_posb.column);
                break;
            }
            else
            {
                if ( line == m_posa.line )
                    res = te.SubString(m_posa.column, te.Length());
                else
                {
                    res += _T('\n');
                    if ( line == m_posb.line)
                    {
                        res += te.SubString(0, m_posb.column);
                        break;
                    }
                    else
                        res += te;
                }
            }
        }
        else if (line > m_posb.line )
            break;

        ++line;
    }
    while ( pos != -1);

    return res;
}
void EditTextTask::InsertText(const wxString &str) ///
{
    if ( !m_textgraph ) return;

    if ( HasSelection() ) DeleteSelection();

    /// ///////////////////////////////////////////////
    // generate the new string:
    wxString text;

    wxString s( *( m_textgraph->m_brick->GetTextByNumber(m_textgraph->m_nmbr) ) );

    wxString insstr = str;
    insstr.Replace(_T("\t"), _T("    "));
    insstr.Replace(_T("\r"), _T("\n"));

    wxInt32 pos;
    wxUint32 line = 0;
    do{
        pos = s.Find('\n');
        wxString te = s;
        if ( pos != -1 )
        {
            te = s.SubString(0, pos);
            s = s.SubString(pos+1, s.Length() );
        }
        if ( line == m_posa.line )
        {
            text += te.SubString(0, m_posa.column-1);
            text += insstr;
            text += te.SubString( m_posa.column, te.Length() );
        }
        else
            text += te;
        ++line;
    }
    while ( pos != -1);

    // update the position
    // cals size of inserted text (lines and columns)
    line = 0;
    s = insstr;
    do
    {
        pos = s.Find('\n');
        if ( pos != -1 )
        {
            line++;
            s = s.SubString( pos+1, s.Length() );
        }
    }
    while ( pos != -1);
    // update local position variable
    if ( line )
    {
        m_posa.line += line;
        m_posa.column = s.Length();
    }
    else
        m_posa.column += s.Length();
    m_posb = m_posa;

    m_nfc->GetCommandProcessor()->Submit( new NassiEditTextCommand(
        m_nfc,
        m_textgraph->m_brick,
        text,
        m_textgraph->m_nmbr
    ));
}
void EditTextTask::Copy()
{
    if (wxTheClipboard->Open())
    {
// This data objects are held by the clipboard,
// so do not delete them in the app.
        wxTheClipboard->SetData( new wxTextDataObject(GetSelectedText()) );
        wxTheClipboard->Close();
    }
}
void EditTextTask::Paste()
{
    if ( wxTheClipboard->Open() )
    {
        if ( wxTheClipboard->IsSupported( wxDF_TEXT ) )
        {
            wxTextDataObject data;
            wxTheClipboard->GetData( data );
            InsertText( data.GetText() );
        }
        wxTheClipboard->Close();
    }
}
void EditTextTask::UnlinkTextGraph()
{
    m_textgraph = NULL;
}
void EditTextTask::CloseTask()
{
    m_done = true;
    if ( m_view->IsCaretVisible() ) m_view->ShowCaret(false);
}
EditTextTask::EditPosition EditTextTask::GetEditPosition(const wxPoint &pos)
{
    EditTextTask::EditPosition epos;
    for ( wxUint32 n = 0 ; n < m_textgraph->linesizes.size() ; n++ )
    {
        wxPoint offset = m_textgraph->lineoffsets[n] + m_textgraph->offset;
        wxPoint size = m_textgraph->linesizes[n];
        if ( pos.x > offset.x && pos.x < offset.x + size.x &&
             pos.y > offset.y && pos.y < offset.y + size.y )
        {
            wxArrayInt widths = m_textgraph->linewidths[n];
            wxUint32 k;
            wxCoord dx = 0;
            for ( k = 0 ; k < widths.GetCount()-1; ++k )
                if ( pos.x <= offset.x + (widths[k] + widths[k+1] )/2 )
                    break;
            epos.line = n;
            epos.column = k;
        }
    }
    return epos;
}
void EditTextTask::UpdateSize()
{
    this->MoveCaret();
}
void EditTextTask::MoveCaret()
{
    if ( m_textgraph )
    {
        wxPoint offset = m_textgraph->offset;
        if ( m_posa.line >= m_textgraph->lineoffsets.size() )
            m_posa.line = m_textgraph->lineoffsets.size() - 1;
        offset += m_textgraph->lineoffsets[m_posa.line];
        if ( m_posa.column >= (m_textgraph->linewidths[m_posa.line]).size() )
            m_posa.column = (m_textgraph->linewidths[m_posa.line]).size() -1;
        offset.x += (m_textgraph->linewidths[m_posa.line])[m_posa.column];
        m_view->MoveCaret(offset);
    }
}
void EditTextTask::ShowCaret()
{
    if ( !m_view->IsCaretVisible() )
        m_view->ShowCaret(true);
}
void EditTextTask::HideCaret()
{
    if ( m_view->IsCaretVisible() )
        m_view->ShowCaret(false);
}
bool EditTextTask::CanCopy(){ return false; }
bool EditTextTask::CanCut(){ return false; }
void EditTextTask::Cut(){}
