#include "sdk_precomp.h"
#include <wx/file.h>
#include "encodingdetector.h"

EncodingDetector::EncodingDetector(const wxString& filename)
    : m_IsOK(false),
    m_UseBOM(false),
    m_BOMSizeInBytes(0),
    m_Encoding(wxFONTENCODING_ISO8859_1)
{
    m_IsOK = DetectEncoding(filename);
}

EncodingDetector::EncodingDetector(const EncodingDetector& rhs)
    : m_IsOK(rhs.m_IsOK),
    m_UseBOM(rhs.m_UseBOM),
    m_BOMSizeInBytes(rhs.m_BOMSizeInBytes),
    m_Encoding(rhs.m_Encoding)
{

}

EncodingDetector::~EncodingDetector()
{
}

bool EncodingDetector::IsOK() const
{
    return m_IsOK;
}

bool EncodingDetector::UsesBOM() const
{
    return m_UseBOM;
}

int EncodingDetector::GetBOMSizeInBytes() const
{
    return m_BOMSizeInBytes;
}

wxFontEncoding EncodingDetector::GetFontEncoding() const
{
    return m_Encoding;
}

bool EncodingDetector::DetectEncoding(const wxString& filename)
{
    // Simple BOM detection
    wxFile file(filename);
    if (!file.IsOpened())
        return false;

    // BOM is max 4 bytes
    char buff[4] = {};
    file.Read((void*)buff, 4);
    file.Close();

    if (memcmp(buff, "\xEF\xBB\xBF", 3) == 0)
    {
        m_UseBOM = true;
        m_BOMSizeInBytes = 3;
        m_Encoding = wxFONTENCODING_UTF8;
    }
    else if (memcmp(buff, "\xFE\xFF", 2) == 0)
    {
        m_UseBOM = true;
        m_BOMSizeInBytes = 2;
        m_Encoding = wxFONTENCODING_UTF16BE;
    }
    else if (memcmp(buff, "\xFF\xFE", 2) == 0)
    {
        m_UseBOM = true;
        m_BOMSizeInBytes = 2;
        m_Encoding = wxFONTENCODING_UTF16LE;
    }
    else if (memcmp(buff, "\x00\x00\xFE\xFF", 4) == 0)
    {
        m_UseBOM = true;
        m_BOMSizeInBytes = 4;
        m_Encoding = wxFONTENCODING_UTF32BE;
    }
    else if (memcmp(buff, "\xFF\xFE\x00\x00", 4) == 0)
    {
        m_UseBOM = true;
        m_BOMSizeInBytes = 4;
        m_Encoding = wxFONTENCODING_UTF32LE;
    }

    return true;
}
