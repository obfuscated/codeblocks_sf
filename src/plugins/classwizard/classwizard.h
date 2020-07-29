/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CLASSWIZARD_H
#define CLASSWIZARD_H

#include "cbplugin.h"

class ClassWizard : public cbPlugin
{
    public:
        ClassWizard();
        ~ClassWizard() override;

        void OnAttach() override;
        void OnRelease(bool appShutDown) override;

        void BuildMenu(wxMenuBar* menuBar) override;

		void OnLaunch(wxCommandEvent& event);

	protected:
		wxMenu* m_FileNewMenu;

		DECLARE_EVENT_TABLE();
};

#endif // CLASSWIZARD_H
