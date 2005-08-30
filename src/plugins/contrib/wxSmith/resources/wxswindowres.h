#ifndef WXSWINDOWRES_H
#define WXSWINDOWRES_H

#include "../wxsresource.h"
#include "../defwidgets/wxsdialog.h"
#include "../defwidgets/wxspanel.h"
#include "../defwidgets/wxsframe.h"
#include <wx/string.h>

/** Base class for all window resources (Dialog, Frame and Panel) */
class wxsWindowRes : public wxsResource
{
	public:
	
        enum WindowResType
        {
        	Dialog,
        	Frame,
        	Panel
        };
	
        /** Ctor */
		wxsWindowRes(
            wxsProject* Project,
            const wxString& Class,
            const wxString& Xrc,
            const wxString& Src,
            const wxString& Head,
            WindowResType Type
            );
		
		/** Dctor */
		virtual ~wxsWindowRes();
		
		/** Getting name of class implementing this dialog */
		inline const wxString& GetClassName() { return ClassName; }
		
		/** Getting name of xrc file containing structire of dialog */
		inline const wxString& GetXrcFile() { return XrcFile; }
		
		/** Getting name of source file implementing dialog */
		inline const wxString& GetSourceFile() { return SrcFile; }
		
		/** Getting name of header file declaring dialog */
		inline const wxString& GetHeaderFile() { return HFile; }
		
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
        
    protected:
    
        /** Creating editor object */
        virtual wxsEditor* CreateEditor();
		
	private:

        /** Structure used for comparing strings */
        struct ltstr {  bool operator()(const wxChar* s1, const wxChar* s2) const { return wxStrcmp(s1, s2) < 0; } };
        
        /** Map string->widget used when validating variable names and identifiers */
        typedef std::map<const wxChar*,wxsWidget*,ltstr> StrMap;
        typedef StrMap::iterator StrMapI;
	
        /** Creating xml tree for current widget */
        TiXmlDocument* GenerateXml();
        
        /** Helper fuunction giving name of resource from current window type */
        inline const wxChar* GetWidgetClass(bool UseRes = false);
        
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
        static void CollectEventTableEnteries(wxString& Code,wxsWidget* Widget);
            
        wxString      ClassName;
        wxString      XrcFile;
        wxString      SrcFile;
        wxString      HFile;
        WindowResType Type;
        wxsWidget*    RootWidget;
};

/** Resource defining wxDialog class */
class wxsDialogRes: public wxsWindowRes
{
	public:
        wxsDialogRes( wxsProject* Project, const wxString& Class, const wxString& Xrc, const wxString& Src, const wxString& Head ):
            wxsWindowRes(Project,Class,Xrc,Src,Head,wxsWindowRes::Dialog) {}
        
        inline wxsDialog& GetDialog() { return *((wxsDialog*)GetRootWidget()); }
};

/** Resource defining wxFrame class */
class wxsFrameRes: public wxsWindowRes
{
	public:
        wxsFrameRes( wxsProject* Project, const wxString& Class, const wxString& Xrc, const wxString& Src, const wxString& Head ):
            wxsWindowRes(Project,Class,Xrc,Src,Head,wxsWindowRes::Frame) {}
        
        inline wxsFrame& GetFrame() { return *((wxsFrame*)GetRootWidget()); }
};

/** Resource defining wxPanel class */
class wxsPanelRes: public wxsWindowRes
{
	public:
        wxsPanelRes( wxsProject* Project, const wxString& Class, const wxString& Xrc, const wxString& Src, const wxString& Head ):
            wxsWindowRes(Project,Class,Xrc,Src,Head,wxsWindowRes::Panel) {}
        
        inline wxsPanel& GetPanel() { return *((wxsPanel*)GetRootWidget()); }
};

#endif
