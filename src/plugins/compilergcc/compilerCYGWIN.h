/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_CYGWIN_H
#define COMPILER_CYGWIN_H

#include "compilerMINGW.h"

class CompilerCYGWIN : public CompilerMINGW
{
    public:
        CompilerCYGWIN();
        ~CompilerCYGWIN() override;
        AutoDetectResult AutoDetectInstallationDir() override;
    protected:
        Compiler* CreateCopy() override;
    private:

};

#endif // COMPILER_CYGWIN_H
