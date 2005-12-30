#ifndef WXSPLITTERWINDOW_H
#define WXSPLITTERWINDOW_H

#include "../wxscontainer.h"
#include "../wxswidgetevents.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsSplitterWindowStyles)
WXS_EV_DECLARE(wxsSplitterWindowEvents)

class wxsSplitterWindow : public wxsContainer
{
	public:

		wxsSplitterWindow(wxsWidgetManager* Man,wxsWindowRes* Res);
		virtual ~wxsSplitterWindow();
        virtual const wxsWidgetInfo& GetInfo()
        {
            return *GetManager()->GetWidgetInfo(wxsSplitterWindowId);
        }
		virtual int AddChild(wxsWidget* NewWidget,int InsertBeforeThis);
		virtual wxString GetProducingCode(wxsCodeParams& Params);
        virtual wxString GetFinalizingCode(wxsCodeParams& Params);
        virtual wxString GetDeclarationCode(wxsCodeParams& Params);

   protected:

        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        virtual void MyFinalUpdatePreview(wxWindow* Preview);
        virtual bool MyXmlLoad();
        virtual bool MyXmlSave();
        virtual void CreateObjectProperties();

    private:

        int SashPos;
        int MinSize;
        int Orientation;
};



#endif
