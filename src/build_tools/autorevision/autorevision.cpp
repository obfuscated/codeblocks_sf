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
    #include <direct.h>
    inline void set_env(const char* k, const char* v) { SetEnvironmentVariable(k, v); }
    inline bool fileExists(const char* path)
    {
        DWORD attr = GetFileAttributes(path);
        if(attr == INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND)
            return false;   //  not a file
        else
            return true;
    }
    inline std::string getCwd()
    {
        char buffer[1000]={0};
        _getcwd(buffer, 1000);
        return buffer;
    }
#else
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    inline void set_env(const char* k, const char* v) { setenv(k, v, 1); }
    inline bool fileExists(const char* path)
    {
        struct stat buffer;
        return (stat(path, &buffer) == 0);
    }
    inline std::string getCwd()
    {
        char buffer[1000]={0};
        getcwd(buffer, 1000);
        return buffer;
    }
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

bool getProcessOutput(std::string &output, const std::string &cmd)
{
    FILE *svn = popen(cmd.c_str(), "r");
    if (!svn)
        return false;
    char buf[16384] = {'0'};
    fread(buf, 16383, 1, svn);
    int ret = pclose(svn);
    output = buf;
    return (WIFEXITED(ret) && (WEXITSTATUS(ret) == 0));
}

bool QuerySvn(const string& workingDir, string& revision, string &date)
{
    revision = "0";
    date = "unknown date";

    std::string svnRoot = getCwd() + '/' + workingDir + "/.svn";
    std::string gitRoot = getCwd() + '/' + workingDir + "/.git";

    bool hasSvn = fileExists(svnRoot.c_str());
    bool hasGit = fileExists(gitRoot.c_str());

    if (hasSvn)
    {
        // first try svn info with xml-output
        std::string output;
        if(getProcessOutput(output, "svn info --xml --non-interactive " + workingDir))
        {
            TiXmlDocument doc;
            doc.Parse(output.c_str());

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

    // Search git history for the last svn commit.
    if (hasGit)
    {
        // ensure we have an english environment, needed
        // to correctly parse output of localized (git) svn info
#ifndef __MINGW32__
        setenv("LC_ALL", "C", 1);
#else
        setlocale(LC_ALL, "C");
#endif
        bool hasRev = false, hasDate = false;
        string output;
        if (getProcessOutput(output, "git log --grep=\"git-svn-id\" --max-count=1" + workingDir))
        {
            string::size_type lineStart = output.find("git-svn-id");
            if (lineStart != string::npos)
            {
                string::size_type revStart = output.find("@", lineStart);
                if (revStart != string::npos)
                {
                    revStart++;
                    string::size_type revEnd = output.find(" ", revStart);
                    revision = output.substr(revStart, revEnd - revStart);
                    hasRev = true;
                }
            }
        }

        if (getProcessOutput(output, "git log --date=iso --max-count=1 " + workingDir))
        {
            string::size_type lineStart = output.find("Date:");
            if (lineStart != string::npos)
            {
                lineStart += 5;
                while (lineStart < output.length() && output[lineStart] == ' ')
                    lineStart++;
                string::size_type lineEnd = lineStart;
                while (lineEnd < output.length() && output[lineEnd] != ' ')
                    lineEnd++;
                lineEnd++;
                while (lineEnd < output.length() && output[lineEnd] != ' ')
                    lineEnd++;
                date = output.substr(lineStart, lineEnd - lineStart);
                hasDate = true;
            }
        }

        return hasRev && hasDate;
    }
    // if we are here, we could not read the info
    return true;
}



bool WriteOutput(const string& outputFile, string& revision, string& date)
{
    string comment("/*");
    comment.append(revision);
    comment.append("*/");

    {
        ifstream in(outputFile.c_str());
        if (!in.bad() && !in.eof())
        {
            string old;
            getline(in, old);
            size_t l_old = old.length();
            size_t l_comment = comment.length();
            if(l_old > l_comment || ((l_old == l_comment) && old >= comment))
            {
                if(be_verbose)
                    printf("Revision unchanged or older (%s). Skipping.", old.c_str());
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

