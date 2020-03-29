#ifndef JUMPDATA_H
#define JUMPDATA_H
// ----------------------------------------------------------------------------
//  JumpData.h
// ----------------------------------------------------------------------------
#include "wx/string.h"
// ----------------------------------------------------------------------------
class JumpData
// ----------------------------------------------------------------------------
{
    public:
        JumpData(const wxString& filename, const long posn);
        ~JumpData();
        wxString& GetFilename() {return m_Filename;}
        long GetPosition() {return m_Posn;}
        void SetFilename(const wxString& filename) {m_Filename = filename;}
        void SetPosition(const long posn) { m_Posn = posn;}
    protected:
    private:
        JumpData();
        wxString m_Filename;
        long     m_Posn;
};

#endif // JUMPDATA_H
