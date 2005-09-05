#ifndef WXSWINDOWRES_H
#define WXSWINDOWRES_H

#include "../wxsresource.h"
#include "../defwidgets/wxsdialog.h"
#include "../defwidgets/wxspanel.h"
#include "../defwidgets/wxsframe.h"
#include <wx/string.h>
#include <wx/xrc/xmlres.h>

/** Base class for all window resources (Dialog, Frame and Panel).
 *
 *  This resource can be edited in three modes:
 *   - wxsResFile - this mode is used to edit separate xrc file
 *     not st as resource in any project. path for this xrc file is set inside
 *     WxsFile variable (not XrcFile), additional things like variable name are
 *     not used here.
 *   - wxsResFile | wxsResSource - in this mode two things are changed - additional
 *     Xrc file (it's name is in XrcFile variable) is created and there's dynamically
 *     changing source code loading this resource, fetching it's members and mamaning
 *     events.
 *   - wxsResSource - in this mode, resource is stored inside source code, no XrcFile is
 *     needed
 */
class wxsWindowRes : public wxsResource
{
	public:
	
        /** Ctor */
		wxsWindowRes(
            wxsProject* Project,
            int EditMode,
            const wxString& Class,
            const wxString& Wxs,
            const wxString& Src,
            const wxString& Head,
            const wxString& Xrc );
		
		/** Dctor */
		virtual ~wxsWindowRes();
		
		/** Getting name of class implementing this dialog */
		inline const wxString& GetClassName() { return ClassName; }
		
		/** Getting name of internal wxsmith's file containing structire
		 *  of resource. The file name uses global path */
		inline const wxString& GetWxsFile() { return WxsFile; }
		
		/** Getting name of source file implementing dialog
		 *  File name is relative to path of project's .cbp file
		 */
		inline const wxString& GetSourceFile() { return SrcFile; }
		
		/** Getting name of header file declaring dialog
		 *  File name is relative to path of project's .cbp file
		 */
		inline const wxString& GetHeaderFile() { return HFile; }
		
		/** Getting name of xrc file */
		inline const wxString& GetXrcFile() { return XrcFile; }
		
		/** Saving current dialog to xml file */
		void Save();
		
        /** Ckecking if this resource ecan be Previewed */
        virtual bool CanPreview() { return true; }
        
        /** This function should show preview in modal window */
        virtual void ShowPreview();
        
        /** Getting resource name */
        virtual const wxString& GetResourceName();
        
        /** Generating empty source and header file.
          *
          * WARNING: Not checking if files exist
          */
        bool GenerateEmptySources();
        
        /** Function which should check and (if necessary) correct widget's base properties */
        bool CheckBaseProperties(bool Correct,wxsWidget* Changed=NULL);
        
        /** This will be used to shedule code rebuilds */
        virtual void NotifyChange();
        
		/** Getting main dialog widget */
		inline wxsWidget* GetRootWidget() { return RootWidget; }
		
        /** Setting default variable names and identifiers for widgets with empty ones */
        void UpdateWidgetsVarNameId();
        
        /** Function rebuilding definition of event table */
        void UpdateEventTable();
        
        /** Generating Xml document compatible with XRC structure (without additional
         *  parameters */
        TiXmlDocument* GenerateXrc();
        
    protected:
    
        /** Creating editor object */
        virtual wxsEditor* CreateEditor();
        
        /** Function initializing this class - it must be called in constructor
         *  of derived class since virtual functinos can be used from top
         *  constrructor only */
        void Initialize();
        
        /** Function showing resource */
        virtual void ShowResource(wxXmlResource& Res) = 0;
        
        /** Getting string added as constructor code for base widget */
        virtual wxString GetConstructor() = 0;
        
        /** Helper function giving name of resource from current window type */
        virtual const wxChar* GetWidgetClass(bool UseRes = false) = 0;
        
        /** Function creating code which will set up resource */
        virtual wxString ResSetUpCode(int TabSize) = 0;
        
        /** Function generating code loading this resource from xrc file */
        virtual wxString GetXrcLoadingCode(int TabSize) = 0;
        
	private:

        /** Structure used for comparing strings */
        struct ltstr {  bool operator()(const wxChar* s1, const wxChar* s2) const { return wxStrcmp(s1, s2) < 0; } };
        
        /** Map string->widget used when validating variable names and identifiers */
        typedef std::map<const wxChar*,wxsWidget*,ltstr> StrMap;
        typedef StrMap::iterator StrMapI;
	
        /** Creating xml tree for current widget */
        TiXmlDocument* GenerateXml();
        
        /** Adding declaration codes for locally stored widgets */
        void AddDeclarationsReq(wxsWidget* Widget,wxString& LocalCode,wxString& GlobalCode,int LocalTabSize,int GlobalTabSize,bool& WasLocal);
        
        /** Function used internally by SetNewWidgetsIdVarName */
        void UpdateWidgetsVarNameIdReq(StrMap& NamesMap,StrMap& IdsMap,wxsWidget* Widget);
            
        /** Function craeting set of used names and ids for this resource */
        void CreateSetsReq(StrMap& NamesMap, StrMap& IdsMap, wxsWidget* Widget, wxsWidget* Without = NULL);
        
        /** Function checking and correcting base params for one widget */
        bool CorrectOneWidget(StrMap& NamesMap,StrMap& IdsMap,wxsWidget* Changed,bool Correct);
        
        /** Helper function used inside CkeckBaseProperties function */
        bool CheckBasePropertiesReq(wxsWidget* Widget,bool Correct,StrMap& NamesMap,StrMap& IdsMap);
        
        /** Function building array of identifiers */
        void BuildIdsArray(wxsWidget* Widget,wxArrayString& Array);
        
        /** Function building array of header files */
        void BuildHeadersArray(wxsWidget* Widget,wxArrayString& Array);
        
        /** Fuunction collecting code for event table for given widget */
        static void CollectEventTableEnteries(wxString& Code,wxsWidget* Widget,int TabSize);
        
        /** Function generating code fetching controls from xrc structure */
        static void GenXrcFetchingCode(wxString& Code,wxsWidget* Widget,int TabSize);
        
        wxString      ClassName;
        wxString      WxsFile;
        wxString      SrcFile;
        wxString      HFile;
        wxString      XrcFile;
        wxsWidget*    RootWidget;
};


#define wxsGenericWindowResourceBody(Name,ClassR,CtorText,ShowCode)         \
	public:                                                                 \
        wxs##Name##Res( wxsProject* Project,                                \
                        int EditMode,                                       \
                        const wxString& ClassS,                             \
                        const wxString& Wxs,                                \
                        const wxString& Src,                                \
                        const wxString& Head,                               \
                        const wxString& Xrc ):                              \
            wxsWindowRes(Project,EditMode,ClassS,Wxs,Src,Head,Xrc)          \
        { Initialize(); }                                                   \
                                                                            \
        virtual ~wxs##Name##Res()                                           \
        {                                                                   \
            if ( GetProject() )                                             \
            {                                                               \
                GetProject()->Delete##Name                                  \
                ((wxs##Name##Res*)this);                                    \
            }                                                               \
        }                                                                   \
                                                                            \
        inline wxs##Name& Get##Name()                                       \
        { return *((wxs##Name*)GetRootWidget()); }                          \
                                                                            \
    protected:                                                              \
                                                                            \
        virtual wxString GetConstructor()                                   \
        {                                                                   \
        	return _T(CtorText);                                            \
        }                                                                   \
                                                                            \
        virtual void ShowResource(wxXmlResource& Res)                       \
        {                                                                   \
            ShowCode;                                                       \
        }                                                                   \
                                                                            \
        virtual const wxChar* GetWidgetClass(bool UseRes)                   \
        {                                                                   \
            return UseRes ? _T(#ClassR) : _T("wx") _T(#Name);               \
        }                                                                   \
                                                                            \
        virtual wxString GetXrcLoadingCode(int TabSize)                     \
        {                                                                   \
            wxString Code;                                                  \
            Code.Append(_T(' '),TabSize);                                   \
            Code.Append(wxString::Format(                                   \
                _T("wxXmlResource::Get()->Load%s(this,parent,_T(%s));"),    \
                _T(#Name), GetCString(GetClassName()).c_str() ));           \
            return Code;                                                    \
        }                                                                   \
        

/* Generic resources */

class wxsDialogRes: public wxsWindowRes
{
    wxsGenericWindowResourceBody(
        Dialog,
        wxDialog,
        "wxDialog(parent,id,_T(\"\"),wxDefaultPosition,wxDefaultSize)",
            wxDialog Dlg;
            if ( Res.LoadDialog(&Dlg,NULL,GetClassName()) )
            {
                Dlg.ShowModal();
            }
        )
        
    virtual wxString ResSetUpCode(int TabSize)
    {
        // TODO (SpOoN#1#): Apply title and centered properties for dialog
        return _T("");
    }
};

class wxsFrameRes: public wxsWindowRes
{
    wxsGenericWindowResourceBody(
        Frame,
        wxFrame,
        "wxFrame(parent,id,_T(\"\"))",
            wxFrame* Frm = new wxFrame;
            if ( Res.LoadFrame(Frm,NULL,GetClassName()) )
            {
                Frm->Show();
                //Frm->MakeModal();
            }
        )

    virtual wxString ResSetUpCode(int TabSize)
    {
        // TODO (SpOoN#1#): Apply title and centered properties for frame
        return _T("");
    }
};

class wxsPanelRes: public wxsWindowRes
{
    wxsGenericWindowResourceBody(
        Panel,
        wxPanelr,
        "wxPanel(parent,id)",
        	wxDialog Dlg(NULL,-1,wxString::Format(_("Frame preview: %s"),GetClassName().c_str()));
        	wxPanel* Panel = Res.LoadPanel(&Dlg,GetClassName());
        	if ( Panel )
        	{
        		Dlg.Fit();
        		Dlg.ShowModal();
        	}
        )
        
    virtual wxString ResSetUpCode(int TabSize) { return _T(""); }
};

#endif
