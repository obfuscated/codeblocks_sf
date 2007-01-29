/*
* Humbug!
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../../sdk/tinyxml/tinystr.h"
#include "../../sdk/tinyxml/tinyxml.h"


void ProcessFile(const char* fileName);
int main(int argc, char** argv);
void ls(char *dirname);


void ls(char *dirname)
{
	struct dirent *entry;
	struct stat buf;

    chdir(dirname);
	DIR *d = opendir(".");

	if(d == 0)
        return;


	for(entry = readdir(d); entry != NULL; entry = readdir(d))
	{
		if (stat(entry->d_name, &buf) < 0 || entry->d_name[0] == '.' || entry->d_name[0] == '_')
			continue;

		if (S_ISDIR(buf.st_mode))
		{
            ls(entry->d_name);
		}
		else
		{
            if(strstr(entry->d_name, ".xrc"))
                ProcessFile(entry->d_name);
		}
	}
	closedir(d);
    chdir("..");
}



int main(int argc, char** argv)
{
    puts("Humbug!");

	if(argc != 2)
		return 0;

    ls(argv[1]);

    return 0;
}


void ProcessFile(const char* fileName)
{
        TiXmlDocument doc;
        doc.LoadFile(fileName);
		if(doc.Error())
		{
            printf("error in file %s: %s\n", fileName, doc.ErrorDesc());
			return;
        }
		TiXmlPrinter printer;
		printer.SetStreamPrinting();
		doc.Accept(&printer);

		FILE *f = fopen(fileName, "wb");
		if(!f)
			return;

		fwrite(printer.CStr(), printer.Size(), 1, f);
		fclose(f);
}

