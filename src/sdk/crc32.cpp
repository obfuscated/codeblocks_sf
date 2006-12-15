/*
    This code was taken from:
    http://wikisource.org/wiki/CRC32_Checksum_function
    by an unknown author...

    I just changed the function names to match the conventions used
    in the rest of the project.

    Yiannis Mandravellos <mandrav@codeblocks.org>
*/

#include <wx/string.h>
#include <wx/file.h>
#include "crc32.h"
#include "globals.h"

static wxUint32 *GetCRC32Table( wxUint32 *crc_table )
{
    // First call to this function? Or after freeing the memory?
    if ( !crc_table )
    {
        // Allocate memory
        crc_table = new wxUint32[256];

        // Was the allocation succesfull?
        if ( crc_table )
        {
            // Generate the crc table
            for(unsigned int i = 0; i < 256; ++i)
            {
                wxUint32 crc = i;
                for (unsigned int j = 8; j > 0; --j)
                {
                    if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320UL;
                    else         crc >>= 1;
                }
                crc_table[i] = crc;
            }
        }
    }

    // Return the new pointer
    return ( crc_table ) ;
}

wxUint32 wxCrc32::FromFile(const wxString& file)
{
    wxFile f(file);
    wxString contents = cbReadFileContents(f);
    if (contents.IsEmpty())
        return 0;
    return FromString(contents);
}

wxUint32 wxCrc32::FromString(const wxString& text)
{
    static wxUint32 *crc_table = NULL;
    wxUint32 crc = 0;
    unsigned int i = 0;

    if (text)
    {
        // Get the crc table, on first call, generate, otherwise do nothing
        crc_table = GetCRC32Table( crc_table ) ;

        // Do we have a crc table?
        if ( crc_table )
        {
            // Calculate the checksum
            crc = 0xFFFFFFFFUL;
            while (text[i])
                { crc = (crc>>8) ^ crc_table[ (crc^(text[i++])) & 0xFF ]; }

            crc ^= 0xFFFFFFFFUL ;
        }
    }

    // If we have a crc table, delete it from memory
    if ( crc_table ) { delete[] crc_table; }

    // Set it to a null pointer, the have it (re)created on next calls to this
    // function
    crc_table = NULL;

    // Return the checksum result
    return( crc ) ;
}
