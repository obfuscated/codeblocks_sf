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
    else
    {
        return true;

//-------------------------------------------------------------------//
// The code below is currently ignored because it's missing          //
// a conversion to utf8, in case of utf16, so scintilla can show it. //
//-------------------------------------------------------------------//

        // try guessing by scanning the buffer
        file.Open(filename);

        // read the file's contents in a buffer
        size_t len = file.Length();
        unsigned char* buff = new unsigned char[len + 1];
        memset(buff, 0, len + 1);
        file.Read((void*)buff, len);
        file.Close();

        // detection code copied from Notepad++ (notepad-plus.sf.net)
        bool rv = true;
        bool ASCII7only = true;
        unsigned char* sx	= (unsigned char*)buff;
        unsigned char* endx	= sx + len;
        while (sx < endx)
        {
            if (!*sx)
            { // For detection, we'll say that NUL means not UTF8
                ASCII7only = false;
                rv  = false;
                break;
            }
            else if (*sx < 0x80)
            { // 0nnnnnnn If the byte's first hex code begins with 0-7, it is an ASCII character.
                sx++;
            }
            else if (*sx < (0x80 + 0x40))
            { // 10nnnnnn 8 through B cannot be first hex codes
                ASCII7only = false;
                rv = false;
                break;
            }
            else if (*sx < (0x80 + 0x40 + 0x20))
            { // 110xxxvv 10nnnnnn  If it begins with C or D, it is an 11 bit character
                ASCII7only = false;
                if (sx >= endx - 1)
                    break;
                if (!(*sx & 0x1F) || (sx[1]&(0x80+0x40)) != 0x80)
                {
                    rv = false;
                    break;
                }
                sx += 2;
            }
            else if (*sx < (0x80 + 0x40 + 0x20 + 0x10))
            { // 1110qqqq 10xxxxvv 10nnnnnn If it begins with E, it is 16 bit
                ASCII7only = false;
                if (sx >= endx - 2)
                    break;
                if (!(*sx & 0xF) || (sx[1]&(0x80+0x40)) != 0x80 || (sx[2]&(0x80+0x40)) != 0x80)
                {
                    rv = false;
                    break;
                }
                sx += 3;
            }
            else
            { // more than 16 bits are not allowed here
                ASCII7only = false;
                rv = false;
                break;
            }
        }

        // done with the buffer
        delete[] buff;

        m_UseBOM = false;
        m_BOMSizeInBytes = 0;
        if (ASCII7only || rv)
            m_Encoding = wxFONTENCODING_UTF8;
        else
            m_Encoding = wxFONTENCODING_UTF16;
    }

    return true;
}
