#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include "manager.h"
#include "projectmanager.h"
#include "messagemanager.h"
#include "cbproject.h"
#include "globals.h"
#include "devcpploader.h"

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
        
    wxFileConfig* dev = new wxFileConfig("", "", filename, "", wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
    dev->SetPath("/Project");
    int unitCount;
    dev->Read("UnitCount", &unitCount, 0);
        
    wxString path, tmp, title, output, out_path, obj_path; 
    wxArrayString array;
    int typ;
        
    // read project options
    dev->Read("Name", &title, "");
    m_pProject->SetTitle(title);

    dev->Read("CppCompiler", &tmp, "");
    if (tmp.IsEmpty())
        dev->Read("Compiler", &tmp, "");
    //tmp = tmp;
    array = GetArrayFromString(tmp, "_@@@@_");
    m_pProject->SetCompilerOptions(array);

    dev->Read("Linker", &tmp, "");
    //tmp = tmp;
    array = GetArrayFromString(tmp, "_@@@@_");
    m_pProject->SetLinkerOptions(array);

    dev->Read("Includes", &tmp, "");
    //tmp = tmp;
    array = GetArrayFromString(tmp, ";");
    m_pProject->SetIncludeDirs(array);

    dev->Read("Libs", &tmp, "");
    //tmp = tmp;
    array = GetArrayFromString(tmp, ";");
    m_pProject->SetLibDirs(array);

    dev->Read("Resources", &tmp, "");
    array = GetArrayFromString(tmp, ","); // make sure that this is comma-separated
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        tmp = array[i];
        m_pProject->AddFile(0, tmp, true, true);
    }

    // read project units
    for (int x = 0; x < unitCount; ++x)
    {
        path.Printf("/Unit%d", x + 1);
        dev->SetPath(path);
        tmp.Clear();
        dev->Read("FileName", &tmp, "");

        bool compile, compileCpp, link;
        dev->Read("Compile", &compile, false);
        dev->Read("CompileCpp", &compileCpp, true);
        dev->Read("Link", &link, true);
        ProjectFile* pf = m_pProject->AddFile(0, tmp, compile || compileCpp, link);
        if (pf)
            pf->compilerVar = compileCpp ? "CPP" : "CC";
    }
    dev->SetPath("/Project");

    ProjectBuildTarget* target = m_pProject->GetBuildTarget(0);
    dev->Read("Type", &typ, 0);
    target->SetTargetType(TargetType(typ));

    if (dev->Read("OverrideOutput", 1) == 1)
        dev->Read("OverrideOutputName", &output, "");
    if (output.IsEmpty())
        output = m_pProject->GetOutputFilename();
    dev->Read("ExeOutput", &out_path, "");
    if (!out_path.IsEmpty())
        output = out_path + "\\" + output;
    target->SetOutputFilename(output);

    dev->Read("ObjectOutput", &obj_path, "");
    if (!obj_path.IsEmpty())
        target->SetObjectOutput(obj_path);

    delete dev;

    m_pProject->SetModified(true);
    return true;
}

bool DevCppLoader::Save(const wxString& filename)
{
    // no support to save DevCpp projects
    return false;
}
