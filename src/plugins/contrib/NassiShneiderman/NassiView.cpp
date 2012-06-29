#include "NassiView.h"
#include "NassiFileContent.h"
#include "NassiDiagramWindow.h"
#include "GraphFabric.h"

#include "Task.h"

#include "commands.h"
#include "InsertBrickTask.h"
#include "RedLineDrawlet.h"
#include "RedHatchDrawlet.h"
#include "PasteTask.h"
#include "TextCtrlTask.h"
#include "TextCtrl.h"

#include <wx/clipbrd.h>
#ifdef USE_SVG
    #include <wx/svg/dcsvg.h>
#endif
#include <wx/wfstream.h>

#if wxUSE_POSTSCRIPT
    #include <wx/dcps.h>
#endif
#include <wx/textfile.h>

#include "DataObject.h"
#include "NassiDropTarget.h"

#include "rc/dnd_copy_cur.xpm"
#include "rc/dnd_move_cur.xpm"
#include "rc/dnd_none_cur.xpm"

NassiView::NassiView(NassiFileContent *nfc):
    m_nfc(nfc),
    m_fontsize(10),
    m_sourcefont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL),
    m_commentfont(10, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL),
    m_DrawSource(true),
    m_DrawComment(true),
    m_GraphBricks(),
    m_diagramwindow(0),
    m_graphFabric(0),
    m_updated(false),
    itsTask(0),
    hasSelectedBricks(false),
    reverseSelected(false),
    firstSelectedGBrick(0),
    lastSelectedGBrick(0),
    ChildIndicatorParent(0),
    ChildIndicatorIsSelected(false),
    ChildIndicator(0),
    cursorOverText(false),
    m_EmptyRootRect(),
    DragStartPoint(),
    DragPossible(false),
    ThisIsDnDSource(false),
    m_txt(0)
{
    m_graphFabric = new GraphFabric(this, &m_GraphBricks);
    nfc->AddObserver(this);
}
const wxPoint NassiView::offset = wxPoint(20,20);
NassiView::~NassiView()
{
    m_nfc->RemoveObserver(this);

    /// remove graph bricks
    while ( m_GraphBricks.size() )
    {
        BricksMap::iterator it = m_GraphBricks.begin();
        GraphNassiBrick *gbrick = it->second;
        if ( gbrick ) delete gbrick;
        m_GraphBricks.erase(it->first);
    }

    delete m_graphFabric;
    if(itsTask)
        delete itsTask;
}
bool NassiView::IsDrawingSource()
{
    return m_DrawSource;
}
bool NassiView::IsDrawingComment()
{
    return m_DrawComment;
}
void NassiView::EnableDrawSource(bool en )
{
    m_DrawSource = en;
    UpdateSize();
}
void NassiView::EnableDrawComment(bool en )
{
    m_DrawComment = en;
    UpdateSize();
}
void NassiView::ChangeToolTo(NassiTools tool)
{
    SetTask( new InsertBrickTask(this, m_nfc, tool) );
}
void NassiView::ToolSelect(void)
{
    //m_tool = NASSI_TOOL_ESC;

    RemoveTask();
    m_diagramwindow->SetCursor(wxCursor(wxCURSOR_ARROW));
    ClearSelection();
}
bool NassiView::CanZoomIn()
{
    return m_fontsize < 416;
}
bool NassiView::CanZoomOut()
{
    return m_fontsize > 6;
}
void NassiView::ZoomIn()
{
    if ( m_fontsize < 416 )
    {
        for ( wxInt16 n = 0 ; n < 37 ; n++ )
            if ( FontSizes[n] >= m_fontsize )
            {
                m_fontsize = FontSizes[n+1];
                break;
            }
        m_commentfont.SetPointSize(m_fontsize);
        m_sourcefont.SetPointSize(m_fontsize);
    }
    UpdateSize();
}
void NassiView::ZoomOut()
{
    if ( m_fontsize > 6 )
    {
        for ( wxInt16 n = 37 ; n > 0 ; n-- )
            if ( FontSizes[n] <= m_fontsize)
            {
                m_fontsize = FontSizes[n-1];
                break;
            }
        m_commentfont.SetPointSize(m_fontsize);
        m_sourcefont.SetPointSize(m_fontsize);
    }
    UpdateSize();
}
const wxInt16 NassiView::FontSizes[] =
    { 6,  7,  8,  9, 10, 11,
     12, 14, 16, 18, 20, 22,
     24, 28, 32, 36, 40, 44,
     48, 56, 64, 72, 80, 88,
     96,112,128,144,160,176,
    192,224,256,288,320,352,
    384,416};
void NassiView::DeleteSelection()
{
    if ( itsTask && itsTask->CanEdit() )
    {
        itsTask->DeleteSelection();
        if ( itsTask->Done() ) RemoveTask();
        return;
    }

    wxCommand *delcmd = Delete();
    if ( delcmd )
    {
        wxCommandProcessor *cmdp = m_nfc->GetCommandProcessor();
        cmdp->Submit(delcmd);
        ClearSelection();
    }
}
wxCommand *NassiView::Delete()
{
    if ( ChildIndicatorIsSelected )
        return new NassiDeleteChildRootCommand(m_nfc, ChildIndicatorParent->GetBrick(), ChildIndicator );

    if ( HasSelectedBricks() )
    {
        NassiBrick *last, *first;
        if ( !lastSelectedGBrick )
            last = firstSelectedGBrick->GetBrick();
        else
            last = lastSelectedGBrick->GetBrick();

        if ( reverseSelected )
        {
            first = last;
            last = firstSelectedGBrick->GetBrick();
        }
        else
            first = firstSelectedGBrick->GetBrick();

        return new NassiDeleteCommand( m_nfc, first, last);
    }

    return 0;
}
void NassiView::Cut()
{
    if ( itsTask && itsTask->CanEdit() )
    {
        itsTask->Cut();
        if ( itsTask->Done() ) RemoveTask();
        return;
    }
    CopyBricks();
    DeleteSelection();
}
void NassiView::Copy()
{
    if ( itsTask && itsTask->CanEdit() )
    {
        itsTask->Copy();
        if ( itsTask->Done() ) RemoveTask();
        return;
    }

    /// /////////////////////////////////////////////
    CopyBricks();
}
void NassiView::CopyBricks()
{
    wxClipboardLocker locker;
    if ( !locker ) return;

    if ( !m_nfc->GetFirstBrick() ) return;

    NassiDataObject *ndo = 0;
    if ( HasSelectedBricks() )
    {
        NassiBrick *first = (NassiBrick *)0,
                    *last = (NassiBrick *)0,
                     *tmp = (NassiBrick *)0;
        //check the active Bricks
        first = firstSelectedGBrick->GetBrick();
        last = firstSelectedGBrick->GetBrick();
        if ( !reverseSelected )
        {
            if ( lastSelectedGBrick )
                last = lastSelectedGBrick->GetBrick();
        }
        else
        {
            if ( lastSelectedGBrick )
                first = lastSelectedGBrick->GetBrick();
        }

        tmp = last->GetNext();
        last->SetNext( (NassiBrick *)0 );


        NassiBrick *parent = first->GetParent();
        wxString strc, strs;
        if ( ChildIndicatorIsSelected && parent )
        {
            strc = *parent->GetTextByNumber(2+2*ChildIndicator);
            strs = *parent->GetTextByNumber(3+2*ChildIndicator);
            ndo = new NassiDataObject(first, this, strc, strs);
        }
        else
            ndo = new NassiDataObject(first, this);

        ///restore the original diagram:
        if ( first && last && tmp )
            last->SetNext(tmp);
    }
    else
    {
        if ( ChildIndicatorIsSelected )
        {
            NassiBrick *parent = ChildIndicatorParent->GetBrick();
            if ( parent )
                ndo = new NassiDataObject(0, this, *parent->GetTextByNumber(2+2*ChildIndicator), *parent->GetTextByNumber(3+2*ChildIndicator));
        }
    }

    if ( wxTheClipboard->Open() && ndo )
    {
        wxTheClipboard->SetData( ndo );
        wxTheClipboard->Close();
    }
    else
        if ( ndo )
            delete ndo;


}
void NassiView::Paste()
{
    if ( itsTask && itsTask->CanEdit() )
    {
        itsTask->Paste();
        if ( itsTask->Done() ) RemoveTask();
        return;
    }

    ClearSelection();
    /// /////////////////////////////////////////////
    wxClipboardLocker locker;
    if ( !locker ) return;
    NassiDataObject  dataobj( (NassiBrick *)0, this);

    if ( wxTheClipboard->Open() )
    {
        if ( wxTheClipboard->IsSupported( wxDataFormat(NassiDataObject::NassiFormatId) ) )
        {
            wxTheClipboard->GetData(dataobj);
            NassiBrick *brick = dataobj.GetBrick();
            wxString insertStrC = dataobj.GetText(0);
            wxString insertStrS = dataobj.GetText(1);
            SetTask( new PasteTask(this, m_nfc, brick, insertStrC, insertStrS) );
        }
        wxTheClipboard->Close();
    }
}
bool NassiView::CanPaste()const
{
    if ( itsTask && itsTask->CanEdit())
        return itsTask->CanPaste();

    return wxTheClipboard->IsSupported(wxDataFormat(NassiDataObject::NassiFormatId));
}
void NassiView::MoveTextCtrl(const wxPoint &pt )
{
    wxPoint p;
    m_diagramwindow->CalcScrolledPosition(pt.x, pt.y, &p.x, &p.y);
    m_txt->Move(p);
}
bool NassiView::HasSelection()const
{
    if ( itsTask && itsTask->CanEdit() )
        return itsTask->HasSelection();

    return hasSelectedBricks || ChildIndicatorIsSelected;
}
bool NassiView::HasSelectedBricks()const
{
    return hasSelectedBricks;
}
void NassiView::Update( wxObject* /*hint*/ )
{
    for (BricksMap::iterator it = m_GraphBricks.begin() ; it != m_GraphBricks.end() ; it++ )
        it->second->SetUsed(false); // mark every brick as unused

    NassiBrick *firstbrick = m_nfc->GetFirstBrick();
    for ( NassiBricksCompositeIterator itr(firstbrick) ; !itr.IsDone() ; itr.Next())
    {
        // when a brick from the view corresponding to a brick from the document is found
        // mark it as used
        GraphNassiBrick *gbrick = GetGraphBrick(itr.CurrentItem());
        if ( !gbrick )
            CreateGraphBrick( itr.CurrentItem() )->SetUsed();
        else
            gbrick->SetUsed();
    }

    std::vector<NassiBrick *> PtrsOfNonexistentBricks;
    for (BricksMap::iterator it = m_GraphBricks.begin() ; it != m_GraphBricks.end() ; it++ )
        if ( ! it->second->IsUsed() )
            PtrsOfNonexistentBricks.push_back(it->first);

    for ( wxUint32 i = 0 ; i < PtrsOfNonexistentBricks.size() ; i++ )
    {
        NassiBrick *brick = PtrsOfNonexistentBricks[i];
        GraphNassiBrick *gbrick = GetGraphBrick(brick);
        if ( gbrick ) delete gbrick;
        m_GraphBricks.erase(brick);
    }

    UpdateSize();


    Updated(true);
}
void NassiView::UpdateSize()
{
    wxPoint minsize(0,0);
    wxClientDC *dc = new wxClientDC(m_diagramwindow);

    wxCaret *caret = m_diagramwindow->GetCaret();
    dc->SetFont( GetCommentFont() );
    if ( caret ) caret->SetSize( 1, dc->GetCharHeight() );

    GraphNassiBrick *gbrick = this->GetGraphBrick(m_nfc->GetFirstBrick());
    if ( !gbrick )
    {
        wxString str(_("Insert your code here."));
        wxCoord w, h;
        dc->GetTextExtent(str, &w, &h);
        m_diagramwindow->SetVirtualSize(40 + 2*dc->GetCharWidth() + w, 20 + 2*dc->GetCharHeight() + h );
    }
    else
    {
        gbrick->CalcMinSize(dc, minsize);
        gbrick->SetOffsetAndSize(dc, offset, minsize);

        m_diagramwindow->SetVirtualSize(
            minsize.x + 2*dc->GetCharWidth(),
            minsize.y + 2*dc->GetCharHeight() );
    }

    wxTextAttr attr = m_txt->GetDefaultStyle();
    wxFont font = attr.GetFont();
    font.SetPointSize(m_commentfont.GetPointSize() );
    attr.SetFont( font );
    m_txt->SetDefaultStyle( attr );
    m_txt->SetStyle(0, m_txt->GetLastPosition(), attr);

    m_diagramwindow->Refresh();
    if ( itsTask ) itsTask->UpdateSize();
}
const wxFont &NassiView::GetCommentFont()
{
    return m_commentfont;
}
const wxFont &NassiView::GetSourceFont()
{
    return m_sourcefont;
}
NassiDiagramWindow *NassiView::CreateDiagramWindow(wxWindow *parent)
{
    if ( !m_diagramwindow )
        m_diagramwindow = new NassiDiagramWindow(parent, this);

    if ( !m_txt )
        m_txt = new TextCtrl(m_diagramwindow, wxID_ANY, _T(""), wxPoint(100,100), wxDefaultSize );
    m_txt->Show(false);
    return m_diagramwindow;
}
const wxRect &NassiView::GetEmptyRootRect()
{
    return m_EmptyRootRect;
}
void NassiView::DrawDiagram(wxDC *dc)
{
    dc->SetFont( m_commentfont);
    dc->SetPen(*wxBLACK_PEN);
    wxCoord dx, dy;
    dx = dc->GetCharWidth();
    dy = dc->GetCharHeight();

    NassiBrick *brick = m_nfc->GetFirstBrick();
    if ( !HasUpdated() ) return;

    if ( !brick )
    {
        //draw a field to place a new Brick into
        // there is no brick at the moment
        wxString str(_("Insert your code here."));
        wxCoord w, h, desc;
        dc->GetTextExtent(str, &w, &h, &desc);
        dc->SetBrush(*wxLIGHT_GREY_BRUSH);
        dc->DrawRectangle(offset.x, offset.y, w+2*dx, h+2*dy);
        dc->SetBrush(wxNullBrush);
        dc->DrawText(str, offset.x + dx, offset.y + dy);
        m_EmptyRootRect.x = offset.x;
        m_EmptyRootRect.y = offset.y;
        m_EmptyRootRect.width = w+2*dx;
        m_EmptyRootRect.height = h+2*dy;
    }
    else
    {
        for ( BricksMap::iterator it = m_GraphBricks.begin();it != m_GraphBricks.end(); it++)
            it->second->Draw(dc);
        // mark active bricks
        for ( BricksMap::iterator it = m_GraphBricks.begin();it != m_GraphBricks.end(); it++)
            it->second->DrawActive(dc);
    }
}
HooverDrawlet *NassiView::OnMouseMove(wxMouseEvent &event, const wxPoint &pos)
{
    cursorOverText = false;

    if ( itsTask )
        return itsTask->OnMouseMove(event, pos);

    // if no Task is active, we check if hoovering over text
    GraphNassiBrick *gbrick = GetBrickAtPosition(pos);
    if ( !HasSelection() && gbrick && gbrick->IsOverText(pos) != 0 )
    {
        cursorOverText = true;
        m_diagramwindow->SetCursor(wxCursor(wxCURSOR_IBEAM));
        return 0;
    }
    else
        // show the normal arrow cursor
        m_diagramwindow->SetCursor(wxCursor(wxCURSOR_ARROW));

    if ( DragPossible )
    {
        if ( (pos.x - DragStartPoint.x)*(pos.x - DragStartPoint.x) +
             (pos.y - DragStartPoint.y)*(pos.y - DragStartPoint.y) > 9 )
        {
            DragPossible = false;
            DragStart();
        }
    }

    return 0;
}
void NassiView::OnMouseLeftDown(wxMouseEvent &event, const wxPoint &pos)
{
    DragPossible = false;

    if ( itsTask )
    {
        itsTask->OnMouseLeftDown(event, pos);
        if ( itsTask->Done() ) RemoveTask();
        return;
    }

    GraphNassiBrick *gbrick = GetBrickAtPosition(pos);


    bool clickedIsActive = gbrick && gbrick->IsActive();
    GraphNassiMinimizableBrick *gminbrick = dynamic_cast<GraphNassiMinimizableBrick *>(gbrick);
    if ( gminbrick && gminbrick->IsOverMinMaxBox(pos))
    {
        gminbrick->Minimize(!gminbrick->IsMinimized());
        UpdateSize();
        return;
    }

    if ( gbrick && cursorOverText )
    {
        TextGraph *textgraph = gbrick->IsOverText(pos);
        if ( textgraph )
        {
            SetTask( new TextCtrlTask(this, m_nfc, m_txt, textgraph, pos));
            return;
        }
    }

    if ( event.ShiftDown() )
    {
        SelectLast( gbrick );
    }
    else
    {
        DragPossible = true;
        DragStartPoint = pos;

        wxUint32 childN;
        if (!clickedIsActive && gbrick && gbrick->IsOverChildIndicator(pos, &childN) )
        {
            //LoggerSingleton::exemplar()->Log( wxString::Format(_T("ChildIndicator %d"), childN ));
            SelectChildIndicator(gbrick, childN);
        }
        else
            if ( !clickedIsActive ) SelectFirst( gbrick );
    }
}
void NassiView::OnMouseLeftUp(wxMouseEvent &event, const wxPoint &pos)
{
    if ( itsTask )
    {
        itsTask->OnMouseLeftUp(event, pos);
        if ( itsTask->Done() ) RemoveTask();
        return;
    }

//    if ( DragPossible ) // left click on a selected brick and not drag started -> activate the brick
//    {
//        GraphNassiBrick *gbrick = GetBrickAtPosition(pos);
//        //wxUint32 childN;
//        //if (gbrick && gbrick->IsOverChildIndicator(pos, &childN) )
//        SelectFirst( gbrick );
//    }
    DragPossible = false;
}
void NassiView::OnMouseRightDown(wxMouseEvent &event, const wxPoint &pos)
{
    if ( itsTask )
    {
        itsTask->OnMouseRightDown(event, pos);
        if ( itsTask->Done() ) RemoveTask();
    }
}
void NassiView::OnMouseRightUp(wxMouseEvent& event, const wxPoint &pos)
{
    //LoggerSingleton::exemplar()->Log(_T("View::OnMouseRightUp"));
    if ( itsTask )
    {
        itsTask->OnMouseRightUp(event, pos);
        if ( itsTask->Done() ) RemoveTask();
    }
}
void NassiView::OnKeyDown(wxKeyEvent &event)
{
    if ( itsTask )
    {
        itsTask->OnKeyDown( event );
        if ( itsTask->Done() ) RemoveTask();
        return;
    }

    int kcode = event.GetKeyCode();

    if ( kcode == WXK_DELETE || kcode == WXK_NUMPAD_DELETE )
    {
        DeleteSelection();
    }
    if ( kcode == WXK_HOME || kcode == WXK_NUMPAD_HOME )
    {
        if ( !event.ShiftDown() )
            SelectFirst(GetGraphBrick(m_nfc->GetFirstBrick()));
        else
            SelectLast(GetGraphBrick(m_nfc->GetFirstBrick()));
        return;
    }
    if ( kcode == WXK_END || kcode == WXK_NUMPAD_END )
    {
        NassiBrick *brick = m_nfc->GetFirstBrick();
        if ( !brick ) return;
        while ( brick->GetNext() )
            brick = brick->GetNext();
        GraphNassiBrick *gbrick = GetGraphBrick(brick);
        if ( !event.ShiftDown() )
            SelectFirst(gbrick);
        else
            SelectLast(gbrick);
        return;
    }

    if ( !hasSelectedBricks ) return;
    if ( kcode == WXK_DOWN || kcode == WXK_NUMPAD_DOWN || kcode == WXK_UP || kcode == WXK_NUMPAD_UP )
    {
        if ( lastSelectedGBrick == 0 )
            lastSelectedGBrick = firstSelectedGBrick;
        NassiBrick *brick;
        if ( !event.ShiftDown() )
            brick = firstSelectedGBrick->GetBrick();
        else
            brick = lastSelectedGBrick->GetBrick();

        if ( kcode == WXK_DOWN || kcode == WXK_NUMPAD_DOWN )
        {
            if ( brick->GetNext() )
                brick = brick->GetNext();
            else
            {
                NassiBrick *tmp = brick;
                while ( tmp->GetPrevious() )
                    tmp = tmp->GetPrevious();
                if ( tmp->GetParent() )
                {
                    NassiBrick *parent = tmp->GetParent();
                    for ( wxUint32 n = 0 ; n < parent->GetChildCount() ; ++n )
                    {
                        if ( parent->GetChild(n) == tmp  && parent->GetChild(n+1))
                        {
                            brick = parent->GetChild(n+1);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if ( brick->GetPrevious() )
                brick = brick->GetPrevious();
            else
            {
                NassiBrick *tmp = brick;
                while ( tmp->GetPrevious() )
                    tmp = tmp->GetPrevious();
                if ( tmp->GetParent() )
                {
                    NassiBrick *parent = tmp->GetParent();
                    for ( wxUint32 n = 1 ; n < parent->GetChildCount() ; ++n )
                    {
                        if ( parent->GetChild(n) == tmp  && parent->GetChild(n-1))
                        {
                            brick = parent->GetChild(n-1);
                            break;
                        }
                    }
                }
            }
        }

        if ( !event.ShiftDown() )
            SelectFirst(GetGraphBrick(brick));
        else
            SelectLast(GetGraphBrick(brick));
    }
    if ( lastSelectedGBrick != 0 || event.ShiftDown() ) return;
    //ClearSelection();
    if ( kcode == WXK_RIGHT || kcode == WXK_NUMPAD_RIGHT )
    {
        NassiBrick *brick = firstSelectedGBrick->GetBrick();
        if ( brick->GetChild(0) )
            brick = brick->GetChild(0);
        SelectFirst(GetGraphBrick(brick));
    }
    if ( kcode == WXK_LEFT || kcode == WXK_NUMPAD_LEFT )
    {
        NassiBrick *brick = firstSelectedGBrick->GetBrick();
        while ( brick->GetPrevious() )
            brick = brick->GetPrevious();
        if ( brick->GetParent() )
        {
            brick = brick->GetParent();
            SelectFirst(GetGraphBrick(brick));
        }
    }
}
void NassiView::OnChar(wxKeyEvent &event)
{
    if ( itsTask )
    {
        itsTask->OnChar(event);
        if ( itsTask->Done() ) RemoveTask();
    }
}
void NassiView::SetTask(Task* task)
{
    SelectFirst(0);
    if ( itsTask )
        delete itsTask;

    itsTask = task;
    if ( itsTask )
        m_diagramwindow->SetCursor(itsTask->Start());
}
void NassiView::RemoveTask()
{
    if ( itsTask )
        delete itsTask;
    itsTask = 0;
    ClearSelection();
    m_diagramwindow->SetCursor(wxCursor(wxCURSOR_ARROW));
}
void NassiView::ClearSelection()
{
    hasSelectedBricks = false;
    reverseSelected = false;
    ChildIndicatorIsSelected = false;
    firstSelectedGBrick = 0;
    lastSelectedGBrick = 0;
    ChildIndicatorParent = 0;

    for ( BricksMap::iterator  it = m_GraphBricks.begin() ; it != m_GraphBricks.end() ; it++)
    {
        it->second->SetActive(false);
        it->second->SetChildIndicatorActive(false);
    }

    m_diagramwindow->Refresh();
}
void NassiView::SelectFirst(GraphNassiBrick *gfirst)
{
    ClearSelection();
    if ( !gfirst ) return;

    hasSelectedBricks = true;
    reverseSelected = false;
    ChildIndicatorIsSelected = false;
    ChildIndicatorParent = 0;
    firstSelectedGBrick = gfirst;
    lastSelectedGBrick = 0;

    gfirst->SetActive(true, true);

    m_diagramwindow->Refresh();
}
void NassiView::Select(GraphNassiBrick *gfirst, GraphNassiBrick *glast)
{
    if ( !gfirst )
    {
        ClearSelection();
        return;
    }

    if ( !glast )
    {
        SelectFirst(gfirst);
        return;
    }


    NassiBrick *first = gfirst->GetBrick();
    NassiBrick *last  = glast->GetBrick();
    ChildIndicatorIsSelected = false;
    ChildIndicatorParent = 0;

    if ( !first || !last )
    {
        ClearSelection();
        return;
    }

    hasSelectedBricks = false;
    wxUint32 flevel = first->GetLevel();
    wxUint32 llevel = last->GetLevel();

    while ( flevel > llevel && flevel != 0 )
    {
        while ( first->GetPrevious() )
            first = first->GetPrevious();
        first = first->GetParent();
        if ( !first )
        {
            //LoggerSingleton::exemplar()->Log(_T("failure while checking for parents"));
            return;
        }
        flevel--;
    }
    if ( llevel > flevel && llevel != 0 )
    {
        while ( last->GetPrevious() )
            last = last->GetPrevious();
        last = last->GetParent();
        if ( !last )
        {
            //LoggerSingleton::exemplar()->Log(_T("failure while checking for parents"));
            return;
        }
        llevel--;
    }
    if ( flevel == llevel )
    {
        while ( !first->IsSibling(last) )
        {
            while ( first->GetPrevious() )
                first = first->GetPrevious();
            first = first->GetParent();

            while ( last->GetPrevious() )
                last = last->GetPrevious();
            last = last->GetParent();

            if ( !last || !first ) // we have a problem
            {
                //LoggerSingleton::exemplar()->Log(_T("failure while checking for siblings"));
                return;
            }
        }

        //to start, deactivate all
        for ( BricksMap::iterator  it = m_GraphBricks.begin() ; it != m_GraphBricks.end() ; it++)
        {
            it->second->SetActive(false);
            it->second->SetChildIndicatorActive(false);
        }

        firstSelectedGBrick = GetGraphBrick(first);
        lastSelectedGBrick = GetGraphBrick(last);
        hasSelectedBricks = true;

        if ( first->IsOlderSibling(last) )
        {
            // swap
            NassiBrick *tmp = first;
            first = last;
            last = tmp;
            reverseSelected = true;
        }
        else
            reverseSelected = false;

        do
        {
            GraphNassiBrick *gbrick = GetGraphBrick(first);
            gbrick->SetActive(true, true);

            if ( first != last )
                first = first->GetNext();
            else
                first = 0;

        }while ( first );

    }


    m_diagramwindow->Refresh();
}
void NassiView::SelectLast(GraphNassiBrick *gbrick)
{
    Select(firstSelectedGBrick, gbrick);
}
void NassiView::SelectChildIndicator(GraphNassiBrick *gbrick, wxUint32 child)
{
    ClearSelection();
    if ( !gbrick ) return;

    GraphNassiBrick *gchild = GetGraphBrick( gbrick->GetBrick()->GetChild(child) );

    ChildIndicator = child;
    ChildIndicatorIsSelected = true;
    ChildIndicatorParent = GetGraphBrick(gbrick->GetBrick());
    gbrick->SetChildIndicatorActive(true, child);
    hasSelectedBricks = false;
    reverseSelected = false;

    if ( !gchild ) return;
    hasSelectedBricks = true;
    firstSelectedGBrick = gchild;
    while ( gchild->GetBrick()->GetNext() )
    {
        gchild->SetActive(true, true);
        gchild = GetGraphBrick( gchild->GetBrick()->GetNext() );
    }
    gchild->SetActive(true, true);
    lastSelectedGBrick = gchild;

    m_diagramwindow->Refresh();
}
void NassiView::SelectAll(void)
{
    ChildIndicatorIsSelected = false;
    ChildIndicatorParent = 0;
    NassiBrick *brick = m_nfc->GetFirstBrick();
    if ( !brick )
    {
        ClearSelection();
        return;
    }
    hasSelectedBricks = true;
    firstSelectedGBrick = GetGraphBrick(brick);
    while ( brick->GetNext() )
        brick = brick->GetNext();
    lastSelectedGBrick = GetGraphBrick(brick);

    for ( BricksMap::iterator  it = m_GraphBricks.begin() ; it != m_GraphBricks.end() ; it++)
    {
        it->second->SetActive(true);
        it->second->SetChildIndicatorActive(false);
    }

    m_diagramwindow->Refresh();
}
bool NassiView::CanSelectAll()
{
    if ( m_nfc->GetFirstBrick() ) return true;
    return false;
}
NassiBrick *NassiView::GenerateNewBrick(NassiTools tool)
{
    NassiBrick *brick;
    switch ( tool )
    {
        case NASSI_TOOL_CONTINUE:
            brick = new NassiContinueBrick();
            brick->SetTextByNumber( _T("..."), 0);
            break;
        case NASSI_TOOL_BREAK:
            brick = new NassiBreakBrick();
            brick->SetTextByNumber( _T("..."), 0);
            break;
        case NASSI_TOOL_RETURN:
            brick = new NassiReturnBrick();
            brick->SetTextByNumber( _("returning 0"), 0);
            brick->SetTextByNumber( _("NULL"), 1);
            break;
        case NASSI_TOOL_WHILE:
            brick = new NassiWhileBrick();
            brick->SetTextByNumber( _("as long as"), 0);
            brick->SetTextByNumber( _("condition"), 1);
            break;
        case NASSI_TOOL_DOWHILE:
            brick = new NassiDoWhileBrick();
            brick->SetTextByNumber( _("as long as"), 0);
            brick->SetTextByNumber( _("condition"), 1);
            break;
        case NASSI_TOOL_FOR:
            brick = new NassiForBrick();
            brick->SetTextByNumber( _("every element in vec"), 0);
            brick->SetTextByNumber( _T("uint i = 0 ; i < vec.size() ; i++"), 1);
            break;
        case NASSI_TOOL_BLOCK:
            brick = new NassiBlockBrick();
            break;
        case NASSI_TOOL_IF:
            brick = new NassiIfBrick();
            brick->SetTextByNumber( _("check that ..."), 0);
            brick->SetTextByNumber( _("condition"), 1);
            break;
        case NASSI_TOOL_SWITCH:
            brick = new NassiSwitchBrick();
            brick->SetTextByNumber( _("switch to"), 0);
            brick->SetTextByNumber( _("expression"), 1);
            break;
        default:
        case NASSI_TOOL_INSTRUCTION:
            brick = new NassiInstructionBrick();
            brick->SetTextByNumber( _T("..."), 0);
            brick->SetTextByNumber( _T("foo();"), 1);
            break;
    }
    return brick;
}
void NassiView::DragStart()
{

    wxString strc, strs;

    if ( ChildIndicatorIsSelected )
    {
        NassiBrick *parent = ChildIndicatorParent->GetBrick();
        if ( parent )
        {
            strc = *parent->GetTextByNumber(2 + 2*ChildIndicator);
            strs = *parent->GetTextByNumber(3 + 2*ChildIndicator);
        }
    }

    wxDataObject *dataptr  = 0;

    if ( HasSelectedBricks() )
    {
        if ( !m_nfc->GetFirstBrick() ) return;
        NassiBrick *first = (NassiBrick *)0,
                    *last = (NassiBrick *)0,
                     *tmp = (NassiBrick *)0;
        //check the active Bricks
        first = firstSelectedGBrick->GetBrick();
        last = firstSelectedGBrick->GetBrick();
        if ( !reverseSelected )
        {
            if ( lastSelectedGBrick )
                last = lastSelectedGBrick->GetBrick();
        }
        else
        {
            if ( lastSelectedGBrick )
                first = lastSelectedGBrick->GetBrick();
        }

        tmp = last->GetNext();
        last->SetNext( (NassiBrick *)0 );

        dataptr = new NassiDataObject(first, this, strc, strs);

        ///restore the original diagram
        if ( first && last && tmp )
            last->SetNext(tmp);
    }
    else
        dataptr = new NassiDataObject(0, this, strc, strs);

    if ( dataptr )
    {
        #if defined(__WXMSW__)
            wxCursor copycursor(dnd_copy_cur_xpm);
            wxCursor movecursor(dnd_move_cur_xpm);
            wxCursor nonecursor(dnd_none_cur_xpm);
        #else
            wxIcon copycursor(dnd_copy_cur_xpm);
            wxIcon movecursor(dnd_move_cur_xpm);
            wxIcon nonecursor(dnd_none_cur_xpm);
        #endif
        //wxDragResult result;

        wxDropSource dndSource(m_diagramwindow, copycursor, movecursor, nonecursor);
        dndSource.SetData(*dataptr);
        //dndSource.SetData(myData);

        ThisIsDnDSource = true;
        /*result = */dndSource.DoDragDrop(wxDrag_DefaultMove );
    }
    ThisIsDnDSource = false;

    DragPossible = false;

//    switch ( result )
//    {
//        case wxDragLink:
//        case wxDragCopy:
//            break;
//        case wxDragMove:
//            //DeleteSelection();
//            break;
//        default:
//            break;
//    }

}
void NassiView::OnDragLeave(void)
{}
void NassiView::OnDragEnter(void)
{}
wxDragResult NassiView::OnDrop(const wxPoint &pos, NassiBrick *brick, wxString strc, wxString strs, wxDragResult def)
{
    wxDragResult res = def;
    wxCommand *delcmd = 0, *addcmd = 0;
    if ( !m_nfc->GetFirstBrick() )
    {
        wxRect rect = GetEmptyRootRect();
        if ( brick && rect.Contains(pos) )
            addcmd = new NassiInsertFirstBrick(m_nfc, brick);
        else
            res = wxDragError;
    }
    else
    {
        GraphNassiBrick *gbrick = GetBrickAtPosition(pos);
        if ( gbrick )
        {
            GraphNassiBrick::Position p = gbrick->GetPosition(pos);
            if (brick && (p.pos == GraphNassiBrick::Position::bottom ))
                addcmd = new NassiInsertBrickAfter( m_nfc, gbrick->GetBrick(), brick );
            else if (brick && (p.pos == GraphNassiBrick::Position::top ))
                addcmd = new NassiInsertBrickBefore(m_nfc, gbrick->GetBrick(), brick );
            else if ( brick && ( p.pos == GraphNassiBrick::Position::child ))
                addcmd = new NassiInsertChildBrickCommand(m_nfc, gbrick->GetBrick(), brick, p.number);
            else if ( p.pos == GraphNassiBrick::Position::childindicator )
            {
                wxUint32 ChildAddNumber = p.number;
                if ( ThisIsDnDSource && def == wxDragMove && gbrick->HasActiveChildIndicator() &&
                     ChildAddNumber < gbrick->ActiveChildIndicator()
                   )
                        SelectChildIndicator(gbrick, gbrick->ActiveChildIndicator() + 1);
                addcmd = new NassiAddChildIndicatorCommand(m_nfc, gbrick->GetBrick(), brick, ChildAddNumber, strc, strs);
            }
            else
                res = wxDragError;
        }
        else
            res = wxDragError;
    }

    if ( addcmd  && def == wxDragMove )
        delcmd = Delete();

    if ( delcmd )
    {
        m_nfc->GetCommandProcessor()->Submit( new NassiMoveBrick( addcmd, delcmd) );
        ClearSelection();
    }
    else if ( addcmd )
         m_nfc->GetCommandProcessor()->Submit( addcmd );

    if ( def == wxDragError && ThisIsDnDSource )
        ClearSelection();

    m_diagramwindow->Refresh();

    return res;
}
HooverDrawlet *NassiView::OnDragOver(const wxPoint &pos, wxDragResult &def, bool HasNoBricks)
{
    if ( !m_nfc->GetFirstBrick() )
    {
        wxRect rect = GetEmptyRootRect();
        if ( rect.Contains(pos) )
            return new RedHatchDrawlet(rect);
        else
        {
            def = wxDragError;
            return 0;
        }
    }

    GraphNassiBrick *gbrick = GetBrickAtPosition(pos);
    if ( gbrick )
    {
        HooverDrawlet *drawlet = gbrick->GetDrawlet(pos, HasNoBricks);
        if ( drawlet == 0 ) def = wxDragError;
        return drawlet;
    }

    def = wxDragError;
    return 0;
}
void NassiView::ExportCSource()
{
    wxFileDialog dlg( m_diagramwindow, _("Choose a file to exporting into"), _T(""), _T(""), _("C sources (*.c)|*.c"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if ( dlg.ShowModal() != wxID_OK ) return;
    wxString filename = dlg.GetPath();
    if ( filename.empty() ) return;

    wxFFileOutputStream stream(filename);
    wxTextOutputStream text_stream( stream );
    text_stream << _T("{\n");
    ExportCSource(text_stream);
    text_stream << _T("}\n") << endl;
}

bool NassiView::ExportCSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    if ( !m_nfc->GetFirstBrick() ) return false;
    NassiBrick *first = (NassiBrick *)0,
                *last = (NassiBrick *)0,
                 *tmp = (NassiBrick *)0;

    if ( firstSelectedGBrick )
    {
        //check the active Bricks
        first = firstSelectedGBrick->GetBrick();
        last = firstSelectedGBrick->GetBrick();
        if ( !reverseSelected )
        {
            if ( lastSelectedGBrick )
                last = lastSelectedGBrick->GetBrick();
        }
        else
        {
            if ( lastSelectedGBrick )
                first = lastSelectedGBrick->GetBrick();
        }
    }
    else
    {
        first = m_nfc->GetFirstBrick();
        last = first;
        while ( last->GetNext() )
            last = last->GetNext();
    }

    tmp = last->GetNext();
    last->SetNext( (NassiBrick *)0 );

    if ( first ) first->SaveSource(text_stream, n);
    else return false;


    ///restore the original diagram
    if ( first && last && tmp )
        last->SetNext(tmp);

    return true;
}

void NassiView::ExportVHDLSource()
{

}
void NassiView::ExportStrukTeX()
{
    wxFileDialog dlg( m_diagramwindow, _("Choose a file to exporting into"), _T(""), _T(""), _("LaTeX files (*.tex)|*.tex"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
    if ( dlg.ShowModal() != wxID_OK ) return;
    wxString filename = dlg.GetPath();
    if ( filename.empty() ) return;

    if ( !m_nfc->GetFirstBrick() ) return;
    NassiBrick *first = (NassiBrick *)0,
                *last = (NassiBrick *)0,
                 *tmp = (NassiBrick *)0;

    if ( firstSelectedGBrick )
    {
        //check the active Bricks
        first = firstSelectedGBrick->GetBrick();
        last = firstSelectedGBrick->GetBrick();
        if ( !reverseSelected )
        {
            if ( lastSelectedGBrick )
                last = lastSelectedGBrick->GetBrick();
        }
        else
        {
            if ( lastSelectedGBrick )
                first = lastSelectedGBrick->GetBrick();
        }
    }
    else
    {
        first = m_nfc->GetFirstBrick();
        last = first;
        while ( last->GetNext() )
            last = last->GetNext();
    }

    tmp = last->GetNext();
    last->SetNext( (NassiBrick *)0 );

    // generate the code


    wxTextFile txtfile(filename);
    if ( txtfile.Exists() )
        txtfile.Open();
    else
        txtfile.Create();
    txtfile.Clear();
    wxString str;
    first->GenerateStrukTeX(str);

    while ( !str.IsEmpty() )
    {
        wxInt32 pos = str.Find('\n');
        if ( pos == -1 )
        {
            txtfile.AddLine(str);
            str.Empty();
        }
        else
        {
            txtfile.AddLine(str.SubString(0, pos-1 ));
            str = str.SubString(pos+1, str.Length());
        }
    }
    txtfile.Write();

    ///restore the original diagram
    if ( first && last && tmp )
        last->SetNext(tmp);
}
#if wxUSE_POSTSCRIPT
void NassiView::ExportPS()
{
    wxFileDialog dlg( m_diagramwindow, _("Choose a file to exporting into"), _T(""), _T(""), _("PostScript files (*.ps)|*.ps"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if ( dlg.ShowModal() != wxID_OK ) return;
    wxString filename = dlg.GetPath();
    if ( filename.empty() ) return;

    if ( !m_nfc->GetFirstBrick() ) return;
    NassiBrick *first = (NassiBrick *)0,
                *last = (NassiBrick *)0,
                 *tmp = (NassiBrick *)0;

    if ( firstSelectedGBrick )
    {
        //check the active Bricks
        first = firstSelectedGBrick->GetBrick();
        last = firstSelectedGBrick->GetBrick();
        if ( !reverseSelected )
        {
            if ( lastSelectedGBrick )
                last = lastSelectedGBrick->GetBrick();
        }
        else
        {
            if ( lastSelectedGBrick )
                first = lastSelectedGBrick->GetBrick();
        }
    }
    else
    {
        first = m_nfc->GetFirstBrick();
        last = first;
        while ( last->GetNext() )
            last = last->GetNext();
    }

    tmp = last->GetNext();
    last->SetNext( (NassiBrick *)0 );


    wxPrintData g_printData;
    g_printData.SetFilename(filename);
    wxPostScriptDC *psdc = new wxPostScriptDC(g_printData);

    psdc->StartDoc(_("Printing PS"));
    psdc->Clear();
    psdc->SetBackgroundMode(wxTRANSPARENT);



    BricksMap GraphBricks;
    GraphFabric *graphFabric = new GraphFabric(this, &GraphBricks);
    for ( NassiBricksCompositeIterator itr(first) ; !itr.IsDone() ; itr.Next())
    {
        NassiBrick *brk =  itr.CurrentItem();
        GraphBricks[brk] = graphFabric->CreateGraphBrick(brk);
    }

    wxPoint minsize(0,0);
    GraphNassiBrick *gbrick = GraphBricks[first];
    gbrick->CalcMinSize(psdc, minsize);
    gbrick->SetOffsetAndSize(psdc, wxPoint(0,0), minsize);

    psdc->SetPen(*wxBLACK_PEN);
    ///draw the diagram
    BricksMap::iterator it;
    for ( it = GraphBricks.begin() ; it != GraphBricks.end() ; it++)
        it->second->Draw(psdc);

    delete psdc;

    ///restore the original diagram
    if ( first && last && tmp )
        last->SetNext(tmp);

    while ( GraphBricks.size() )
    {
        BricksMap::iterator it = GraphBricks.begin();
        GraphNassiBrick *gbrick = it->second;
        if ( gbrick ) delete gbrick;
        GraphBricks.erase(it->first);
    }
    delete graphFabric;
}
#endif
#ifdef USE_SVG
void NassiView::ExportSVG()
{
    wxFileDialog dlg( m_diagramwindow, _("Choose a file to exporting into"),_T(""),_T(""),_("SVG files (*.SVG)|*.SVG"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
    if ( dlg.ShowModal() != wxID_OK ) return;
    wxString filename = dlg.GetPath();
    if ( filename.empty() ) return;


    if ( !m_nfc->GetFirstBrick() ) return;
    NassiBrick *first = (NassiBrick *)0,
                *last = (NassiBrick *)0,
                 *tmp = (NassiBrick *)0;

    if ( firstSelectedGBrick )
    {
        //check the active Bricks
        first = firstSelectedGBrick->GetBrick();
        last = firstSelectedGBrick->GetBrick();
        if ( !reverseSelected )
        {
            if ( lastSelectedGBrick )
                last = lastSelectedGBrick->GetBrick();
        }
        else
        {
            if ( lastSelectedGBrick )
                first = lastSelectedGBrick->GetBrick();
        }
    }
    else
    {
        first = m_nfc->GetFirstBrick();
        last = first;
        while ( last->GetNext() )
            last = last->GetNext();
    }

    tmp = last->GetNext();
    last->SetNext( (NassiBrick *)0 );


    wxSVGFileDC *svgdc = new wxSVGFileDC(filename, 10, 10);

    BricksMap GraphBricks;
    GraphFabric *graphFabric = new GraphFabric(this, &GraphBricks);
    for ( NassiBricksCompositeIterator itr(first) ; !itr.IsDone() ; itr.Next())
    {
        NassiBrick *brk =  itr.CurrentItem();
        GraphBricks[brk] = graphFabric->CreateGraphBrick(brk);
    }

    wxPoint minsize(0,0);
    GraphNassiBrick *gbrick = GraphBricks[first];
    gbrick->CalcMinSize(svgdc, minsize);
    gbrick->SetOffsetAndSize(svgdc, wxPoint(0,0), minsize);

    if ( svgdc )
        delete svgdc;
    svgdc = new wxSVGFileDC(filename, minsize.x, minsize.y);

    svgdc->SetPen(*wxBLACK_PEN);
    ///draw the diagram
    BricksMap::iterator it;
    for ( it = GraphBricks.begin() ; it != GraphBricks.end() ; it++)
        it->second->Draw(svgdc);

    delete svgdc;

    ///restore the original diagram
    if ( first && last && tmp )
        last->SetNext(tmp);

    while ( GraphBricks.size() )
    {
        BricksMap::iterator it = GraphBricks.begin();
        GraphNassiBrick *gbrick = it->second;
        if ( gbrick ) delete gbrick;
        GraphBricks.erase(it->first);
    }
    delete graphFabric;
}
#endif
void NassiView::ExportBitmap()
{
    wxFileDialog dlg( m_diagramwindow, _("Choose a file to exporting into"),_T(""),_T(""),_("PNG files (*.png)|*.png"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
    if ( dlg.ShowModal() != wxID_OK ) return;
    wxString filename = dlg.GetPath();
    if ( filename.empty() ) return;


    if ( !m_nfc->GetFirstBrick() ) return;
    NassiBrick *first = (NassiBrick *)0,
                *last = (NassiBrick *)0,
                 *tmp = (NassiBrick *)0;

    if ( firstSelectedGBrick )
    {
        //check the active Bricks
        first = firstSelectedGBrick->GetBrick();
        last = firstSelectedGBrick->GetBrick();
        if ( !reverseSelected )
        {
            if ( lastSelectedGBrick )
                last = lastSelectedGBrick->GetBrick();
        }
        else
        {
            if ( lastSelectedGBrick )
                first = lastSelectedGBrick->GetBrick();
        }
    }
    else
    {
        first = m_nfc->GetFirstBrick();
        last = first;
        while ( last->GetNext() )
            last = last->GetNext();
    }

    tmp = last->GetNext();
    last->SetNext( (NassiBrick *)0 );


    wxMemoryDC *memdc = new wxMemoryDC();

    BricksMap GraphBricks;
    GraphFabric *graphFabric = new GraphFabric(this, &GraphBricks);
    for ( NassiBricksCompositeIterator itr(first) ; !itr.IsDone() ; itr.Next())
    {
        NassiBrick *brk =  itr.CurrentItem();
        GraphBricks[brk] = graphFabric->CreateGraphBrick(brk);
    }

    wxPoint minsize(0,0);
    GraphNassiBrick *gbrick = GraphBricks[first];
    gbrick->CalcMinSize(memdc, minsize);
    gbrick->SetOffsetAndSize(memdc, wxPoint(0,0), minsize);

    wxBitmap bitmap(minsize.x, minsize.y);
    memdc->SelectObject(bitmap);
    memdc->SetPen(*wxBLACK_PEN);
    ///draw the diagram
    BricksMap::iterator it;
    for ( it = GraphBricks.begin() ; it != GraphBricks.end() ; it++)
        it->second->Draw(memdc);

    memdc->SelectObject(wxNullBitmap);
    delete memdc;
    bitmap.SaveFile(filename, wxBITMAP_TYPE_PNG); //wxBITMAP_TYPE_JPEG);


    ///restore the original diagram
    if ( first && last && tmp )
        last->SetNext(tmp);

    while ( GraphBricks.size() )
    {
        BricksMap::iterator it = GraphBricks.begin();
        GraphNassiBrick *gbrick = it->second;
        if ( gbrick ) delete gbrick;
        GraphBricks.erase(it->first);
    }
    delete graphFabric;
}
GraphNassiBrick *NassiView::GetGraphBrick(NassiBrick *brick)
{
    if ( m_GraphBricks.find(brick) == m_GraphBricks.end() )
        return (GraphNassiBrick *)0;
    return m_GraphBricks[brick];
}
GraphNassiBrick *NassiView::CreateGraphBrick(NassiBrick *brick)
{
    BricksMap::iterator it = m_GraphBricks.find(brick);
    if ( it != m_GraphBricks.end() )
    {
        GraphNassiBrick *gbrick = m_GraphBricks[brick];
        m_GraphBricks.erase(it);
        delete gbrick;
    }

    m_GraphBricks[brick] = m_graphFabric->CreateGraphBrick(brick);
    return m_GraphBricks[brick];
}
GraphNassiBrick *NassiView::GetBrickAtPosition(const wxPoint &pos)
{
    BricksMap::iterator it;
    for ( it = m_GraphBricks.begin() ; it != m_GraphBricks.end() ; it++)
    {

        GraphNassiBrick *gbrick = it->second;
        if ( gbrick->HasPoint(pos) )
            return gbrick;
    }
    return 0;
}
void NassiView::ShowCaret(bool show)
{
    wxCaret *caret = m_diagramwindow->GetCaret();
    if ( caret ) caret->Show( show );
}
bool NassiView::IsCaretVisible()
{
    wxCaret *caret = m_diagramwindow->GetCaret();
    if ( caret )
        return caret->IsVisible();
    return false;
}
void NassiView::MoveCaret(const wxPoint& pt)
{
    wxCaret *caret = m_diagramwindow->GetCaret();
    wxPoint unscrolledpoint;
    m_diagramwindow->CalcScrolledPosition( pt.x, pt.y, &unscrolledpoint.x, &unscrolledpoint.y);
    if ( caret ) caret->Move( unscrolledpoint );
}

