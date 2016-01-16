#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "parseactions.h"
#include "bricks.h"


AddSpace_to_collector::AddSpace_to_collector(wxString &str)
:   m_str(str){}
//void AddSpace_to_collector::operator() (iterator_t first, iterator_t const& last)const
void AddSpace_to_collector::operator() ( wxChar const * /*first*/, wxChar const * /*last*/) const
{
    m_str += _T(" ");
}

AddNewline_to_collector::AddNewline_to_collector(wxString &str)
    : m_str(str){}
//void AddNewline_to_collector::operator() (iterator_t first, iterator_t const& last)const
void AddNewline_to_collector::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    m_str += _T("\n");
}

RemoveDoubleSpaces_from_collector::RemoveDoubleSpaces_from_collector(wxString &str)
:   m_str(str){}
//void RemoveDoubleSpaces_from_collector::operator() (iterator_t first, iterator_t const& last)const
void RemoveDoubleSpaces_from_collector::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    while ( m_str.Find(_T("\n ")) != -1 || m_str.Find(_T("\n\t")) != -1 )
    {
        m_str.Replace(_T("\n "),  _T("\n"));
        m_str.Replace(_T("\n\t"), _T("\n"));
    }
}

comment_collector::comment_collector(wxString &str):
        m_str(str) {}
//void comment_collector::operator() (const iterator_t first, iterator_t const& last)const
void comment_collector::operator() ( wxChar const *first, wxChar const *last ) const
{

    if ( m_str.length() > 1 && m_str[m_str.length()-1] != _T('\n') )
        m_str += _T("\n");

    wxString str;
    while ( first != last )
        str += (char)*first++;

    if ( str.StartsWith(_T("/*")) )
        m_str += str.Mid( 2, str.Len()-4 );
    else
    {
        if ( str.StartsWith(_T("//")) )
            m_str += str.Mid(2, str.Len() - 3);
        else
            m_str += str;
    }
    wxInt32 n;
    while ( (n = m_str.Find(_T("\r"))) != wxNOT_FOUND )
    {
        m_str = m_str.Mid(0, n) + m_str.Mid(n +1);
    }
    while ( (n = m_str.Find(_T("\n\n"))) != wxNOT_FOUND )
    {
        m_str.Replace(_T("\n\n"), _T("\n"), true);
    }
    //wxMessageBox( m_str, _T("Comment:"));
}
MoveComment::MoveComment(wxString &src, wxString &dst):
	m_src(src), m_dst(dst) {}
void MoveComment::operator()( wxChar const *first, wxChar const *last ) const
{
	if (!m_src.IsEmpty())
    {
        m_dst = m_src;
        m_src.clear();
    }
}
instr_collector::instr_collector(wxString &str):m_str(str) {}
//void instr_collector::operator() (iterator_t first, iterator_t const& last) const
void instr_collector::operator() ( wxChar const *first, wxChar const *last ) const
{
    while ( first != last )
        m_str += *first++; // (char*)
    remove_carrage_return();
}
void instr_collector::operator() (const wxChar *ch)const
{
    m_str += *ch;
    remove_carrage_return();
}
void instr_collector::operator() (const wxChar ch)const
{
    m_str += ch;
    remove_carrage_return();
}
//void instr_collector::operator() (iterator_t  first)const
//void instr_collector::operator() ( char const *first ) const
//{
//    m_str += (char)*first;
//    remove_carrage_return();
//}
void instr_collector::remove_carrage_return(void) const
{
    wxInt32 n;
    while ( (n = m_str.Find(_T("\r"))) != -1 )
    {
        m_str = m_str.Mid(0, n) + m_str.Mid(n +1);
        //m_str = m_str.BeforeFirst(_T("\r")) + m_str.AfterFirst(_T("\r"));
    }
}

CreateNassiBreakBrick::CreateNassiBreakBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick)
 :   m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiBreakBrick::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiBreakBrick::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    m_brick->SetNext( new NassiBreakBrick() );
    m_brick = m_brick->GetNext();
    m_brick->SetTextByNumber(m_c_str, 0);
    m_brick->SetTextByNumber(_T("break;")/*m_s_str*/, 1);
    m_c_str.clear();
    m_s_str.clear();
}

CreateNassiContinueBrick::CreateNassiContinueBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiContinueBrick::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiContinueBrick::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    m_brick->SetNext( new NassiContinueBrick() );
    m_brick = m_brick->GetNext();
    m_brick->SetTextByNumber(m_c_str, 0);
    m_brick->SetTextByNumber(_T("continue;")/*m_s_str*/, 1);
    m_c_str.clear();
    m_s_str.clear();
}


CreateNassiReturnBrick::CreateNassiReturnBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiReturnBrick::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiReturnBrick::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    m_s_str.Trim(true);
    m_s_str.Trim(false);
    m_brick->SetNext( new NassiReturnBrick() );
    m_brick = m_brick->GetNext();
    m_brick->SetTextByNumber(m_c_str, 0);
    m_brick->SetTextByNumber( m_s_str , 1);
    m_c_str.clear();
    m_s_str.clear();
}

CreateNassiInstructionBrick::CreateNassiInstructionBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiInstructionBrick::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiInstructionBrick::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    /// add the brick only if the strings are empty
    if ( !(m_c_str.IsEmpty() && m_s_str.IsEmpty()) )
    {
        m_brick->SetNext( new NassiInstructionBrick() );
        m_brick = m_brick->GetNext();
        m_brick->SetTextByNumber(m_c_str, 0);
        m_brick->SetTextByNumber(m_s_str, 1);
        m_c_str.clear();
        m_s_str.clear();
    }
}


CreateNassiBlockBrick::CreateNassiBlockBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
    //void operator()(iterator_t first, iterator_t const& last)const
void CreateNassiBlockBrick::operator()(const wxChar /*ch*/)const
{
    DoCreate();
}
void CreateNassiBlockBrick::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    DoCreate();
}
void CreateNassiBlockBrick::DoCreate() const
{
    NassiBrick *brick = new NassiBlockBrick();
    m_brick->SetNext( brick );
    brick->SetTextByNumber(m_c_str, 0);
    brick->SetTextByNumber(m_s_str, 1);
    m_c_str.clear();
    m_s_str.clear();
    m_brick = brick;

    brick = new NassiInstructionBrick();
    brick->SetTextByNumber( _("created by CreateNassiBlockBrick"), 0);
    m_brick->SetChild(brick);
    m_brick = brick;
}

CreateNassiBlockEnd::CreateNassiBlockEnd(wxString &c_str, wxString &s_str, NassiBrick *&brick)
      :   m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiBlockEnd::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiBlockEnd::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{  DoEnd();  }
void CreateNassiBlockEnd::operator()(const wxChar /*ch*/)const
{  DoEnd();  }
void CreateNassiBlockEnd::DoEnd() const
{
    NassiBrick *parent, *child;
    while ( m_brick->GetPrevious() )
        m_brick = m_brick->GetPrevious();
    parent = m_brick->GetParent();
    child = m_brick->GetNext();
    m_brick->SetNext((NassiBrick *)0);
    m_brick->SetParent((NassiBrick *)0);
    m_brick->SetPrevious((NassiBrick *)0);

    parent->SetChild(child);
    delete m_brick;
    m_brick = parent;

    wxString str = *(parent->GetTextByNumber(0));
    str += m_c_str;
    parent->SetTextByNumber(str, 0);
    str = *(parent->GetTextByNumber(1));
    str += m_s_str;
    parent->SetTextByNumber(str, 1);
    m_c_str.clear();
    m_s_str.clear();
}

CreateNassiIfBrick::CreateNassiIfBrick(wxString &c_str, wxString &tc_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_tc_str(tc_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiIfBrick::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiIfBrick::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    //wxMessageDialog dlg(0, _T("open begin"), _T("test"));
    //dlg.ShowModal();
    NassiBrick *brick = new NassiIfBrick();
    m_brick->SetNext( brick );
    brick->SetTextByNumber(m_c_str, 0);
    brick->SetTextByNumber(m_s_str, 1);
    brick->SetTextByNumber(m_tc_str, 2);
    m_c_str.clear();
    m_s_str.clear();
    m_tc_str.clear();
    m_brick = brick;

    brick = new NassiInstructionBrick();
    m_brick->SetChild(brick, 0);
    m_brick = brick;
    //wxMessageDialog dlg2(0, _T("open end"), _T("test"));
    //dlg2.ShowModal();
}
CreateNassiIfThenText::CreateNassiIfThenText(wxString &c_str, wxString &s_str, NassiBrick *&brick)
    :m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiIfThenText::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiIfThenText::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    NassiBrick *parent;
    parent = m_brick->GetParent();

    parent->SetTextByNumber(m_c_str, 2);
    parent->SetTextByNumber(m_s_str, 3);
    m_c_str.clear();
    m_s_str.clear();
}

CreateNassiIfEndIfClause::CreateNassiIfEndIfClause(NassiBrick *&brick)
    :   m_brick(brick){}
//void CreateNassiIfEndIfClause::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiIfEndIfClause::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    //wxMessageDialog dlg(0, _T("close begin"), _T("test"));
    //dlg.ShowModal();
    NassiBrick *parent, *child, *block;
    while ( m_brick->GetPrevious() )
        m_brick = m_brick->GetPrevious();
    parent = m_brick->GetParent();
    child = m_brick->GetNext();
    m_brick->SetNext((NassiBrick *)0);
    //m_brick->SetParent((NassiBrick *)0);
    m_brick->SetPrevious((NassiBrick *)0);
    parent->SetChild(child,0 );
    delete m_brick;

    if ( child && child->IsBlock() )
    {
        block = child;
        child = block->GetChild();

        block->SetChild((NassiBrick *)0);
        //block->SetParent((NassiBrick *)0);
        block->SetPrevious((NassiBrick *)0);
        delete block;
        parent->SetChild(child,0 );
    }
    m_brick = parent; // if block
    //wxMessageDialog dlg2(0, _T("colse end"), _T("test"));
    //dlg2.ShowModal();
}

CreateNassiIfBeginElseClause::CreateNassiIfBeginElseClause(wxString &c_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiIfBeginElseClause::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiIfBeginElseClause::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    m_brick->SetTextByNumber(m_c_str, 4);
    m_brick->SetTextByNumber(m_s_str, 5);
    m_c_str.clear();
    m_s_str.clear();

    NassiBrick *brick = new NassiInstructionBrick();
    m_brick->SetChild(brick, 1);
    m_brick = brick;
}

CreateNassiIfEndElseClause::CreateNassiIfEndElseClause(NassiBrick *&brick)
        :   m_brick(brick){}
//void CreateNassiIfEndElseClause::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiIfEndElseClause::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    NassiBrick *parent, *child, *block;
    while ( m_brick->GetPrevious() )
        m_brick = m_brick->GetPrevious();
    parent = m_brick->GetParent();
    child = m_brick->GetNext();
    m_brick->SetNext((NassiBrick *)0);
    //m_brick->SetParent((NassiBrick *)0);
    m_brick->SetPrevious((NassiBrick *)0);
    parent->SetChild(child, 1);
    delete m_brick;
    if ( child && child->IsBlock() )
    {
        block = child;
        child = block->GetChild();

        block->SetChild((NassiBrick *)0);
        block->SetPrevious((NassiBrick *)0);
        delete block;
        parent->SetChild(child, 1);
    }
    m_brick = parent;
}

CreateNassiForBrick::CreateNassiForBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_s_str(s_str), m_brick(brick){}
//void CreateNassiForBrick::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiForBrick::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    NassiBrick *brick = new NassiForBrick();
    m_brick->SetNext( brick );
    brick->SetTextByNumber(m_c_str, 0);
    brick->SetTextByNumber(m_s_str, 1);
    m_c_str.clear();
    m_s_str.clear();
    m_brick = brick;

    brick = new NassiInstructionBrick();
    m_brick->SetChild(brick);
    m_brick = brick;
}

CreateNassiWhileBrick::CreateNassiWhileBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_s_str(s_str), m_brick(brick){}
//void CreateNassiWhileBrick::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiWhileBrick::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    NassiBrick *brick = new NassiWhileBrick();
    m_brick->SetNext( brick );
    brick->SetTextByNumber(m_c_str, 0);
    brick->SetTextByNumber(m_s_str, 1);
    m_c_str.clear();
    m_s_str.clear();
    m_brick = brick;

    brick = new NassiInstructionBrick();
    m_brick->SetChild(brick);
    m_brick = brick;
}

CreateNassiForWhileEnd::CreateNassiForWhileEnd(NassiBrick *&brick)
      :   m_brick(brick){}
//void CreateNassiForWhileEnd::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiForWhileEnd::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    NassiBrick *parent, *child, *block;
    while ( m_brick->GetPrevious() )
        m_brick = m_brick->GetPrevious();
    parent = m_brick->GetParent();
    child = m_brick->GetNext();
    m_brick->SetNext((NassiBrick *)0);
    m_brick->SetPrevious((NassiBrick *)0);
    parent->SetChild(child);
    delete m_brick;
    if ( child && child->IsBlock() )
    {
        block = child;
        child = block->GetChild();
        block->SetChild((NassiBrick *)0);
        //block->SetParent((NassiBrick *)0);
        block->SetPrevious((NassiBrick *)0);
        delete block;
        parent->SetChild(child, 0);
    }
    m_brick = parent;
}


CreateNassiDoWhileBrick::CreateNassiDoWhileBrick(NassiBrick *&brick)
      :   m_brick(brick){}
//void CreateNassiDoWhileBrick::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiDoWhileBrick::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    NassiBrick *brick = new NassiDoWhileBrick();
    m_brick->SetNext(brick);
    //brick->SetTextByNumber(m_c_str, 0);
    //brick->SetTextByNumber(m_s_str, 1);
    //m_c_str.clear();
    //m_s_str.clear();
    m_brick = brick;
    brick = new NassiInstructionBrick();
    m_brick->SetChild(brick);
    m_brick = brick;
}
CreateNassiDoWhileEnd::CreateNassiDoWhileEnd(wxString &c_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiDoWhileEnd::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiDoWhileEnd::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    NassiBrick *parent, *child, *block;
    while ( m_brick->GetPrevious() )
        m_brick = m_brick->GetPrevious();
    parent = m_brick->GetParent();
    child = m_brick->GetNext();
    m_brick->SetNext((NassiBrick *)0);
    m_brick->SetPrevious((NassiBrick *)0);
    parent->SetChild(child);
    delete m_brick;
    if ( child && child->IsBlock() )
    {
        block = child;
        child = block->GetChild();
        block->SetChild((NassiBrick *)0);
        block->SetPrevious((NassiBrick *)0);
        delete block;
        parent->SetChild(child);
    }
    m_brick = parent;
    parent->SetTextByNumber(m_c_str, 0);
    parent->SetTextByNumber(m_s_str, 1);
    m_c_str.clear();
    m_s_str.clear();
}



CreateNassiSwitchBrick::CreateNassiSwitchBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiSwitchBrick::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiSwitchBrick::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    NassiBrick *brick = new NassiSwitchBrick();
    m_brick->SetNext(brick);
    brick->SetTextByNumber(m_c_str, 0);
    brick->SetTextByNumber(m_s_str, 1);
    m_c_str.clear();
    m_s_str.clear();
    m_brick = brick;
    brick = new NassiInstructionBrick();
    m_brick->AddChild(0);
    m_brick->SetChild(brick, 0);
    m_brick = brick;
    //wxMessageBox(_T("Switch brick"), _T("Created:"));
}

CreateNassiSwitchEnd::CreateNassiSwitchEnd( NassiBrick *&brick)
      :   m_brick(brick){}
//void CreateNassiSwitchEnd::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiSwitchEnd::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    wxInt32 n;
    NassiBrick *parent, *child;//, *block;

    while ( m_brick->GetPrevious() )
        m_brick = m_brick->GetPrevious();
    parent = m_brick->GetParent();
    n = parent->GetChildCount();
    child = m_brick->GetNext();
    m_brick->SetNext((NassiBrick *)0);
    m_brick->SetPrevious((NassiBrick *)0);
    parent->SetChild(child, n-1);
    delete m_brick;
    m_brick = parent;
    m_brick->RemoveChild(0);
    //wxMessageBox(_T("Switch End"), _T("Created:"));
}

CreateNassiSwitchChild::CreateNassiSwitchChild(wxString &c_str, wxString &s_str, NassiBrick *&brick)
     :   m_c_str(c_str), m_s_str(s_str),m_brick(brick){}
//void CreateNassiSwitchChild::operator()(iterator_t first, iterator_t const& last)const
void CreateNassiSwitchChild::operator() ( wxChar const * /*first*/, wxChar const * /*last*/ ) const
{
    NassiBrick *parent, *child, *brick;
    wxInt32 n;
    while ( m_brick->GetPrevious() )
        m_brick = m_brick->GetPrevious();
    parent = m_brick->GetParent();
    n = parent->GetChildCount(); //0..n-1
    brick = parent->GetChild(n-1);

    child = brick->GetNext();
    brick->SetNext((NassiBrick *)0);
    brick->SetParent((NassiBrick *)0);
    brick->SetPrevious((NassiBrick *)0);
    parent->SetChild(child, n-1);

    parent->AddChild(n);
    parent->SetTextByNumber(m_c_str, 2 * (n+1));
    parent->SetTextByNumber(m_s_str, 2 * (n+1)  +  1);
    m_c_str.clear();
    m_s_str.clear();

    parent->SetChild(brick, n);
    m_brick = brick;
    //wxMessageBox(_T("Switch Child"), _T("Created:"));
}

/// //////////////////////////////////////////////////////////////////////////


