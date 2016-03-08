#include "child_pids.h"
#include <string.h>

#ifndef __WXMSW__
#   include <dirent.h>
#endif // __WXMSW__

namespace
{
static int ParseParentPID(const char *line)
{
    const char *p = strchr(line, '(');
    if (!p)
        return -1;
    ++p;
    int openParenCount = 1;
    while (*p && openParenCount > 0)
    {
        switch (*p)
        {
        case '(':
            openParenCount++;
            break;
        case ')':
            openParenCount--;
            break;
        }

        ++p;
    }
    if (*p == ' ')
        ++p;
    int dummy;
    int ppid;
    int count = sscanf(p, "%c %d", (char *) &dummy, &ppid);
    return count == 2 ? ppid : -1;
}

} // namespace

struct cbChildPIDs::Data
{
#ifdef __WXMSW__
    typedef HANDLE WINAPI (*CreateToolhelp32SnapshotApiCall)(DWORD dwFlags, DWORD th32ProcessID);

    Data()
    {
        kernelLib= LoadLibrary(TEXT("kernel32.dll"));
        if (kernelLib)
        {
            CreateToolhelp32SnapshotFunc = (CreateToolhelp32SnapshotApiCall)GetProcAddress(kernelLib, "CreateToolhelp32Snapshot");
            Process32FirstFunc = (Process32FirstApiCall)GetProcAddress(kernelLib, "Process32First");
            Process32NextFunc = (Process32NextApiCall)GetProcAddress(kernelLib, "Process32Next");
        }
    }

    ~Data()
    {
        FreeLibrary(kernelLib);
    }

    HMODULE kernelLib = 0;
    CreateToolhelp32SnapshotApiCall CreateToolhelp32SnapshotFunc = nullptr;
    Process32FirstApiCall Process32FirstFunc = nullptr;
    Process32NextApiCall Process32NextFunc = nullptr;
#endif // __WXMSW__
};

cbChildPIDs::cbChildPIDs() :
    m_data(new Data)
{
}
cbChildPIDs::~cbChildPIDs() = default;

#ifndef __WXMSW__
void cbChildPIDs::GetChildrenPIDs(std::vector<int> &children, int parent)
{
    const char *c_proc_base = "/proc";
    DIR *dir = opendir(c_proc_base);
    if (!dir)
        return;
    struct dirent *entry;
    do
    {
        entry = readdir(dir);
        if (entry)
        {
            int pid = atoi(entry->d_name);
            if (pid != 0)
            {
                char filestr[PATH_MAX + 1];
                snprintf(filestr, PATH_MAX, "%s/%d/stat", c_proc_base, pid);
                FILE *file = fopen(filestr, "r");
                if (file)
                {
                    char line[101];
                    fgets(line, 100, file);
                    fclose(file);
                    int ppid = ParseParentPID(line);
                    if (ppid == parent)
                        children.push_back(pid);
                }
            }
        }
    } while (entry);
    closedir(dir);
}
#else
void cbChildPIDs::GetChildrenPIDs(std::vector<int> &children, int parent)
{
    if (m_data->CreateToolhelp32SnapshotFunc && m_data->Process32FirstFunc && m_data->Process32NextFunc)
    {
        HANDLE snap = CreateToolhelp32SnapshotFunc(TH32CS_SNAPALL,0);
        if (snap != INVALID_HANDLE_VALUE)
        {
            PROCESSENTRY32 lppe;
            lppe.dwSize = sizeof(PROCESSENTRY32);
            BOOL ok = Process32FirstFunc(snap, &lppe);
            while (ok == TRUE)
            {
                if (lppe.th32ParentProcessID == parent) // Have my Child...
                    children.push_back(lppe.th32ProcessID);
                lppe.dwSize = sizeof(PROCESSENTRY32);
                ok = Process32NextFunc(snap, &lppe);
            }
            CloseHandle(snap);
        }
    }
}
#endif // __WXMSW__
