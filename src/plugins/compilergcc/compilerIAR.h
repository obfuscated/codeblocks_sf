/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_IAR_H
#define COMPILER_IAR_H

#include <compiler.h>

class CompilerIAR8051 : public Compiler
{
    public:
        CompilerIAR8051();
        virtual ~CompilerIAR8051();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
    private:
};

#endif // COMPILER_IAR_H
