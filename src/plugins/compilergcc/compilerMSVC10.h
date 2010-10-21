/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifdef __WXMSW__
// this compiler is valid only in windows
// for VC10 (VS2010)

#ifndef COMPILERMSVC10_H_
#define COMPILERMSVC10_H_

#include <compiler.h>

class CompilerMSVC10 : public Compiler
{
public:
    CompilerMSVC10();
    virtual ~CompilerMSVC10();
    virtual void Reset();
    virtual void LoadDefaultRegExArray();
    virtual AutoDetectResult AutoDetectInstallationDir();

protected:
    Compiler* CreateCopy();
};

#endif // COMPILERMSVC10_H_

#endif // __WXMSW__
