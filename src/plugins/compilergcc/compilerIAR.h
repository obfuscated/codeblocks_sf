/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_IAR_H
#define COMPILER_IAR_H

#include <compiler.h>

class CompilerIAR : public Compiler
{
    public:
        CompilerIAR(wxString arch);
        virtual ~CompilerIAR();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
        wxString m_Arch;
    private:
};

#endif // COMPILER_IAR_H
