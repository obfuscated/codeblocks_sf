/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifdef __WXMSW__
// this compiler is valid only in windows

#ifndef COMPILERBCC_H
#define COMPILERBCC_H

#include <compiler.h>

class CompilerBCC : public Compiler
{
	public:
		CompilerBCC();
		virtual ~CompilerBCC();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILERBCC_H

#endif // __WXMSW__
