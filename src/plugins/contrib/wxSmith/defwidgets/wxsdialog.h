#ifndef WXSDIALOG_H
#define WXSDIALOG_H

#include "wxswindow.h"

WXS_ST_DECLARE(wxsDialogStyles)
WXS_EV_DECLARE(wxsDialogEvents)

class wxsDialog : public wxsWindow
{
	public:
		wxsDialog(wxsWidgetManager* Man,wxsWindowRes* Res);

		virtual ~wxsDialog();

        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();

        /** Getting title */
        inline const wxString& GetTitle() { return Title; }

        /** Checking if it's centered */
        inline bool GetCentered() { return Centered; }

        /** Function generating code which should produce widget */
        virtual wxString GetProducingCode(const wxsCodeParams& Params);

        /** Function generating code which finishes production process of this
         *  widget
         */
        virtual wxString GetFinalizingCode(const wxsCodeParams& Params);

    protected:

        void MyCreateProperties();
        virtual bool MyXmlLoad();
        virtual bool MyXmlSave();

    private:

        wxString Title;
        bool Centered;

        friend class wxsDialogRes;
};

#endif
