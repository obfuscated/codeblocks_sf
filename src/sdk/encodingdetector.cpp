#include "sdk_precomp.h"
#ifndef CB_PRECOMP
#include <wx/file.h>
#include <wx/string.h>
#endif // CB_PRECOMP


#include "encodingdetector.h"
#include "filemanager.h"


EncodingDetector::EncodingDetector(const wxString& filename)
    : m_IsOK(false),
    m_UseBOM(false),
    m_BOMSizeInBytes(0),
    m_ConvStr(wxEmptyString)
{
    m_Encoding = wxLocale::GetSystemEncoding();
    m_IsOK = DetectEncoding(filename);
}

EncodingDetector::EncodingDetector(LoaderBase* fileLdr)
    : m_IsOK(false),
    m_UseBOM(false),
    m_BOMSizeInBytes(0),
    m_ConvStr(wxEmptyString)
{
    m_Encoding = wxLocale::GetSystemEncoding();
    m_IsOK = DetectEncoding((wxByte*)fileLdr->GetData(), fileLdr->GetLength());
}

EncodingDetector::EncodingDetector(const wxByte* buffer, size_t size)
    : m_IsOK(false),
    m_UseBOM(false),
    m_BOMSizeInBytes(0),
    m_ConvStr(wxEmptyString)
{
    m_Encoding = wxLocale::GetSystemEncoding();
    m_IsOK = DetectEncoding(buffer, size);
}

EncodingDetector::EncodingDetector(const EncodingDetector& rhs)
    : m_IsOK(rhs.m_IsOK),
    m_UseBOM(rhs.m_UseBOM),
    m_BOMSizeInBytes(rhs.m_BOMSizeInBytes),
    m_Encoding(rhs.m_Encoding),
    m_ConvStr(rhs.m_ConvStr)
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

wxString EncodingDetector::GetWxStr() const
{
    return m_ConvStr;
}

bool EncodingDetector::ConvertToWxStr(const wxByte* buffer, size_t size)
{
    if (!buffer || size == 0)
        return false;

    if (m_BOMSizeInBytes > 0)
    {
        for (int i = 0; i < m_BOMSizeInBytes; ++i)
            *buffer++;
    }

    size_t outlen = 0, nullBytes = 1, bufferLen = size - m_BOMSizeInBytes;
    wxCSConv conv(m_Encoding);
    nullBytes = conv.GetMBNulLen();
    if (nullBytes == wxCONV_FAILED)
        nullBytes = 1;

    char* newBuffer = (char*) malloc(bufferLen + nullBytes);
    if (!newBuffer)
        return false;

    memcpy(newBuffer, buffer, bufferLen);
    for (size_t i = 0; i < nullBytes; ++i)
        newBuffer[bufferLen + i] = '\0';
    wxWCharBuffer wideBuff = conv.cMB2WC((char*)newBuffer, bufferLen + nullBytes, &outlen);
    m_ConvStr = wxString(wideBuff);

    free(newBuffer);

    if (outlen == 0)
        return false;

    return true;
}

bool EncodingDetector::DetectEncoding(const wxString& filename, bool ConvertToWxString)
{
    wxFile file(filename);
    if (!file.IsOpened())
        return false;

    size_t size = file.Length();
    if (size == 0)
    {
        file.Close();
        return false;
    }

    wxByte* buffer = (wxByte*) malloc(sizeof(wxByte) * (size + 1));
    if (!buffer)
    {
        file.Close();
        return false;
    }

    size_t readBytes = file.Read((void*)buffer, size);
    bool result = false;
    if (readBytes > 0)
    {
        result = DetectEncoding(buffer, size, ConvertToWxString);
    }

    file.Close();
    free(buffer);
    return result;
}

bool EncodingDetector::DetectEncoding(const wxByte* buffer, size_t size, bool ConvertToWxString)
{
    if (!buffer)
        return false;
    if (size >= 4)
    {
        // BOM is max 4 bytes
        char buff[4] = {'\0'};
        memcpy(buff, buffer, 4);

        if (memcmp(buff, "\xEF\xBB\xBF", 3) == 0)
        {
            m_UseBOM = true;
            m_BOMSizeInBytes = 3;
            m_Encoding = wxFONTENCODING_UTF8;
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
    }


    if (!m_UseBOM)
    {
        if (DetectUTF8((wxByte*)buffer, size))
        {
            m_Encoding = wxFONTENCODING_UTF8;
        }
        else if (!DetectUTF16((wxByte*)buffer, size) && !DetectUTF32((wxByte*)buffer, size))
        {
            #ifdef __WXMSW__
            m_Encoding = wxLocale::GetSystemEncoding();
            #else
            /* On Linux, system encoding is UTF-8. But as our code didn't
            detect UTF-8, we'll set it to ansi. */
            m_Encoding = wxFONTENCODING_ISO8859_1;
            #endif
        }

        m_UseBOM = false;
        m_BOMSizeInBytes = 0;
    }

    if (ConvertToWxString)
    {
        ConvertToWxStr(buffer, size);
    }

    return true;
} // end of DetectEncoding

bool EncodingDetector::DetectUTF8(wxByte *byt, size_t size)
{
    /* The following detection code is based on modified code
    *  of MadEdit
    */
    size_t i = 0;

    if (!byt)
        return false;

    while (i < size)
    {
        if (byt[i] < 0x80)
        {
            ++i; // Continue searching for any possible UTF-8 encoded characters
        }
        else if (byt[i] <= 0xDF) // 1110xxxx 10xxxxxx
        {
            if (++i < size && IsUTF8Tail(byt[i]))
            {
                return true; // We would stop our search assuming the whole file is UTF-8
            }
            else if (i != size) // Possibly a malformed UTF-8 file
            {
                return false;
            }
        }
        else if (byt[i] <= 0xEF) // 1110xxxx 10xxxxxx 10xxxxxx
        {
            if ((++i < size && IsUTF8Tail(byt[i]))
                && (++i < size && IsUTF8Tail(byt[i])))
            {
                return true; // We would stop our search assuming the whole file is UTF-8
            }
            else if(size != i)
            {
                return false;
            }
        }
        else if (byt[i] <= 0xF4) // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
            if ((++i < size && IsUTF8Tail(byt[i]))
                && (++i < size && IsUTF8Tail(byt[i]))
                && (++i < size && IsUTF8Tail(byt[i])))
            {
                return true;
            }
            else if(size != i)
            {
                return false;
            }
        }
        /* Begin: Extra code (not from MadEdit) */
        else if (byt[i] <= 0xFB)  // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
            if ((++i < size && IsUTF8Tail(byt[i]))
                && (++i < size && IsUTF8Tail(byt[i]))
                && (++i < size && IsUTF8Tail(byt[i]))
                && (++i < size && IsUTF8Tail(byt[i])))
            {
                return true;
            }
            else if(size != i)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        /* End: Extra code (not from MadEdit) */
    }

    return false;
}

/* ==============================================
*  Begin: Detection Code from MadEdit
*         Modified to suit C::B
*  ---------------------------------------------- */
bool EncodingDetector::IsTextUTF16LE(wxByte *text, size_t size)
{
    if (size < 2)
        return false;

    bool ok = false;
    bool highsurrogate = false;

    size = size & 0x1FFFFFFE;   // to even
    while(size > 0)
    {
        if(text[1] == 0)
        {
            if(text[0] == 0)
                return false;
            ok = true;
        }
        else if(text[1] >= 0xD8 && text[1] <= 0xDB)
        {
            if(highsurrogate)
                return false;
            highsurrogate = true;
        }
        else if(text[1] >= 0xDC && text[1] <= 0xDF)
        {
            if(!highsurrogate)
                return false;
            highsurrogate = false;
        }

        size -= 2;
        text += 2;
    }

    return ok;
}

bool EncodingDetector::IsTextUTF16BE(wxByte *text, size_t size)
{
    if (size < 2)
        return false;

    bool ok = false;
    bool highsurrogate = false;

    size = size & 0x1FFFFFFE;     // to even

    while(size > 0)
    {
        if(text[0] == 0)
        {
            if(text[1] == 0)
                return false;
            ok = true;
        }
        else if(text[0] >= 0xD8 && text[0] <= 0xDB)
        {
            if(highsurrogate)
                return false;
            highsurrogate = true;
        }
        else if(text[0] >= 0xDC && text[0] <= 0xDF)
        {
            if(!highsurrogate)
                return false;
            highsurrogate = false;
        }

        size -= 2;
        text += 2;
    }

    return ok;
}
/* ----------------------------------------------
*  End: Detection Code from MadEdit
*       Modified to suit C::B
*  ============================================== */

bool EncodingDetector::DetectUTF16(wxByte *byt, size_t size)
{
    if (IsTextUTF16BE(byt, size))
    {
        m_Encoding = wxFONTENCODING_UTF16BE;
        return true;
    }
    else if (IsTextUTF16LE(byt, size))
    {
        m_Encoding = wxFONTENCODING_UTF16LE;
        return true;
    }
    return false;
}


/* ==============================================
*  Begin: Detection Code from MadEdit
*         Modified to suit C::B
*  ---------------------------------------------- */
bool EncodingDetector::IsTextUTF32LE(wxByte *text, size_t size)
{
    size >>= 2;   // to count
    if (size < 4)
        return false;

    wxUint32 ucs4, *p=(wxUint32 *)text;

    for(size_t i = 0; i < size; i++, p++)
    {
        ucs4 = wxINT32_SWAP_ON_BE(*p);

        if(ucs4 <= 0 || ucs4 > 0x10FFFF)
            return false;
    }

    return true;
}

bool EncodingDetector::IsTextUTF32BE(wxByte *text, size_t size)
{
    size >>= 2;   // to count
    if (size < 4)
        return false;

    wxUint32 ucs4, *p = (wxUint32 *)text;

    for (size_t i = 0; i < size; i++, p++)
    {
        ucs4 = wxINT32_SWAP_ON_LE(*p);

        if (ucs4 <= 0 || ucs4 > 0x10FFFF)
            return false;
    }

    return true;
}
/* ----------------------------------------------
*  End: Detection Code from MadEdit
*       Modified to suit C::B
*  ============================================== */

bool EncodingDetector::DetectUTF32(wxByte *byt, size_t size)
{
    if (IsTextUTF32BE(byt, size))
    {
        m_Encoding = wxFONTENCODING_UTF32BE;
        return true;
    }
    else if (IsTextUTF32LE(byt, size))
    {
        m_Encoding = wxFONTENCODING_UTF32LE;
        return true;
    }
    return false;
}
