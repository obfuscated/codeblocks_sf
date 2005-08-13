#ifndef WXSDEFSIZER_H
#define WXSDEFSIZER_H

#include "wxscontainer.h"
#include "defwidgets/wxsstdmanager.h"

#define wxsDSDeclareBegin(Name,SizerId)                                 \
    class wxs##Name: public wxsDefSizer                                 \
    {                                                                   \
        public:                                                         \
            wxs##Name(wxsWidgetManager* Man,wxsWindowRes* Res):         \
                wxsDefSizer(Man,Res,propSizer)                          \
            {                                                           \
                Init();                                                 \
            }                                                           \
                                                                        \
            virtual const wxsWidgetInfo& GetInfo()                      \
            {                                                           \
            	return *wxsStdManager.GetWidgetInfo(SizerId);           \
            }                                                           \
                                                                        \
            virtual wxString GetProducingCode(wxsCodeParams& Params);   \
                                                                        \
            virtual wxString GetDeclarationCode(wxsCodeParams& Params)  \
            {                                                           \
                return wxString::Format(_T("wx") _T(#Name) _T("* %s;"), \
                    GetBaseParams().VarName.c_str());                   \
            }                                                           \
                                                                        \
            virtual wxSizer* ProduceSizer(wxWindow* Panel);             \
                                                                        \
        protected:                                                      \
                                                                        \
            virtual bool MyXmlLoad();                                   \
            virtual bool MyXmlSave();                                   \
            virtual void CreateObjectProperties();                      \
            virtual void Init();                                        \
                                                                        \
        private:
        
#define wxsDSDeclareEnd()                                               \
    };

class wxsDefSizer : public wxsContainer
{
	public:
		wxsDefSizer(wxsWidgetManager* Man,wxsWindowRes* Res,BasePropertiesType pType = propSizer);
		virtual ~wxsDefSizer();
		
        virtual wxString GetFinalizingCode(wxsCodeParams& Params);
        
        /** This function should produce sizer with properties handled by this objeect */
        virtual wxSizer* ProduceSizer(wxWindow* Panel) = 0;
        
    protected:
    
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        virtual void MyFinalUpdatePreview(wxWindow* Preview);
};

#endif
