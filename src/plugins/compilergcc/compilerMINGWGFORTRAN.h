/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_MINGWGFORTRAN_H
#define COMPILER_MINGWGFORTRAN_H

#include <wx/intl.h>
#include <compiler.h>

class CompilerMINGWGFORTRAN : public Compiler
{
    public:
        // added arguments to ctor so we can derive other gcc-flavours directly
        // from MinGW (e.g. the cygwin compiler is derived from this one).
        CompilerMINGWGFORTRAN(const wxString& name = _("GNU GFORTRAN Compiler"), const wxString& ID = _T("gfortran"));
        virtual ~CompilerMINGWGFORTRAN();
        virtual void Reset();
        virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
        virtual CompilerCommandGenerator* GetCommandGenerator();
    protected:
        virtual Compiler* CreateCopy();
        virtual void SetVersionString();
    private:
};
#endif // COMPILER_MINGWGFORTRAN_H

