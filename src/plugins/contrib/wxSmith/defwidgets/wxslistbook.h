#ifndef WXSLISTBOOK_H
#define WXSLISTBOOK_H

#include "../wxscontainer.h"
#include "../wxswidgetevents.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsListbookStyles)
WXS_EV_DECLARE(wxsListbookEvents)

struct wxsListbookExtraParams
{
	wxString Label;
	bool Selected;

	wxsListbookExtraParams():
        Label(_("Page name")),
        Selected(false)
    {}
};

class wxsListbook : public wxsContainer
{
	public:

		wxsListbook(wxsWidgetManager* Man,wxsWindowRes* Res);
		virtual ~wxsListbook();
        virtual const wxsWidgetInfo& GetInfo()
        {
            return *GetManager()->GetWidgetInfo(wxsListbookId);
        }
        virtual bool CanAddChild(wxsWidget* NewWidget,int InsertBeforeThis=-1);
		virtual int AddChild(wxsWidget* NewWidget,int InsertBeforeThis=-1);
		virtual wxString GetProducingCode(const wxsCodeParams& Params);
        virtual wxString GetFinalizingCode(const wxsCodeParams& Params);
        virtual void PreviewMouseEvent(wxMouseEvent& event);
        virtual void EnsurePreviewVisible(wxsWidget* Child);
        virtual bool ChildReallyVisible(wxsWidget* Child);
        inline wxsListbookExtraParams* GetExtraParams(int Index) { return (wxsListbookExtraParams*)GetExtra(Index); }
        virtual wxWindow* BuildChildQuickPanel(wxWindow* Parent,int ChildPos);

   protected:

        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        virtual void MyFinalUpdatePreview(wxWindow* Preview);
        virtual bool XmlLoadChild(TiXmlElement* Element);
        virtual bool XmlSaveChild(int ChildIndex,TiXmlElement* AddHere);
        virtual void AddChildProperties(int ChildIndex);
        virtual void* NewExtra() { return new wxsListbookExtraParams; }
        virtual void DelExtra(void* Data) { delete (wxsListbookExtraParams*)Data; }

    private:
        wxsWidget* CurrentSelection;
        friend class wxsListbookPreview;
};

#endif
