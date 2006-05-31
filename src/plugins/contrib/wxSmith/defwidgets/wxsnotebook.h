#ifndef WXSNOTEBOOK_H
#define WXSNOTEBOOK_H

#include "../wxscontainer.h"
#include "../wxswidgetevents.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsNotebookStyles)
WXS_EV_DECLARE(wxsNotebookEvents)

struct wxsNotebookExtraParams
{
	wxString Label;
	bool Selected;

	wxsNotebookExtraParams():
        Label(_("Page name")),
        Selected(false)
    {}
};

class wxsNotebook : public wxsContainer
{
	public:

		wxsNotebook(wxsWidgetManager* Man,wxsWindowRes* Res);
		virtual ~wxsNotebook();
        virtual const wxsWidgetInfo& GetInfo()
        {
            return *GetManager()->GetWidgetInfo(wxsNotebookId);
        }
        virtual bool CanAddChild(wxsWidget* NewWidget,int InsertBeforeThis=-1);
		virtual int AddChild(wxsWidget* NewWidget,int InsertBeforeThis=-1);
		virtual wxString GetProducingCode(const wxsCodeParams& Params);
        virtual wxString GetFinalizingCode(const wxsCodeParams& Params);
        virtual void PreviewMouseEvent(wxMouseEvent& event);
        virtual void EnsurePreviewVisible(wxsWidget* Child);
        virtual bool ChildReallyVisible(wxsWidget* Child);
        inline wxsNotebookExtraParams* GetExtraParams(int Index) { return (wxsNotebookExtraParams*)GetExtra(Index); }
        virtual wxWindow* BuildChildQuickPanel(wxWindow* Parent,int ChildPos);

   protected:

        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        virtual void MyFinalUpdatePreview(wxWindow* Preview);
        virtual bool XmlLoadChild(TiXmlElement* Element);
        virtual bool XmlSaveChild(int ChildIndex,TiXmlElement* AddHere);
        virtual void AddChildProperties(int ChildIndex);
        virtual void* NewExtra() { return new wxsNotebookExtraParams; }
        virtual void DelExtra(void* Data) { delete (wxsNotebookExtraParams*)Data; }

    private:
        wxsWidget* CurrentSelection;
        friend class wxsNotebookPreview;
};

#endif
