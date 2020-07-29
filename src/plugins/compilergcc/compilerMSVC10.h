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
    ~CompilerMSVC10() override;
    AutoDetectResult AutoDetectInstallationDir() override;

protected:
    Compiler* CreateCopy() override;
};

#endif // COMPILERMSVC10_H_
