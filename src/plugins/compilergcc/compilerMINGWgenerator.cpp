#include <sdk.h>
#include "compilerMINGWgenerator.h"
#include <wx/intl.h>
#include "cbexception.h"
#include "cbproject.h"
#include "compilerfactory.h"
#include "compiler.h"
#include "manager.h"
#include "configmanager.h"
#include "messagemanager.h"
#include "macrosmanager.h"
#include "scriptingmanager.h"

CompilerMINGWGenerator::CompilerMINGWGenerator()
{
    //ctor
    m_VerStr = wxEmptyString;
}

CompilerMINGWGenerator::~CompilerMINGWGenerator()
{
    //dtor
}

wxString CompilerMINGWGenerator::SetupIncludeDirs(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString result = CompilerCommandGenerator::SetupIncludeDirs(compiler, target);
    m_VerStr = compiler->GetVersionString();
    wxString pch_prepend;
    bool IsGcc4 = m_VerStr.Left(1).IsSameAs(_T("4"));

    // for PCH to work, the very first include dir *must* be the object output dir
    // *only* if PCH is generated in the object output dir
    if (target &&
        target->GetParentProject()->GetModeForPCH() == pchObjectDir)
    {
        wxArrayString includedDirs; // avoid adding duplicate dirs...
        wxString sep = wxFILE_SEP_PATH;
        // find all PCH in project
        int count = target->GetParentProject()->GetFilesCount();
        for (int i = 0; i < count; ++i)
        {
            ProjectFile* f = target->GetParentProject()->GetFile(i);
            if (FileTypeOf(f->relativeFilename) == ftHeader &&
                f->compile)
            {
                // it is a PCH; add it's object dir to includes
                wxString dir = wxFileName(target->GetObjectOutput() + sep + f->GetObjName()).GetPath();
                if (includedDirs.Index(dir) == wxNOT_FOUND)
                {
                    includedDirs.Add(dir);
                    QuoteStringIfNeeded(dir);
                    if (!IsGcc4)
                        pch_prepend << compiler->GetSwitches().includeDirs << dir << _T(' ');
                    else
                        pch_prepend << _T("-iquote") << dir << _T(' ');
                }
            }
        }
        // for gcc-4.0+, use the following:
        // pch_prepend << _T("-iquote") << dir << _T(' ');
        // for earlier versions, -I- must be used
        if (!IsGcc4)
            pch_prepend << _T("-I- ");
        count = (int)includedDirs.GetCount();
        for (int i = 0; i < count; ++i)
        {
            QuoteStringIfNeeded(includedDirs[i]);
            pch_prepend << compiler->GetSwitches().includeDirs << includedDirs[i] << _T(' ');
        }
        pch_prepend << _T("-I. ");
        result.Prepend(pch_prepend);
    }

    // add in array
    return result;
}
