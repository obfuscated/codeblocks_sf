#include <stdio.h>
#include <stdlib.h>
#include "depslib.h"

#ifdef DEPSLIB_WINDOWS
    #include <io.h> // for getcwd
#elif DEPSLIB_UNIX
    #include <unistd.h> // for getcwd
#endif

#if 0
void test(const char *path)
{
    PATHSPLIT f;
    char buf[1024];

    printf("> %s\n", path);
    path_split(path, &f);
    path_print(&f);
    path_normalize(&f, NULL);
    path_tostring(&f, buf);
    printf("< %s\n", buf);
}
#endif

void depsOK(void)
{
    int error = depsError();

    if (error) exit(error);
}

int main(void)
{
    char *SEARCH1[] = {
        "include",
        0
    };

    char *sources1[] = {
        "src/foo.c",
        "src/main.c",
        "utils/bar.c",
        0
    };

#ifdef DEPSLIB_WINDOWS
    char *cwd1 = "C:/Programming/jam-test";

    char *SEARCH2[] = {
        "..\\..\\..\\sdk",
        "..\\..\\..\\..\\..\\..\\wxWindows-2.4.2\\include",
        "..\\..\\..\\..\\..\\..\\wxWindows-2.4.2-build\\lib\\wx\\include\\mswd-2.4",
        "..\\..\\..\\..\\..\\..\\wxWindows-2.4.2\\contrib\\include",
        "C:\\cygwin\\usr\\include\\mingw",
        0
    };
    char *sources2[] = {
        "..\\advancedcompileroptionsdlg.cpp",
        "..\\compilerBCC.cpp",
        "..\\compilerDMC.cpp",
        "..\\compilererrors.cpp",
        "..\\compilergcc.cpp",
        "..\\compilermessages.cpp",
        "..\\compilerMINGW.cpp",
        "..\\compilerMSVC.cpp",
        "..\\compileroptionsdlg.cpp",
        "..\\customvars.cpp",
        "..\\depends.cpp",
        "..\\directcommands.cpp",
        "..\\directdeps.cpp",
        "..\\linklibdlg.cpp",
        "..\\makefilegenerator.cpp",
        0
    };

    char *cwd2 = "C:/Programming/codeblocks-cvs/codeblocks/src/plugins/compilergcc/depends";
#endif
#ifdef DEPSLIB_UNIX
    char *cwd1 = "/cygdrive/c/Programming/jam-test";

    char *SEARCH2[] = {
        "../../../sdk",
        "../../../../../../wxWindows-2.4.2/include",
        "../../../../../../wxWindows-2.4.2-build/lib/wx/include/mswd-2.4",
        "../../../../../../wxWindows-2.4.2/contrib/include",
        "/usr/include",
        0
    };
    char *sources2[] = {
        "../advancedcompileroptionsdlg.cpp",
        "../compilerBCC.cpp",
        "../compilerDMC.cpp",
        "../compilererrors.cpp",
        "../compilergcc.cpp",
        "../compilermessages.cpp",
        "../compilerMINGW.cpp",
        "../compilerMSVC.cpp",
        "../compileroptionsdlg.cpp",
        "../customvars.cpp",
        "../depends.cpp",
        "../directcommands.cpp",
        "../directdeps.cpp",
        "../linklibdlg.cpp",
        "../makefilegenerator.cpp",
        0
    };

    char *cwd2 = "/cygdrive/c/Programming/codeblocks-cvs/codeblocks/src/plugins/compilergcc/depends";
#endif

    int i;
    struct depsStats stats;

    char cwd[1024];
    getcwd(cwd, sizeof(cwd)); // let wxWindows handle this nastiness

#if 0
test("C:/foo/bar/baz");
test("../foo/bar/baz");
test("C:/");
test("C:");
test("/foo/bar/baz");
test("foo/./bar/../baz");
test("../compilergcc.cpp");
test("..\\..\\..\\..\\..\\..\\wxWindows-2.4.2\\include");
return 0;
}
#endif

    depsStart(); depsOK();
    depsSetCWD(cwd1); depsOK();

    depsSearchStart(); depsOK();
    for (i = 0; SEARCH1[i]; i++)
    {
        depsAddSearchDir(SEARCH1[i]); depsOK();
    }

    for (i = 0; sources1[i]; i++)
    {
        depsRef ref = depsScanForHeaders(sources1[i]);
        depsOK();
        if (ref)
        {
            time_t time;
            const char *newest = depsGetNewest(ref, &time);
            depsOK();
            printf("newest %s > %s\n", newest, sources1[i]);
        }
    }

    depsGetStats(&stats); depsOK();
    printf("scanned %ld files for #include, cache used %ld, cache updated %ld\n",
        stats.scanned, stats.cache_used, stats.cache_updated);

    depsDone(); depsOK();

/* ===== */

    depsStart(); depsOK();
    depsSetCWD(cwd2); depsOK();

    depsCacheRead("depends.cache");

    depsSearchStart(); depsOK();
    for (i = 0; SEARCH2[i]; i++)
    {
        depsAddSearchDir(SEARCH2[i]); depsOK();
    }

    for (i = 0; sources2[i]; i++)
    {
        depsRef ref = depsScanForHeaders(sources2[i]);
        depsOK();
        if (ref)
        {
            time_t time;
            const char *newest = depsGetNewest(ref, &time);
            depsOK();
            printf("newest %s > %s\n", newest, sources2[i]);
        }
    }

    depsGetStats(&stats);
    printf("scanned %ld files for #include, cache used %ld, cache updated %ld\n",
        stats.scanned, stats.cache_used, stats.cache_updated);

    depsCacheWrite("depends.cache");

    depsDone(); depsOK();

    return 0;
}
