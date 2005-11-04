#ifndef WXSPALETTEHEADER_H
#define WXSPALETTEHEADER_H

//(*Headers(wxsPaletteHeader)
#include <wx/button.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
//*)

class WXSCLASS wxsWidget;

class WXSCLASS wxsPaletteHeader: public wxPanel
{
	public:

		wxsPaletteHeader(wxWindow* parent,wxWindowID id = -1);
		virtual ~wxsPaletteHeader();

		//(*Identifiers(wxsPaletteHeader)
        enum Identifiers
        {
            ID_AFTER = 0x1000,
            ID_BEFORE,
            ID_BUTTON1,
            ID_BUTTON2,
            ID_INTO
        };
        //*)

		//(*Declarations(wxsPaletteHeader)
        wxFlexGridSizer* MainSizer;
        wxStaticBoxSizer* StaticBoxSizer1;
        wxRadioButton* AddInto;
        wxRadioButton* AddBefore;
        wxRadioButton* AddAfter;
        wxStaticBoxSizer* StaticBoxSizer2;
        wxButton* ButtonDel;
        wxButton* ButtonPreview;
        //*)

        /** Getting current insertion type */
        int GetInsertionType() { return InsType; }

        /** Setting current insertion type */
        void SetInsertionType(int Type);

        /** Setting mask for insertion type */
        void SetInsertionTypeMask(int Mask);

        /** Notifying about selection change */
        void SeletionChanged(wxsWidget* Widget);

	protected:

		//(*Handlers(wxsPaletteHeader)
		void OnAddIntoChange(wxCommandEvent& event);
        void OnAddBeforeChange(wxCommandEvent& event);
        void OnAddAfterChange(wxCommandEvent& event);
        void OnButtonDelClick(wxCommandEvent& event);
        void OnButtonPreviewClick(wxCommandEvent& event);
        //*)

	private:

        /** Current insertion type */
        int InsType;

        /** Current insertion type mask */
        int InsTypeMask;

        /** Additional quick param panel */
        wxWindow* ParentHeader;

		DECLARE_EVENT_TABLE()
};

#endif
