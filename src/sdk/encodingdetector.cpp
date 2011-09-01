/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"
#ifndef CB_PRECOMP
    #include <wx/fontmap.h>
    #include <wx/file.h>
    #include <wx/string.h>
    #include "manager.h"
    #include "logmanager.h"
    #include "configmanager.h"
#endif // CB_PRECOMP


#include "encodingdetector.h"
#include "filemanager.h"
#include "nsUniversalDetector.h"
#include <wx/encconv.h>

EncodingDetector::EncodingDetector(const wxString& filename, bool useLog)
        : nsUniversalDetector(NS_FILTER_ALL),
        m_IsOK(false),
    m_UseBOM(false),
    m_UseLog(useLog),
    m_BOMSizeInBytes(0),
    m_ConvStr(wxEmptyString)
{
    m_Encoding = wxLocale::GetSystemEncoding();
    m_IsOK = DetectEncoding(filename);
}

EncodingDetector::EncodingDetector(LoaderBase* fileLdr, bool useLog)
        : nsUniversalDetector(NS_FILTER_ALL),
        m_IsOK(false),
    m_UseBOM(false),
    m_UseLog(useLog),
    m_BOMSizeInBytes(0),
    m_ConvStr(wxEmptyString)
{
    m_Encoding = wxLocale::GetSystemEncoding();
    m_IsOK = DetectEncoding((wxByte*)fileLdr->GetData(), fileLdr->GetLength());
}

EncodingDetector::EncodingDetector(const wxByte* buffer, size_t size, bool useLog)
        : nsUniversalDetector(NS_FILTER_ALL),
        m_IsOK(false),
    m_UseBOM(false),
    m_UseLog(useLog),
    m_BOMSizeInBytes(0),
    m_ConvStr(wxEmptyString)
{
    m_Encoding = wxLocale::GetSystemEncoding();
    m_IsOK = DetectEncoding(buffer, size);
}

//EncodingDetector::EncodingDetector(const EncodingDetector& rhs)
//    : m_IsOK(rhs.m_IsOK),
//    m_UseBOM(rhs.m_UseBOM),
//    m_BOMSizeInBytes(rhs.m_BOMSizeInBytes),
//    m_Encoding(rhs.m_Encoding),
//    m_ConvStr(rhs.m_ConvStr)
//{
//
//}

EncodingDetector::~EncodingDetector()
{
}

void EncodingDetector::Report(const char *aCharset)
{
    mResult = cbC2U(aCharset);
    if (mResult == _T("gb18030")) // hack, because wxWidgets only knows cp936
        mResult = _T("cp936");
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

const wxString& EncodingDetector::DoIt(const char* aBuf,
                                       PRUint32 aLen)
{
    this->Reset();
    nsresult rv = this->HandleData(aBuf, aLen);
    if (NS_FAILED(rv))
        mResult=_T("failed");
    else
        this->DataEnd();
    return mResult;
}

bool EncodingDetector::ConvertToWxStr(const wxByte* buffer, size_t size)
{
    LogManager* logmgr = Manager::Get()->GetLogManager();
    wxString    logmsg;

    if (!buffer || size == 0)
    {
        if(m_UseLog)
        {
            logmsg.Printf(_T("Encoding conversion has failed (buffer is empty)!"));
            logmgr->DebugLog(logmsg);
        }
        return false;
    }

    if (m_BOMSizeInBytes > 0)
    {
        for (int i = 0; i < m_BOMSizeInBytes; ++i)
            buffer++;
    }

    size_t outlen = 0;

/* NOTE (Biplab#5#): FileManager returns a buffer with 4 extra NULL chars appended.
    But the buffer size is returned sans the NULL chars */

    wxWCharBuffer wideBuff;

    // if possible use the special conversion-routines, they are much faster than wxCSCov (at least on linux)
    if ( m_Encoding == wxFONTENCODING_UTF7 )
    {
        wxMBConvUTF7 conv;
        wideBuff = conv.cMB2WC((char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else if ( m_Encoding == wxFONTENCODING_UTF8 )
    {
        wxMBConvUTF8 conv;
        wideBuff = conv.cMB2WC((char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else if ( m_Encoding == wxFONTENCODING_UTF16BE )
    {
        wxMBConvUTF16BE conv;
        wideBuff = conv.cMB2WC((char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else if ( m_Encoding == wxFONTENCODING_UTF16LE )
    {
        wxMBConvUTF16LE conv;
        wideBuff = conv.cMB2WC((char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else if ( m_Encoding == wxFONTENCODING_UTF32BE )
    {
        wxMBConvUTF32BE conv;
        wideBuff = conv.cMB2WC((char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else if ( m_Encoding == wxFONTENCODING_UTF32LE )
    {
        wxMBConvUTF32LE conv;
        wideBuff = conv.cMB2WC((char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else
    {
        // try wxEncodingConverter first, even it it only works for
        // wxFONTENCODING_ISO8859_1..15, wxFONTENCODING_CP1250..1257 and wxFONTENCODING_KOI8
        // but it's much, much faster than wxCSConv (at least on linux)
        wxEncodingConverter conv;
        wchar_t* tmp = new wchar_t[size + 4 - m_BOMSizeInBytes];
        if(   conv.Init(m_Encoding, wxFONTENCODING_UNICODE)
		   && conv.Convert((char*)buffer, tmp) )
        {
            wideBuff = tmp;
            outlen = size + 4 - m_BOMSizeInBytes; // should be correct, because Convert has returned true
        }
        else
        {
            // try wxCSConv, if nothing else works
            wxCSConv conv(m_Encoding);
            wideBuff = conv.cMB2WC((char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
        }
        delete [] tmp;
    }

    m_ConvStr = wxString(wideBuff);

    if (outlen == 0)
    {
        if(m_UseLog)
        {
            logmsg.Printf(_T("Encoding conversion using settings has failed!\n"
                             "Encoding choosen was: %s (ID: %d)"),
                          wxFontMapper::Get()->GetEncodingDescription(m_Encoding).c_str(),
                          m_Encoding);
            logmgr->DebugLog(logmsg);
        }

        // Try system locale (if requested by the settings)
        ConfigManager* cfgMgr = Manager::Get()->GetConfigManager(_T("editor"));
        if (cfgMgr->ReadBool(_T("/default_encoding/use_system"), true))
        {
            // Conversion has failed. Let's try with system-default encoding.
            if (platform::windows)
            {
                if(m_UseLog)
                {
                    logmgr->DebugLog(_T("Trying system locale as fallback..."));
                }
                m_Encoding = wxLocale::GetSystemEncoding();
            }
            else
            {
                // We can rely on the UTF-8 detection code ;-)
                if(m_UseLog)
                {
                    logmgr->DebugLog(_T("Trying ISO-8859-1 as fallback..."));
                }
                m_Encoding = wxFONTENCODING_ISO8859_1;
            }

            wxCSConv conv_system(m_Encoding);
            wideBuff = conv_system.cMB2WC((char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
            m_ConvStr = wxString(wideBuff);

            if (outlen == 0)
            {
                if(m_UseLog)
                {
                    logmsg.Printf(_T("Encoding conversion using system locale fallback has failed!\n"
                                     "Last encoding choosen was: %s (ID: %d)\n"
                                     "Don't know what to do."),
                                  wxFontMapper::Get()->GetEncodingDescription(m_Encoding).c_str(),
                                  m_Encoding);
                    logmgr->DebugLog(logmsg);
                }
                return false;
            }
        }
        else
        {
            return false;
        }
    }

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

    wxByte* buffer = (wxByte*) malloc(sizeof(wxByte) * (size + 4));
    if (!buffer)
    {
        file.Close();
        return false;
    }
    buffer[size + 0] = 0;
    buffer[size + 1] = 0;
    buffer[size + 2] = 0;
    buffer[size + 3] = 0;

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
    ConfigManager* cfgMgr = Manager::Get()->GetConfigManager(_T("editor"));
    wxString encname = cfgMgr->Read(_T("/default_encoding"));

    if (cfgMgr->ReadInt(_T("/default_encoding/use_option"), 0) == 1)
    {
        // Bypass C::B's auto-detection
        m_Encoding = wxFontMapper::Get()->CharsetToEncoding(encname, false);

        if(m_UseLog)
        {
            wxString msg;
            msg.Printf(_T("Warning: bypassing C::B's auto-detection!\n"
                          "Encoding requested is: %s (ID: %d)"),
                       wxFontMapper::Get()->GetEncodingDescription(m_Encoding).c_str(),
                       m_Encoding);
            Manager::Get()->GetLogManager()->DebugLog(msg);
        }
    }
    else
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
            else if (memcmp(buff, "\x00\x00\xFF\xFE", 4) == 0)
            {
            // 00 00 FF FE  UCS-4, unusual octet order BOM (2143)
            // X-ISO-10646-UCS-4-2143 can not (yet) be handled by wxWidgets
                m_Encoding = (wxFontEncoding)-1;
            }
            else if (memcmp(buff, "\xFF\xFE\x00\x00", 4) == 0)
            {
                m_UseBOM = true;
                m_BOMSizeInBytes = 4;
                m_Encoding = wxFONTENCODING_UTF32LE;
            }
            else if (memcmp(buff, "\xFE\xFF\x00\x00", 4) == 0)
            {
            // FE FF 00 00  UCS-4, unusual octet order BOM (3412)
            // X-ISO-10646-UCS-4-3412 can not (yet) be handled by wxWidgets
                m_Encoding = (wxFontEncoding)-1;
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


        if (m_UseBOM)
        {
            if(m_UseLog)
            {
                wxString msg;
                msg.Printf(_T("Detected encoding via BOM: %s (ID: %d)"),
                           wxFontMapper::Get()->GetEncodingDescription(m_Encoding).c_str(),
                           m_Encoding);
                Manager::Get()->GetLogManager()->DebugLog(msg);
            }
        }
        // Try our own detection for UTF-16 and UTF-32, the mozilla-version does not work without BOM
        else if ( !DetectUTF16((wxByte*)buffer, size) &&
                  !DetectUTF32((wxByte*)buffer, size) )
            {
            // if we still have no results try mozilla's detection
            m_Encoding = wxFontMapper::Get()->CharsetToEncoding(DoIt((char*)buffer, size), false);
            if(m_Encoding == wxFONTENCODING_DEFAULT)
            {
                wxString enc_name = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/default_encoding"), wxLocale::GetSystemEncodingName());
                m_Encoding = wxFontMapper::GetEncodingFromName(enc_name);
                if(m_UseLog)
                {
                    wxString msg;
                    msg.Printf(_T("Text seems to be pure ASCII!\n"
                                  "We use user specified encoding: %s (ID: %d)"),
                               wxFontMapper::Get()->GetEncodingDescription(m_Encoding).c_str(),
                               m_Encoding);
                    Manager::Get()->GetLogManager()->DebugLog(msg);
                }
            }
            if (m_Encoding < 0)
            {
                // Use user-specified one; as a fallback
                m_Encoding = wxFontMapper::Get()->CharsetToEncoding(encname, false);

                if(m_UseLog)
                {
                    wxString msg;
                    msg.Printf(_T("Warning: Using user specified encoding as fallback!\n"
                                  "Encoding fallback is: %s (ID: %d)"),
                               wxFontMapper::Get()->GetEncodingDescription(m_Encoding).c_str(),
                               m_Encoding);
                    Manager::Get()->GetLogManager()->DebugLog(msg);
                }
            }

            m_UseBOM = false;
            m_BOMSizeInBytes = 0;
        }
    }

    if(m_UseLog)
    {
        wxString msg;
        msg.Printf(_T("Final encoding detected: %s (ID: %d)"),
                   wxFontMapper::Get()->GetEncodingDescription(m_Encoding).c_str(),
                   m_Encoding);
        Manager::Get()->GetLogManager()->DebugLog(msg);
    }

    if (ConvertToWxString)
    {
        ConvertToWxStr(buffer, size);
    }

    return true;
} // end of DetectEncoding

//bool EncodingDetector::DetectUTF8(const wxByte *byt, size_t size)
//{
//    /* The following detection code is based on modified code
//    *  of MadEdit
//    */
//    size_t i = 0;
//
//    if (!byt)
//        return false;
//
//    while (i < size)
//    {
//        if (byt[i] < 0x80)
//        {
//            ++i; // Continue searching for any possible UTF-8 encoded characters
//        }
//        else if (byt[i] <= 0xDF) // 1110xxxx 10xxxxxx
//        {
//            if (++i < size && IsUTF8Tail(byt[i]))
//            {
//                return true; // We would stop our search assuming the whole file is UTF-8
//            }
//            else if (i != size) // Possibly a malformed UTF-8 file
//            {
//                return false;
//            }
//        }
//        else if (byt[i] <= 0xEF) // 1110xxxx 10xxxxxx 10xxxxxx
//        {
//            if ((++i < size && IsUTF8Tail(byt[i]))
//                && (++i < size && IsUTF8Tail(byt[i])))
//            {
//                return true; // We would stop our search assuming the whole file is UTF-8
//            }
//            else if(size != i)
//            {
//                return false;
//            }
//        }
//        else if (byt[i] <= 0xF4) // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//        {
//            if ((++i < size && IsUTF8Tail(byt[i]))
//                && (++i < size && IsUTF8Tail(byt[i]))
//                && (++i < size && IsUTF8Tail(byt[i])))
//            {
//                return true;
//            }
//            else if(size != i)
//            {
//                return false;
//            }
//        }
//        /* Begin: Extra code (not from MadEdit) */
//        else if (byt[i] <= 0xFB)  // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//        {
//            if ((++i < size && IsUTF8Tail(byt[i]))
//                && (++i < size && IsUTF8Tail(byt[i]))
//                && (++i < size && IsUTF8Tail(byt[i]))
//                && (++i < size && IsUTF8Tail(byt[i])))
//            {
//                return true;
//            }
//            else if(size != i)
//            {
//                return false;
//            }
//        }
//        else
//        {
//            return false;
//        }
//        /* End: Extra code (not from MadEdit) */
//    }
//
//    return false;
//}

/* ==============================================
*  Begin: Detection Code from MadEdit
*         Modified to suit C::B
*  ---------------------------------------------- */
bool EncodingDetector::IsTextUTF16LE(const wxByte *text, size_t size)
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

bool EncodingDetector::IsTextUTF16BE(const wxByte *text, size_t size)
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

bool EncodingDetector::DetectUTF16(const wxByte *byt, size_t size)
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
bool EncodingDetector::IsTextUTF32LE(const wxByte *text, size_t size)
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

bool EncodingDetector::IsTextUTF32BE(const wxByte *text, size_t size)
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

bool EncodingDetector::DetectUTF32(const wxByte *byt, size_t size)
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
