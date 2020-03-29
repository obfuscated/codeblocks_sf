/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_KEILC51_H
#define COMPILER_KEILC51_H

#include <compiler.h>

class CompilerKeilC51 : public Compiler
{
    public:
        CompilerKeilC51();
        virtual ~CompilerKeilC51();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        CompilerKeilC51(const wxString& name, const wxString& ID);
        virtual Compiler* CreateCopy();
        AutoDetectResult AutoDetectInstallationDir(bool keilx);
    private:
};

class CompilerKeilCX51 : public CompilerKeilC51
{
    public:
        CompilerKeilCX51();
        virtual ~CompilerKeilCX51();
    protected:
        virtual Compiler* CreateCopy();
    private:
};

#endif // COMPILER_KEILC51_H
