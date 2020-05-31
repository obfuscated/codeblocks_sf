/***************************************************************
 * Name:      TextFileSearcher
 * Purpose:   TextFileSearcher is used to search text files
 *            for text or regular expressions.
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <globals.h>
#endif

#include "encodingdetector.h"

#include <wx/filename.h>

#include "TextFileSearcher.h"
#include "TextFileSearcherText.h"
#include "TextFileSearcherRegEx.h"


TextFileSearcher* TextFileSearcher::BuildTextFileSearcher(const wxString& searchText,
                                                          bool matchCase,
                                                          bool matchWordBegin,
                                                          bool matchWord,
                                                          bool regEx)
{
    TextFileSearcher* pFileSearcher = NULL;
    if ( regEx == true )
    {
        pFileSearcher = new TextFileSearcherRegEx(searchText, matchCase, matchWordBegin, matchWord);
    }
    else
    {
        pFileSearcher = new TextFileSearcherText(searchText, matchCase, matchWordBegin, matchWord);
    }

    // Tests if construction is OK
    wxString errorMessage(wxEmptyString);
    if ( pFileSearcher && !pFileSearcher->IsOk(&errorMessage)  )
    {
        delete pFileSearcher;
        pFileSearcher = 0;
    }

    return pFileSearcher;
}


TextFileSearcher::eFileSearcherReturn TextFileSearcher::FindInFile(const wxString& filePath,
                                                                   wxArrayString &foundLines,
                                                                   std::vector<int> &matchedPositions)
{
    eFileSearcherReturn success=idStringNotFound;
    wxString line;

    // skip empty fles
    if(wxFileName::GetSize(filePath) != 0)
    {
        // Tests file size and existence
        if ( !wxFileName::FileExists(filePath) )
        {
            // We skip missing files without alerting user.
            // If a file has disappeared, it is not our problem.
            // cbMessageBox( filePath + _T(" does not exist."), _T("Error"), wxICON_ERROR);
            return idFileNotFound;
        }

        // File open
        EncodingDetector enc(filePath, false);
        if( !enc.IsOK())
        {
    //        return idFileOpenError;
            return idFileNotFound;
        }
        wxFontEncoding fe =  enc.GetFontEncoding();

        if ( fe == wxFONTENCODING_UTF7 )
        {
            wxMBConvUTF7 conv;
            if ( !m_TextFile.Open(filePath, conv) )
            {
                return idFileOpenError;
            }
        }
        else if ( fe == wxFONTENCODING_UTF8 )
        {
            wxMBConvUTF8 conv;
            if ( !m_TextFile.Open(filePath, conv) )
            {
                return idFileOpenError;
            }
        }
        else if ( fe == wxFONTENCODING_UTF16BE )
        {
            wxMBConvUTF16BE conv;
            if ( !m_TextFile.Open(filePath, conv) )
            {
                return idFileOpenError;
            }
        }
        else if ( fe == wxFONTENCODING_UTF16LE )
        {
            wxMBConvUTF16LE conv;
            if ( !m_TextFile.Open(filePath, conv) )
            {
                return idFileOpenError;
            }
        }
        else if ( fe == wxFONTENCODING_UTF32BE )
        {
            wxMBConvUTF32BE conv;
            if ( !m_TextFile.Open(filePath, conv) )
            {
                return idFileOpenError;
            }
        }
        else if ( fe == wxFONTENCODING_UTF32LE )
        {
            wxMBConvUTF32LE conv;
            if ( !m_TextFile.Open(filePath, conv) )
            {
                return idFileOpenError;
            }
        }
        else
        {
            wxCSConv conv(fe);
            if ( !m_TextFile.Open(filePath, conv) )
            {
                return idFileOpenError;
            }

        }


        // Tests all file lines
        for ( size_t i = 0; i < m_TextFile.GetLineCount(); ++i )
        {
            line = m_TextFile.GetLine(i);

            std::vector<int>::size_type idxMatchedCount = matchedPositions.size();

            if ( MatchLine(&matchedPositions, line) )
            {
                success=idStringFound;
                // An interesting line is found. We clean and add it to the provided array
                line.Replace(_T("\t"), _T(" "));
                line.Replace(_T("\r"), _T(" "));
                line.Replace(_T("\n"), _T(" "));

                int compensateStart = line.length();
                line.Trim(false);
                compensateStart = compensateStart - line.length();

                line.Trim(true);

                foundLines.Add(wxString::Format(wxT("%lu"), static_cast<unsigned long>(i + 1)));
                foundLines.Add(line);

                // We have to compensate any trimming on the left. If we don't do it the matched
                // positions would be incorrect.
                if (idxMatchedCount < matchedPositions.size())
                {
                    const int count = matchedPositions[idxMatchedCount];
                    for (int matchedIdx = 0; matchedIdx < count; ++matchedIdx)
                    {
                        const std::vector<int>::size_type index = idxMatchedCount + 1 + matchedIdx * 2;
                        const int oldStart = matchedPositions[index];
                        matchedPositions[index] = std::max(oldStart - compensateStart, 0);
                    }
                }
            }
        }

        // File close
        m_TextFile.Close();
    }
    return success;
}


bool TextFileSearcher::IsOk(wxString* WXUNUSED(pErrorMessage))
{
    return true;
}
