#include <stdio.h>
#include <stdlib.h>
#ifdef __WXMSW__
	#include <conio.h>
	#define wait_key getch
#else
	#define wait_key getchar
#endif
#include <string.h>

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

int main(int argc, char** argv)
{
	if (argc < 2)
    {
        printf("Usage: console_runner <filename> <args ...>\n");
        return 1;
    }

    // count size of arguments
    int fullsize = 0;
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
    
    //printf("Would run '%s'\n", cmdline);
    int ret = system(cmdline);
    printf("\nPress ENTER to continue.\n");
    wait_key();
    
    delete[] cmdline;
    return ret;
}
