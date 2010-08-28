/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_LDC_H
#define COMPILER_LDC_H

#include <compiler.h>

class CompilerLDC : public Compiler
{
    public:
        CompilerLDC();
        virtual ~CompilerLDC();
        virtual void Reset();
        virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
    private:
};

#endif // COMPILER_LDC_H
