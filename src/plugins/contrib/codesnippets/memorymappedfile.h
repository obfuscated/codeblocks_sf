/**
\class wxMemoryMappedFile
\author Blake Madden (blake.madden@oleandersolutions.com)
\date (C) 2006
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the wxWidgets License.                          *
 *                                                                         *
 ***************************************************************************/

/** \example wxMemoryMappedFile
  try<br>
    {<br>
    wxMemoryMappedFile fileMap(_T("/home/bmadden/file.txt"), <b>true</b>);<br>
    const char* fileText = (const char*)fileMap.GetStream();<br><br>
    <b>//now map another file (note that fileText will not be valid after this)</b><br>
    fileMap.UnmapFile();<br>
    fileMap.MapFile(_T("/home/bmadden/DifferentFile.txt"), <b>false</b>);<br>
    char* writableFileText = (char*)fileMap.GetStream();<br><br>
    <b>//now write back to the file by simply writing to the pointer</b><br>
    std::strncpy(writableFileText, "Hello, world!", 13);<br>
    }<br>
  catch(...)<br>
    {<br>
    //handle error here<br>
    }

  \todo Currently only supports files under 2GBs.
*/
/** \example wxMemoryMappedFile
  try
    {
        wxMemoryMappedFile fileMap(_T("/home/bmadden/file.txt"), true);
        const char* fileText = (const char*)fileMap.GetStream();

        //now map another file (note that fileText will not be valid after this)
        fileMap.UnmapFile();
        fileMap.MapFile(_T("/home/bmadden/DifferentFile.txt"), false);
        char* writableFileText = (char*)fileMap.GetStream();
        //now write back to the file by simply writing to the pointer
        std::strncpy(writableFileText, "Hello, world!", 13);
    }
  catch(...)
    {
        //handle error here
    }

  \todo Currently only supports files under 2GBs.
*/

#ifndef __WXMEMMAPPEDFILE_H__
#define __WXMEMMAPPEDFILE_H__

// NB: "UNICODE" must be defined for this code to compile
#if defined(wxUSE_UNICODE)
#define UNICODE
#endif

#ifdef __WXMSW__
#include <windows.h>
#else
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#endif
#include <wx/string.h>
#include <wx/longlong.h>
#include <exception>

///General exception that can be thrown when mapping a file.
class wxMemoryMappedFileException : public std::exception
    {};
///Exception that can be thrown when mapping if the file is zero length.
class wxMemoryMappedFileEmptyException : public std::exception
    {};
///Exception that can be thrown when mapping if the file can't be exclusively locked.
class wxMemoryMappedFileShareViolationException : public std::exception
    {};
///Exception that can be thrown when mapping if the file isn't something that can be mapped.
class wxMemoryMappedInvalidFileType : public std::exception
    {};
///Exception that can be thrown when mapping if the size of the file can't be determined.
class wxMemoryMappedInvalidFileSize : public std::exception
    {};

#ifdef __WXMSW__
typedef HANDLE wxMemoryMappedFileHandleType;
#else
typedef int wxMemoryMappedFileHandleType;
#endif

/**Class for mapping a file into your address space
(rather than having to buffer its contents)*/
// ----------------------------------------------------------------------------
class wxMemoryMappedFile
// ----------------------------------------------------------------------------
{
public:
    ///Default Constructor
    wxMemoryMappedFile() :
    #ifdef __WXMSW__
            m_hFile(INVALID_HANDLE_VALUE),
            m_hsection(NULL),
    #else
            m_hFile(-1),
    #endif
            m_data(NULL),
            m_mapSize(0),
            m_open(false),
            m_isReadOnly(false),
            m_lastError(0)
    {}

    /**
    \brief Constructor which will automatically map the file.

    \exception wxMemoryMappedInvalidFileSize
    \exception wxMemoryMappedFileEmptyException
    \exception wxMemoryMappedFileShareViolationException
    \exception wxMemoryMappedInvalidFileType
    \exception wxMemoryMappedInvalidFileSize

    \param filePath Path to the file to map.
    \param readOnly Flag specifying whether to open the file as read only.
    */
    wxMemoryMappedFile(const wxString& filePath, bool readOnly = true) :
    #ifdef __WXMSW__
            m_hFile(INVALID_HANDLE_VALUE),
            m_hsection(NULL),
    #else
            m_hFile(-1),
    #endif
            m_data(NULL),
            m_mapSize(0),
            m_open(false),
            m_isReadOnly(readOnly),
            m_lastError(0)
    {
        m_lastError = MapFile(filePath, readOnly);
    }
    ///Destructor which implicitly unmaps the file
    ~wxMemoryMappedFile()
    {
        UnmapFile();
    }
    ///Last error recorded
    unsigned GetLastError()
    {
        return m_lastError;
    }
    ///Indicates whether a file is currently (and successfully) mapped
    bool IsOk() const
    {
        return m_open;
    }
    ///Indicates whether the current file mapping is read only
    bool IsReadOnly() const
    {
        return m_isReadOnly;
    }
    /**
    \brief Manually maps a new file.
    \warning  If this object is currently mapping another file then
    you need to call UnmapFile() first.

    \exception wxMemoryMappedInvalidFileSize
    \exception wxMemoryMappedFileEmptyException
    \exception wxMemoryMappedFileShareViolationException
    \exception wxMemoryMappedInvalidFileType
    \exception wxMemoryMappedInvalidFileSize

    \param filePath Path to the file to map.
    \param readOnly Flag specifying whether to open the file as read only.

    \return True if file mapping was successful.
    */
    long MapFile(const wxString& filePath, const bool readOnly = true);
    ///Closes the handles and mappings
    void UnmapFile();
    /**Returns the raw byte stream of the file
    \warning Do not attempt to write to the returned pointer if you mapped the file as read only.
    The read only status of the current mapping can be checked by calling IsReadOnly().
    */
    void* GetStream()
    {
        return m_data;
    }
    ///Returns the length of the mapped file
    size_t GetMapSize() const
    {
        return m_mapSize;
    }
    ///Returns the path of the file currently mapped
    wxString GetFilePath() const
    {
        return m_filePath;
    }

    ///Returns the size of a large file (as an unsigned long long)
    static wxULongLong GetFileSize64(const wxMemoryMappedFileHandleType hFile);

    private:
        void Reset();
      #ifdef __WXMSW__
        HANDLE m_hFile;
        HANDLE m_hsection;
      #else
        int m_hFile;
      #endif
        void* m_data;
        size_t m_mapSize;
        wxString m_filePath;
        bool m_open;
        bool m_isReadOnly;
        unsigned m_lastError;
};

#endif //__WXMEMMAPPEDFILE_H__
