/*
 * This file is licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_GFORTRAN_H
#define COMPILER_GFORTRAN_H

#include <compiler.h>

class CompilerGNUFortran : public Compiler
{
    public:
        CompilerGNUFortran();
        ~CompilerGNUFortran() override;
        AutoDetectResult AutoDetectInstallationDir() override;
    protected:
        Compiler* CreateCopy() override;
    private:
};

#endif // COMPILER_GFORTRAN_H

