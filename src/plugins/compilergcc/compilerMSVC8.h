/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifdef __WXMSW__
// this compiler is valid only in windows
// for VC8 and VC9 (DevStudio 2005 and 2008 that is)

#ifndef COMPILERMSVC8_H
#define COMPILERMSVC8_H

#include <compiler.h>

class CompilerMSVC8 : public Compiler
{
	public:
		CompilerMSVC8();
		virtual ~CompilerMSVC8();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILERMSVC8_H

#endif // __WXMSW__
