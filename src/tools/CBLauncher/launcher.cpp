/**
 * @file launcher.cpp
 * @version 1.0.1
 * @brief  This file is part of the Portable Code::Blocks Launcher.
 */
/*  Copyright (C) 2009 Biplab Kumar Modak
 *  Homepage: http://biplab.in
 *  Copyright (C) 2010-2011 Codeur (codeur at taillecode dot org)
 *
 *  Portable Code::Blocks Launcher is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  Portable Code::Blocks Launcher is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***************************************************************************************/
#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <wchar.h>

// Define application directories and application call string at file scope
static const wchar_t *dataDir     = L"AppData";
static const wchar_t *CBdataDir   = L"AppData\\codeblocks";
static const wchar_t *fontsDir    = L"share\\CodeBlocks\\fonts";
static const wchar_t *toolDir     = L"tool";
static const wchar_t *cmd         = L"codeblocks.exe";
static wchar_t appDir[MAX_PATH];

/** @brief tests if Code::Blocks is already running
 * @return true if CB is already running
 */
bool isCodeblocksRunning()
{
   PROCESSENTRY32 pe32;
   pe32.dwSize = sizeof( PROCESSENTRY32 );
   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

   if(Process32First(hSnapshot, &pe32)) // enumerate through the processes
   {
      do
      {
         if(! _wcsicmp(pe32.szExeFile, cmd)) // Codeblocks is running
         {
            CloseHandle(hSnapshot);
            return true;
         }
      } while(Process32Next(hSnapshot, &pe32));
   }

   CloseHandle(hSnapshot);
   return false;
}

/** @brief Modifies the system %APPDATA% environment variable to point to subdirectory
 * AppData of Code::Blocks.
 *
 * @return success (true) or failure (false)
 */
bool setAppData()
{
    wchar_t currDir[MAX_PATH];
    WIN32_FIND_DATAW findData;
    HANDLE hFind;
    bool success = true;

    wcscpy(currDir, appDir);
    wcscat(currDir, dataDir);

    hFind = FindFirstFileW(currDir, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        fwprintf(stdout, L"Creating AppData directory %ls\n", currDir);
        if (CreateDirectory(currDir, NULL) == 0)
            success = false;
    }

    FindClose(hFind);
    if (!success)
        return false;

    wcscpy(currDir, appDir);
    wcscat(currDir, CBdataDir);

    hFind = FindFirstFileW(currDir, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        fwprintf(stdout, L"Creating directory %ls\n", currDir);
        if (CreateDirectory(currDir, NULL) == 0)
            success = false;
    }
    FindClose(hFind);
    if (!success)
        return false;

    wcscpy(currDir, appDir);
    wcscat(currDir, dataDir);
    fwprintf(stdout, L"Setting %%APPDATA%% to %ls\n", currDir);
    return SetEnvironmentVariable(L"APPDATA", currDir);
}

/** @brief Add the tool subdirectories of Code::Blocks to the system path.
 * The tool subdirectory and its subdirectories or the bin subdir of its subdirectories
 * @return success (true) or failure (false)
 */
bool setToolPath()
{
    const size_t MAX_ENV = 4096; // Maximum size of path
    wchar_t toolPath[MAX_ENV], toFind[MAX_PATH];
    WIN32_FIND_DATAW findData;
    HANDLE hFind;
    bool success = true;

    wcscpy(toolPath, appDir);
    wcscat(toolPath, toolDir);
    hFind = FindFirstFileW(toolPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        fwprintf(stdout, L"No tool subdirectory.\n");
        success = false;
    }
    FindClose(hFind);
    if (!success)
        return false; // Give up if tool subdir does not exist

    // tool subdirectory exists. Add it and all its subdirectories to system path
    wcscpy(toFind, toolPath);
    wcscat(toFind, L"\\*.*");

    wchar_t newPath[MAX_ENV];
    wcscpy(newPath, toolPath); // Add tools directory

    // Add tool subdirectories to path
    hFind = FindFirstFile(toFind, &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do // Add all tool directories or their bin subdirectory if it exists to path
        {
            if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    && (wcscmp(findData.cFileName, L"."))
                    && (wcscmp(findData.cFileName, L"..")))
            {
                wcscat(newPath, L";"); // Add a new directory to path

                wcscpy(toFind, toolPath);
                wcscat(toFind, L"\\");
                wcscat(toFind, findData.cFileName);
                wcscat(newPath, toFind);
                wcscat(toFind, L"\\bin");
                // If bin subdirectory exists, add bin to the path
                HANDLE fHandle = CreateFile(toFind, GENERIC_READ,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                                            NULL,
                                            OPEN_EXISTING,
                                            FILE_FLAG_BACKUP_SEMANTICS,
                                            NULL );
                if(fHandle != INVALID_HANDLE_VALUE)
                {
                    CloseHandle(fHandle);
                    wcscat(newPath, L"\\bin");
                }
            }
        }
        while (FindNextFile(hFind, &findData));
    }
    FindClose(hFind);

    // Concatenate with existing system path
    wchar_t oldPath[MAX_ENV];
    if (GetEnvironmentVariable( L"PATH", oldPath, MAX_ENV))
    {
        wcscat(newPath, L";");
        wcscat(newPath, oldPath);
    }
    fwprintf(stdout, L"\nSetting path: %ls\n\n", newPath);

    return SetEnvironmentVariable(L"PATH", newPath);
}

/** @brief Add Code::Blocks fonts to system fonts if they exist in the given directory,
 * or recursively in its subdirectories.
 *
 * @param fontsPath directory (potentially) containing .ttf or .otf fonts
 * @return The number of fonts actually added
 */
int addFontsFrom(wchar_t fontsPath[])
{
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    wchar_t fontFile[MAX_PATH], toFind[MAX_PATH];
    wchar_t newPath[MAX_PATH];
    int numFonts = 0;

    // First call addFontsFrom(<subdirectory>) recursively for any subdirectory
    wcscpy(toFind, fontsPath);
    wcscat(toFind, L"\\*.*");

    hFind = FindFirstFile(toFind, &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do // Add all fonts in subdirectories if they exist to system fonts
        {
            if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    && (wcscmp(findData.cFileName, L"."))
                    && (wcscmp(findData.cFileName, L"..")))
            {
                // call addFontsFrom() recursively
                wcscpy(newPath, fontsPath);
                wcscat(newPath, L"\\");
                wcscat(newPath, findData.cFileName);
                numFonts += addFontsFrom(newPath);
            }
        }
        while (FindNextFile(hFind, &findData));
    }
    FindClose(hFind);

    // Then add CB fonts (ttf or otf) in directory fontsPath to system fonts
    wcscpy(fontFile, fontsPath);
    wcscat(fontFile, L"\\*.?tf");
    hFind = FindFirstFile(fontFile, &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            wcscpy(fontFile, fontsPath);
            wcscat(fontFile, L"\\");
            wcscat(fontFile, findData.cFileName);

            if (AddFontResource(fontFile))
            {
                fwprintf(stdout, L"\nInstalled font %ls", findData.cFileName);
                numFonts++;
            }
            else
            {
                fwprintf(stdout, L"\nInstallation of %ls failed\n", fontFile);
                break; // Give up at the 1st font that fails to load
            }
        }
        while (FindNextFile(hFind, &findData));
    }
    FindClose(hFind);

    return numFonts;
}

/** @brief Add Code::Blocks fonts to system fonts if they exist in the given directory
 * - Assume fonts (if any) are available in /share/CodeBlocks/fonts directory
 *
 * @return success (true) or failure (false)
 */
bool addFonts()
{
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    wchar_t fontsPath[MAX_PATH];

    wcscpy(fontsPath, appDir);
    wcscat(fontsPath, fontsDir);

    // Give up if no fonts directory exists
    hFind = FindFirstFile(fontsPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return false;
    FindClose(hFind);

    // Add CB fonts (ttf or otf) in fonts directory or subdirectories to system fonts.
    // Broadcast the font change.
    if(addFontsFrom(fontsPath) > 0)
    {
        SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
        return true;
    }
    return false;
}

/** @brief Remove Code::Blocks fonts from system fonts if they exist in given directory,
 * or recursively in its subdirectories, assuming they were previously added.
 *
 * @param fontsPath directory (potentially) containing .ttf or .otf fonts
 * @return The number of fonts actually removed
 */
int removeFontsFrom(wchar_t fontsPath[])
{
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    wchar_t fontFile[MAX_PATH], toFind[MAX_PATH];
    wchar_t newPath[MAX_PATH];
    int numFonts = 0;

    // First call removeFontsFrom(<subdirectory>) recursively for any subdirectory
    wcscpy(toFind, fontsPath);
    wcscat(toFind, L"\\*.*");

    hFind = FindFirstFile(toFind, &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do // remove all fonts in subdirectories if they exist from system fonts
        {
            if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    && (wcscmp(findData.cFileName, L"."))
                    && (wcscmp(findData.cFileName, L"..")))
            {
                // call removeFontsFrom() recursively
                wcscpy(newPath, fontsPath);
                wcscat(newPath, L"\\");
                wcscat(newPath, findData.cFileName);
                numFonts += removeFontsFrom(newPath);
            }
        }
        while (FindNextFile(hFind, &findData));
    }
    FindClose(hFind);

    // Then remove CB fonts (ttf or otf) from system fonts if they exist in fontsPath directory
    wcscpy(fontFile, fontsPath);
    wcscat(fontFile, L"\\*.?tf");
    hFind = FindFirstFile(fontFile, &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            wcscpy(fontFile, fontsPath);
            wcscat(fontFile, L"\\");
            wcscat(fontFile, findData.cFileName);

            if (RemoveFontResource(fontFile))
                numFonts++;
        }
        while (FindNextFile(hFind, &findData));
    }
    FindClose(hFind);

    return numFonts;
}

/** @brief Remove Code::Blocks fonts from system fonts if CB (another instance)
 *         is not running.
 * - Assume fonts were added from the /share/CodeBlocks/fonts subdirectory of CB and
 * its subdirectories.
 *
 * @return The number of fonts actually removed
 */
int removeFonts()
{
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    wchar_t fontsPath[MAX_PATH];
    int numRemoved = 0;
    if(isCodeblocksRunning()) // give up if another instance of CB is still running
        return  numRemoved;

    wcscpy(fontsPath, appDir);
    wcscat(fontsPath, fontsDir);

    // Give up if no fonts directory exists
    hFind = FindFirstFile(fontsPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return 0; // No font removed in this case
    }

    // Remove CB fonts (ttf or otf) previously loaded from the fonts directory or
    // subdirectories. Broadcast the font change.
    if((numRemoved = removeFontsFrom(fontsPath)) > 0)
        SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    return numRemoved;
}

/** CbLauncher main function <br>
 *  Launches CB portably (local %AppData% directory) then waits until it exits & cleans up.
 */
int main()
{
    wchar_t cmdline[MAX_PATH];
    wchar_t arguments[MAX_PATH];
    wchar_t *args; // pointer to the second argument on the command line
    bool notRunning = !isCodeblocksRunning(); // true if codeblocks is not running on entry

    // Console output will show only in debug build target
    wprintf(L"*******************************************************************\n");
    wprintf(L"*              Portable Code::Blocks Launcher v1.0.1              *\n");
    wprintf(L"*             Copyright (C) 2009 - Biplab Kumar Modak             *\n");
    wprintf(L"*                    Homepage: http://biplab.in                   *\n");
    wprintf(L"* Copyright (C) 2010-2011 - Codeur (codeur at taillecode dot org) *\n");
    wprintf(L"*                        License: GPL v3                          *\n");
    wprintf(L"*******************************************************************\n");

    if (! GetModuleFileNameW( NULL, appDir, MAX_PATH ))
    {
        fwprintf(stdout,
                 L"Couldn't retrieve CBLauncher directory. Giving up!.. [Enter] to exit..");
        getchar();
        return 1;
    }

    wchar_t *lastSlashPtr = wcsrchr( appDir, '\\' );
    *(lastSlashPtr + 1) = '\0'; // Keep the '\'

    fwprintf(stdout, L"Launcher started in directory %ls\n", appDir);

    // Set APPDATA environment variable
    if (!setAppData())
    {
        fwprintf(stdout,
                 L"\nUnable to set %%APPDATA%% environment variable\n[Enter] to exit.. ");
        getchar();
        return 1;
    }

    // Add tools subdirectories to %path%
    if (!setToolPath())
        fwprintf(stdout, L"Not modifying the PATH environment variable\n");

    // Add programmer fonts to system if CB is not already running
    if(notRunning)
        addFonts();

    // Launch Code::Blocks. Pass on the commandline arguments of this process
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    // Build the commandline
    wcscpy(cmdline, appDir);
    wcscat(cmdline, cmd);

    // Add the commandline arguments if they exist
    wcscpy(arguments, GetCommandLineW());
    args = wcschr(arguments, L' ');
    if(args)
    {
        wcscat(cmdline, L" ");
        wcscat(cmdline, args);
    }
    if (! CreateProcessW(NULL,
                         cmdline,
                         NULL, NULL,
                         FALSE, DETACHED_PROCESS, NULL, NULL,
                         &si, &pi ))
    {
        fwprintf(stdout, L"\nUnable to launch %ls\n[Enter] to exit..\n", cmdline);
        fwprintf(stdout, L"\n\tRemoved %d CB programmer fonts", removeFonts());
        getchar();
        return 1;
    }

    // Wait until CB terminates
    wprintf(L"\nLaunched %ls. Waiting for process to exit\n", cmdline);
    if(WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED)
    {
        // (a failed launch may in fact never get here). Remove fonts if installed.
        fwprintf(stdout, L"\n\tRemoved %d CB programmer fonts", removeFonts());
        fwprintf(stdout, L"Codeblocks launch failed...\n\t[Enter] to exit..");
        getchar();
        return 1;
    }

    // Remove portable CB fonts from system if installed and no instance of CB is running.
    fwprintf(stdout, L"\n\tRemoved %d CB programmer fonts\n\tPress [Enter] to exit. ",
             removeFonts());
    getchar();

    return 0;
}
