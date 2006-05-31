#ifndef WXSCHOICEBOOK_H
#define WXSCHOICEBOOK_H

#include "../wxscontainer.h"
#include "../wxswidgetevents.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsChoicebookStyles)
WXS_EV_DECLARE(wxsChoicebookEvents)

struct wxsChoicebookExtraParams
{
	wxString Label;
	bool Selected;

	wxsChoicebookExtraParams():
        Label(_("Page name")),
        Selected(false)
    {}
};

class wxsChoicebook : public wxsContainer
{
	public:

		wxsChoicebook(wxsWidgetManager* Man,wxsWindowRes* Res);
		virtual ~wxsChoicebook();
        virtual const wxsWidgetInfo& GetInfo()
        {
            return *GetManager()->GetWidgetInfo(wxsChoicebookId);
        }
        virtual bool CanAddChild(wxsWidget* NewWidget,int InsertBeforeThis=-1);
		virtual int AddChild(wxsWidget* NewWidget,int InsertBeforeThis=-1);
		virtual wxString GetProducingCode(const wxsCodeParams& Params);
        virtual wxString GetFinalizingCode(const wxsCodeParams& Params);
        virtual void PreviewMouseEvent(wxMouseEvent& event);
        virtual void EnsurePreviewVisible(wxsWidget* Child);
        virtual bool ChildReallyVisible(wxsWidget* Child);
        inline wxsChoicebookExtraParams* GetExtraParams(int Index) { return (wxsChoicebookExtraParams*)GetExtra(Index); }
        virtual wxWindow* BuildChildQuickPanel(wxWindow* Parent,int ChildPos);

   protected:

        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        virtual void MyFinalUpdatePreview(wxWindow* Preview);
        virtual bool XmlLoadChild(TiXmlElement* Element);
        virtual bool XmlSaveChild(int ChildIndex,TiXmlElement* AddHere);
        virtual void AddChildProperties(int ChildIndex);
        virtual void* NewExtra() { return new wxsChoicebookExtraParams; }
        virtual void DelExtra(void* Data) { delete (wxsChoicebookExtraParams*)Data; }

    private:
        wxsWidget* CurrentSelection;
        friend class wxsChoicebookPreview;
};

#endif
