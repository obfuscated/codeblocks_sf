/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_GNUAVR_H
#define COMPILER_GNUAVR_H

#include "compiler.h"

class CompilerGNUAVR : public Compiler
{
    public:
        CompilerGNUAVR();
        virtual ~CompilerGNUAVR();
        virtual void Reset();
        virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
};

#endif // COMPILER_GNUAVR_H
