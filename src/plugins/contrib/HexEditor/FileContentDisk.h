/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008-2009 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with HexEditor. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef FILECONTENTDISK_H
#define FILECONTENTDISK_H

#include "FileContentBase.h"
#include "TestCasesBase.h"

#include <vector>
#include <wx/file.h>

/** \brief Class responsible for providing file content by reading from disk
 *         as much as possible.
 *
 * Parts of the file which has been modified are kept inside of the memory
 */
class FileContentDisk: public FileContentBase
{
    public:

        /** \brief Ctor */
        FileContentDisk();

        /** \brief Dctor */
        virtual ~FileContentDisk();

        /** \brief Reading the data from the file */
        virtual bool ReadFile( const wxString& fileName );

        /** \brief Writing the data to the file */
        virtual bool WriteFile( const wxString& fileName );

        /** \brief Getting size of the content */
        virtual OffsetT GetSize();

        /** \brief Reading some part of data */
        virtual OffsetT Read( void* buff, OffsetT position, OffsetT length );

        /** \brief Return collection of tests for this class */
        static TestCasesBase& GetTests();

        class TestData;


    protected:

        /** \brief Create modification object for change operation */
        virtual ModificationData* BuildChangeModification( OffsetT position, OffsetT length, const void* data = 0 );

        /** \brief Create modification object for data add operation */
        virtual ModificationData* BuildAddModification( OffsetT position, OffsetT length, const void* data = 0 );

        /** \brief Create modification object for data remove operation */
        virtual ModificationData* BuildRemoveModification( OffsetT position, OffsetT length );

        /** \brief Set given block of data */
        void SetBlock( const char* data, OffsetT pos, OffsetT lengthBefore, OffsetT lengthAfter );


        /** \brief Class keeping informations about one data block */
        struct DataBlock
        {
            OffsetT             start;      ///< \brief Block's start
            OffsetT             fileStart;  ///< \brief Start position of this block in file
            OffsetT             size;       ///< \brief Block's size
            std::vector< char > data;       ///< \brief Block's data, empty vector means that this block is from file

            bool IsFromDisk() { return data.empty(); }
        };


        /** \brief Util routine used to add new data blocks */
        DataBlock* InsertNewBlock( size_t blockIndex, OffsetT position );

        class DiskModificationData;

        wxString                  m_FileName;    ///< \brief Name of opened file
        wxFile                    m_File;        ///< \brief File used as disk source
        std::vector< DataBlock* > m_Contents;    ///< \brief Contents of the data in form of blocks sorted by start offsets
        bool                      m_TestMode;    ///< \brief Flag indicating that we're running internal tests

        /** \brief Delete all data blocks */
        void ClearBlocks();

        /** \brief Find index of block containing given data offset */
        inline size_t FindBlock( OffsetT offset );

        /** \brief Try to merge some blocks lying next to each other */
        inline void MergeBlocks( size_t startPosition );

        /** \brief Test buffers for damages made by bugs */
        inline void ConsistencyCheck();

        /** \brief Write contents to disk using the easiest method */
        bool WriteFileEasiest( );

        /** \brief Write contents to disk without using temporary files */
        bool WriteFileOnDisk( );

        /** \brief Write contents to disk using temporary file */
        bool WriteFileTemporary( );

        /** \brief Write to totally different file */
        bool WriteToDifferentFile( const wxString& fileName );

        /** \brief Dump the content to given wxFile file */
        bool WriteToFile( wxFile& file );

        /** \brief Reset m_Content blocks to contain data from file only */
        void ResetBlocks();

};

#endif
