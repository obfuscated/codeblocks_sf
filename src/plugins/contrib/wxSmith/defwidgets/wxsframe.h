#ifndef WXSFRAME_H
#define WXSFRAME_H

#include "wxswindow.h"

WXS_ST_DECLARE(wxsFrameStyles)
WXS_EV_DECLARE(wxsFrameEvents)

class wxsFrame : public wxsWindow
{
	public:
		wxsFrame(wxsWidgetManager* Man,wxsWindowRes* Res);

		virtual ~wxsFrame();

        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();

        /** Getting title */
        inline const wxString& GetTitle() { return Title; }

        /** Setting title */
        inline void SetTitle(const wxString& title) { Title = title; }

        /** Checking if it's centered */
        inline bool GetCentered() { return Centered; }

        /** Function generating code which should produce widget */
        virtual wxString GetProducingCode(const wxsCodeParams& Params);

        /** Function generating code which finishes production process of this
         *  widget
         */
        virtual wxString GetFinalizingCode(const wxsCodeParams& Params);

    protected:

        virtual void MyCreateProperties();
        virtual bool MyXmlLoad();
        virtual bool MyXmlSave();

    private:

        wxString Title;
        bool Centered;

        friend class wxsFrameRes;
};

#endif
