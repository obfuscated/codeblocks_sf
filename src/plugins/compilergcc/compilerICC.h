/********************************************************************
    filename:        compilerICC.h
    created by:    Yorgos Pagles (yop [at] protiamail.gr)
    description:    Support of Intel's ICC compiler for CodeBlocks IDE
********************************************************************/
#ifndef COMPILER_ICC_H
#define COMPILER_ICC_H

#include <wx/dir.h>
#include <wx/arrstr.h>

#include <compiler.h>

class wxIccDirTraverser : public wxDirTraverser
{
    public:
        wxIccDirTraverser(wxArrayString& folders) : m_Dirs(folders)
        {
        }

        virtual wxDirTraverseResult OnFile(const wxString& WXUNUSED(filename))
        {
            return wxDIR_CONTINUE;
        }

        virtual wxDirTraverseResult OnDir(const wxString& dirname)
        {
            if (m_Dirs.Index(dirname) == wxNOT_FOUND
                && dirname.AfterLast(_T('/')).BeforeFirst(_T('.')).IsNumber())
                m_Dirs.Add(dirname);
            return wxDIR_CONTINUE;
        }

    private:
        wxArrayString& m_Dirs;
};


class CompilerICC : public Compiler
{
    public:
        CompilerICC();
        virtual ~CompilerICC();
        virtual void Reset();
        virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
    private:
};

#endif // COMPILER_ICC_H
