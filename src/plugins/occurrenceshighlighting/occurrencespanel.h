/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef OCCURRENCESPANEL_H
#define OCCURRENCESPANEL_H

//(*Headers(OccurrencesPanel)
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
//*)

class OccurrencesPanel: public wxPanel
{
    public:

        OccurrencesPanel(wxWindow* parent,wxWindowID id=wxID_ANY);
        virtual ~OccurrencesPanel();

        //(*Declarations(OccurrencesPanel)
        wxListCtrl* ListCtrl1;
        //*)
        wxListCtrl* GetListCtrl(){return ListCtrl1;}


    protected:

        //(*Identifiers(OccurrencesPanel)
        static const long ID_LISTCTRL1;
        //*)

    private:

        //(*Handlers(OccurrencesPanel)
        //*)

        DECLARE_EVENT_TABLE()
};

#endif
