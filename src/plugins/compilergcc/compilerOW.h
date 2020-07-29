/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILEROW_H
#define COMPILEROW_H

#include <compiler.h>

class CompilerOW : public Compiler
{
	public:
		CompilerOW();
		~CompilerOW() override;
        AutoDetectResult AutoDetectInstallationDir() override;

        void LoadSettings(const wxString& baseKey) override;
		void SetMasterPath(const wxString& path) override;
		CompilerCommandGenerator* GetCommandGenerator(cbProject *project) override;
	protected:
        Compiler * CreateCopy() override;
	private:
};

#endif // COMPILEROW_H
