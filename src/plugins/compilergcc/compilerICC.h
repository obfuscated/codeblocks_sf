/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

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
