/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifdef __WXMSW__
// this compiler is valid only in windows

#ifndef COMPILER_CYGWIN_H
#define COMPILER_CYGWIN_H

#include "compilerMINGW.h"

class CompilerCYGWIN : public CompilerMINGW
{
	public:
		CompilerCYGWIN();
		~CompilerCYGWIN();
        virtual void Reset();
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        virtual Compiler* CreateCopy();
	private:

};

#endif // COMPILER_CYGWIN_H

#endif // __WXMSW__
