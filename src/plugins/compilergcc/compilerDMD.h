/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#if defined(_WIN32) || defined(__linux__)
// this compiler is valid only in windows and linux

#ifndef COMPILERDMD_H
#define COMPILERDMD_H

#include <compiler.h>

class CompilerDMD : public Compiler
{
    public:
        CompilerDMD();
        virtual ~CompilerDMD();
        virtual void Reset();
        virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        Compiler * CreateCopy();
    private:
};

#endif // COMPILERDMD_H

#endif // _WIN32 || linux
