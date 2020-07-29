/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_GNUARM_H
#define COMPILER_GNUARM_H

#include "compiler.h"

class CompilerGNUARM : public Compiler
{
    public:
        CompilerGNUARM();
        ~CompilerGNUARM() override;
        AutoDetectResult AutoDetectInstallationDir() override;
    protected:
        Compiler* CreateCopy() override;
    private:
};

#endif // COMPILER_GNUARM_H
