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

#include <tinyxml.h>

#if defined(__WIN32__) || defined(_WIN32) || defined(__WIN32) || defined(__WIN64) || defined(__WIN64__)
    #define WIN32_LEAN_AND_MEAN 1
    #define NOGDI
    #include <windows.h>
    #include <direct.h>
    inline void set_env(const char* k, const char* v) { SetEnvironmentVariable(k, v); }
    inline bool fileExists(const char* path)
    {
        DWORD attr = GetFileAttributes(path);
        if (attr == INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND)
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

bool GetProcessOutput(std::string& output, const std::string& cmd);
bool QueryVersionControl(const std::string& workingDir, std::string& vcsExecutable, std::string& revision, std::string& date);
bool ParseFile(const std::string& docFile, std::string& revision, std::string& date);
bool WriteOutput(const std::string& outputFile, std::string& revision, std::string& date);
int main(int argc, char** argv);

bool do_int = false;
bool do_std = false;
bool do_wx  = false;
bool do_translate  = false;
bool be_verbose  = false;

int main(int argc, char** argv)
{
    std::string workingDir = "";
    std::string outputFile = "";
    std::string vcsExecutable = "";

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp("+int", argv[i]) == 0)
            do_int = true;
        else if (strcmp("+std", argv[i]) == 0)
            do_std = true;
        else if (strcmp("+wx", argv[i]) == 0)
            do_wx = true;
        else if (strcmp("+t", argv[i]) == 0)
            do_translate = true;
        else if (strcmp("-v", argv[i]) == 0)
            be_verbose = true;
        else if (workingDir.empty())
            workingDir.assign(argv[i]);
        else if (outputFile.empty())
            outputFile.assign(argv[i]);
        else if (vcsExecutable.empty())
            vcsExecutable.assign(argv[i]);
        else
            break;
    }

    if (workingDir.empty())
    {
        puts("Usage: autorevision [options] directory [autorevision.h] [vcs_executable]");
        puts("Options:");
        puts("         +int assign const unsigned int");
        puts("         +std assign const std::string");
        puts("         +wx  assing const wxString");
        puts("         +t   add Unicode translation macros to strings");
        puts("         -v   be verbose");
        puts("Example on Windows: autorevision.exe +int +wx C:\\SVNSources include/autorevision.h C:\\Subversion\\svn.exe");
        puts("Example on Linux:   autorevision     +int +wx /home/user/SVNSources include/autorevision.h /usr/bin/svn");
        return 1;
    }

    if (outputFile.empty())
        outputFile.assign("autorevision.h");

    std::string revision;
    std::string date;
    std::string comment;
    std::string old;

    QueryVersionControl(workingDir, vcsExecutable, revision, date);
    WriteOutput(outputFile, revision, date);

    return 0;
}

bool GetProcessOutput(std::string& output, const std::string& cmd)
{
    FILE *vcsCommand = popen(cmd.c_str(), "r");
    if (!vcsCommand)
        return false;

    char buf[16384] = {'0'};
    fread(buf, 16383, 1, vcsCommand);
    int ret = pclose(vcsCommand);
    output = buf;

    return (WIFEXITED(ret) && (WEXITSTATUS(ret) == 0));
}

bool QueryVersionControl(const std::string& workingDir, std::string& vcsExecutable, std::string& revision, std::string& date)
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
        if (vcsExecutable.empty())
            vcsExecutable = "svn";

        if (GetProcessOutput(output, vcsExecutable + " info --xml --non-interactive " + workingDir))
        {
            TiXmlDocument doc;
            doc.Parse(output.c_str());

            if (doc.Error())
            {
                puts("Warning: Invalid XML output from SVN executable.");
                return false;
            }

            TiXmlHandle hCommit(&doc);
            hCommit = hCommit.FirstChildElement("info").FirstChildElement("entry").FirstChildElement("commit");
            if (const TiXmlElement* e = hCommit.ToElement())
            {
                revision = e->Attribute("revision") ? e->Attribute("revision") : "";
                if (revision.empty())
                    puts("Warning: Could not find 'revision' in SVN output.");

                const TiXmlElement* d = e->FirstChildElement("date");
                if (d && d->GetText())
                {
                    date = d->GetText();
                    std::string::size_type pos = date.find('T');
                    if (pos != std::string::npos)
                    {
                        date[pos] = ' ';
                    }
                    pos = date.rfind('.');
                    if (pos != std::string::npos)
                    {
                        date = date.substr(0, pos);
                    }
                }
                else
                    puts("Warning: Could not find 'date' in SVN output.");

                return true;
            }
            else
                puts("Warning: Could not find '<info><entry><commit>' chain in SVN output.");
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
        std::string output;
        if (vcsExecutable.empty())
            vcsExecutable = "git";

        if (GetProcessOutput(output, vcsExecutable + " log --grep=\"git-svn-id\" --max-count=1" + workingDir))
        {
            std::string::size_type lineStart = output.find("git-svn-id");
            if (lineStart != std::string::npos)
            {
                std::string::size_type revStart = output.find("@", lineStart);
                if (revStart != std::string::npos)
                {
                    revStart++;
                    std::string::size_type revEnd = output.find(" ", revStart);
                    revision = output.substr(revStart, revEnd - revStart);
                    hasRev = true;
                }
                else
                    puts("Warning: Could not find '@' (revision) tag in GIT output.");
            }
            else
                puts("Warning: Could not find 'git-svn-id' tag in GIT output.");
        }

        if (GetProcessOutput(output, vcsExecutable + " log --date=iso --max-count=1 " + workingDir))
        {
            std::string::size_type lineStart = output.find("Date:");
            if (lineStart != std::string::npos)
            {
                lineStart += 5;
                while (lineStart < output.length() && output[lineStart] == ' ')
                    lineStart++;
                std::string::size_type lineEnd = lineStart;
                while (lineEnd < output.length() && output[lineEnd] != ' ')
                    lineEnd++;
                lineEnd++;
                while (lineEnd < output.length() && output[lineEnd] != ' ')
                    lineEnd++;
                date = output.substr(lineStart, lineEnd - lineStart);
                hasDate = true;
            }
            else
                puts("Warning: Could not find 'Date:' tag in GIT output.");
        }

        return hasRev && hasDate;
    }
    // if we are here, we could not read the info
    return true;
}



bool WriteOutput(const std::string& outputFile, std::string& revision, std::string& date)
{
    std::string comment("/*");
    comment.append(revision);
    comment.append("*/");

    {
        std::ifstream in(outputFile.c_str());
        if (!in.bad() && !in.eof())
        {
            std::string old;
            getline(in, old);
            size_t l_old = old.length();
            size_t l_comment = comment.length();
            if (l_old > l_comment || ((l_old == l_comment) && old >= comment))
            {
                if (be_verbose)
                    printf("Revision unchanged or older (%s). Skipping.", old.c_str());
                in.close();
                return false;
            }
        }
    }


    FILE *header = fopen(outputFile.c_str(), "wb");
    if (!header)
    {
        puts("Error: Could not open output file.");
        return false;
    }

    fprintf(header, "%s\n", comment.c_str());
    fprintf(header, "// Don't include this header, only configmanager-revision.cpp should do this.\n");
    fprintf(header, "#ifndef AUTOREVISION_H\n");
    fprintf(header, "#define AUTOREVISION_H\n\n\n");

    if (do_std)
        fprintf(header, "#include <string>\n");
    if (do_wx)
        fprintf(header, "#include <wx/string.h>\n");

    if (do_int || do_std || do_wx)
        fprintf(header, "\nnamespace autorevision\n{\n");

    if (do_int)
        fprintf(header, "\tconst unsigned int svn_revision = %s;\n", revision.c_str());

    if (do_translate)
    {
        revision = "_T(\"" + revision + "\")";
        date = "_T(\"" + date + "\")";
    }
    else
    {
        revision = "\"" + revision + "\"";
        date = "\"" + date + "\"";
    }

    if (do_std)
        fprintf(header, "\tconst std::string svn_revision_s(%s);\n", revision.c_str());
    if (do_wx)
        fprintf(header, "\tconst wxString svnRevision(%s);\n", revision.c_str());

    if (do_std)
        fprintf(header, "\tconst std::string svn_date_s(%s);\n", date.c_str());
    if (do_wx)
        fprintf(header, "\tconst wxString svnDate(%s);\n", date.c_str());

    if (do_int || do_std || do_wx)
        fprintf(header, "}");

    fprintf(header, "\n\n#endif\n");
    fclose(header);

    return true;
}
