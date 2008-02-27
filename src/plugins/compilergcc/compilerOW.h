/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifdef __WXMSW__
// this compiler is valid only in windows

#ifndef COMPILEROW_H
#define COMPILEROW_H

#include <compiler.h>

class CompilerOW : public Compiler
{
	public:
		CompilerOW();
		virtual ~CompilerOW();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();

        virtual void LoadSettings(const wxString& baseKey);
		virtual void SetMasterPath(const wxString& path);
		virtual CompilerCommandGenerator* GetCommandGenerator();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILEROW_H

#endif // __WXMSW__
