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

#include "nsError.h"
#include "nsUniversalDetector.h"

#include <wx/encconv.h>

/* ----------------------------------------------
 *  Some detection code is borrowed from MadEdit,
 *  but modified to suit C::B. Other portions are
 *  using the Mozilla universal char detector.
 * ---------------------------------------------- */

EncodingDetector::EncodingDetector(const wxString& filename, bool useLog) :
    nsUniversalDetector(NS_FILTER_ALL),
    m_IsOK(false),
    m_UseBOM(false),
    m_UseLog(useLog),
    m_BOMSizeInBytes(0),
    m_ConvStr(wxEmptyString)
{
    m_Encoding = wxLocale::GetSystemEncoding();
    m_IsOK = DetectEncoding(filename);
}

EncodingDetector::EncodingDetector(LoaderBase* fileLdr, bool useLog) :
    nsUniversalDetector(NS_FILTER_ALL),
    m_IsOK(false),
    m_UseBOM(false),
    m_UseLog(useLog),
    m_BOMSizeInBytes(0),
    m_ConvStr(wxEmptyString)
{
    m_Encoding = wxLocale::GetSystemEncoding();
    m_IsOK = DetectEncoding((wxByte*)fileLdr->GetData(), fileLdr->GetLength());
}

EncodingDetector::EncodingDetector(const wxByte* buffer, size_t size, bool useLog) :
    nsUniversalDetector(NS_FILTER_ALL),
    m_IsOK(false),
    m_UseBOM(false),
    m_UseLog(useLog),
    m_BOMSizeInBytes(0),
    m_ConvStr(wxEmptyString)
{
    m_Encoding = wxLocale::GetSystemEncoding();
    m_IsOK = DetectEncoding(buffer, size);
}

EncodingDetector::~EncodingDetector()
{
}

void EncodingDetector::Report(const char *aCharset)
{
    m_MozillaResult = cbC2U(aCharset);

    if (m_UseLog)
        Manager::Get()->GetLogManager()->DebugLog(F(_T("Mozilla universal detection engine detected '%s'."), m_MozillaResult.wx_str()));

    if (m_MozillaResult == _T("gb18030")) // hack, because wxWidgets only knows cp936
        m_MozillaResult = _T("cp936");
    else if (m_MozillaResult.Contains(wxT("*ASCII*"))) // remove our "specials"
        m_MozillaResult = wxEmptyString;
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

bool EncodingDetector::DetectEncoding(const wxString& filename, bool convert_to_wxstring)
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
        result = DetectEncoding(buffer, size, convert_to_wxstring);

    file.Close();
    free(buffer);
    return result;
}

bool EncodingDetector::DetectEncoding(const wxByte* buffer, size_t size, bool convert_to_wxstring)
{
    ConfigManager* cfgMgr = Manager::Get()->GetConfigManager(_T("editor"));
    wxString encname = cfgMgr->Read(_T("/default_encoding"));

    if (cfgMgr->ReadInt(_T("/default_encoding/use_option"), 0) == 1)
    {
        // Bypass C::B's auto-detection
        m_Encoding = wxFontMapper::Get()->CharsetToEncoding(encname, false);

        if (m_UseLog)
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

        // Try our own detection for UTF-16 and UTF-32, the Mozilla-version does not work without BOM
        if ( DetectEncodingEx(buffer, size) )
        {
            if (m_UseBOM && m_UseLog)
            {
                wxString msg;
                msg.Printf(_T("Detected encoding via BOM: %s (ID: %d)"),
                           wxFontMapper::Get()->GetEncodingDescription(m_Encoding).c_str(),
                           m_Encoding);
                Manager::Get()->GetLogManager()->DebugLog(msg);
            }
        }
        else
        {
            //{ MOZILLA nsUniversalDetector START
            // If we still have no results try Mozilla (taken from nsUdetXPCOMWrapper.cpp):
            Reset(); nsresult res = HandleData((char*)buffer, size);
            if (res==NS_OK)
                DataEnd();
            else
            {
                m_MozillaResult = wxEmptyString;
                if (m_UseLog)
                    Manager::Get()->GetLogManager()->DebugLog(F(_T("Mozilla universal detection failed with %d."), res));
            }
            //} MOZILLA nsUniversalDetector END

            if ( !m_MozillaResult.IsEmpty() )
                m_Encoding = wxFontMapper::Get()->CharsetToEncoding(m_MozillaResult, false);

            if (m_Encoding == wxFONTENCODING_DEFAULT)
            {
                wxString enc_name = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/default_encoding"), wxLocale::GetSystemEncodingName());
                m_Encoding = wxFontMapper::GetEncodingFromName(enc_name);
                if (m_UseLog)
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
                if (m_UseLog)
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

    if (m_UseLog)
    {
        wxString msg;
        msg.Printf(_T("Final encoding detected: %s (ID: %d)"),
                   wxFontMapper::Get()->GetEncodingDescription(m_Encoding).c_str(),
                   m_Encoding);
        Manager::Get()->GetLogManager()->DebugLog(msg);
    }

    if (convert_to_wxstring && !ConvertToWxString(buffer, size) && m_UseLog)
        Manager::Get()->GetLogManager()->DebugLog(_T("Something seriously went wrong while converting file content to wxString!"));

    return true;
}

// Stolen from  https://github.com/etexteditor/e/blob/master/src/Strings.cpp
//        and:  https://github.com/etexteditor/e/blob/master/src/Utf.cpp
// Copyright (c) 2009, Alexander Stigsen, e-texteditor.com (All rights reserved)
// http://www.e-texteditor.com/
bool EncodingDetector::DetectEncodingEx(const wxByte* buffer, size_t size)
{
    if (!buffer || size == 0) return false;

    const wxByte*  buff_ptr = buffer;
    const wxByte*  buff_end = &buffer[size];
    wxFontEncoding enc      = wxFONTENCODING_DEFAULT;

    // Check if the buffer starts with a BOM (Byte Order Marker)
    if (size >= 2)
    {
        if      (size >= 4 && memcmp(buffer, "\xFF\xFE\x00\x00", 4) == 0)
        {
            enc = wxFONTENCODING_UTF32LE;
            m_BOMSizeInBytes = 4;
            m_UseBOM = true;
        }
        else if (size >= 4 && memcmp(buffer, "\xFE\xFF\x00\x00", 4) == 0)
        {
            // FE FF 00 00  UCS-4, unusual octet order BOM (3412)
            // X-ISO-10646-UCS-4-3412 can not (yet) be handled by wxWidgets
            enc = (wxFontEncoding)-1;
        }
        else if (size >= 4 && memcmp(buffer, "\x00\x00\xFE\xFF", 4) == 0)
        {
            enc = wxFONTENCODING_UTF32BE;
            m_BOMSizeInBytes = 4;
            m_UseBOM = true;
        }
        else if (size >= 4 && memcmp(buffer, "\x00\x00\xFF\xFE", 4) == 0)
        {
            // 00 00 FF FE  UCS-4, unusual octet order BOM (2143)
            // X-ISO-10646-UCS-4-2143 can not (yet) be handled by wxWidgets
            enc = (wxFontEncoding)-1;
        }
        else if (             memcmp(buffer, "\xFF\xFE", 2) == 0)
        {
            enc = wxFONTENCODING_UTF16LE;
            m_BOMSizeInBytes = 2;
            m_UseBOM = true;
        }
        else if (             memcmp(buffer, "\xFE\xFF", 2) == 0)
        {
            enc = wxFONTENCODING_UTF16BE;
            m_BOMSizeInBytes = 2;
            m_UseBOM = true;
        }
        else if (size >= 3 && memcmp(buffer, "\xEF\xBB\xBF", 3) == 0)
        {
            enc = wxFONTENCODING_UTF8;
            m_BOMSizeInBytes = 3;
            m_UseBOM = true;
        }
        else if (size >= 5 && memcmp(buffer, "\x2B\x2F\x76\x38\x2D", 5) == 0)
        {
            enc = wxFONTENCODING_UTF7;
            m_BOMSizeInBytes = 5;
            m_UseBOM = true;
        }

        buff_ptr += m_BOMSizeInBytes;
    }

    // If the file starts with a leading < (less) sign, it is probably an XML file
    // and we can determine the encoding by how the sign is encoded.
    if (enc == wxFONTENCODING_DEFAULT && size >= 2)
    {
        if      (size >= 4 && memcmp(buffer, "\x3C\x00\x00\x00", 4) == 0) enc = wxFONTENCODING_UTF32LE;
        else if (size >= 4 && memcmp(buffer, "\x00\x00\x00\x3C", 4) == 0) enc = wxFONTENCODING_UTF32BE;
        else if (             memcmp(buffer, "\x3C\x00",         2) == 0) enc = wxFONTENCODING_UTF16LE;
        else if (             memcmp(buffer, "\x00\x3C",         2) == 0) enc = wxFONTENCODING_UTF16BE;
    }

    // Unicode Detection
    if (enc == wxFONTENCODING_DEFAULT)
    {
        unsigned int null_byte_count  = 0;
        unsigned int utf_bytes        = 0;
        unsigned int good_utf_count   = 0;
        unsigned int bad_utf_count    = 0;
        unsigned int bad_utf32_count  = 0;
        unsigned int bad_utf16_count  = 0;
        unsigned int nl_utf32le_count = 0;
        unsigned int nl_utf32be_count = 0;
        unsigned int nl_utf16le_count = 0;
        unsigned int nl_utf16be_count = 0;

        while (buff_ptr != buff_end)
        {
            if (*buff_ptr == 0) ++null_byte_count;

            // Detect UTF-8 by scanning for invalid sequences
            if (utf_bytes == 0)
            {
                if ((*buff_ptr & 0xC0) == 0x80 || *buff_ptr == 0)
                    ++bad_utf_count;
                else
                {
                    const char c = *buff_ptr;
                    utf_bytes = 5; // invalid length
                    if      ((c & 0x80) == 0x00) utf_bytes = 1;
                    else if ((c & 0xE0) == 0xC0) utf_bytes = 2;
                    else if ((c & 0xF0) == 0xE0) utf_bytes = 3;
                    else if ((c & 0xF8) == 0xF0) utf_bytes = 4;
                    if (utf_bytes > 3)
                    {
                        ++bad_utf_count;
                        utf_bytes = 0;
                    }
                }
            }
            else if ((*buff_ptr & 0xC0) == 0x80)
            {
                --utf_bytes;
                if (utf_bytes == 0)
                    ++good_utf_count;
            }
            else
            {
                ++bad_utf_count;
                utf_bytes = 0;
            }

            // Detect UTF-32 by scanning for newlines (and lack of null chars)
            if ((wxUIntPtr)buff_ptr % 4 == 0 && buff_ptr+4 <= buff_end)
            {
                if (*((wxUint32*)buff_ptr) == 0                        ) ++bad_utf32_count;
                if (*((wxUint32*)buff_ptr) == wxUINT32_SWAP_ON_BE(0x0A)) ++nl_utf32le_count;
                if (*((wxUint32*)buff_ptr) == wxUINT32_SWAP_ON_LE(0x0A)) ++nl_utf32be_count;
            }

            // Detect UTF-16 by scanning for newlines (and lack of null chars)
            if ((wxUIntPtr)buff_ptr % 2 == 0 && buff_ptr+4 <= buff_end)
            {
                if (*((wxUint16*)buff_ptr) == 0)                         ++bad_utf16_count;
                if (*((wxUint16*)buff_ptr) == wxUINT16_SWAP_ON_BE(0x0A)) ++nl_utf16le_count;
                if (*((wxUint16*)buff_ptr) == wxUINT16_SWAP_ON_LE(0x0A)) ++nl_utf16be_count;
            }

            ++buff_ptr;
        }

        if      (bad_utf_count   == 0)                                  enc = wxFONTENCODING_UTF8;
        else if (bad_utf32_count == 0 && nl_utf32le_count > size / 400) enc = wxFONTENCODING_UTF32LE;
        else if (bad_utf32_count == 0 && nl_utf32be_count > size / 400) enc = wxFONTENCODING_UTF32BE;
        else if (bad_utf16_count == 0 && nl_utf16le_count > size / 200) enc = wxFONTENCODING_UTF16LE;
        else if (bad_utf16_count == 0 && nl_utf16be_count > size / 200) enc = wxFONTENCODING_UTF16BE;
        else if (null_byte_count)
            return false; // Maybe this is a binary file?
    }

    if (enc != wxFONTENCODING_DEFAULT)
    {
        m_Encoding = enc; // Success.
        return true;
    }

    // If we can't detect encoding and it does not contain null bytes
    // just ignore it and try backup-procedures (Mozilla) later...
    return false;
}

bool EncodingDetector::ConvertToWxString(const wxByte* buffer, size_t size)
{
    LogManager* logmgr = Manager::Get()->GetLogManager();
    wxString    logmsg;

    if (!buffer || size == 0)
    {
        if (m_UseLog)
        {
            logmsg.Printf(_T("Encoding conversion has failed (buffer is empty)!"));
            logmgr->DebugLog(logmsg);
        }
        return false; // Nothing we can do...
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
    if      ( m_Encoding == wxFONTENCODING_UTF7 )
    {
        wxMBConvUTF7 conv;
        wideBuff = conv.cMB2WC((const char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else if ( m_Encoding == wxFONTENCODING_UTF8 )
    {
        wxMBConvUTF8 conv;
        wideBuff = conv.cMB2WC((const char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else if ( m_Encoding == wxFONTENCODING_UTF16BE )
    {
        wxMBConvUTF16BE conv;
        wideBuff = conv.cMB2WC((const char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else if ( m_Encoding == wxFONTENCODING_UTF16LE )
    {
        wxMBConvUTF16LE conv;
        wideBuff = conv.cMB2WC((const char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else if ( m_Encoding == wxFONTENCODING_UTF32BE )
    {
        wxMBConvUTF32BE conv;
        wideBuff = conv.cMB2WC((const char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else if ( m_Encoding == wxFONTENCODING_UTF32LE )
    {
        wxMBConvUTF32LE conv;
        wideBuff = conv.cMB2WC((const char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
    }
    else
    {
        // try wxEncodingConverter first, even it it only works for
        // wxFONTENCODING_ISO8859_1..15, wxFONTENCODING_CP1250..1257 and wxFONTENCODING_KOI8
        // but it's much, much faster than wxCSConv (at least on Linux)
        wxEncodingConverter conv;
        wchar_t* tmp = new wchar_t[size + 4 - m_BOMSizeInBytes];
        if (  conv.Init(m_Encoding, wxFONTENCODING_UNICODE)
           && conv.Convert((const char*)buffer, tmp) )
        {
            wideBuff = tmp;
            outlen = size + 4 - m_BOMSizeInBytes; // should be correct, because Convert has returned true
            if (m_UseLog && outlen>0)
            {
                logmsg.Printf(_T("Conversion succeeded using wxEncodingConverter "
                                 "(buffer size = %lu, converted size = %lu."), static_cast<unsigned long>(size), static_cast<unsigned long>(outlen));
                logmgr->DebugLog(logmsg);
            }
        }
        else
        {
            // try wxCSConv, if nothing else works
            wxCSConv csconv(m_Encoding);
            if (csconv.IsOk())
            {
                wideBuff = csconv.cMB2WC((const char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
                if (m_UseLog && outlen>0)
                {
                    logmsg.Printf(_T("Conversion succeeded using wxCSConv "
                                     "(buffer size = %lu, converted size = %lu."), static_cast<unsigned long>(size), static_cast<unsigned long>(outlen));
                    logmgr->DebugLog(logmsg);
                }
            }
        }
        delete [] tmp;
    }

    if (outlen>0)
    {
        m_ConvStr = wxString(wideBuff);
        return true; // Done.
    }

    // Here, outlen == 0, so an error occurred during conversion.
    if (m_UseLog)
    {
        logmsg.Printf(_T("Encoding conversion using settings has failed!\n"
                         "Encoding chosen was: %s (ID: %d)"),
                      wxFontMapper::Get()->GetEncodingDescription(m_Encoding).wx_str(),
                      m_Encoding);
        logmgr->DebugLog(logmsg);
    }

    // Try system locale as fall-back (if requested by the settings)
    ConfigManager* cfgMgr = Manager::Get()->GetConfigManager(_T("editor"));
    if (cfgMgr->ReadBool(_T("/default_encoding/use_system"), true))
    {
        if (platform::windows)
        {
            if (m_UseLog)
                logmgr->DebugLog(_T("Trying system locale as fallback..."));

            m_Encoding = wxLocale::GetSystemEncoding();
        }
        else
        {
            // We can rely on the UTF-8 detection code ;-)
            if (m_UseLog)
                logmgr->DebugLog(_T("Trying ISO-8859-1 as fallback..."));

            m_Encoding = wxFONTENCODING_ISO8859_1;
        }

        wxCSConv conv_system(m_Encoding);
        wideBuff = conv_system.cMB2WC((const char*)buffer, size + 4 - m_BOMSizeInBytes, &outlen);
        m_ConvStr = wxString(wideBuff);

        if (outlen == 0)
        {
            if (m_UseLog)
            {
                logmsg.Printf(_T("Encoding conversion using system locale fallback has failed!\n"
                                 "Last encoding choosen was: %s (ID: %d)\n"
                                 "Don't know what to do."),
                              wxFontMapper::Get()->GetEncodingDescription(m_Encoding).c_str(),
                              m_Encoding);
                logmgr->DebugLog(logmsg);
            }
            return false; // Nothing we can do...
        }
    }
    else
    {
        if (m_UseLog)
        {
            logmgr->DebugLog(_T("Encoding conversion has seriously failed!\n"
                                "Don't know what to do."));
        }
        return false; // Nothing we can do...
    }

    return true;
}
