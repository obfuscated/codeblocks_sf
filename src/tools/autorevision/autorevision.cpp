/*
* autorevision - a tool to incorporate Subversion revisions into binary builds
* Copyright (C) 2005 Thomas Denk
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <stdio.h>
#include <string>
#include <fstream>

using namespace std;


bool QuerySvn(const string& workingDir, string& revision, string &date);
bool ParseFile(const string& docFile, string& revision, string &date);
bool WriteOutput(const string& outputFile, string& revision, string& date);
int main(int argc, char** argv);


bool do_int = false;
bool do_std = false;
bool do_wx  = false;
bool do_translate  = false;
bool be_verbose  = false;

int main(int argc, char** argv)
{
    string outputFile;
    string workingDir;


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

    string revision;
    string date;
    string comment;
    string old;

    if(!QuerySvn(workingDir, revision, date) && !ParseFile(docFile, revision, date))
    {
        puts("Error: failed retrieving version information.");
        return -1;
    }

    WriteOutput(outputFile, revision, date);

    return 0;
}



bool QuerySvn(const string& workingDir, string& revision, string &date)
{
    string svncmd("svn info ");
    svncmd.append(workingDir);
    FILE *svn = popen(svncmd.c_str(), "r");

    if(svn)
    {
        char buf[1024];
        string line;
        while(fgets(buf, 4095, svn))
        {
            line.assign(buf);
            if(line.find("Revision:") != string::npos)
            {
                revision = line.substr(strlen("Revision: "));

                    string lbreak("\r\n");
                    size_t i;
                    while((i = revision.find_first_of(lbreak)) != string::npos)
                        revision.erase(revision.length()-1);
            }
            if(line.find("Last Changed Date: ") != string::npos)
            {
                    date = line.substr(strlen("Last Changed Date: "), strlen("2006-01-01 12:34:56"));
            }
        }
    }
    pclose(svn);
    return !revision.empty();
}


bool ParseFile(const string& docFile, string& revision, string &date)
{
    string token;
    date.clear();

    ifstream inFile(docFile.c_str());
    if (!inFile)
    {
        puts("Warning: could not open input file.\nThis does not seem to be a revision controlled project.\nRevision set to 0.");
        revision = "0";
        return true;
    }
    else
        while(!inFile.eof())
        {
            inFile >> token;
            if(token.find("revision=\"") != string::npos)
            {
                size_t start = token.find('\"') + 1;
                size_t end   = token.find('\"', start);
                if(start < end)
                {
                    revision = token.substr(start, end - start);
                }
                return true;
            }
        }
    return false;
}


bool WriteOutput(const string& outputFile, string& revision, string& date)
{
    string old;
    string comment("/*");
    comment.append(revision);
    comment.append("*/");

    {
        ifstream in(outputFile.c_str());
        if (!in.bad() && !in.eof())
        {
            in >> old;
            if(old == comment)
            {
                if(be_verbose)
                    printf("Revision unchanged (%s). Skipping.", revision.c_str());
                in.close();
                return false;
            }
        }
        in.close();
    }


    FILE *header = fopen(outputFile.c_str(), "wb");
    if(!header)
    {
        puts("Error: Could not open output file.");
        return false;
    }

    fprintf(header, "%s\n", comment.c_str());
    fprintf(header, "#ifndef AUTOREVISION_H\n");
    fprintf(header, "#define AUTOREVISION_H\n\n\n");

    if(do_std)
        fprintf(header, "#include <string>\n");
    if(do_wx)
        fprintf(header, "#include <wx/string.h>\n");

    fprintf(header, "\n#define SVN_REVISION \"%s\"\n", revision.c_str());
    fprintf(header, "\n#define SVN_DATE     \"%s\"\n\n", date.c_str());

    if(do_int || do_std || do_wx)
        fprintf(header, "namespace autorevision\n{\n");

    if(do_int)
        fprintf(header, "\tconst unsigned int svn_revision = %s;\n", revision.c_str());

    if(do_translate)
    {
        revision = "_T(\"" + revision + "\")";
        date = "_T(\"" + date + "\")";
    }
    else
    {
        revision = "\"" + revision + "\"";
        date = "\"" + date + "\"";
    }

    if(do_std)
        fprintf(header, "\tconst std::string svn_revision_s(%s);\n", revision.c_str());
    if(do_wx)
        fprintf(header, "\tconst wxString svnRevision(%s);\n", revision.c_str());

    if(do_int || do_std || do_wx)
        fprintf(header, "}\n\n");

    fprintf(header, "\n\n#endif\n");
    fclose(header);

    return true;
}

