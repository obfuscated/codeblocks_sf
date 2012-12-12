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
        virtual ~CompilerGNUARM();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
    private:
};

#endif // COMPILER_GNUARM_H
