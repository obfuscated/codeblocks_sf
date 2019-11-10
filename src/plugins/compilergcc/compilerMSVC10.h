/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILERMSVC10_H_
#define COMPILERMSVC10_H_

#include <compiler.h>

class CompilerMSVC10 : public Compiler
{
public:
    CompilerMSVC10();
    virtual ~CompilerMSVC10();
    virtual AutoDetectResult AutoDetectInstallationDir();

protected:
    Compiler* CreateCopy();
};

#endif // COMPILERMSVC10_H_
