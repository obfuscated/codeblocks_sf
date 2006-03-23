#ifndef WXSDEFSIZER_H
#define WXSDEFSIZER_H

#include <manager.h>
#include <configmanager.h>
#include "wxscontainer.h"
#include "defwidgets/wxsstdmanager.h"
#include <vector>

/** Structure containing additional parameters for each widget insidee sizer */
struct wxsSizerExtraParams
{
    enum PlacementType              ///< Values used in Placement member
    {
        LeftTop = 1,
        CenterTop,
        RightTop,
        LeftCenter,
        Center,
        RightCenter,
        LeftBottom,
        CenterBottom,
        RightBottom
    };

    enum BorderFlagsValues          ///< Values which can be used in BorderFlags (ored values)
    {
        None   = 0,
        Top    = 1,
        Bottom = 2,
        Left   = 4,
        Right  = 8,
        All    = Top | Bottom | Left | Right
    };

    int  Proportion;                ///< Proportion param (see wxW documentation for details)
    int  BorderFlags;               ///< Which sides should use additional bordeer ?
    bool Expand;                    ///< Expand flag
    bool Shaped;                    ///< Shaped flag
    bool FixedMinSize;              ///< FixedMinSize flag
    int  Placement;                 ///< Placement of this element
    int  Border;                    ///< Size of additional border (in pixels)

    wxsSizerExtraParams()
        : FixedMinSize(false)
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));

        Proportion = cfg->ReadInt(_T("/defsizer/proportion"), 0);
        BorderFlags = (BorderFlagsValues)cfg->ReadInt(_T("/defsizer/borderflags"), All);
        Expand = cfg->ReadBool(_T("/defsizer/expand"), false);
        Shaped = cfg->ReadBool(_T("/defsizer/shaped"), false);
        Placement = (PlacementType)cfg->ReadInt(_T("/defsizer/placement"), LeftTop);
        Border = cfg->ReadInt(_T("/defsizer/border"), 0);
    }
};

/** Macro starting declaration of sizer class */
#define wxsDSDeclareBegin(Name,SizerId)                                 \
    class wxs##Name: public wxsDefSizer                        \
    {                                                                   \
        public:                                                         \
            wxs##Name(wxsWidgetManager* Man,wxsWindowRes* Res):         \
                wxsDefSizer(Man,Res)                                    \
            {                                                           \
                Init();                                                 \
            }                                                           \
                                                                        \
            virtual const wxsWidgetInfo& GetInfo()                      \
            {                                                           \
            	return *wxsStdManager.GetWidgetInfo(SizerId);           \
            }                                                           \
                                                                        \
            virtual wxString GetProducingCode(const wxsCodeParams& Params);   \
                                                                        \
            virtual wxSizer* ProduceSizer(wxWindow* Panel);             \
                                                                        \
        protected:                                                      \
                                                                        \
            virtual bool MyXmlLoad();                                   \
            virtual bool MyXmlSave();                                   \
            virtual void MyCreateProperties();                          \
            virtual void Init();                                        \
                                                                        \
        private:

/** Macro finishing declaration of sizer class */
#define wxsDSDeclareEnd()                                               \
    };

/** Base class for sizers */
class wxsDefSizer : public wxsContainer
{
	public:
		wxsDefSizer(wxsWidgetManager* Man,wxsWindowRes* Res);
		virtual ~wxsDefSizer();

        virtual wxString GetFinalizingCode(const wxsCodeParams& Params);

        /** This function should produce sizer with properties handled by this objeect */
        virtual wxSizer* ProduceSizer(wxWindow* Panel) = 0;

        /** Getting wxsSierExtraParams for given widget */
        inline wxsSizerExtraParams* GetExtraParams(int Index)
        {
        	return (wxsSizerExtraParams*)GetExtra(Index);
        }

        virtual wxWindow* BuildChildQuickPanel(wxWindow* Parent,int ChildPos);

    protected:

        virtual bool XmlLoadChild(TiXmlElement* Element);
        virtual bool XmlSaveChild(int ChildIndex,TiXmlElement* AddHere);
        virtual void AddChildProperties(int ChildIndex);

        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        virtual void MyFinalUpdatePreview(wxWindow* Preview);

        virtual void* NewExtra() { return new wxsSizerExtraParams; }
        virtual void DelExtra(void* Data) { delete (wxsSizerExtraParams*)Data; }

    private:

        bool LoadSizerStuff(wxsSizerExtraParams* Params,TiXmlElement* Elem);
        bool SaveSizerStuff(wxsSizerExtraParams* Params,TiXmlElement* Elem);
        wxString GetFlagToSizer(wxsSizerExtraParams* Params);

};

#endif
