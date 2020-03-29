#include "NassiFileContent.h"
#include "bricks.h"

#include <wx/intl.h>

NassiFileContent::NassiFileContent():
m_firstbrick(0)
{
}

NassiFileContent::~NassiFileContent()
{
    if ( m_firstbrick )
        delete m_firstbrick;
}

wxOutputStream& NassiFileContent::SaveObject(wxOutputStream& stream)
{
    if ( m_firstbrick ) m_firstbrick->Serialize(stream);
    return stream;
}

wxInputStream& NassiFileContent::LoadObject(wxInputStream& stream)
{
    if ( m_firstbrick ) delete m_firstbrick;

    m_firstbrick = NassiBrick::SetData(stream);

    return stream;
}

wxString NassiFileContent::GetWildcard()
{
    return _("Nassi Shneiderman diagram, (*.nsd)|*.nsd");
}

NassiBrick *NassiFileContent::GetFirstBrick()
{
    return m_firstbrick;
}

NassiBrick *NassiFileContent::SetFirstBrick(NassiBrick *brick)
{
    NassiBrick *tmp = m_firstbrick;
    m_firstbrick = brick;
    if ( m_firstbrick )
    {
        m_firstbrick->SetParent(nullptr);
        m_firstbrick->SetPrevious(nullptr);
    }
    return tmp;
}

