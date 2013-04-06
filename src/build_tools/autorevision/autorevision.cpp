/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <stdio.h>
#include <string>
#include <fstream>

#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"

using namespace std;

#ifdef __WIN32__
    #define WIN32_LEAN_AND_MEAN 1
    #define NOGDI
    #include <windows.h>
    inline void set_env(const char* k, const char* v) { SetEnvironmentVariable(k, v); }
#else
    #include <stdlib.h>
    inline void set_env(const char* k, const char* v) { setenv(k, v, 1); }
#endif

#if !defined WIFEXITED
    #define WIFEXITED(x) 1
#endif

#if !defined WEXITSTATUS
    #define WEXITSTATUS(x) x
#endif

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

    string revision;
    string date;
    string comment;
    string old;

    QuerySvn(workingDir, revision, date);
    WriteOutput(outputFile, revision, date);

    return 0;
}



bool QuerySvn(const string& workingDir, string& revision, string &date)
{
    revision = "0";
    date = "unknown date";

    string svncmd("svn info --xml --non-interactive ");
    svncmd.append(workingDir);

    FILE *svn = popen(svncmd.c_str(), "r");

    // first try svn info with xml-output
    if(svn)
    {
        char buf[16384] = {'0'};
        fread(buf, 16383, 1, svn);
        int ret = pclose(svn);
        if(WIFEXITED(ret) && (WEXITSTATUS(ret) == 0))
        {
            TiXmlDocument doc;
            doc.Parse(buf);

            if(doc.Error())
                return false;

            TiXmlHandle hCommit(&doc);
            hCommit = hCommit.FirstChildElement("info").FirstChildElement("entry").FirstChildElement("commit");
            if(const TiXmlElement* e = hCommit.ToElement())
            {
                revision = e->Attribute("revision") ? e->Attribute("revision") : "";
                const TiXmlElement* d = e->FirstChildElement("date");
                if(d && d->GetText())
                {
                    date = d->GetText();
                    string::size_type pos = date.find('T');
                    if (pos != string::npos)
                    {
                        date[pos] = ' ';
                    }
                    pos = date.rfind('.');
                    if (pos != string::npos)
                    {
                        date = date.substr(0, pos);            }
                    }
                    return true;
            }
            return false;
        }
    }

    // ensure we have an english environment, needed
    // to correctly parse output of localized (git) svn info
#ifndef __MINGW32__
    setenv("LC_ALL", "C", 1);
#else
    setlocale(LC_ALL, "C");
#endif

    svncmd = "git svn info ";
    svncmd.append(workingDir);
    svn = popen(svncmd.c_str(), "r");
    // second try git svn info
    if(svn)
    {
        char buf[16384] = {'0'};
        fread(buf, 16383, 1, svn);
        int ret = pclose(svn);
        if (!WIFEXITED(ret) || (WEXITSTATUS(ret) != 0))
        {
            svncmd = "svn info --non-interactive ";
            svncmd.append(workingDir);
            svn = popen(svncmd.c_str(), "r");

            // third try oldstyle (outated) svn info (should not be needed anymore)
            if(svn)
            {
                memset(buf, 0, 16384);
                fread(buf, 16383, 1, svn);
                ret = pclose(svn);
                if (!WIFEXITED(ret) || (WEXITSTATUS(ret) != 0))
                    return true;
            }
            else
                return true;
        }
        string what("Last Changed Rev: ");
        string output(buf);
        string::size_type pos = string::npos;
        string::size_type len = 0;
        pos = output.find(what);
        if (pos != string::npos)
        {
            pos += what.length();
            len = 0;
            // revision must be numeric
            while (buf[ pos + len ] >= '0' && buf[ pos + len++ ] <= '9')
                ;
        }
        if (len != 0)
            revision = output.substr(pos, len);

        what = "Last Changed Date: ";
        pos = output.find(what);
        if (pos != string::npos)
        {
            pos += what.length();
            len = output.find(" ", pos);
            // we want the position of the second space
            if (len != string::npos)
                len = output.find(" ", len + 1);
            if (len != string::npos)
                len -= pos;
            else
                len = 0;
        }
        if (len != 0)
            date = output.substr(pos, len);

        return false;
    }
    // if we are here, we could not read the info
    return true;
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
            if(old >= comment)
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
    fprintf(header, "//don't include this header, only configmanager-revision.cpp should do this.\n");
    fprintf(header, "#ifndef AUTOREVISION_H\n");
    fprintf(header, "#define AUTOREVISION_H\n\n\n");

    if(do_std)
        fprintf(header, "#include <string>\n");
    if(do_wx)
        fprintf(header, "#include <wx/string.h>\n");

    if(do_int || do_std || do_wx)
        fprintf(header, "\nnamespace autorevision\n{\n");

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

    if(do_std)
        fprintf(header, "\tconst std::string svn_date_s(%s);\n", date.c_str());
    if(do_wx)
        fprintf(header, "\tconst wxString svnDate(%s);\n", date.c_str());

    if(do_int || do_std || do_wx)
        fprintf(header, "}\n\n");

    fprintf(header, "\n\n#endif\n");
    fclose(header);

    return true;
}

