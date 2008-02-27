/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

/*
    This code was taken from:
    http://wikisource.org/wiki/CRC32_Checksum_function
    by an unknown author...

    I just changed the function names to match the conventions used
    in the rest of the project.

    Yiannis Mandravellos <mandrav@codeblocks.org>
*/

#include <cstdio>

static unsigned long *GetCRC32Table( unsigned long *crc_table )
{
    // First call to this function? Or after freeing the memory?
    if ( !crc_table )
    {
        // Allocate memory
        crc_table = new unsigned long[256];

        // Was the allocation succesfull?
        if ( crc_table )
        {
            // Generate the crc table
            unsigned long crc ;
            int i, j ;

            for(i = 0; i < 256; i++)
            {
                crc = i;
                for (j = 8; j > 0; j--)
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

unsigned long GetFileCRC32(const char* file)
{
    static unsigned long *crc_table = NULL;
    unsigned long crc = 0;

    if (file)
    {
        // Get the crc table, on first call, generate, otherwise do nothing
        crc_table = GetCRC32Table( crc_table ) ;

        // Do we have a crc table?
        if ( crc_table )
        {
            // Open the file for reading
            FILE *fp = fopen(file, "r");

            // Was the file open succesfull?
            if (fp)
            {
                // Calculate the checksum
                int ch;

                crc = 0xFFFFFFFFUL;
                while ((ch = getc(fp)) != EOF)
                    { crc = (crc>>8) ^ crc_table[ (crc^ch) & 0xFF ]; }

                crc ^= 0xFFFFFFFFUL ;

                // Close the file
                fclose(fp);
            }
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

unsigned long GetTextCRC32(const char* text)
{
    static unsigned long *crc_table = NULL;
    unsigned long crc = 0;

    if (text)
    {
        // Get the crc table, on first call, generate, otherwise do nothing
        crc_table = GetCRC32Table( crc_table ) ;

        // Do we have a crc table?
        if ( crc_table )
        {
            // Calculate the checksum
            crc = 0xFFFFFFFFUL;
            while (*text)
                { crc = (crc>>8) ^ crc_table[ (crc^(*text++)) & 0xFF ]; }

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
