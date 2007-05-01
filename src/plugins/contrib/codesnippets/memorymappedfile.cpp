/***************************************************************************
                          wxMemoryMappedFile.h  -  description
                             -------------------
    copyright: (C) 2006 by Blake Madden (blake.madden@oleandersolutions.com)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the wxWidgets License.                          *
 *                                                                         *
 ***************************************************************************/
#include "memorymappedfile.h"
#if defined(__WXGTK__)
    #include "errno.h"
#endif

// ----------------------------------------------------------------------------
long wxMemoryMappedFile::MapFile(const wxString& filePath,
                                 const bool readOnly /*= true*/)
// ----------------------------------------------------------------------------
{
    m_lastError = 0;
    //if another files is currently mapped this fail
    if (IsOk())
    {
        return false;
    }
    m_isReadOnly = readOnly;
    m_filePath = filePath;
#ifdef __WXMSW__
    unsigned long dwDesiredFileAccess = GENERIC_READ;
    if (!IsReadOnly())
    {
        dwDesiredFileAccess |= GENERIC_WRITE;
    }
    //get the handle to the file... The file MUST exists
    //asm("int3");
    m_hFile =
    #ifdef __WXWINCE__
        ::CreateFileForMapping
    #else
        ::CreateFile
    #endif
            //-(filePath.c_str(), dwDesiredFileAccess, FILE_SHARE_READ,
            (filePath.c_str(), dwDesiredFileAccess, FILE_SHARE_READ|FILE_SHARE_WRITE,
             0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN|SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION, 0);
    if (INVALID_HANDLE_VALUE == m_hFile)
    {
        Reset();
        m_lastError = ::GetLastError();
        if (ERROR_SHARING_VIOLATION == m_lastError )
        {
            //throw wxMemoryMappedFileShareViolationException();
            return -1*m_lastError;
        }
        else
        {
            //throw wxMemoryMappedFileException();
            return -1 * m_lastError;
        }
    }
    //this will fail if the file path was really a drive or printer (don't want to map that!)
    else if (FILE_TYPE_DISK != ::GetFileType(m_hFile))
    {
        ::CloseHandle(m_hFile);
        Reset();
        //throw wxMemoryMappedInvalidFileType();
        return -1 ;
    }

    /*Get the length of the file*/
    try
    {
        m_mapSize = GetFileSize64(m_hFile).GetLo();
    }
    catch (wxMemoryMappedInvalidFileSize)
    {
        m_mapSize = ::SetFilePointer(m_hFile, 0, NULL, FILE_END);
        if (0 == m_mapSize || INVALID_SET_FILE_POINTER == m_mapSize)
        {
            ::CloseHandle(m_hFile);
            Reset();
            //throw wxMemoryMappedFileEmptyException();
            return -1 ;

        }
        ::SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
    }
    //asm("int3");
    //now, we create a file mapping object for that file
    m_hsection = ::CreateFileMapping(m_hFile, 0, IsReadOnly() ? PAGE_READONLY : PAGE_READWRITE, 0, 0, NULL);
    if (NULL == m_hsection)
    {
        ::CloseHandle(m_hFile);
        Reset();
        //throw wxMemoryMappedFileException();
        return  -1 ;

    }
    m_data = ::MapViewOfFile(m_hsection, IsReadOnly() ? FILE_MAP_READ : (FILE_MAP_READ|FILE_MAP_WRITE), 0, 0, 0);
    if (NULL == m_data)
    {
        return -1;
    }
#else //Linux
    m_hFile = open(filePath.mb_str(), readOnly ? O_RDONLY : O_RDWR);
    if  (-1 == m_hFile)
    {
        //throw wxMemoryMappedFileException();
        return errno;
    }
    //get the size of the file
    m_mapSize = GetFileSize64(m_hFile).GetLo();
    if (static_cast<size_t>(-1) == m_mapSize || 0 == m_mapSize)
    {
        close(m_hFile);
        Reset();
        //throw wxMemoryMappedFileEmptyException();
        return errno;
    }
    //now get a map of the file
    m_data = mmap(NULL, m_mapSize, readOnly ? PROT_READ : (PROT_READ|PROT_WRITE),
                  (MAP_FILE|MAP_SHARED), m_hFile, 0);
    if (MAP_FAILED == m_data)
    {
        close(m_hFile);
        Reset();
        //throw wxMemoryMappedFileException();
        return errno;
    }
#endif
    m_open = true;
    return true;
}

///closes the handles and mappings
// ----------------------------------------------------------------------------
void wxMemoryMappedFile::UnmapFile()
// ----------------------------------------------------------------------------
{
#ifdef __WXMSW__
    if (m_data)
    {
        ::FlushViewOfFile(m_data, 0);
        ::UnmapViewOfFile(m_data);
        m_data = NULL;
    }
    if (m_hsection)
    {
        ::CloseHandle(m_hsection);
        m_hsection = NULL;
    }
    if (INVALID_HANDLE_VALUE != m_hFile)
    {
        ::CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
#else
    munmap(m_data, static_cast<size_t>(m_mapSize));
    m_data = NULL;
    close(m_hFile);
    m_hFile = -1;
#endif
    m_open = false;
    m_isReadOnly = false;
    m_mapSize = 0;
    m_filePath = wxEmptyString;
}

// ----------------------------------------------------------------------------
void wxMemoryMappedFile::Reset()
// ----------------------------------------------------------------------------
{
#ifdef __WXMSW__
    m_hFile = INVALID_HANDLE_VALUE;
    m_hsection = NULL;
#else
    m_hFile = -1;
#endif
    m_data = NULL;
    m_mapSize = 0;
    m_open = false;
    m_isReadOnly = false;
    m_filePath = wxEmptyString;
}

// ----------------------------------------------------------------------------
wxULongLong wxMemoryMappedFile::GetFileSize64(const wxMemoryMappedFileHandleType hFile)
// ----------------------------------------------------------------------------
{
#ifdef __WXMSW__
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return INVALID_FILE_SIZE;
    }
    //this will fail if the file path was really a drive or printer
    if (FILE_TYPE_DISK != ::GetFileType(hFile))
    {
        return INVALID_FILE_SIZE;
    }

    try
    {
        DWORD Lo = 0, Hi = 0;
        Lo = ::GetFileSize(hFile, &Hi);

        if (INVALID_FILE_SIZE == Lo)
        {
            throw wxMemoryMappedInvalidFileSize();
        }
        else
        {
            return wxULongLong(Hi, Lo);
        }
    }
    catch (...)
    {
        return wxULongLong(INVALID_FILE_SIZE);
    }
#else
    off64_t size = lseek64(hFile, 0, SEEK_END);
    lseek64(hFile, 0, SEEK_SET);//go back to the start of the file
    if (-1 == size)
    {
        throw wxMemoryMappedInvalidFileSize();
    }
    return wxULongLong(size);
#endif
}
// ----------------------------------------------------------------------------
