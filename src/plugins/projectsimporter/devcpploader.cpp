/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"


#ifndef CB_PRECOMP
    #include <wx/confbase.h>
    #include <wx/intl.h>
    #include <wx/filename.h>

    #include "manager.h"
    #include "projectmanager.h"
    #include "logmanager.h"
    #include "cbproject.h"
    #include "globals.h"
#endif

#include "devcpploader.h"

#include <wx/fileconf.h>


DevCppLoader::DevCppLoader(cbProject* project)
    : m_pProject(project)
{
	//ctor
}

DevCppLoader::~DevCppLoader()
{
	//dtor
}

bool DevCppLoader::Open(const wxString& filename)
{
    m_pProject->ClearAllProperties();

    wxFileConfig* dev = new wxFileConfig(_T(""), _T(""), filename, _T(""), wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
    dev->SetPath(_T("/Project"));
    int unitCount;
    dev->Read(_T("UnitCount"), &unitCount, 0);

    wxString path, tmp, title, output, out_path, obj_path;
    wxArrayString array;
    int typ;

    // read project options
    dev->Read(_T("Name"), &title, _T(""));
    m_pProject->SetTitle(title);

    dev->Read(_T("CppCompiler"), &tmp, _T(""));
    if (tmp.IsEmpty())
        dev->Read(_T("Compiler"), &tmp, _T(""));
    array = GetArrayFromString(tmp, _T("_@@_"));
    m_pProject->SetCompilerOptions(array);

    dev->Read(_T("Linker"), &tmp, _T(""));
    // some .dev I got my hands on, had the following in the linker options
    // remove them
    tmp.Replace(_T("-o$@"), _T(""));
    tmp.Replace(_T("-o $@"), _T(""));
    // read the list of linker options
    array = GetArrayFromString(tmp, _T("_@@_"));
    // but separate the libs
    size_t i = 0;
    while (i < array.GetCount())
    {
        if (array[i].StartsWith(_T("-l")))
        {
            wxString tmplib = array[i].Right(array[i].Length() - 2);
            // there might be multiple libs defined in a single line, like:
            // -lmingw32 -lscrnsave -lcomctl32 -lpng -lz -mwindows
            // we got to split by "-l" too...
            if (tmplib.Find(_T(' ')) != wxNOT_FOUND)
            {
                wxArrayString tmparr = GetArrayFromString(array[i], _T(" "));
                while (tmparr.GetCount())
                {
                    if (tmparr[0].StartsWith(_T("-l")))
                        m_pProject->AddLinkLib(tmparr[0].Right(tmparr[0].Length() - 2));
                    else
                        array.Add(tmparr[0]);
                    tmparr.RemoveAt(0, 1);
                }
            }
            else
                m_pProject->AddLinkLib(tmplib);
            array.RemoveAt(i, 1);
        }
        else
            ++i;
    }
    // the remaining are linker options
    m_pProject->SetLinkerOptions(array);

    // read compiler's dirs
    dev->Read(_T("Includes"), &tmp, _T(""));
    array = GetArrayFromString(tmp, _T(";"));
    m_pProject->SetIncludeDirs(array);

    // read linker's dirs
    dev->Read(_T("Libs"), &tmp, _T(""));
    array = GetArrayFromString(tmp, _T(";"));
    m_pProject->SetLibDirs(array);

    // read resource files
    dev->Read(_T("Resources"), &tmp, _T(""));
    array = GetArrayFromString(tmp, _T(",")); // make sure that this is comma-separated
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        if (array[i].IsEmpty())
            continue;
        tmp = array[i];
        m_pProject->AddFile(0, tmp, true, true);
    }

    // read project units
    for (int x = 0; x < unitCount; ++x)
    {
        path.Printf(_T("/Unit%d"), x + 1);
        dev->SetPath(path);
        tmp.Clear();
        dev->Read(_T("FileName"), &tmp, _T(""));
        if (tmp.IsEmpty())
            continue;

        bool compile, compileCpp, link;
        dev->Read(_T("Compile"), &compile, false);
        dev->Read(_T("CompileCpp"), &compileCpp, true);
        dev->Read(_T("Link"), &link, true);

        // .dev files set Link=0 for resources which is plain wrong for C::B.
        // correct this...
        if (!link && FileTypeOf(tmp) == ftResource)
            link = true;

        ProjectFile* pf = m_pProject->AddFile(0, tmp, compile || compileCpp, link);
        if (pf)
            pf->compilerVar = compileCpp ? _T("CPP") : _T("CC");
    }
    dev->SetPath(_T("/Project"));

    // set the target type
    ProjectBuildTarget* target = m_pProject->GetBuildTarget(0);
    dev->Read(_T("Type"), &typ, 0);
    target->SetTargetType(TargetType(typ));

    // decide on the output filename
    if (dev->Read(_T("OverrideOutput"), (long)0) == 1)
        dev->Read(_T("OverrideOutputName"), &output, _T(""));
    if (output.IsEmpty())
        output = target->SuggestOutputFilename();
    dev->Read(_T("ExeOutput"), &out_path, _T(""));
    if (!out_path.IsEmpty())
        output = out_path + _T("\\") + output;
    target->SetOutputFilename(output);

    // set the object output
    dev->Read(_T("ObjectOutput"), &obj_path, _T(""));
    if (!obj_path.IsEmpty())
        target->SetObjectOutput(obj_path);

    // all done
    delete dev;

    m_pProject->SetModified(true);
    return true;
}

bool DevCppLoader::Save(const wxString& filename)
{
    // no support to save DevCpp projects
    return false;
}
