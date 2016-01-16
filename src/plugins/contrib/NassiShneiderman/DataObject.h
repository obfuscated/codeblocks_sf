#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __DATAOBJECT_H__
#define __DATAOBJECT_H__

#include <wx/dataobj.h>


class NassiView;
class NassiBrick;

class NassiDataObject : public wxDataObject
{
public:
    NassiDataObject(NassiBrick *brick, NassiView *view, wxString strc = _T("X"), wxString strs = _T("case :") );
    virtual ~NassiDataObject();
    NassiBrick *GetBrick(void);
    wxString GetText(wxInt32 n = 0);
    virtual wxDataFormat GetPreferredFormat(Direction dir) const;
    virtual size_t GetFormatCount(Direction dir) const;
    virtual void GetAllFormats(wxDataFormat *formats, Direction dir) const;
    virtual size_t GetDataSize(const wxDataFormat& format) const;
    virtual bool SetData(const wxDataFormat& format, size_t len, const void *buf);
    virtual bool GetDataHere(const wxDataFormat& format, void *pBuf)const;
    bool HasBrick();

	static const wxChar * NassiFormatId;

private:
    NassiDataObject(const NassiDataObject &p);
    NassiDataObject &operator=(const NassiDataObject &rhs);
private:
    wxDataFormat         m_format;
    wxBitmapDataObject   m_dobjBitmap;
    NassiBrick           *m_firstbrick;
    bool                 m_hasBitmap;
    wxString             m_caseSource, m_caseComment;
};

#endif

