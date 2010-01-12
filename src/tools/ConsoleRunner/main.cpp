/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#ifdef __WXMSW__
    #include <windows.h>
	#include <conio.h>
	#define wait_key getch
#else
	#define wait_key getchar
#endif
#include <string.h>

#ifdef __MINGW32__
int _CRT_glob = 0;
#endif

bool hasSpaces(const char* str)
{
	char last = 0;
	while (str && *str)
	{
		if ((*str == ' ' || *str == '\t') && last != '\\')
            return true;
        last = *str++;
	}
	return false;
}

int execute_command(char *cmdline)
{
#ifdef __WXMSW__
    //Windows's system() seems to not be able to handle parentheses in
    //the path, so we have to launch the program a different way.

    SetConsoleTitle(cmdline);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process.
    CreateProcess( NULL, TEXT(cmdline), NULL, NULL, FALSE, 0,
                   NULL, NULL, &si, &pi );

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Get the return value of the child process
    DWORD ret;
    GetExitCodeProcess( pi.hProcess, &ret );

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    return ret;
#else
    int ret = system(cmdline);
    if(WIFEXITED(ret))
    {
        return WEXITSTATUS(ret);
    }
    else
    {
        return -1;
    }
#endif
}

int main(int argc, char** argv)
{
	if (argc < 2)
    {
        printf("Usage: cb_console_runner <filename> <args ...>\n");
        return 1;
    }

    // count size of arguments
    size_t fullsize = 0;
    for (int i = 1; i < argc; ++i)
    {
        fullsize += strlen(argv[i]);
    }
    // add some slack for spaces between args plus quotes around executable
    fullsize += argc + 32;

    char* cmdline = new char[fullsize];
    memset(cmdline, 0, fullsize);

    // 1st arg (executable) enclosed in quotes to support filenames with spaces
    bool sp = hasSpaces(argv[1]);
    if (sp)
        strcat(cmdline, "\"");
    strcat(cmdline, argv[1]);
    if (sp)
        strcat(cmdline, "\"");
    strcat(cmdline, " ");

    for (int i = 2; i < argc; ++i)
    {
        sp = hasSpaces(argv[i]);
        if (sp)
            strcat(cmdline, "\"");
        strcat(cmdline, argv[i]);
        if (sp)
            strcat(cmdline, "\"");
        strcat(cmdline, " ");
    }

    timeval tv;
    gettimeofday(&tv, NULL);
    double cl = tv.tv_sec + (double)tv.tv_usec / 1000000;

    int ret = execute_command(cmdline);

    gettimeofday(&tv, NULL);
    cl = (tv.tv_sec + (double)tv.tv_usec / 1000000) - cl;

    printf("\nProcess returned %d (0x%X)   execution time : %0.3f s", ret, ret, cl);
    printf
    (
        "\nPress "
#ifdef __WXMSW__
        "any key"
#else
        "ENTER"
#endif
        " to continue.\n"
    );

    wait_key();

    delete[] cmdline;
    return ret;
}
