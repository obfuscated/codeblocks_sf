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
        ~ClassWizard();

        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);

        virtual void BuildMenu(wxMenuBar* menuBar);

		void OnLaunch(wxCommandEvent& event);

	protected:
		wxMenu* m_FileNewMenu;

		DECLARE_EVENT_TABLE();
};

#endif // CLASSWIZARD_H
