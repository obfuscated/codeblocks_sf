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
        ~CompilerKeilC51() override;
        AutoDetectResult AutoDetectInstallationDir() override;
    protected:
        CompilerKeilC51(const wxString& name, const wxString& ID);
        Compiler* CreateCopy() override;
        AutoDetectResult AutoDetectInstallationDir(bool keilx);
    private:
};

class CompilerKeilCX51 : public CompilerKeilC51
{
    public:
        CompilerKeilCX51();
        ~CompilerKeilCX51() override;
    protected:
        Compiler* CreateCopy() override;
    private:
};

#endif // COMPILER_KEILC51_H
