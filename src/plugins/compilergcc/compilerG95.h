/*
 * This file is licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_G95_H
#define COMPILER_G95_H

#include <wx/intl.h>
#include <compiler.h>

class CompilerG95 : public Compiler
{
    public:
        CompilerG95();
        ~CompilerG95() override;
        AutoDetectResult AutoDetectInstallationDir() override;
    protected:
        Compiler* CreateCopy() override;
    private:
};

#endif // COMPILER_G95_H

