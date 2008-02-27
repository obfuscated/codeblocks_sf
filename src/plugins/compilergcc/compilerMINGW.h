/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef COMPILER_MINGW_H
#define COMPILER_MINGW_H

#include <wx/intl.h>
#include <compiler.h>

class CompilerMINGW : public Compiler
{
    public:
        // added arguments to ctor so we can derive other gcc-flavours directly
        // from MinGW (e.g. the cygwin compiler is derived from this one).
        CompilerMINGW(const wxString& name = _("GNU GCC Compiler"), const wxString& ID = _T("gcc"));
        virtual ~CompilerMINGW();
        virtual void Reset();
        virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
        virtual CompilerCommandGenerator* GetCommandGenerator();
    protected:
        virtual Compiler* CreateCopy();
        virtual void SetVersionString();
    private:
};

#endif // COMPILER_MINGW_H
