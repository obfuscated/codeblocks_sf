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

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!

#include <wx/txtstrm.h>
#include <wx/arrstr.h>

#include "bricks.h"
#include "NassiBrickVisitor.h"


enum {
    NASSI_BRICK_SELECT      =   0,
    NASSI_BRICK_INSTRUCTION =   1,
    NASSI_BRICK_CONTINUE    =   2,
    NASSI_BRICK_BREAK       =   3,
    NASSI_BRICK_RETURN      =   4,
    NASSI_BRICK_WHILE       =   5,
    NASSI_BRICK_DOWHILE     =   6,
    NASSI_BRICK_FOR         =   7,
    NASSI_BRICK_BLOCK       =   8,
    NASSI_BRICK_IF          =   9,
    NASSI_BRICK_SWITCH      =  10,
    NASSI_BRICK_ESC         =  11,
    NASSI_BRICK_PASTE       = 100,
    NASSI_BRICK_EDIT_TEXT   = 101
};

NassiBrick::NassiBrick():
    previous(0),
    next(0),
    parent(0),
    Source(_T("")),
    Comment(_T(""))
{}
NassiBrick::~NassiBrick()
{
    if ( next != (NassiBrick *)0 )
    {
        //wxMessageBox(_T("deleting next"));
        delete next;
        next = (NassiBrick *)0;
    }
    //wxMessageBox(_T("destructing brick"));
}

NassiBrick *NassiBrick::SetNext(NassiBrick *nex)
{
    NassiBrick *tmp;
    tmp = next;
    next = nex;
    if ( next )
        next->SetPrevious(this);
    return( tmp );
}
NassiBrick *NassiBrick::SetPrevious(NassiBrick *prev)
{
    NassiBrick *tmp;
    tmp = previous;
    previous = prev;
    if ( previous )
        parent = (NassiBrick *)0;
    return( tmp );
}
NassiBrick *NassiBrick::SetParent(NassiBrick *par)
{
    NassiBrick *tmp = parent;
    parent = par;
    if ( parent )
        previous = (NassiBrick *)0;
    return tmp;
}
NassiBrick *NassiBrick::SetChild(NassiBrick *brick, wxUint32 /*n*/)
{
	delete brick;
    return 0;
}

bool NassiBrick::IsParent(NassiBrick *brick)
{
    if ( !parent ) return false;
    if ( brick == parent ) return true;
    return parent->IsParent(brick);
}
wxUint32 NassiBrick::GetLevel()
{
    if ( parent ) return parent->GetLevel() + 1;
    if ( previous ) return previous->GetLevel();
    return 0;
}

bool NassiBrick::IsOlderSibling(NassiBrick *brick)
{
    if ( previous == brick) return true;
    if ( previous && previous->IsOlderSibling(brick) ) return true;
    return false;
}
bool NassiBrick::IsYoungerSibling(NassiBrick *brick)
{
    if (next == brick ) return true;
    if ( next && next->IsYoungerSibling(brick) ) return true;
    return false;
}

bool NassiBrick::IsSibling(NassiBrick *brick)
{
    if ( this == brick )    return true;
    if ( IsOlderSibling(brick) ) return true;
    if ( IsYoungerSibling(brick) ) return true;
    return false;
}

//static
NassiBrick *NassiBrick::SetData(wxInputStream &stream)
{
    wxUint32 n;
    NassiBrick *brick;
    wxTextInputStream inp(stream);
    inp >> n;
    switch ( n )
    {
        case NASSI_BRICK_INSTRUCTION:
            brick = new NassiInstructionBrick();
            break;
        case NASSI_BRICK_CONTINUE:
            brick = new NassiContinueBrick();
            break;
        case NASSI_BRICK_BREAK:
            brick = new NassiBreakBrick();
            break;
        case NASSI_BRICK_RETURN:
            brick = new NassiReturnBrick();
            break;
        case NASSI_BRICK_WHILE:
            brick = new NassiWhileBrick();
            break;
        case NASSI_BRICK_DOWHILE:
            brick = new NassiDoWhileBrick();
            break;
        case NASSI_BRICK_FOR:
            brick = new NassiForBrick();
            break;
        case NASSI_BRICK_BLOCK:
            brick = new NassiBlockBrick();
            break;
        case NASSI_BRICK_IF:
            brick = new NassiIfBrick();
            break;
        case NASSI_BRICK_SWITCH:
            brick = new NassiSwitchBrick();
            break;
        case NASSI_BRICK_ESC:
        default:
            brick = (NassiBrick *)0;
    }
    if ( brick )
        brick->Deserialize(stream);
    return ( brick );
}
wxOutputStream &NassiBrick::SerializeString(wxOutputStream &stream, wxString str)
{
    wxTextOutputStream out(stream);
    wxArrayString arstr;
    while ( !str.IsEmpty() )
    {
        wxInt32 n = str.Find('\n');
        if ( n != -1 )
        {
            arstr.Add(str.SubString(0, n-1));
            str = str.SubString(n+1, str.Length() );
        }
        else
        {
            arstr.Add(str);
            str.Empty();
        }
    }
    out << static_cast<wxUint32>(arstr.GetCount()) << _T('\n');
    for ( wxUint32 n = 0 ; n < arstr.GetCount() ; n++ )
    {
        out << arstr.Item(n) << _T('\n');
    }
    return stream;
}
wxInputStream &NassiBrick::DeserializeString(wxInputStream &stream, wxString &str)
{
    wxTextInputStream inp(stream);

    str.Empty();

    wxUint32 n;
    inp >> n;
    for ( wxUint32 k = 0 ; k < n ; ++k )
    {
        wxString s;
        //inp >> s;
        s = inp.ReadLine();
        if ( k != 0 )
            str += _T('\n');
        str += s;
    }
    return stream;
}

NassiInstructionBrick::NassiInstructionBrick():
        NassiBrick()
{}
NassiInstructionBrick::NassiInstructionBrick(const NassiInstructionBrick &rhs)
        :NassiBrick()
{
    Comment = wxString(*(rhs.GetTextByNumber(0)));
    Source = wxString(*(rhs.GetTextByNumber(1)));
    if ( rhs.GetNext() )
        SetNext( rhs.GetNext()->Clone()  );
}
NassiInstructionBrick::~NassiInstructionBrick()
{}
NassiBrick *NassiInstructionBrick::SetChild(NassiBrick *brick, wxUint32 /*n*/)
{
    delete brick;
    return 0;
}
void NassiInstructionBrick::SetTextByNumber(const  wxString &str, wxUint32 n)
{
    if ( n == 0 )
        Comment = str;
    else
        Source = str;
}
const wxString *NassiInstructionBrick::GetTextByNumber(wxUint32 n)const
{
    if ( n == 0 )
        return &Comment;
    else
        return &Source;
}
void NassiInstructionBrick::accept(NassiBrickVisitor *visitor)
{
    visitor->Visit(this);
}

//public: virtual
wxOutputStream &NassiInstructionBrick::Serialize(wxOutputStream &stream)
{
    wxTextOutputStream out(stream);
    out << NASSI_BRICK_INSTRUCTION << _T('\n');
    for (wxUint32 n = 0 ; n < 2 ; n++ )
        SerializeString(stream,*GetTextByNumber(n));
    if ( GetNext() != (NassiBrick *)0 )
        GetNext()->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');
    return stream;
}
wxInputStream &NassiInstructionBrick::Deserialize(wxInputStream &stream)
{
    wxTextInputStream inp(stream);
    wxString str;
    for (wxUint32 n = 0 ; n < 2 ; n++ )
    {
        DeserializeString(stream, str);
        SetTextByNumber(str, n);
    }
    SetNext(NassiBrick::SetData(stream));
    return stream;
}


NassiBreakBrick::NassiBreakBrick():
        NassiBrick()
{}
NassiBreakBrick::NassiBreakBrick(const NassiBreakBrick &rhs)
        :NassiBrick()
{
    Comment = *(rhs.GetTextByNumber(0));
    Source = *(rhs.GetTextByNumber(1));
    if ( rhs.GetNext() )
        SetNext( rhs.GetNext()->Clone()  );
}
NassiBreakBrick::~NassiBreakBrick()
{}
NassiBrick *NassiBreakBrick::SetChild(NassiBrick *brick, wxUint32 /*n*/)
{
    delete brick;
    return 0;
}
void NassiBreakBrick::SetTextByNumber(const  wxString &str, wxUint32 n)
{
    if ( n == 0 )
        Comment = str;
    else
        Source = str;
}
const wxString *NassiBreakBrick::GetTextByNumber(wxUint32 n)const
{
    if ( n == 0 )
        return &Comment;
    else
        return &Source;
}
void NassiBreakBrick::accept(NassiBrickVisitor *visitor)
{
    visitor->Visit(this);
}

//public: virtual
wxOutputStream &NassiBreakBrick::Serialize(wxOutputStream &stream)
{
    wxTextOutputStream out(stream);
    out << NASSI_BRICK_BREAK << _T('\n');

    SerializeString(stream, *(GetTextByNumber(0)));

    if ( GetNext() != (NassiBrick *)0 )
        GetNext()->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');
    return stream;
}
wxInputStream &NassiBreakBrick::Deserialize(wxInputStream &stream)
{
    wxTextInputStream inp(stream);

    wxString str;
    DeserializeString(stream, str);
    SetTextByNumber(str, 0);

    SetNext(NassiBrick::SetData(stream));
    return stream;
}



NassiContinueBrick::NassiContinueBrick():
        NassiBrick()
{}
NassiContinueBrick::NassiContinueBrick(const NassiContinueBrick &rhs)
        :NassiBrick()
{
    Comment = *(rhs.GetTextByNumber(0));
    Source = *(rhs.GetTextByNumber(1));
    if ( rhs.GetNext() )
        SetNext( rhs.GetNext()->Clone()  );
}
NassiContinueBrick::~NassiContinueBrick()
{}
NassiBrick *NassiContinueBrick::SetChild(NassiBrick *brick, wxUint32 /*n*/)
{
    delete brick;
    return 0;
}
void NassiContinueBrick::SetTextByNumber(const  wxString &str, wxUint32 n)
{
    if ( n == 0 )
        Comment = str;
    else
        Source = str;
}
const wxString *NassiContinueBrick::GetTextByNumber(wxUint32 n)const
{
    if ( n == 0 )
        return &Comment;
    else
        return &Source;
}
void NassiContinueBrick::accept(NassiBrickVisitor *visitor)
{
    visitor->Visit(this);
}
//public: virtual
wxOutputStream &NassiContinueBrick::Serialize(wxOutputStream &stream)
{
    wxTextOutputStream out(stream);
    out << NASSI_BRICK_CONTINUE << _T('\n');
    SerializeString(stream, *(GetTextByNumber(0)) );
    if ( GetNext() != (NassiBrick *)0 )
        GetNext()->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');
    return stream;
}
wxInputStream &NassiContinueBrick::Deserialize(wxInputStream &stream)
{
    wxTextInputStream inp(stream);
    wxString str;
    DeserializeString(stream, str);
    SetTextByNumber(str, 0);
    SetNext(NassiBrick::SetData(stream));
    return stream;
}




NassiReturnBrick::NassiReturnBrick():
        NassiBrick()
{}
NassiReturnBrick::NassiReturnBrick(const NassiReturnBrick &rhs)
        :NassiBrick()
{
    Comment = *(rhs.GetTextByNumber(0));
    Source = *(rhs.GetTextByNumber(1));
    if ( rhs.GetNext() )
        SetNext( rhs.GetNext()->Clone()  );
}
NassiReturnBrick::~NassiReturnBrick()
{}
NassiBrick *NassiReturnBrick::SetChild(NassiBrick *brick, wxUint32 /*n*/)
{
    delete brick;
    return 0;
}
void NassiReturnBrick::SetTextByNumber(const  wxString &str, wxUint32 n)
{
    if ( n == 0 )
        Comment = str;
    else
        Source = str;
}
const wxString *NassiReturnBrick::GetTextByNumber(wxUint32 n)const
{
    if ( n == 0 )
        return &Comment;
    else
        return &Source;
}
void NassiReturnBrick::accept(NassiBrickVisitor *visitor)
{
    visitor->Visit(this);
}

//public: virtual
wxOutputStream &NassiReturnBrick::Serialize(wxOutputStream &stream)
{
    wxTextOutputStream out(stream);
    out << NASSI_BRICK_RETURN << _T('\n');
    for (wxUint32 n = 0 ; n < 2 ; n++ )
        SerializeString(stream, *(GetTextByNumber(n)) );

    if ( GetNext() != (NassiBrick *)0 )
        GetNext()->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');
    return stream;
}
wxInputStream &NassiReturnBrick::Deserialize(wxInputStream &stream)
{
    wxTextInputStream inp(stream);
    wxString str;
    for (wxUint32 n = 0 ; n < 2 ; n++ )
    {
        DeserializeString(stream, str);
        SetTextByNumber(str, n);
    }
    SetNext(NassiBrick::SetData(stream));
    return stream;
}



NassiIfBrick::NassiIfBrick():
    NassiBrick(),
    TrueChild(0),
    FalseChild(0),
    TrueSourceText(_T("")),
    TrueCommentText(_T("")),
    FalseSourceText(_T("")),
    FalseCommentText(_T(""))
{}
NassiIfBrick::NassiIfBrick(const NassiIfBrick &rhs):
    NassiBrick(),
    TrueChild(0),
    FalseChild(0),
    TrueSourceText(_T("")),
    TrueCommentText(_T("")),
    FalseSourceText(_T("")),
    FalseCommentText(_T(""))

{
    for ( wxUint32 n=0 ; n < 6 ; n++ )
        SetTextByNumber(*(rhs.GetTextByNumber(n)), n);
    if ( rhs.GetChild(0) )
        TrueChild = rhs.GetChild(0)->Clone();
    if ( rhs.GetChild(1) )
        FalseChild = rhs.GetChild(1)->Clone();

    if ( rhs.GetNext() )
        SetNext( rhs.GetNext()->Clone()  );
}
NassiIfBrick::~NassiIfBrick()
{
    delete TrueChild;
    TrueChild = 0;
    delete FalseChild;
    FalseChild = 0;
}
NassiBrick *NassiIfBrick::GetChild(wxUint32 n) const
{
    if ( n == 0 )
        return(TrueChild);
    else
        return(FalseChild);
}
NassiBrick *NassiIfBrick::SetChild(NassiBrick *brick, wxUint32 n)
{
    if ( brick )
    {
        brick->SetParent(this);
        brick->SetPrevious((NassiBrick *)0);
    }
    NassiBrick *tmp;
    if ( n == 0 )
    {
        tmp = TrueChild;
        TrueChild = brick;
    }
    else
    {
        tmp = FalseChild;
        FalseChild = brick;
    }
    return(tmp);
}
void NassiIfBrick::SetTextByNumber(const  wxString &str, wxUint32 n)
{
    switch ( n )
    {
        case 0:
        Comment = str;
        break;
        case 1:
        Source = str;
        break;
        case 2:
        TrueCommentText = str;
        break;
        case 3:
        TrueSourceText = str;
        break;
        case 4:
        FalseCommentText = str;
        break;
        default:
        FalseSourceText = str;
    }
}
const wxString *NassiIfBrick::GetTextByNumber(wxUint32 n)const
{
    switch ( n )
    {
        case 0:
            return &Comment;
            break;
        case 1:
            return &Source;
            break;
        case 2:
            return &TrueCommentText;
            break;
        case 3:
            return &TrueSourceText;
            break;
        case 4:
            return &FalseCommentText;
            break;
        default:
            return &FalseSourceText;
    }
}
void NassiIfBrick::accept(NassiBrickVisitor *visitor)
{
    visitor->Visit(this);
}
//public: virtual
wxOutputStream &NassiIfBrick::Serialize(wxOutputStream &stream)
{
    wxTextOutputStream out(stream);
    out << NASSI_BRICK_IF << _T('\n');
    for ( wxUint32 n = 0 ; n < 6 ; n++ )
        SerializeString(stream, *(GetTextByNumber(n)) );
    for ( wxUint32 n = 0; n < 2 ; n++ )
    {
        if ( GetChild(n) != (NassiBrick *)0 )
            GetChild(n)->Serialize(stream);
        else
            out << NASSI_BRICK_ESC << _T('\n');
    }

    if ( GetNext() != (NassiBrick *)0 )
        GetNext()->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');
    return stream;
}
wxInputStream &NassiIfBrick::Deserialize(wxInputStream &stream)
{
    wxTextInputStream inp(stream);
    wxString str;
    for (wxUint32 n = 0 ; n < 6 ; n++ )
    {
        DeserializeString(stream, str);
        SetTextByNumber(str, n);
    }
    for ( wxUint32 n = 0; n < 2 ; n++ )
        SetChild(NassiBrick::SetData(stream), n);

    SetNext(NassiBrick::SetData(stream));
    return stream;
}




NassiForBrick::NassiForBrick()
    :NassiBrick(),
    Child(0),
    InitSourceText(_T("")),
    InitCommentText(_T("")),
    InstSourceText(_T("")),
    InstCommentText(_T(""))
{}
NassiForBrick::NassiForBrick(const NassiForBrick &rhs)
    :NassiBrick(),
    Child(0),
    InitSourceText(_T("")),
    InitCommentText(_T("")),
    InstSourceText(_T("")),
    InstCommentText(_T(""))
{
    Child = ( NassiBrick * ) 0;
    for ( wxUint32 n=0 ; n < 6 ; n++ )
        SetTextByNumber(*(rhs.GetTextByNumber(n)), n);

    if ( rhs.GetChild(0) )
        Child = rhs.GetChild(0)->Clone();
    if ( rhs.GetNext() )
        SetNext( rhs.GetNext()->Clone()  );
}

NassiForBrick::~NassiForBrick()
{
    delete Child;
    Child = (NassiBrick *)0;
}
NassiBrick *NassiForBrick::GetChild(wxUint32 /*n*/) const
{
    return Child;
}
NassiBrick *NassiForBrick::SetChild(NassiBrick *brick, wxUint32 /*n*/)
{
    if ( brick )
    {
        brick->SetParent(this);
        brick->SetPrevious((NassiBrick *)0);
    }
    NassiBrick *tmp;
    tmp = Child;
    Child = brick;
    return(tmp);
}
void NassiForBrick::SetTextByNumber(const  wxString &str, wxUint32 n)
{
    switch ( n )
    {
        case 0:
        Comment = str;
        break;
        case 1:
        Source = str;
        break;
        case 2:
        InitCommentText = str;
        break;
        case 3:
        InitSourceText = str;
        break;
        case 4:
        InstCommentText = str;
        break;
        default:
        InstSourceText = str;
    }
}
const wxString *NassiForBrick::GetTextByNumber(wxUint32 n)const
{
    switch ( n )
    {
        case 0:
        return &Comment;
        break;
        case 1:
        return &Source;
        break;
        case 2:
        return &InitCommentText;
        break;
        case 3:
        return &InitSourceText;
        break;
        case 4:
        return &InstCommentText;
        break;
        default:
        return &InstSourceText;
    }
}

void NassiForBrick::accept(NassiBrickVisitor *visitor)
{
    visitor->Visit(this);
}
wxOutputStream &NassiForBrick::Serialize(wxOutputStream &stream)
{
    wxTextOutputStream out(stream);
    out << NASSI_BRICK_FOR << _T('\n');
    for ( wxUint32 n = 0 ; n < 6 ; n++ )
        SerializeString(stream, *(GetTextByNumber(n)) );

    if ( GetChild(0) != (NassiBrick *)0 )
        GetChild(0)->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');

    if ( GetNext() != (NassiBrick *)0 )
        GetNext()->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');
    return stream;
}
wxInputStream &NassiForBrick::Deserialize(wxInputStream &stream)
{
    wxTextInputStream inp(stream);

    wxString str;
    for (wxUint32 n = 0 ; n < 6 ; n++ )
    {
        DeserializeString(stream, str);
        SetTextByNumber(str, n);
    }

    SetChild(NassiBrick::SetData(stream), 0);

    SetNext(NassiBrick::SetData(stream));
    return stream;
}

NassiBlockBrick::NassiBlockBrick():
    NassiBrick(),
    Child(0)
{}
NassiBlockBrick::NassiBlockBrick(const NassiBlockBrick &rhs):
        NassiBrick()
{
    Child = ( NassiBrick * ) 0;

    for ( wxUint32 n=0 ; n < 2 ; n++ )
        SetTextByNumber(*(rhs.GetTextByNumber(n)), n);

    if ( rhs.GetChild(0) )
        Child = rhs.GetChild(0)->Clone();
    if ( rhs.GetNext() )
        SetNext( rhs.GetNext()->Clone()  );
}
NassiBlockBrick::~NassiBlockBrick()
{
    delete Child;
    Child = 0;
}
NassiBrick *NassiBlockBrick::GetChild(wxUint32 /*n*/) const
{
    return Child;
}
NassiBrick *NassiBlockBrick::SetChild(NassiBrick *brick, wxUint32 /*n*/)
{
    if ( brick )
    {
        brick->SetParent(this);
        brick->SetPrevious((NassiBrick *)0);
    }
    NassiBrick *tmp;
    tmp = Child;
    Child = brick;
    return tmp;
}
void NassiBlockBrick::SetTextByNumber(const  wxString &str, wxUint32 n)
{
    if ( n == 0 )
        Comment = str;
    else
        Source = str;
}
const wxString *NassiBlockBrick::GetTextByNumber(wxUint32 n)const
{
    if ( n== 0 )
        return &Comment;
    else
        return &Source;
}
void NassiBlockBrick::accept(NassiBrickVisitor *visitor)
{
    visitor->Visit(this);
}
wxOutputStream &NassiBlockBrick::Serialize(wxOutputStream &stream)
{
    wxTextOutputStream out(stream);
    out << NASSI_BRICK_BLOCK << _T('\n');
    for ( wxUint32 n = 0 ; n < 2 ; n++ )
        SerializeString(stream, *(GetTextByNumber(n)) );

    if ( GetChild(0) != (NassiBrick *)0 )
        GetChild(0)->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');

    if ( GetNext() != (NassiBrick *)0 )
        GetNext()->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');
    return stream;
}
wxInputStream &NassiBlockBrick::Deserialize(wxInputStream &stream)
{
    wxTextInputStream inp(stream);

    wxString str;
    for (wxUint32 n = 0 ; n < 2 ; n++ )
    {
        DeserializeString(stream, str);
        SetTextByNumber(str, n);
    }

    SetChild(NassiBrick::SetData(stream), 0);

    SetNext(NassiBrick::SetData(stream));
    return stream;
}


NassiWhileBrick::NassiWhileBrick():
    NassiBrick(),
    Child(0)
{}
NassiWhileBrick::NassiWhileBrick(const NassiWhileBrick &rhs):
    NassiBrick(),
    Child(0)
{
    Child = ( NassiBrick * ) 0;
    for ( wxUint32 n=0 ; n < 2 ; n++ )
        SetTextByNumber(*(rhs.GetTextByNumber(n)), n);

    if ( rhs.GetChild(0) )
        Child = rhs.GetChild(0)->Clone();
    if ( rhs.GetNext() )
        SetNext( rhs.GetNext()->Clone()  );
}
NassiWhileBrick::~NassiWhileBrick()
{
    delete Child;
    Child = (NassiBrick *)0;
}
NassiBrick *NassiWhileBrick::GetChild(wxUint32 /*n*/) const
{
    return Child;
}
NassiBrick *NassiWhileBrick::SetChild(NassiBrick *brick, wxUint32 /*n*/)
{
    if ( brick )
    {
        brick->SetParent(this);
        brick->SetPrevious((NassiBrick *)0);
    }
    NassiBrick *tmp;
    tmp = Child;
    Child = brick;
    return(tmp);
}
void NassiWhileBrick::SetTextByNumber(const  wxString &str, wxUint32 n)
{
    if ( n == 0 )
        Comment = str;
    else
        Source = str;
}
const wxString *NassiWhileBrick::GetTextByNumber(wxUint32 n) const
{
    if ( n== 0 )
        return &Comment;
    else
        return &Source;
}
void NassiWhileBrick::accept(NassiBrickVisitor *visitor)
{
    visitor->Visit(this);
}
wxOutputStream &NassiWhileBrick::Serialize(wxOutputStream &stream)
{
    wxTextOutputStream out(stream);
    out << NASSI_BRICK_WHILE << _T('\n');
    for ( wxUint32 n = 0 ; n < 2 ; n++ )
        SerializeString(stream, *(GetTextByNumber(n)) );

    if ( GetChild(0) != (NassiBrick *)0 )
        GetChild(0)->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');

    if ( GetNext() != (NassiBrick *)0 )
        GetNext()->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');
    return stream;
}
wxInputStream &NassiWhileBrick::Deserialize(wxInputStream &stream)
{
    wxTextInputStream inp(stream);

    wxString str;
    for (wxUint32 n = 0 ; n < 2 ; n++ )
    {
        DeserializeString(stream, str);
        SetTextByNumber(str, n);
    }

    SetChild(NassiBrick::SetData(stream), 0);

    SetNext(NassiBrick::SetData(stream));
    return stream;
}



NassiDoWhileBrick::NassiDoWhileBrick():
    NassiBrick(),
    Child(0)
{}
NassiDoWhileBrick::NassiDoWhileBrick(const NassiDoWhileBrick &rhs):
    NassiBrick(),
    Child(0)
{
    Child = ( NassiBrick * ) 0;
    for ( wxUint32 n=0 ; n < 2 ; n++ )
        SetTextByNumber(*(rhs.GetTextByNumber(n)), n);
    if ( rhs.GetChild(0) )
        Child = rhs.GetChild(0)->Clone();
    if ( rhs.GetNext() )
        SetNext( rhs.GetNext()->Clone()  );
}
NassiDoWhileBrick::~NassiDoWhileBrick()
{
    delete Child;
    Child = 0;
}
NassiBrick *NassiDoWhileBrick::GetChild(wxUint32 /*n*/) const
{
    return Child;
}
NassiBrick *NassiDoWhileBrick::SetChild(NassiBrick *brick, wxUint32 /*n*/)
{
    if ( brick )
    {
        brick->SetParent(this);
        brick->SetPrevious((NassiBrick *)0);
    }
    NassiBrick *tmp;
    tmp = Child;
    Child = brick;
    return(tmp);
}
void NassiDoWhileBrick::SetTextByNumber(const  wxString &str, wxUint32 n)
{
    if ( n == 0 )
        Comment = str;
    else
        Source = str;
}
const wxString *NassiDoWhileBrick::GetTextByNumber(wxUint32 n) const
{
    if ( n== 0 )
        return &Comment;
    else
        return &Source;
}
void NassiDoWhileBrick::accept(NassiBrickVisitor *visitor)
{
    visitor->Visit(this);
}
wxOutputStream &NassiDoWhileBrick::Serialize(wxOutputStream &stream)
{
    wxTextOutputStream out(stream);
    out << NASSI_BRICK_DOWHILE << _T('\n');

    for ( wxUint32 n = 0 ; n < 2 ; n++ )
        SerializeString( stream, *(GetTextByNumber(n)) );

    if ( GetChild(0) != (NassiBrick *)0 )
        GetChild(0)->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');

    if ( GetNext() != (NassiBrick *)0 )
        GetNext()->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');
    return stream;
}
wxInputStream &NassiDoWhileBrick::Deserialize(wxInputStream &stream)
{
    wxTextInputStream inp(stream);
    wxString str;
    for (wxUint32 n = 0 ; n < 2 ; n++ )
    {
        DeserializeString(stream, str);
        SetTextByNumber(str, n);
    }

    SetChild(NassiBrick::SetData(stream), 0);

    SetNext(NassiBrick::SetData(stream));
    return stream;
}





//WX_DEFINE_OBJARRAY(ArrayOfNassiBrickPtrs);

NassiSwitchBrick::NassiSwitchBrick():
    NassiBrick(),
    nChilds(0),
    Comments(),
    Sources(),
    childBlocks()
{}
NassiSwitchBrick::NassiSwitchBrick(const NassiSwitchBrick &rhs)
        :NassiBrick()
{
    nChilds = 0;
    for ( wxUint32 k = 0 ; k < rhs.GetChildCount() ; k++ )
    {
        AddChild(k);
        if ( rhs.GetChild(k) )
            SetChild( rhs.GetChild(k)->Clone()  , k);
    }
    for ( wxUint32 n = 0 ; n < 2 + 2*rhs.GetChildCount() ; n++ )
        SetTextByNumber(*(rhs.GetTextByNumber(n)), n);
    if ( rhs.GetNext() )
        SetNext( rhs.GetNext()->Clone()  );
}
NassiSwitchBrick::~NassiSwitchBrick()
{
    Destructor();
}
void NassiSwitchBrick::Destructor()
{
    while ( childBlocks.size() > 0 )
    {
        NassiBrick *brick;
        brick = childBlocks[0];
        if ( brick != (NassiBrick *)0 )
            delete brick;
        childBlocks.erase(childBlocks.begin());
    }
    for ( wxUint32 n = 0 ; n < Sources.size() ; n++ )
    {
        wxString *str = Sources[n];
        if ( str )
            delete str;
    }
    for ( wxUint32 n = 0 ; n < Comments.size() ; n++ )
    {
        wxString *str = Comments[n];
        if ( str )
            delete str;
    }

    nChilds = 0;
}

wxUint32 NassiSwitchBrick::GetChildCount() const
{
    return nChilds;
}
NassiBrick *NassiSwitchBrick::GetChild( wxUint32 n ) const
{
    if ( n < nChilds )
        return childBlocks[n];
    else
        return (NassiBrick *)0;
}
NassiBrick *NassiSwitchBrick::SetChild(NassiBrick *brick, wxUint32 n)
{
    if ( brick != (NassiBrick *)0 )
    {
        brick->SetParent(this);
        brick->SetPrevious((NassiBrick *)0);
    }
    NassiBrick *tmp;
    if ( n >= nChilds )
        n = nChilds-1;
    tmp = childBlocks[n];
    childBlocks[n] = brick;
    return(tmp);
}
void NassiSwitchBrick::SetTextByNumber(const  wxString &str, wxUint32 n)
{
    if ( n <= 0 )
    {
        Comment = str;//SetCommentText(str);
        return;
    }
    if ( n == 1 )
    {
        Source = str; //SetSourceText(str);
        return;
    }
    if ( n > 2*nChilds + 1)
        return;//n = 2*nChilds + 1;

    if ( n%2 ) // =1 odd
    {
        n--;
        n/=2;
        n--;
        Sources[n] = new wxString(str);
    }
    else // even
    {
        n/=2;
        n--;
        Comments[n] = new wxString(str);
    }
}
wxString NassiSwitchBrick::EmptyString = wxEmptyString;
const wxString *NassiSwitchBrick::GetTextByNumber(wxUint32 n) const
{
    if ( n <= 0 )
        return &Comment;
    if ( n == 1 )
        return &Source;
    if ( n > 2*nChilds + 1)
        return &EmptyString; //n = 2*nChilds + 1
    if ( n%2 ) // =1 odd
    {
        n--;
        n/=2;
        n--;
        return Sources[n];
    }
    else // even
    {
        n/=2;
        n--;
        return Comments[n];
    }
}
void NassiSwitchBrick::accept(NassiBrickVisitor *visitor)
{
    visitor->Visit(this);
}
void NassiSwitchBrick::RemoveChild(wxUint32 pos)
{
    if ( pos > nChilds ) return;
    NassiBrick *brick;
    brick = childBlocks[pos];
    //childBlocks.RemoveAt(pos);
    //{
    wxUint32 p = 0;
    ArrayOfNassiBrickPtrs::iterator it = childBlocks.begin();
    std::vector<wxString *>::iterator cit = Comments.begin();
    std::vector<wxString *>::iterator sit = Sources.begin();
    while ( p < pos )
    {
        it++;
        p++;
        cit++;
        sit++;
    }
    childBlocks.erase(it);
    Comments.erase(cit);
    Sources.erase(sit);
    //}
    nChilds--;
}
void NassiSwitchBrick::AddChild(wxUint32 pos = 0)
{
    if ( pos > nChilds )
        pos = nChilds; // the last

    //childBlocks.Insert((NassiBrick*) 0, pos);
    wxUint32 p = 0;
    ArrayOfNassiBrickPtrs::iterator it = childBlocks.begin();
    std::vector<wxString *>::iterator cit = Comments.begin();
    std::vector<wxString *>::iterator sit = Sources.begin();
    while ( p < pos )
    {
        it++;
        p++;
        cit++;
        sit++;
    }
    childBlocks.insert(it, (NassiBrick*) 0);
    Comments.insert(cit, new wxString(_T("")));
    Sources.insert(sit, new wxString(_T("")));

    nChilds++;
}
//public: virtual
wxOutputStream &NassiSwitchBrick::Serialize(wxOutputStream &stream)
{
    wxTextOutputStream out(stream);
    out << NASSI_BRICK_SWITCH << _T('\n');
    wxUint32 k = nChilds;
    out << k << _T('\n');
    for ( wxUint32 n = 0 ; n < 2*k+2 ; n++ )
        SerializeString( stream, *(GetTextByNumber(n)) );
    for ( wxUint32 n = 0 ; n < k ; n++ )
    {
        if ( GetChild(n) != (NassiBrick *)0 )
            GetChild(n)->Serialize(stream);
        else
            out << NASSI_BRICK_ESC << _T('\n');
    }

    if ( GetNext() != (NassiBrick *)0 )
        GetNext()->Serialize(stream);
    else
        out << NASSI_BRICK_ESC << _T('\n');
    return stream;
}
wxInputStream &NassiSwitchBrick::Deserialize(wxInputStream &stream)
{
    wxTextInputStream inp(stream);
    wxUint32 count;
    inp >> count;
    wxString str;
    wxArrayString arstr;
    for (wxUint32 n = 0 ; n < 2*count+2 ; n++ )
    {
        DeserializeString( stream, str );
        //SetTextByNumber(str, n);
        arstr.Add(str);
    }
    for ( wxUint32 n = 0; n < count ; n++ )
    {
        AddChild(n);
        SetChild(NassiBrick::SetData(stream), n);
    }
    for ( wxUint32 n = 0 ; n < arstr.GetCount() ; n++ )
    {
        SetTextByNumber(arstr[n], n);
    }

    SetNext(NassiBrick::SetData(stream));
    return stream;
}


///////////////////////////////// iterator

NassiBricksCompositeIterator::NassiBricksCompositeIterator(NassiBrick *frst)
{
    first = frst;
    itr = (NassiBricksCompositeIterator *)0;
    First();
}
void NassiBricksCompositeIterator::First()
{
    currentParent = first;
    current = first;
    if (current)
        done = false;
    else
        done = true;
    child = 0;
    if ( itr != (NassiBricksCompositeIterator *)0 )
        delete itr;
    itr = (NassiBricksCompositeIterator *)0;
}

void NassiBricksCompositeIterator::SetNext(void)
{
    current = currentParent->GetNext();
    currentParent = current;
    if ( !current )
        done = true;
}
bool NassiBricksCompositeIterator::SetItrNextChild(void)
{
    if ( child < currentParent->GetChildCount() ) //currentParent->nChilds )//not yet iterated throug all childs
    {
        itr = new NassiBricksCompositeIterator(currentParent->GetChild(child));  //(currentParent->childBlocks.Item(child) );// [child]);
        child++;
        if ( !itr->IsDone() )
        {
            current = itr->CurrentItem();
            return(true);
        }
        else//neuer itr nicht gültig
        {
            delete itr;
            itr = (NassiBricksCompositeIterator *)0;
            return( SetItrNextChild() );
        }
    }
    else
        return false;
}
void NassiBricksCompositeIterator::Next()
{
    if ( current )
    {
        if ( itr ) // iterator for childs is created
        {
            itr->Next();
            if ( itr->IsDone() ) // letztes in kette erreicht
            {
                delete itr;
                itr = (NassiBricksCompositeIterator *)0;
                if ( SetItrNextChild() ) //nächste kette versuchen
                {
                    current = itr->CurrentItem();
                }
                else // keine kette mehr gefunden
                {
                    SetNext();
                    child = 0;
                    return;
                }
            }
            else // child iterator has a valid item:
                current = itr->CurrentItem();
        }
        else // itr == 0
        {
            if ( current->GetChildCount() > 0 )  // current->nChilds > 0 )
            {
                currentParent = current;
                if ( SetItrNextChild() ) //nächste kette versuchen
                {
                    current = itr->CurrentItem();
                }
                else // keine kette mehr gefunden
                {
                    SetNext();
                    child = 0;
                    return;
                }
            }
            else
            {
                SetNext();
                child = 0;
                return;
            }
        }
    }
}

void NassiBrick::GenerateStrukTeX(wxString &str)
{
    str = _T("\\begin{struktogramm}(width,height)\n");
    GetStrukTeX(str, 2);
    str += _T("\\end{struktogramm}\n");
}
void NassiBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}
void NassiInstructionBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\assign{");
    str += *(GetTextByNumber(0));
    str += _T("}\n");

    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}
void NassiDoWhileBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\until{");
    str += *(GetTextByNumber(0));
    str += _T("}\n");

    NassiBrick *child = GetChild(0);
    if ( child )
        child->GetStrukTeX(str, n+2);

    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\untilend\n");

    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}
void NassiForBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    ///instruction
    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\assign{");
    str += *(GetTextByNumber(2)); //init comment
    str += _T("}\n");

    ///loop
    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\while{");
    str += *(GetTextByNumber(0)); //main comment
    str += _T("}\n");

    ///child
    NassiBrick *child = GetChild(0);
    if ( child )
        child->GetStrukTeX(str, n+2);
    ///instruction at end
    for (wxUint32 i = 0 ; i < n+2 ; i++ )
        str+= _T(" ");
    str+= _T("\\assign{");
    str += *(GetTextByNumber(4)); //inst comment
    str += _T("}\n");

    ///end of loop
    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\whileend\n");

    ///next brick
    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}
void NassiWhileBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\while{");
    str += *(GetTextByNumber(0));
    str += _T("}\n");

    NassiBrick *child = GetChild(0);
    if ( child )
        child->GetStrukTeX(str, n+2);

    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\whileend\n");

    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}
void NassiBlockBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    NassiBrick *child = GetChild(0);
    if ( child )
        child->GetStrukTeX(str, n);

    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}
void NassiReturnBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\return{");
    str += *(GetTextByNumber(0));
    str += _T("}\n");

    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}
void NassiContinueBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    /// is not siported by struktex
    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}
void NassiBreakBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\exit{");
    str += *(GetTextByNumber(0));
    str += _T("}\n");

    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}
void NassiIfBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\ifthenelse{3}{3}");
    str += (_T("{") + *(GetTextByNumber(0)) + _T("}"));
    str += (_T("{") + *(GetTextByNumber(2)) + _T("}"));
    str += (_T("{") + *(GetTextByNumber(4)) + _T("}\n"));

    NassiBrick *child = GetChild(0);
    if ( child )
        child->GetStrukTeX(str, n+2);

    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\change\n");

    child = GetChild(1);
    if ( child )
        child->GetStrukTeX(str, n+2);

    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\ifend\n");

    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}
void NassiSwitchBrick::GetStrukTeX(wxString &str, wxUint32 n = 0)
{
    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\case{4}");
    str += _T("{") + wxString::Format(_T("%d"), GetChildCount()) + _T("}");
    str += _T("{") + *(GetTextByNumber(0)) + _T("}");
    str += _T("{") + *(GetTextByNumber(2)) + _T("}\n");

    NassiBrick *child = GetChild(0);
    if ( child )
        child->GetStrukTeX(str, n+2);

    for ( wxUint32 i = 1 ; i < GetChildCount() ; i++ )
    {
        for ( wxUint32 k = 0 ; k < n ; k++ )
            str += _T(" ");
        str += _T("\\switch{") + *(GetTextByNumber(2*(i+1))) + _T("}\n");
        child = GetChild(i);
        if ( child )
            child->GetStrukTeX(str, n+2);
    }

    for ( wxUint32 i = 0 ; i < n ; i++ )
        str += _T(" ");
    str += _T("\\caseend\n");

    NassiBrick *next = GetNext();
    if ( next )
        next->GetStrukTeX(str, n);
}


void NassiBrick::SaveCommentString(wxTextOutputStream &text_stream, const wxString &str, wxUint32 n)
{
    if ( !str.IsEmpty() )
        NassiBrick::SaveSourceString(text_stream, _T("/*") + str + _T("*/\n"), n);
}
void NassiBrick::SaveSourceString(wxTextOutputStream &text_stream,  const wxString &str, wxUint32 n)
{
    wxString s = str + _T("\n");
    while ( ! s.IsEmpty() )
    {
        for ( wxUint32 k = 0 ; k < n /*&& fillfirst*/; k++ )
            text_stream << _T(" ");
        wxInt32 pos = s.Find('\n');
        if ( pos != -1 )
        {
            text_stream << s.SubString(0, pos-1) << _T("\n");
            s = s.SubString(pos+1, s.Length() );
        }
        else
        {
            text_stream << s;
            s.Empty();
        }
    }
}
void NassiBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    NassiBrick *next = GetNext();
    if ( next )
        next->SaveSource(text_stream, n);
}
void NassiInstructionBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    SaveCommentString(text_stream, Comment, n);
    SaveSourceString(text_stream, Source, n);

    NassiBrick::SaveSource(text_stream, n);
}
void NassiBlockBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    SaveCommentString(text_stream, Comment, n);
    //SaveSourceString(text_stream, Source, n);

    SaveSourceString(text_stream, _T("{"), n);

    NassiBrick *child = GetChild(0);
    if ( child )
        child->SaveSource(text_stream, n+4);

    SaveSourceString(text_stream, _T("}"), n);


    NassiBrick::SaveSource(text_stream, n);
}
void NassiWhileBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    SaveCommentString(text_stream, Comment, n);

    wxString src = _T("while ") + Source;
    SaveSourceString(text_stream,src, n);

    NassiBrick *child = GetChild(0);
    if ( child )
    {
        SaveSourceString(text_stream, _T("{"), n);
        child->SaveSource(text_stream, n+4);
        SaveSourceString(text_stream, _T("}"), n);
    }
    else
        SaveSourceString(text_stream, _T(";"), n+4);


    NassiBrick::SaveSource(text_stream, n);
}
void NassiDoWhileBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    SaveCommentString(text_stream, Comment, n);

    SaveSourceString(text_stream, _T("do"), n);

    NassiBrick *child = GetChild(0);
    if ( child )
    {
        SaveSourceString(text_stream, _T("{"), n);
        child->SaveSource(text_stream, n+4);
        SaveSourceString(text_stream, _T("}"), n);
    }
    else
        SaveSourceString(text_stream, _T(";"), n+4);

    wxString src = _T("while") + Source + _T(";");
    SaveSourceString(text_stream, src, n);


    NassiBrick::SaveSource(text_stream, n);
}
void NassiForBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    SaveCommentString(text_stream, Comment, n);

    wxString src = _T("for ") + Source;
    SaveSourceString(text_stream, src, n);

    NassiBrick *child = GetChild(0);
    if ( child )
    {
        SaveSourceString(text_stream, _T("{"), n);
        child->SaveSource(text_stream, n+4);
        SaveSourceString(text_stream, _T("}"), n);
    }
    else
        SaveSourceString(text_stream, _T(";"), n+4);


    NassiBrick::SaveSource(text_stream, n);
}
void NassiIfBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    SaveCommentString(text_stream, Comment, n);

    wxString src = _T("if ") + Source;
    SaveSourceString(text_stream, src, n);

    SaveCommentString(text_stream, TrueCommentText, n+4);

    NassiBrick *child = GetChild(0);
    if ( child )
    {
        SaveSourceString(text_stream, _T("{"), n);
        child->SaveSource(text_stream, n+4);
        SaveSourceString(text_stream, _T("}"), n);
    }
    else
        SaveSourceString(text_stream, _T(";"), n+4);

    child = GetChild(1);
    if ( child )
    {
        SaveSourceString(text_stream, _T("else\n{"), n);

        SaveCommentString(text_stream, FalseCommentText, n+4);
        child->SaveSource(text_stream, n+4);

        SaveSourceString(text_stream, _T("}"), n);
    }


    NassiBrick::SaveSource(text_stream, n);
}
void NassiSwitchBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    SaveCommentString(text_stream, Comment, n);

    wxString src = _T("switch ( ") + Source + _T(" )\n{");
    SaveSourceString(text_stream, src, n);

    for ( wxUint32 i = 0 ; i < GetChildCount(); i++ )
    {
        NassiBrick *child = GetChild(i);
        wxString cmt(*GetTextByNumber((i+1)*2));

        wxString src = *GetTextByNumber((i+1)*2+1);
        if ( src.StartsWith( _T("default") ) )
            src = _T("default:");
        else
            src = _T("case ") + src + _T(":");

        SaveCommentString(text_stream, cmt, n);
        SaveSourceString(text_stream, src, n);
        if ( child )
            child->SaveSource(text_stream, n+4);
    }

    SaveSourceString(text_stream, _T("}"), n);


    NassiBrick::SaveSource(text_stream, n);
}

void NassiReturnBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    SaveCommentString(text_stream, Comment, n);

    if ( Source.IsEmpty() )
        SaveSourceString(text_stream,  _T("return;"), n);
    else
    {
        wxString src = _T("return ") + Source + _T(";");
        SaveSourceString(text_stream, src, n);
    }


    NassiBrick::SaveSource(text_stream, n);
}
void NassiContinueBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    SaveCommentString(text_stream, Comment, n);
    SaveSourceString(text_stream, _T("continue;"), n);

    NassiBrick::SaveSource(text_stream, n);
}
void NassiBreakBrick::SaveSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    SaveCommentString(text_stream, Comment, n);
    SaveSourceString(text_stream, _T("break;"), n);

    NassiBrick::SaveSource(text_stream, n);
}

