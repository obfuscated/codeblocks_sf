#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/dcmemory.h>
#include <wx/msgdlg.h>
#include <wx/mstream.h>
#include <wx/txtstrm.h>

//#include "CalcSizeVisitor.h"
//#include "DrawVisitor.h"
#include "bricks.h"
#include "DataObject.h"
#include "NassiView.h"
#include "GraphFabric.h"
#include "GraphBricks.h"

const wxChar * NassiDataObject::NassiFormatId = _T("NassiDataFormat");


NassiDataObject::NassiDataObject(NassiBrick *brick, NassiView *view, wxString strc, wxString strs):
    m_format(),
    m_dobjBitmap(),
    m_firstbrick(0),
    m_hasBitmap(false),
    m_caseSource(strs),
    m_caseComment(strc)
{
    if ( brick )
    {
        ///generate the bitmap (the things on the view can change until the
        ///image is used)

        wxMemoryDC *memdc = new wxMemoryDC();

        BricksMap GraphBricks;
        GraphFabric *graphFabric = new GraphFabric(view, &GraphBricks);

        NassiBrick *firstbrick = brick;
        for ( NassiBricksCompositeIterator itr(firstbrick) ; !itr.IsDone() ; itr.Next())
        {
            NassiBrick *brk =  itr.CurrentItem();
            GraphBricks[brk] = graphFabric->CreateGraphBrick(brk);
        }

        wxPoint minsize(0,0);
        GraphNassiBrick *gbrick = GraphBricks[firstbrick];
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
        m_dobjBitmap.SetBitmap(bitmap);
        m_hasBitmap = true;

        while ( GraphBricks.size() )
        {
            BricksMap::iterator it = GraphBricks.begin();
            GraphNassiBrick *gbrick = it->second;
            if ( gbrick ) delete gbrick;
            GraphBricks.erase(it->first);
        }
        delete graphFabric;

        ///copy the structure
        m_firstbrick = brick->Clone();
    }
    else
    {
        m_firstbrick = (NassiBrick *)0;
        m_hasBitmap = false;
    }
    m_format.SetId(NassiFormatId);
}
NassiDataObject::~NassiDataObject()
{
    delete m_firstbrick;
}
NassiBrick *NassiDataObject::GetBrick(void)
{
    NassiBrick* ptr = m_firstbrick;
    m_firstbrick = 0;
    return ptr;
}
wxString NassiDataObject::GetText(wxInt32 n)
{
    if ( n == 0 )
        return m_caseComment;
    return m_caseSource;
}
wxDataFormat NassiDataObject::GetPreferredFormat(Direction /*dir*/) const
{
    return m_format;
}
size_t NassiDataObject::GetFormatCount(Direction dir) const
{
    size_t nFormats = 1;
    if ( dir == Get && m_hasBitmap )
        nFormats += m_dobjBitmap.GetFormatCount(dir);
    return nFormats;
}
void NassiDataObject::GetAllFormats(wxDataFormat *formats, Direction dir) const
{
    formats[0] = m_format;
    if ( dir == Get && m_hasBitmap )
        m_dobjBitmap.GetAllFormats(&formats[1], dir);
}
size_t NassiDataObject::GetDataSize(const wxDataFormat& format) const
{
    if ( format == m_format )
    {
        wxMemoryOutputStream memstream;
        NassiBrick::SerializeString(memstream, m_caseComment);
        NassiBrick::SerializeString(memstream, m_caseSource);
        if ( m_firstbrick )
            m_firstbrick->Serialize( memstream );

        if ( memstream.GetLength() != wxInvalidOffset )
            return( memstream.GetLength() );

        return 0;
    }
    if ( m_hasBitmap )
    {
        if ( m_dobjBitmap.IsSupported(format) )
            return m_dobjBitmap.GetDataSize();
    }
    return 0;
}
bool NassiDataObject::SetData(const wxDataFormat& format, size_t len, const void *buf)
{
    if ( format != m_format )
        return false ;
    delete m_firstbrick;
    ///
    wxMemoryInputStream memstream((char *)buf, len);
    NassiBrick::DeserializeString(memstream, m_caseComment);
    NassiBrick::DeserializeString(memstream, m_caseSource);
    m_firstbrick = NassiBrick::SetData(memstream);
    ///
    m_hasBitmap = false;
    return( true );
}
bool NassiDataObject::GetDataHere(const wxDataFormat& format, void *pBuf) const
{
    if ( format == m_format )
    {
        wxMemoryOutputStream memstream;
        NassiBrick::SerializeString(memstream, m_caseComment);
        NassiBrick::SerializeString(memstream, m_caseSource);
        if ( m_firstbrick )
            ///serialize bricks into memorystream
            m_firstbrick->Serialize( memstream );

        ///copy the data
        wxInt32 len =  memstream.CopyTo((char *)pBuf, memstream.GetLength() );

        ///check that copied data has the same length as the data in the memorystream
        if ( len == memstream.GetLength() )
            return true;
        else
            return false;
    }


    if ( m_hasBitmap )
        return m_dobjBitmap.GetDataHere(pBuf);


    return false;
}
bool NassiDataObject::HasBrick()
{
    return m_firstbrick != 0;
}
