/***************************************************************
 * Name:      avHeader.h
 * Purpose:   Declaration of the class to parse the version values from version.h
 * Author:    JGM (jgmdev@gmail.com)
 * Created:   2007-10-20
 * Copyright: JGM
 * License: GPL
 **************************************************************/

#ifndef AVHEADER_H
#define AVHEADER_H

//{Headers
#include <wx/string.h>
#include <wx/regex.h>
//}

/**Class to parse the version values from the version.h header.*/
class avHeader
{
private:
    wxString m_text; /**< Text to search the variable values. */

public:
    /**Initialize the m_text to an empty string.*/
    avHeader():m_text(_T("")){}

    /**Optional constructor.
    *Initialize the m_text to a predefined value.
    *@param text The value to store on m_text.
    */
    avHeader(const wxString& text );

    ~avHeader(){}

    /**Sets the value of m_text.
    *@param text The value to store on m_text.
    */
    void SetString(const wxString& text );

    /**Loads a file content into m_text.
    *@param fileName The full location of the file to open.
    *@return True on successful.
    */
    bool LoadFile(const wxString& fileName);

    /**To search a long variable value.
    *@param nameOfVariable The variable to search for.
    *@return The variable value or zero if not found, but zero could be the real value.
    */
    long GetValue(const wxString& nameOfVariable ) const;

    /**To search a char[] variable value.
    *@param nameOfVariable The variable to search for.
    *@return The variable value or empty string if not found, but a empty string could be the real value.
    */
    wxString GetString(const wxString& nameOfVariable ) const;
};

#endif // AVHEADER_H
