/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008-2009 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Frsee Software Foundation; either version 3 of the License, or
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

#include "FileContentDisk.h"
#include "TestCasesHelper.h"
#include "globals.h"

#include <wx/utils.h>
#include <wx/progdlg.h>

#include <algorithm>
#include <memory>

#include <logmanager.h>
#include <annoyingdialog.h>

#define my_assertimpl( expr, exprStr, file, line ) \
    do \
    if ( !( expr ) ) \
    { \
        LogManager::Get()->DebugLog( F( _T("HEXEDIT Assertion failed, file: %s, line: %d"), file, line ) ); \
        LogManager::Get()->DebugLog( F( _T("          EXPR: %s"), exprStr ) ); \
        LogManager::Get()->DebugLog( F( _T("          Block1: %d:%d"), (int)b1->start, (int)b1->size ) ); \
        LogManager::Get()->DebugLog( F( _T("          Block2: %d:%d"), (int)b2->start, (int)b2->size ) ); \
    } \
    while ( false )


#define my_assert( expr ) assert( expr )
//#define my_assert( expr ) my_assertimpl( expr, #expr, __FILE__, __LINE__ )


class FileContentDisk::DiskModificationData: public FileContentBase::ModificationData
{
    public:

        typedef FileContentBase::OffsetT OffsetT;

        DiskModificationData( FileContentDisk* fcd, OffsetT start )
            : m_Fcd( fcd )
            , m_Start( start )
        {}

        /** \brief Apply the modification */
        virtual void Apply()
        {
            m_Fcd->SetBlock( &m_DataAfter[0], m_Start, m_DataBefore.size(), m_DataAfter.size() );
        }

        /** \brief Revert the modification */
        virtual void Revert()
        {
            m_Fcd->SetBlock( &m_DataBefore[0], m_Start, m_DataAfter.size(), m_DataBefore.size() );
        }

        /** \brief Get the length of modification */
        virtual OffsetT Length()
        {
            return wxMax( m_DataBefore.size(), m_DataAfter.size() );
        }

        inline std::vector< char >& GetDataBefore() { return m_DataBefore; }
        inline std::vector< char >& GetDataAfter()  { return m_DataAfter; }

    private:

        FileContentDisk* m_Fcd;

        OffsetT m_Start;        ///< \brief Beginning position of the modification

        std::vector< char > m_DataBefore;    ///< \brief Data of the replaced region
        std::vector< char > m_DataAfter;     ///< \brief Data of the replacing region
};


FileContentDisk::FileContentDisk(): m_TestMode( false )
{
}

FileContentDisk::~FileContentDisk()
{
    ClearBlocks();
}

void FileContentDisk::ClearBlocks()
{
    for ( size_t i=0; i<m_Contents.size(); ++i )
    {
        delete m_Contents[ i ];
    }
    m_Contents.clear();
}

bool FileContentDisk::ReadFile(const wxString& fileName)
{
    m_File.Open( fileName, wxFile::read_write );
    if ( !m_File.IsOpened() ) return false;

    ResetBlocks();
    m_FileName = fileName;

    return true;
}

bool FileContentDisk::WriteFile(const wxString& fileName)
{
    // First we detect what kind of save we need to perform

    if ( fileName != m_FileName )
    {
        if ( WriteToDifferentFile( fileName ) )
        {
            UndoNotifySaved();
            return true;
        }
        return false;
    }

    // 1. The easiest possible save scenario occurs when
    //    each disk blocks has not changed it's position relative to the disk data.
    //    In such case we only have to write modifications to proper offsets
    //    We assume that such modification will be the most common since
    //    Hex editor is used to change bytes not to insert or remove them

    bool noShiftedDiskBlocks = true;
    if ( GetSize() < (OffsetT)m_File.Length() )
    {
        // We can not save data on the same file if the file size has decreased
        noShiftedDiskBlocks = false;
    }
    else
    {
        for ( size_t i=0; i<m_Contents.size(); ++i )
        {
            DataBlock* block = m_Contents[i];
            if ( !block->IsFromDisk() ) continue;

            if ( block->fileStart != block->start )
            {
                noShiftedDiskBlocks = false;
                break;
            }
        }
    }

    if ( noShiftedDiskBlocks )
    {
        if ( WriteFileEasiest( ) )
        {
            UndoNotifySaved();
            return true;
        }
        return false;
    }

    // 2. We detect whether modifications can be made on one file (I.E. no temporary files needed)
    //    This method may be used in some rare cases where some part of data has been moved from
    //    one location to another inside the file but other contents did remain untouched.
    //    Eventually this may also happen when editing the end of file. Since the save process
    //    is dangerous in case of power or system failure, we limit it to a situation when
    //    amount of data writes to the file does not cross some limit.

    bool noExtraFilesNeeded = false;
    if ( GetSize() < (OffsetT)m_File.Length() )
    {
        // We can not save data on the same file if the file size has decreased
        noShiftedDiskBlocks = false;
    }
    else
    {
        // TODO: Write the detection algorithm and save routine
    }

    if ( noExtraFilesNeeded )
    {
        if ( WriteFileOnDisk() )
        {
            UndoNotifySaved();
            return true;
        }
        return false;
    }

    // 3. The last, slowest but the savest method in case of disk blocks shifted require temporary
    //    file to be created. Finally the this temporary file replaces the original one.
    //    The biggest drawback of this solution is additional requirement of extra free space
    //    Which can not always be guaranteed.

    wxLongLong size = GetSize();
    wxLongLong diskFree;

    if ( !wxGetDiskSpace( wxPathOnly( fileName ), 0, &diskFree ) )
    {
        if ( cbMessageBox(
            _("An error occurred while querying for disk free space.\n"
              "This may result in save failure. Do you still want to\n"
              "save the file?" ),
            _("Error while querying for free space"),
            wxYES_NO ) != wxID_YES )
        {
            return false;
        }
    }

    if ( diskFree < size + 128 * 1024 )
    {
        if ( noExtraFilesNeeded )
        {
            if ( cbMessageBox(
                _("There's not enough free space on the drive to save the file using safe methods.\n"
                  "We can still try to save file but any power or system failure during the save\n"
                  "will corrupt the file. Do you want to use the unsafe method ?"),
                _("Not enough free space"),
                wxYES_NO ) != wxID_YES )
            {
                return false;
            }

            if ( WriteFileOnDisk() )
            {
                UndoNotifySaved();
                return true;
            }
            return false;
        }
        else
        {
            cbMessageBox( _("There's not enough free space on the drive to save the changes.\n"
                            "Please free some space and retry"),
                          _("Not enough free space"),
                          wxOK );
            return false;
        }
    }

    // Inform user about the long save process
    if ( size > 16 * 1024 * 1024 )
    {
        if ( AnnoyingDialog(
            _("HexEdit: Save may take long time"),
            _("Saving the file may take long time.\n"
              "Do you want to continue?\n") ).ShowModal() != AnnoyingDialog::rtYES )
        {
            return false;
        }
    }

    if ( WriteFileTemporary() )
    {
        UndoNotifySaved();
        return true;
    }
    return false;
}

FileContentDisk::OffsetT FileContentDisk::GetSize()
{
    return m_Contents.back()->start + m_Contents.back()->size;
}

FileContentDisk::OffsetT FileContentDisk::Read( void* buff, OffsetT position, OffsetT length )
{
    OffsetT read = 0;
    for ( size_t i = FindBlock( position ); length && i<m_Contents.size(); ++i )
    {
        DataBlock* block = m_Contents[i];

        OffsetT blockShift = position - block->start;
        OffsetT readFromThisBlock = wxMin( length, block->size - blockShift );

        if ( block->IsFromDisk() )
        {
            m_File.Seek( block->fileStart + blockShift );
            // TODO: Take care of read errors
            m_File.Read( buff, readFromThisBlock );
        }
        else
        {
            memcpy( buff, &block->data[ blockShift ], readFromThisBlock );
        }

        position += readFromThisBlock;
        length   -= readFromThisBlock;
        read     += readFromThisBlock;

        buff = (char*)buff + readFromThisBlock;
    }

    return read;
}

FileContentDisk::ModificationData * FileContentDisk::BuildChangeModification( OffsetT position, OffsetT length, const void* data )
{
    assert( length > 0 );

    DiskModificationData* modData = new DiskModificationData( this, position );

    std::vector< char >& before = modData->GetDataBefore();
    before.resize( length );
    Read( &before[ 0 ], position, length );

    std::vector< char >& after = modData->GetDataAfter();
    after.resize( length );

    if ( data )
    {
        std::copy( (char*)data, (char*)data + length, after.begin() );
    }

    return modData;
}

FileContentDisk::ModificationData* FileContentDisk::BuildAddModification(OffsetT position, OffsetT length, const void* data)
{
    assert( length > 0 );

    DiskModificationData* modData = new DiskModificationData( this, position );

    std::vector< char >& after = modData->GetDataAfter();
    after.resize( length );

    if ( data )
    {
        std::copy( (char*)data, (char*)data + length, after.begin() );
    }

    return modData;
}

FileContentDisk::ModificationData * FileContentDisk::BuildRemoveModification(OffsetT position, OffsetT length)
{
    assert( length > 0 );

    DiskModificationData* modData = new DiskModificationData( this, position );

    std::vector< char >& before = modData->GetDataBefore();
    before.resize( length );
    Read( &before[ 0 ], position, length );

    return modData;
}


void FileContentDisk::SetBlock( const char* data, OffsetT pos, OffsetT lengthBefore, OffsetT lengthAfter )
{
    OffsetT collectiveLength = wxMin( lengthBefore, lengthAfter );
    lengthBefore -= collectiveLength;
    lengthAfter  -= collectiveLength;


    // Operate on collective length of the blocks
    size_t i = FindBlock( pos );
    for ( ; collectiveLength && i < m_Contents.size(); ++i )
    {
        DataBlock* block    = m_Contents[ i ];
        OffsetT    blockPos = pos - block->start;
        OffsetT    blockLen = wxMin( collectiveLength, block->size - blockPos );

        if ( block->IsFromDisk() )
        {
            if ( blockPos )
            {
                // There's some data read from disk before current position
                // We have to add new block for our stuff
                block = InsertNewBlock( i++, blockPos );
                blockPos = 0;

                ConsistencyCheck();
            }

            if ( block->size > blockLen )
            {
                // There's some data read from disk after current position
                // We have to add new block after this one for our stuff
                InsertNewBlock( i, blockLen );

                ConsistencyCheck();
            }

            assert( block->size == blockLen );
            assert( block->start == pos );

            // By allocating the data for block we notify that it's
            // no longer read from disk
            block->data.resize( blockLen );
        }

        memcpy( &block->data[ blockPos ], data, blockLen );

        pos              += blockLen;
        collectiveLength -= blockLen;
        data             += blockLen;
    }

    MergeBlocks( i-1 );
    ConsistencyCheck();

    // Right now at least one of lengthBefore or lengthAfter
    // must be zeroed
    assert( !( lengthBefore && lengthAfter ) );

    if ( lengthBefore )
    {
        // Block before the change was longer - we have to remove some data

        i = FindBlock( pos );

        OffsetT positionShift = lengthBefore;

        for ( ; lengthBefore && i < m_Contents.size(); )
        {
            DataBlock* block    = m_Contents[ i ];
            OffsetT    blockPos = pos - block->start;
            OffsetT    blockLen = wxMin( lengthBefore, block->size - blockPos );

            if ( blockPos + blockLen < block->size )
            {
                // The removed block ends somewhere inside
                // current block, we must split it
                DataBlock* newBlock = InsertNewBlock( i, blockPos + blockLen );

                if ( !block->IsFromDisk() )
                {
                    // Need to copy the data if not from disk
                    newBlock->data.insert(
                        newBlock->data.end(),
                        block->data.begin() + block->size,
                        block->data.end() );

                    block->data.resize( block->size );
                }
            }

            if ( blockPos )
            {
                // The removed blocks starts somewhere inside current block,
                // we must split it
                DataBlock* newBlock = InsertNewBlock( i++, blockPos );

                if ( !block->IsFromDisk() )
                {
                    block->data.resize( block->size );
                }

                block = newBlock;
            }

            assert( block->start == pos );
            assert( block->size == blockLen );

            delete m_Contents[ i ];
            m_Contents.erase( m_Contents.begin() + i );

            pos          += blockLen;
            lengthBefore -= blockLen;
        }

        while ( i < m_Contents.size() )
        {
            m_Contents[ i++ ]->start -= positionShift;
        }
    }

    if ( lengthAfter )
    {
        // Block before the change was shorter - we have to add some data

        i = FindBlock( pos );

        if ( i == m_Contents.size() )
        {
            DataBlock* last = m_Contents.back();

            // Request to add the block at the end of data
            DataBlock* newBlock = new DataBlock;
            newBlock->start = last->start + last->size;
            newBlock->size  = lengthAfter;
            newBlock->data.resize( lengthAfter );

            memcpy( &newBlock->data[0], data, lengthAfter );
        }
        else
        {
            OffsetT positionShift = lengthAfter;

            DataBlock* block    = m_Contents[ i ];
            OffsetT    blockPos = pos - block->start;

            if ( block->IsFromDisk() )
            {
                assert( blockPos < block->size );

                // Cut the content at the blockPos and put it into
                // new block after current one
                InsertNewBlock( i, blockPos );

                if ( blockPos )
                {
                    // If there's something before the place we want
                    // to use as insertion point, we need to create additional
                    // empty block for it
                    block = InsertNewBlock( i++, blockPos );
                    blockPos = 0;
                }
            }

            block->data.insert( block->data.begin() + blockPos, data, data + lengthAfter );
            block->size += lengthAfter;

            while ( ++i < m_Contents.size() )
            {
                m_Contents[ i ]->start += positionShift;
            }
        }
    }

    ConsistencyCheck();
}

FileContentDisk::DataBlock* FileContentDisk::InsertNewBlock( size_t blockIndex, OffsetT position )
{
    DataBlock* block = m_Contents[ blockIndex ];

    assert( position <= block->size );

    DataBlock* newBlock = new DataBlock;
    newBlock->start     = block->start     + position;
    newBlock->fileStart = block->fileStart + position;
    newBlock->size      = block->size      - position;

    block->size = position;

    m_Contents.insert( m_Contents.begin() + blockIndex + 1, newBlock );

    return newBlock;
}


size_t FileContentDisk::FindBlock(OffsetT offset)
{
    // TODO: Anybody knows how this should be done using stl-like way ?
    //       (I mean to call this:  std::lower_bound( .. , offset, cmp() )
    struct cmp
    {
        static inline bool f( OffsetT offset, const DataBlock* block1 )
        {
            return block1->start > offset;
        }
    };

    // Validate buffers
    ConsistencyCheck();

    std::vector< DataBlock* >::iterator it = std::upper_bound( m_Contents.begin(), m_Contents.end(), offset, &cmp::f );

    assert( it != m_Contents.begin() );

    --it;

    if ( (*it)->start + (*it)->size <= offset )
    {
        return m_Contents.size();
    }

    return it - m_Contents.begin();
}

void FileContentDisk::ConsistencyCheck()
{
    assert( !m_Contents.empty() );

    for ( size_t i=1; i<m_Contents.size(); ++i )
    {
        DataBlock* b1 = m_Contents[ i-1 ];
        DataBlock* b2 = m_Contents[ i ];

        my_assert( b1->size );
        my_assert( b2->size );

        my_assert( b1->start + b1->size == b2->start );

        my_assert( b1->IsFromDisk() || ( b1->size == b1->data.size() ) );
        my_assert( b2->IsFromDisk() || ( b2->size == b2->data.size() ) );
    }
}

void FileContentDisk::MergeBlocks( size_t startPosition )
{
    // Make sure we didn't get out of valid scope
    startPosition = wxMin( startPosition, m_Contents.size() - 1 );

    bool isFromDisk = m_Contents[ startPosition ]->IsFromDisk();

    // Find first block of merged sequence
    size_t firstMergedBlock = startPosition;
    while ( firstMergedBlock > 0  )
    {
        DataBlock* block = m_Contents[ firstMergedBlock - 1 ];

        if ( isFromDisk )
        {
            // Break if we started merging file blocks and this one is
            // not from the file
            if ( !block->IsFromDisk() ) break;

            // Break also if current block is ffrom disk but does not
            // provide continous addressing inside the file
            if ( block->fileStart + block->size != m_Contents[ firstMergedBlock ]->fileStart ) break;
        }
        else
        {
            // Break if current block is from file - we started with memory blocks
            if ( block->IsFromDisk() ) break;
        }

        firstMergedBlock--;
    }

    // Find last block of merged sequence
    size_t lastMergedBlock = startPosition;
    while ( lastMergedBlock < m_Contents.size()-1 )
    {
        DataBlock* block     = m_Contents[ lastMergedBlock + 1 ];
        DataBlock* blockPrev = m_Contents[ lastMergedBlock ];

        if ( isFromDisk )
        {
            // Break if we started merging file blocks and this one is
            // not from the file
            if ( !block->IsFromDisk() ) break;

            // Break also if current block is ffrom disk but does not
            // provide continous addressing inside the file
            if ( blockPrev->fileStart + blockPrev->size != block->fileStart ) break;
        }
        else
        {
            // Break if current block is from file - we started with memory blocks
            if ( block->IsFromDisk() ) break;
        }

        lastMergedBlock++;
    }


    // Return if nothing to do
    if ( firstMergedBlock == lastMergedBlock ) return;

    // Now we have following state:
    //  firstMergedBlock - index inside m_Contents vector for first
    //                     block in merged sequence
    //  lastMergedBlock - index inside m_Contents vector for last
    //                    block in merged sequence

    DataBlock* block = m_Contents[ firstMergedBlock ];

    if ( !isFromDisk )
    {
        // To speed things a little bit first we calculate size of concatenated
        // block and reserve space for it
        size_t size = 0;
        for ( size_t i=firstMergedBlock; i<=lastMergedBlock; ++i )
        {
            size += m_Contents[ i ]->size;
        }
        block->data.reserve( size );
    }

    // Now we can concatenate
    for ( size_t i = firstMergedBlock+1; i <= lastMergedBlock; ++i )
    {
        DataBlock* nextBlock = m_Contents[ i ];

        if ( isFromDisk )
        {
            // Merging disk blocks, we only have to make sure that blocks fit together
            assert( block->fileStart + block->size == nextBlock->fileStart );
        }
        else
        {
            // When merging data blocks we have to concatenate data
            block->data.insert( block->data.end(), nextBlock->data.begin(), nextBlock->data.end() );
        }

        block->size += nextBlock->size;
        delete nextBlock;
    }

    // Removed all merged blocks from m_Contents vector
    m_Contents.erase( m_Contents.begin() + firstMergedBlock + 1, m_Contents.begin() + lastMergedBlock + 1 );
}

bool FileContentDisk::WriteFileEasiest( )
{
    static const int maxProgress = 10000;

    std::unique_ptr< wxProgressDialog > dlg(
        m_TestMode ? 0 : new wxProgressDialog( _("Saving the file"), _("Please wait, saving file..."), maxProgress,
                    Manager::Get()->GetAppWindow(),
                    wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME ) );

    if ( dlg.get() ) dlg->Update( 0 );

    DataBlock* newBlock = new DataBlock;
    newBlock->start = 0;
    newBlock->fileStart = 0;
    newBlock->size = 0;


    // Calculate total number of bytes to be written
    OffsetT totalSize = 0;
    for ( size_t i=0; i<m_Contents.size(); ++i )
    {
        if ( !m_Contents[ i ]->IsFromDisk() )
        {
            totalSize += m_Contents[ i ]->size;
        }
    }

    // Save the data
    OffsetT totalWritten = 0;
    for ( size_t i=0; i<m_Contents.size(); ++i )
    {
        DataBlock* block = m_Contents[ i ];

        if ( !block->IsFromDisk() )
        {
            m_File.Seek( block->start );

            size_t pos = 0;
            for ( OffsetT left = block->size; left > 0; )
            {
                // Store data using 1MB data blocks at most
                OffsetT thisSize = wxMin( left, 1024*1024 );

                if ( m_File.Write( &block->data[pos], (size_t)thisSize ) != thisSize )
                {
                    cbMessageBox( _("Error occured while saving data") );
                    m_Contents.erase( m_Contents.begin(), m_Contents.begin() + i );
                    m_Contents.insert( m_Contents.begin(), newBlock );
                    return false;
                }

                left -= thisSize;
                pos  += thisSize;
                totalWritten += thisSize;

                if ( dlg.get() ) dlg->Update( (int)( (double)totalWritten / (double)totalSize * (double)maxProgress ) );
            }
        }

        newBlock->size += block->size;

        delete block;
        m_Contents[ i ] = 0;
    }

    m_Contents.clear();
    m_Contents.push_back( newBlock );
    return true;
}


bool FileContentDisk::WriteFileOnDisk()
{
    // TODO: This needs an algorithm of detection of save sequence
    return false;
}

bool FileContentDisk::WriteFileTemporary()
{
    wxString nameProposition = m_FileName + _T(".cbTemp");
    for ( int i=0; i<1000 && wxFileExists( nameProposition ); ++i )
    {
        nameProposition = wxString::Format( _T("%s.cbTemp.%03d"), m_FileName.c_str(), i );
    }

    if ( wxFileExists( nameProposition ) )
    {
        cbMessageBox( _("Couldn't create temporary file.\n"
                        "Any temporary name proposition was invalid") );
        return false;
    }

    wxFile fl( nameProposition, wxFile::write );
    if ( !fl.IsOpened() )
    {
        cbMessageBox( _("Couldn't create temporary file.\n") );
        return false;
    }

    if ( !WriteToFile( fl ) )
    {
        fl.Close();
        wxRemoveFile( nameProposition );
        cbMessageBox( _("Couldn't write data to temporary file") );
        return false;
    }

    m_File.Close();
    fl.Close();

    if ( !wxRenameFile( nameProposition, m_FileName, true ) )
    {
        cbMessageBox( _("Couldn not replace old file with new one") );
        return false;
    }

    if ( !m_File.Open( m_FileName, wxFile::read_write ) )
    {
        cbMessageBox( _("Couldn't reopen file after save") );
        return false;
    }

    ResetBlocks();

    return true;
}

bool FileContentDisk::WriteToDifferentFile(const wxString& fileName)
{
    wxFile fl( fileName, wxFile::write );
    if ( !fl.IsOpened() )
    {
        cbMessageBox( _("Can not create output file") );
        return false;
    }

    if ( !WriteToFile( fl ) )
    {
        cbMessageBox( _("Error while saving to file") );
        return false;
    }

    fl.Close();
    m_FileName = fileName;
    if ( !m_File.Open( m_FileName ) )
    {
        cbMessageBox( _("Couldn't reopen file after save") );
        return false;
    }

    ResetBlocks();
    return true;
}

bool FileContentDisk::WriteToFile(wxFile& file)
{
    static const int maxProgress = 10000;

    std::unique_ptr< wxProgressDialog > dlg(
        m_TestMode ? 0 : new wxProgressDialog( _("Saving the file"), _("Please wait, saving file..."), maxProgress,
                    Manager::Get()->GetAppWindow(),
                    wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME ) );

    if ( dlg.get() ) dlg->Update( 0 );

    OffsetT totalSize = GetSize();
    OffsetT totalWritten = 0;

    for ( size_t i=0; i<m_Contents.size(); ++i )
    {
        DataBlock* block = m_Contents[ i ];
        if ( block->IsFromDisk() )
        {
            m_File.Seek( block->fileStart );
            for ( OffsetT left = block->size; left > 0; )
            {
                // Store data using 128kB data blocks at most
                OffsetT thisSize = wxMin( left, 128*1024 );

                char buff[ 128*1024 ];

                size_t read = m_File.Read( buff, (size_t)thisSize );
                if ( read != thisSize )
                {
                    cbMessageBox( _("Couldn't read data from original file") );
                    return false;
                }

                if ( file.Write( buff, read ) != read )
                {
                    cbMessageBox( _("Error while writing data") );
                    return false;
                }

                left -= read;
                totalWritten += read;

                if ( dlg.get() ) dlg->Update( (int)( (double)totalWritten / (double)totalSize * (double)maxProgress ) );
            }
        }
        else
        {
            size_t pos = 0;
            for ( OffsetT left = block->size; left > 0; )
            {
                // Store data using 128kB data blocks at most
                OffsetT thisSize = wxMin( left, 1024*1024 );

                if ( file.Write( &block->data[pos], (size_t)thisSize ) != thisSize )
                {
                    cbMessageBox( _("Error while writing data") );
                    return false;
                }

                left -= thisSize;
                pos  += thisSize;
                totalWritten += thisSize;

                if ( dlg.get() ) dlg->Update( (int)( (double)totalWritten / (double)totalSize * (double)maxProgress ) );
            }

        }
    }

    return true;
}

void FileContentDisk::ResetBlocks()
{
    ClearBlocks();

    DataBlock* first = new DataBlock;
    first->start = 0;
    first->fileStart = 0;
    first->size = m_File.Length();

    m_Contents.push_back( first );
}

class FileContentDisk::TestData
{
    public:

        TestData()
        {
            m_Content.m_TestMode = true;
            // Open temporary file to make sure we won't harm anybody
            OpenTempFile();
        }

        ~TestData()
        {
            CloseTempFile();
        }

        void Reset( int initialSize = 1 )
        {
            CloseTempFile();
            OpenTempFile( initialSize );
        }

        /** \brief Check if the mirrored data is the same as the original one */
        bool MirrorCheck()
        {
            char Buff[0x1000];

            if ( m_ContentMirror.size() != m_Content.GetSize() )
            {
                return false;
            }

            OffsetT pos = 0;
            for ( size_t left = m_ContentMirror.size(); left > 0; )
            {
                size_t thisBlock = wxMin( left, sizeof(Buff) );
                if ( m_Content.Read( Buff, pos, thisBlock ) != thisBlock )
                {
                    return false;
                }
                char* ptr1 = Buff;
                char* ptr2 = &m_ContentMirror[ pos ];

                if ( memcmp( ptr1, ptr2, thisBlock ) )
                {
                    return false;
                }

                left -= thisBlock;
                pos  += thisBlock;
            }

            return true;
        }

        /** \brief Write random data at given position with given length */
        bool Write( OffsetT position, OffsetT length )
        {
            std::vector< char > buff = TempBuff( (int)length );

            if ( m_Content.Write( ExtraUndoData(), &buff[0], position, length ) != length )
            {
                return false;
            }

            for ( size_t i=0; i<buff.size(); ++i )
            {
                if ( position + i < m_ContentMirror.size() )
                {
                    m_ContentMirror[ position+i ] = buff [ i ];
                }
            }

            return MirrorCheck();
        }

        /** \brief Remove given block of data */
        bool Remove( OffsetT position, OffsetT length )
        {
            if ( m_Content.Remove( ExtraUndoData(), position, length ) != length ) return false;

            if ( position < m_ContentMirror.size() )
            {
                m_ContentMirror.erase(
                    m_ContentMirror.begin() + position,
                    m_ContentMirror.begin() + wxMin( m_ContentMirror.size(), position + length ) );
            }

            return MirrorCheck();
        }

        /** \brief Add new block of data */
        bool Add( OffsetT position, OffsetT length )
        {
            std::vector< char > buff = TempBuff( (int)length );

            if ( m_Content.Add( ExtraUndoData(), position, length, &buff[0] ) != length )
            {
                return false;
            }

            if ( position <= m_ContentMirror.size() )
            {
                m_ContentMirror.insert( m_ContentMirror.begin() + position, buff.begin(), buff.end() );
            }

            return MirrorCheck();
        }

        /** \brief Save the file */
        bool Save()
        {
            m_Content.WriteFile( m_Content.m_FileName );
            return MirrorCheck();
        }

    protected:

        size_t Size() { return m_ContentMirror.size(); }
        void   ResetBlocks() { m_Content.ResetBlocks(); }

    private:

        std::vector< char > TempBuff( int length )
        {
            std::vector< char > buff( length );
            for ( size_t i=0; i<buff.size(); ++i ) buff[ i ] = (char)( rand() );
            return buff;
        }

        /** \brief Open temporary file in the filecontent object */
        void OpenTempFile( int initialSize = 1 )
        {
            m_Content.m_FileName = wxFileName::CreateTempFileName( wxEmptyString, &m_Content.m_File );

            std::vector< char > buff = TempBuff( initialSize );
            m_Content.m_File.Write( &buff[0], initialSize );

            m_Content.ResetBlocks();
            m_ContentMirror.clear();
            m_ContentMirror.swap( buff );
        }

        /** \brief Close temporary fileif it has been openeed */
        void CloseTempFile()
        {
            // Close and erase the file we were working on
            m_Content.m_File.Close();
            wxRemoveFile( m_Content.m_FileName );
        }


        FileContentDisk m_Content;
        std::vector< char > m_ContentMirror;
};

typedef TestCasesHelper< FileContentDisk::TestData > TestCases;
static TestCases testCases;

TestCasesBase & FileContentDisk::GetTests()
{
    return testCases;
}

template<>
template<>
void TestCases::Test<1>()
{
    Reset( 1024 );

    for ( int i=0; i<1024; i++ )
    {
        Ensure( Write( i, 1 ), _T("Writing one byte" ) );
    }
}

template<>
template<>
void TestCases::Test<2>()
{
    Reset( 1024 );

    for ( int i=0; i<1024; i+=2 )
    {
        Ensure( Write( i, 1 ), _T("Writing one byte with one byte left untouched" ) );
    }
}


template<>
template<>
void TestCases::Test<3>()
{
    Reset( 1024 );

    for ( int i=0; i<1024; i++ )
    {
        size_t pos = rand() % 1024;
        size_t size = rand() % ( 1024 - pos );
        Ensure( Write( pos, size ), _T("Writing random block of data" ) );
    }
}


template<>
template<>
void TestCases::Test<4>()
{
    Reset( 1024 * 1024 );
    Ensure( MirrorCheck(), _T("Broken from the beginning") );

    for ( int i=0; i<128; i++ )
    {

        switch ( rand() % 10 )
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            {
                size_t pos  = rand() % ( Size() );
                size_t size = rand() % ( Size() - pos );
                Ensure( Write( pos, size ), _T("Stress test over 1MB initial file size - write" ) );
                break;
            }

            case 6:
            case 7:
            {
                size_t pos  = rand() % ( Size() );
                size_t size = 100;
                Ensure( Add( pos, size ), _T("Stress test over 1MB initial file size - add" ) );
                break;
            }

            case 8:
            case 9:
            {
                size_t pos  = rand() % ( Size() - 100 );
                size_t size = 100;
                Ensure( Remove( pos, size ), _T("Stress test over 1MB initial file size - remove" ) );
                break;
            }
            default:
                break;
        }
    }

    Ensure( Save(), _T("Save complicated file") );
}

template<>
template<>
void TestCases::Test<5>()
{
    Reset( 1024 );

    for ( int i=0; i<1024; i+=2 )
    {
        Ensure( Write( i, 1 ), _T("Writing one byte" ) );
    }

    Ensure( Save(), _T("Save file using simple method (chees layout)") );
}

template<>
template<>
void TestCases::Test<6>()
{
    Reset( 1024*1024 );

    Ensure( Remove( 1024*1023, 1024 ), _T("Removing 1kB from the end of 1MB file") );

    Ensure( Save(), _T("Saving file after removing some part at the end") );

    ResetBlocks();

    Ensure( MirrorCheck(), _T("Saving file after removing some part at the end (2)") );
}
