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

#include <wx/txtstrm.h>
#include "commands.h"


/// this command will insert a brick before any other Brick
/// it will check if:
/// - the brick has a brick before,
/// - the Brick is the first of a list of child (a leaf in composite), or
/// - the brick is the first of a diagram.

NassiInsertBrickBefore::NassiInsertBrickBefore(NassiFileContent *nfc, NassiBrick *brick, NassiBrick *InsrBrick)
        :wxCommand(true, _("Insert Brick") )
{
    m_nfc = nfc;
    m_brick = brick;
    m_done = false;
    m_nbrk = InsrBrick;
    m_nlbrk = m_nbrk;
    while ( m_nlbrk->GetNext() )
        m_nlbrk = m_nlbrk->GetNext();
}

NassiInsertBrickBefore::~NassiInsertBrickBefore(void)
{
    if ( !m_done )
    {
        if ( m_nbrk != (NassiBrick *)0 )
            delete m_nbrk;
    }
}

bool NassiInsertBrickBefore::Do(void)
{
    if ( m_done )
        return(false);
    if (!m_brick)
        return(false);
    if ( !m_nbrk )
        return(false);
    NassiBrick *prev = m_brick->GetPrevious();
    if ( prev )
    {
        prev->SetNext(m_nbrk);
        //m_nbrk->SetNext(m_brick);
        m_nlbrk->SetNext(m_brick);
        m_nbrk->SetParent((NassiBrick *)0);
        m_done = true;
        m_nfc->Modify(true);
        m_nfc->NotifyObservers();
        return(true);
    }
    NassiBrick *par = m_brick->GetParent();
    if ( par )
    {
        //wxMessageDialog dlg(0, _T("has Parent"), _T("Info"));
        //tdlg.ShowModal();
        for ( wxUint32 n = 0 ; n < par->GetChildCount() ; n++ )
        {
            if ( par->GetChild(n) == m_brick ) // both pointers point to the same brick
            {
                par->SetChild(m_nbrk, n);
                //m_nbrk->SetNext(m_brick);
                m_nlbrk->SetNext(m_brick);
                m_nbrk->SetPrevious((NassiBrick *)0);
                m_nbrk->SetParent(par);
                m_brick->SetParent((NassiBrick *)0);
                m_done = true;
                m_nfc->Modify(true);
                m_nfc->NotifyObservers();
                return(true);
            }
        }
        return(false);
    }
    if ( m_nfc->GetFirstBrick() == m_brick ) // no previos and no parent
    {
        m_nfc->SetFirstBrick(m_nbrk);
        //m_nbrk->SetNext(m_brick);
        m_nlbrk->SetNext(m_brick);
        m_nbrk->SetParent((NassiBrick *)0);
        m_nbrk->SetPrevious((NassiBrick *)0);
        //m_brick->SetParent((NassiBrick *)0);
        m_done = true;
        m_nfc->Modify(true);
        m_nfc->NotifyObservers();
        return(true);
    }

    return(false);
}
bool NassiInsertBrickBefore::Undo(void)
{
    if ( !m_done)
        return(false);
    if ( !m_brick )
        return(false);
    NassiBrick *prev = m_nbrk->GetPrevious();
    if ( prev )
    {
        prev->SetNext(m_brick);
        m_brick->SetParent((NassiBrick *)0);

        //m_nbrk->SetNext((NassiBrick *)0);
        m_nlbrk->SetNext((NassiBrick *)0);
        m_nbrk->SetPrevious((NassiBrick *)0);
        m_nbrk->SetParent((NassiBrick *)0);

        m_done = false;
        m_nfc->Modify(true);
        m_nfc->NotifyObservers();
        return(true);
    }
    NassiBrick *par = m_nbrk->GetParent();
    if ( par )
    {
        for ( wxUint32 n = 0 ; n < par->GetChildCount() ; n++ )
        {
            if ( par->GetChild(n) == m_nbrk )
            {
                par->SetChild(m_brick,n);
                m_brick->SetPrevious((NassiBrick *)0);

                m_nbrk->SetPrevious((NassiBrick *)0);
                m_nbrk->SetParent((NassiBrick *)0);
                //m_nbrk->SetNext((NassiBrick *)0);
                m_nlbrk->SetNext((NassiBrick *)0);

                m_done = false;
                m_nfc->Modify(true);
                m_nfc->NotifyObservers();
                return(true);
            }
        }
        return(false);
    }
    if ( m_nfc->GetFirstBrick() == m_nbrk )
    {
        m_nfc->SetFirstBrick(m_brick);
        m_brick->SetPrevious((NassiBrick *)0);
        m_brick->SetParent((NassiBrick *)0);

        //m_nbrk->SetNext((NassiBrick *)0);
        m_nlbrk->SetNext((NassiBrick *)0);
        m_nbrk->SetPrevious((NassiBrick *)0);
        m_nbrk->SetParent((NassiBrick *)0);
        m_done = false;
        m_nfc->Modify(true);
        m_nfc->NotifyObservers();
        return(true);
    }
    return(false);
}


/// this command will insert a brick after another brick
/// nothing to check
NassiInsertBrickAfter::NassiInsertBrickAfter( NassiFileContent *nfc, NassiBrick *brick, NassiBrick *InsrBrick)
        : wxCommand(true, _("Insert Brick") )
{
    m_nfc = nfc;
    m_brick = brick;
    //m_tool = tool;
    m_done = false;
    m_nbrk = InsrBrick;
    m_nlbrk = m_nbrk;
    while ( m_nlbrk->GetNext() != (NassiBrick *)0 )
        m_nlbrk = m_nlbrk->GetNext();
}
NassiInsertBrickAfter::~NassiInsertBrickAfter(void)
{
    if ( !m_done )
    {
        if ( m_nbrk != (NassiBrick *)0 )
            delete m_nbrk;
    }
}
bool NassiInsertBrickAfter::Do(void)
{
    if ( m_done )
        return(false);
    if ( !m_brick )
        return(false);
    NassiBrick *next = m_brick->GetNext();
    if ( next != (NassiBrick *)0 )
    {
        m_brick->SetNext(m_nbrk);
        //m_nbrk->SetNext(next);
        m_nlbrk->SetNext(next);
        m_nbrk->SetParent((NassiBrick *)0);
    }
    else
    {
        m_brick->SetNext(m_nbrk);
        //m_nbrk->SetNext((NassiBrick *)0);
        m_nlbrk->SetNext((NassiBrick *)0);
        m_nbrk->SetParent((NassiBrick *)0);
    }
    m_done = true;
    m_nfc->Modify(true);
    m_nfc->NotifyObservers();
    return(true);
}
bool NassiInsertBrickAfter::Undo(void)
{
    if ( !m_done )
        return(false);
    if ( !m_brick )
        return(false);
    //m_brick->SetNext(m_nbrk->GetNext());
    m_brick->SetNext(m_nlbrk->GetNext());
    //m_nbrk->SetNext((NassiBrick *)0);
    m_nlbrk->SetNext((NassiBrick *)0);
    m_done = false;
    m_nfc->Modify(true);
    m_nfc->NotifyObservers();
    return(true);
}

///this command will insert a brick as the first of a diagram
NassiInsertFirstBrick::NassiInsertFirstBrick( NassiFileContent *nfc, NassiBrick *InsrBrick,  bool CanUndoIn)
        :wxCommand(CanUndoIn, _("Insert Brick") )
{
    m_nfc = nfc;
    //m_tool = tool;
    m_done = false;
    m_nbrk = InsrBrick;
    m_nlbrk = m_nbrk;
    while ( m_nlbrk->GetNext() )
        m_nlbrk = m_nlbrk->GetNext();
}
NassiInsertFirstBrick::~NassiInsertFirstBrick(void)
{
    if ( !m_done )
    {
        if ( m_nbrk != (NassiBrick *)0 )
            delete m_nbrk;
    }
}
bool NassiInsertFirstBrick::Do(void)
{
    if ( m_done )
        return(false);
    if ( m_nbrk == (NassiBrick *)0 )
        return(false);
    m_nbrk->SetPrevious((NassiBrick *)0);
    m_nbrk->SetParent((NassiBrick *)0);
    m_nfc->SetFirstBrick(m_nbrk);
    //m_nbrk->SetNext((NassiBrick *)0);
    m_nbrk = (NassiBrick *)0;
    m_done = true;
    m_nfc->Modify(true);
    m_nfc->NotifyObservers();
    return(true);
}
bool NassiInsertFirstBrick::Undo(void)
{
    if ( !m_done )
        return(false);
    m_nbrk = m_nfc->GetFirstBrick();
    m_nfc->SetFirstBrick((NassiBrick *)0);
    //m_nbrk->SetNext((NassiBrick *)0);
    //m_nbrk->SetPrevious((NassiBrick *)0);
    //m_nbrk->SetParent((NassiBrick *)0);
    m_done = false;
    m_nfc->Modify(true);
    m_nfc->NotifyObservers();
    return(true);
}

///this command will add a child a new case of a switch command
NassiAddChildIndicatorCommand::NassiAddChildIndicatorCommand( NassiFileContent *nfc, NassiBrick *brick, NassiBrick *InsrBrick, wxUint32 ChildAddNumber, wxString _strc, wxString _strs )
    :wxCommand(true, _("Insert Brick") ),
    m_nfc(nfc),
    m_brick(brick),
    m_done(false),
    m_ChildAddNumber(ChildAddNumber),
    m_nbrk(InsrBrick),
    m_nlbrk(InsrBrick),
    strc(_strc),
    strs(_strs)
{
    if ( m_nlbrk )
        while ( m_nlbrk->GetNext() )
            m_nlbrk = m_nlbrk->GetNext();
}
NassiAddChildIndicatorCommand::~NassiAddChildIndicatorCommand(void)
{
    if ( !m_done && m_nbrk )
            delete m_nbrk;
}
bool NassiAddChildIndicatorCommand::Do(void)
{
    if ( m_done || !m_brick || (m_ChildAddNumber > m_brick->GetChildCount()) ) return false;

    m_brick->AddChild( m_ChildAddNumber );
    m_brick->SetChild( m_nbrk, m_ChildAddNumber );

    m_brick->SetTextByNumber( strc , 2*(m_ChildAddNumber+1) );
    m_brick->SetTextByNumber( strs , 2*(m_ChildAddNumber+1)+1 );
    m_done = true;
    m_nfc->Modify(true);
    m_nfc->NotifyObservers();
    return true ;
}
bool NassiAddChildIndicatorCommand::Undo(void)
{
    if ( !m_done ||  !m_brick || m_ChildAddNumber >= m_brick->GetChildCount() ) return false;
    m_brick->SetChild((NassiBrick *)0, m_ChildAddNumber);
    m_brick->RemoveChild(m_ChildAddNumber);
    m_done = false;
    m_nfc->Modify(true);
    m_nfc->NotifyObservers();
    return true;
}

NassiInsertChildBrickCommand::NassiInsertChildBrickCommand( NassiFileContent *nfc, NassiBrick *brick, NassiBrick *InsrBrick, wxUint32 ChildNumber):
    wxCommand(true, _("Insert Brick") ),
    m_nfc(nfc),
    m_brick(brick),
    m_done(false),
    m_nbrk(InsrBrick),
    m_nlbrk(InsrBrick),
    m_childNumber(ChildNumber)
{
    if ( m_nlbrk )
        while ( m_nlbrk->GetNext() )
            m_nlbrk = m_nlbrk->GetNext();
}
NassiInsertChildBrickCommand::~NassiInsertChildBrickCommand(void)
{
    if ( !m_done && m_nbrk != (NassiBrick *)0 )
        delete m_nbrk;
}
bool NassiInsertChildBrickCommand::Do(void)
{
    if ( m_done || !m_brick || m_childNumber >= m_brick->GetChildCount() )
        return false;
    m_brick->SetChild( m_nbrk, m_childNumber );
    m_nbrk->SetParent(m_brick);
    m_nlbrk->SetNext((NassiBrick *)0);
    m_nbrk->SetPrevious((NassiBrick *)0);

    m_done = true;
    m_nfc->Modify(true);
    m_nfc->NotifyObservers();
    return(true);
}
bool NassiInsertChildBrickCommand::Undo(void)
{
    if ( ! m_done )
        return(false);
    if ( !m_brick )
        return(false);
    if ((m_childNumber >= m_brick->GetChildCount())  )
        return(false);
    m_brick->SetChild((NassiBrick *)0, m_childNumber);
    m_done = false;
    m_nfc->Modify(true);
    m_nfc->NotifyObservers();
    return(true);
}

/// used command to change text
NassiEditTextCommand::NassiEditTextCommand( NassiFileContent *nfc, NassiBrick *brick, const wxString &str, wxInt32 nmbr)
        :wxCommand(true, _("Change Text") )
{
    m_nfc = nfc;
    m_str = str;
    m_brick = brick;
    m_nmbr = nmbr;
}

NassiEditTextCommand::~NassiEditTextCommand(void)
{}

bool NassiEditTextCommand::Do(void)
{
    if ( m_brick )
    {
        wxString str;
        str = *(m_brick->GetTextByNumber(m_nmbr));
        m_brick->SetTextByNumber(m_str,m_nmbr);
        m_str = str;
        m_nfc->Modify(true);
        m_nfc->NotifyObservers();
        return true;
    }
    return false;
}

bool NassiEditTextCommand::Undo(void)
{
    return(Do());
}

/// will delete a brick (and the following ,first till (including) last )
NassiDeleteCommand::NassiDeleteCommand( NassiFileContent *nfc, NassiBrick *first, NassiBrick *last)
        :   wxCommand(true, _("Delete selected bricks"))
{
    m_nfc = nfc;
    m_first = first;
    m_last = last;
    m_done = false;
    //m_isdndmove = isDnDMoveInOneView;
    parPrev = (NassiBrick*)0;
    m_childnmbr = -1;
    strc.Empty();
    strs.Empty();
    firstCall = true;
}
NassiDeleteCommand::~NassiDeleteCommand(void)
{
    if ( m_done )
    {
        m_last->SetNext((NassiBrick *)0);
        if ( m_first )
            delete m_first;
    }
}
bool NassiDeleteCommand::Do(void)
{
    if ( !m_done && m_first && m_last )
    {
        if ( m_first->GetPrevious() )
        {
            m_childnmbr = -1;
            parPrev = m_first->GetPrevious();
            parPrev->SetNext( m_last->GetNext() );
            m_last->SetNext((NassiBrick *)0);

            m_nfc->Modify(true);
            m_nfc->NotifyObservers();
            m_done = true;
            return(true);
        }
        else
        {
            if ( m_first->GetParent() )
            {
                parPrev = m_first->GetParent();
                for ( wxUint32 n = 0 ; n < parPrev->GetChildCount() ; n++ )
                {
                    if ( parPrev->GetChild(n) == m_first)
                    {
                        m_childnmbr = n;
                        strc = *(parPrev->GetTextByNumber( 2*(n+1) ));
                        strs = *(parPrev->GetTextByNumber( 2*(n+1)+1 ));
                        parPrev->SetChild(m_last->GetNext(), n);
                        if ( ! m_last->GetNext() ) // for a switch Brick:
                        {
                            //parPrev->RemoveChild(n);
                        }
                        m_last->SetNext((NassiBrick *)0);
                        m_nfc->Modify(true);
                        m_nfc->NotifyObservers();
                        m_done = true;
                        return(true);
                    }
                }
            }
            else
            {
                m_childnmbr = -1;
                NassiBrick *newfirst = m_last->GetNext();
                if ( newfirst )
                {
                    newfirst->SetPrevious( (NassiBrick *)0 );
                    newfirst->SetParent( (NassiBrick *)0 );
                }
                m_nfc->SetFirstBrick( newfirst );
                m_last->SetNext((NassiBrick *)0);
                m_nfc->Modify(true);
                m_nfc->NotifyObservers();
                m_done = true;
                return(true);
            }
        }
    }
    m_done = false;
    return false ;
}
bool NassiDeleteCommand::Undo(void)
{
    if ( m_done )
    {
        if ( m_first->GetPrevious() && parPrev )
        {
            m_last->SetNext(parPrev->GetNext());
            parPrev->SetNext(m_first);
            m_nfc->Modify(true);
            m_nfc->NotifyObservers();
            m_done = false;
            return(true);
        }
        else if ( m_first->GetParent() && parPrev )
        {
            if ( ! m_last->GetNext() )
            {
                //parPrev->AddChild(m_childnmbr);
            }
            m_last->SetNext(parPrev->GetChild(m_childnmbr));
            parPrev->SetChild( m_first, m_childnmbr);
            parPrev->SetTextByNumber(strc, 2*(m_childnmbr+1) );
            parPrev->SetTextByNumber(strs, 2*(m_childnmbr+1)+1 );
            m_nfc->Modify(true);
            m_nfc->NotifyObservers();
            m_done = false;
            return(true);
        }
        else
        {
            NassiBrick *oldfirst = m_nfc->GetFirstBrick();
            m_last->SetNext(oldfirst);
            m_nfc->SetFirstBrick(m_first);
            m_nfc->Modify(true);
            m_nfc->NotifyObservers();
            m_done = false;
            return(true);
        }
    }
    return( false );
}

NassiDeleteChildRootCommand::NassiDeleteChildRootCommand(
NassiFileContent *nfc, NassiBrick *parent, wxInt32 childNumber):
    wxCommand(true, _("Delete selected bricks")),
    m_nfc(nfc),
    m_parent(parent),
    m_done(false),
    m_delcmd(0),
    m_childNmbr(childNumber)
{
    m_strC = *parent->GetTextByNumber( 2*(childNumber+1) );
    m_strS = *parent->GetTextByNumber( 2*(childNumber+1)+1 );
}
NassiDeleteChildRootCommand::~NassiDeleteChildRootCommand(void)
{
    if ( m_delcmd )
        delete m_delcmd;
}
bool NassiDeleteChildRootCommand::Do(void)
{
    if ( !m_done )
    {
        m_done = true;
        if ( !m_delcmd )
        {
            delete m_delcmd;
            m_delcmd = 0;
        }

        NassiBrick *first = m_parent->GetChild(m_childNmbr);
        NassiBrick *last = first;
        if ( first )
        {
            while ( last->GetNext() ) last = last->GetNext();
            m_delcmd = new NassiDeleteCommand( m_nfc, first, last);
            m_done = m_delcmd->Do();
        }

        m_parent->RemoveChild(m_childNmbr);
        m_nfc->Modify(true);
        m_nfc->NotifyObservers();
    }

    return m_done;
}
bool NassiDeleteChildRootCommand::Undo(void)
{
    if ( m_done )
    {
        m_parent->AddChild(m_childNmbr);
        m_parent->SetTextByNumber(m_strC, 2*( m_childNmbr + 1 ) );
        m_parent->SetTextByNumber(m_strS, 2*( m_childNmbr + 1 ) + 1 );
        m_nfc->Modify(true);
        m_nfc->NotifyObservers();
        m_done = false;
        if ( m_delcmd )
            return m_delcmd->Undo();
        return true;
    }
    return false;
}

/// moving a brick will add a copy of the brick to move and
/// deletes the original brick.
///////////////////////////
NassiMoveBrick::NassiMoveBrick( wxCommand *addCmd, wxCommand *delCmd)
    :wxCommand(true, _("Drag n Drop Brick"))
{
    m_delCmd = delCmd;
    m_addCmd = addCmd;
}
NassiMoveBrick::~NassiMoveBrick()
{
    if ( m_addCmd )
        delete m_addCmd;
    if ( m_delCmd )
        delete m_delCmd;
}
bool NassiMoveBrick::Do(void)
{
    if ( ! m_addCmd )
        return(false);
    bool res = true;
    if ( res )
        res = m_addCmd->Do();
    if ( m_delCmd )
        res = m_delCmd->Do();
    return( res );
}
bool NassiMoveBrick::Undo(void)
{
    if ( ! m_addCmd )
        return(false);
    bool res = true;
    if ( m_delCmd )
        res = m_delCmd->Undo();
    if ( res )
        res = m_addCmd->Undo();
    return( res );
}



/*NassiAddFunctionCommand::NassiAddFunctionCommand( NassiDocument *doc, wxInt32 nmbr):
    wxCommand(true, _("Add Function") )
{
    m_nfc = doc;
    functionNumber = nmbr;
}
NassiAddFunctionCommand::~NassiAddFunctionCommand(void)
{}

bool NassiAddFunctionCommand::Do(void)
{
    //m_nfc->AddFirstBrick(functionNumber, (NassiBrick *)0 );
    //m_nfc->Modify(true);
    //m_nfc->NotifyObservers();
    return true;
}

bool NassiAddFunctionCommand::Undo(void)
{
    //m_nfc->RemoveFirstBrick(functionNumber);
    //m_nfc->Modify(true);
    //m_nfc->NotifyObservers();
    return true;
}*/

/*NassiRenameFunctionCommand::NassiRenameFunctionCommand( NassiDocument *doc, wxInt32 nmbr, wxString newName)
    :wxCommand(true, _("Rename function"))
{
    m_nfc = doc;
    fname = newName;
    functionNumber = nmbr;
}
NassiRenameFunctionCommand::~NassiRenameFunctionCommand(void)
{}

bool NassiRenameFunctionCommand::Do(void)
{
    wxString oldName = m_nfc->GetFunctionName(functionNumber);
    wxInt32 n = oldName.Find('\n');
    if ( n != -1 )
    {
        fname += oldName.Mid(n+1);
    }
    m_nfc->SetFunctionName(functionNumber, fname);
    fname = oldName;
    m_nfc->Modify(true);
    m_nfc->NotifyObservers();
    return true;
}
bool NassiRenameFunctionCommand::Undo(void)
{
    return Do();
}
*/

/*NassiDeleteFunctionCommand::NassiDeleteFunctionCommand(NassiDocument *doc, wxInt32 nmbr)
    : wxCommand(true,  _("Remove Function"))
{
    m_nfc = doc;
    m_done = false;
    firstbrick = (NassiBrick *)0;
    m_nmbr = nmbr;
}

NassiDeleteFunctionCommand::~NassiDeleteFunctionCommand(void)
{
    if ( m_done )
    {
        if ( firstbrick )
            delete firstbrick;
    }
}
bool NassiDeleteFunctionCommand::Do(void)
{
    if ( ! m_done )
    {
        Name = m_nfc->GetFunctionName(m_nmbr);
        otherSrc = m_nfc->GetOtherText(m_nmbr);
        //firstbrick = m_nfc->RemoveFirstBrick(m_nmbr);
        m_done = true;
        m_nfc->Modify(true);
        m_nfc->NotifyObservers();
        return true;
    }
    else
        return false;
}

bool NassiDeleteFunctionCommand::Undo(void)
{
    if ( m_done )
    {
        //m_nfc->AddFirstBrick(m_nmbr, firstbrick);
        m_nfc->SetFunctionName(m_nmbr, Name);
        m_nfc->SetOtherText(m_nmbr, otherSrc);
        firstbrick = (NassiBrick *)0;
        m_done = false;
        m_nfc->Modify(true);
        m_nfc->NotifyObservers();
        return true;
    }
    else
        return false;
}*/




