/*
* autorevision - a tool to incorporate Subversion revisions into binary builds
* Copyright (C) 2005 Thomas Denk
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Id$
* $Date$
*/

#include <stdio.h>
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char** argv)
{
    string outputFile;
    string workingDir;

    bool do_int = false;
    bool do_std = false;
    bool do_wx  = false;
    bool do_translate  = false;
    bool be_verbose  = false;

    for(int i = 1; i < argc; ++i)
    {
        if(strcmp("+int", argv[i]) == 0)
            do_int = true;
        else if(strcmp("+std", argv[i]) == 0)
            do_std = true;
        else if(strcmp("+wx", argv[i]) == 0)
            do_wx = true;
        else if(strcmp("+t", argv[i]) == 0)
            do_translate = true;
        else if(strcmp("-v", argv[i]) == 0)
            be_verbose = true;
        else if(workingDir.empty())
            workingDir.assign(argv[i]);
        else if(outputFile.empty())
            outputFile.assign(argv[i]);
        else
            break;
    }

    if (workingDir.empty())
    {
        puts("Usage: autorevision [options] directory [autorevision.h]");
        puts("Options:");
        puts("         +int assign const unsigned int");
        puts("         +std assign const std::string");
        puts("         +wx  assing const wxString");
        puts("         +t   add Unicode translation macros to strings");
        puts("         -v   be verbose");
        return 1;
    }

    if(outputFile.empty())
        outputFile.assign("autorevision.h");

    string docFile(workingDir);
    docFile.append("/.svn/entries");


    ifstream inFile(docFile.c_str());
    if (!inFile)
    {
        puts("Error: could not open input file");
        return -1;
    }

    string token;
    string stoken;
    string old;

    while(!inFile.eof())
    {
        inFile >> token;
        if(token.find("revision=\"") != string::npos)
        {
            size_t start = token.find('\"') + 1;
            size_t end   = token.find('\"', start);
            if(start < end)
            {
                token = token.substr(start, end - start);
                string comment("/*");
                comment.append(token);
                comment.append("*/");

                {
                    ifstream in(outputFile.c_str());
                    if (!in.bad() && !in.eof())
                    {
                        in >> old;
                        if(old == comment)
							{
							if(be_verbose)
								printf("Revision unchanged (%s). Skipping.", token.c_str());
							in.close();
							inFile.close();
                            return 0;
							}
						else if(be_verbose)
							printf("Generating %s for revision %s.", outputFile.c_str(), token.c_str());

                    }
                    in.close();
                }


                FILE *f = fopen(outputFile.c_str(), "wb");

                if(!f)
                {
                    puts("Error: Could not open output file.");
                    return -1;
                }
                fprintf(f, "%s\n", comment.c_str());
                fprintf(f, "#ifndef AUTOREVISION_H\n");
                fprintf(f, "#define AUTOREVISION_H\n\n\n");

                if(do_std)
                    fprintf(f, "#include <string>\n");
                if(do_wx)
                    fprintf(f, "#include <wx/string.h>\n");

                fprintf(f, "\n#define SVN_REVISION \"%s\"\n\n", token.c_str());

				if(do_int || do_std || do_wx)
					fprintf(f, "namespace autorevision\n{\n\n");

				if(do_translate)
					{
					stoken.assign("_T(\"");
					stoken.append(token);
					stoken.append("\")");
					}
				else
					{
					stoken.assign("\"");
					stoken.append(token);
					stoken.append("\"");
					}

                if(do_int)
                    fprintf(f, "const unsigned int svn_revision = %s;\n", token.c_str());
                if(do_std)
                    fprintf(f, "const std::string svn_revision_s(%s);\n", stoken.c_str());
                if(do_wx)
                    fprintf(f, "const wxString svnRevision(%s);\n", stoken.c_str());

				if(do_int || do_std || do_wx)
					fprintf(f, "\n}\n\n");

                fprintf(f, "\n\n#endif\n");
                fclose(f);

                return 0;
            }
        }
    }
    puts("Error: Could not find revision.");
    return -1;
}

