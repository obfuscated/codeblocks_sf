/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_ICC_H
#define COMPILER_ICC_H

#include <wx/dir.h>
#include <wx/arrstr.h>

#include <globals.h>
#include <compiler.h>

class wxIccDirTraverser : public wxDirTraverser
{
    public:
        wxIccDirTraverser(wxArrayString& folders) : m_Dirs(folders)
        {
            m_SepChar = (platform::windows == 1) ? _T('\\') : _T('/');
        }

        virtual wxDirTraverseResult OnFile(const wxString& WXUNUSED(filename))
        {
            return wxDIR_CONTINUE;
        }

        virtual wxDirTraverseResult OnDir(const wxString& dirname)
        {
            if (m_Dirs.Index(dirname) == wxNOT_FOUND &&
                dirname.AfterLast(m_SepChar).Contains(_T(".")))
            {
                m_Dirs.Add(dirname);
            }
            return wxDIR_CONTINUE;
        }

    private:
        wxArrayString& m_Dirs;
        wxChar m_SepChar;
};


class CompilerICC : public Compiler
{
    public:
        CompilerICC();
        virtual ~CompilerICC();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
    private:
};

#endif // COMPILER_ICC_H
