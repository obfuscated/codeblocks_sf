/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef COMPILER_GNUARM_H
#define COMPILER_GNUARM_H

#include "compiler.h"

class CompilerGNUARM : public Compiler
{
    public:
        CompilerGNUARM();
        virtual ~CompilerGNUARM();
        virtual void Reset();
        virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
    private:
};

#endif // COMPILER_GNUARM_H
