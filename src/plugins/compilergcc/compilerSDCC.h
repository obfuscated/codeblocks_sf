/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILER_SDCC_H
#define COMPILER_SDCC_H

#include <compiler.h>

class CompilerSDCC : public Compiler
{
	public:
		CompilerSDCC();
		virtual ~CompilerSDCC();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
	private:
};

#endif // COMPILER_SDCC_H
