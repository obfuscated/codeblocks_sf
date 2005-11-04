#ifndef WXSDEFSIZER_H
#define WXSDEFSIZER_H

#include "wxscontainer.h"
#include "defwidgets/wxsstdmanager.h"
#include <vector>

/** Structure containing additional parameters for each widget insidee sizer */
struct wxsSizerExtraParams
{
    enum PlacementType              ///< Values used in Placement member
    {
        LeftTop = 0,
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
        Right  = 8
    };

    int  Proportion;                ///< Proportion param (see wxW documentation for details)
    int  BorderFlags;               ///< Which sides should use additional bordeer ?
    bool Expand;                    ///< Expand flag
    bool Shaped;                    ///< Shaped flag
    bool FixedMinSize;              ///< FixedMinSize flag
    int  Placement;                 ///< Placement of this element
    int  Border;                    ///< Size of additional border (in pixels)

    wxsSizerExtraParams():
        Proportion(1),
        BorderFlags(Top|Bottom|Left|Right),
        Expand(false),
        Shaped(false),
        FixedMinSize(false),
        Placement(Center),
        Border(5)
    {}
};

/** Macro starting declaration of sizer class */
#define wxsDSDeclareBegin(Name,SizerId)                                 \
    class WXSCLASS wxs##Name: public wxsDefSizer                        \
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
            virtual wxString GetProducingCode(wxsCodeParams& Params);   \
                                                                        \
            virtual wxString GetDeclarationCode(wxsCodeParams& Params)  \
            {                                                           \
                return wxString::Format(_T("wx") _T(#Name) _T("* %s;"), \
                    GetBaseProperties().VarName.c_str());               \
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

/** Macro finishing declaration of sizer class */
#define wxsDSDeclareEnd()                                               \
    };

/** Base class for sizers */
class WXSCLASS wxsDefSizer : public wxsContainer
{
	public:
		wxsDefSizer(wxsWidgetManager* Man,wxsWindowRes* Res);
		virtual ~wxsDefSizer();

        virtual wxString GetFinalizingCode(wxsCodeParams& Params);

        /** This function should produce sizer with properties handled by this objeect */
        virtual wxSizer* ProduceSizer(wxWindow* Panel) = 0;

        /** Getting wxsSierExtraParams for given widget */
        inline wxsSizerExtraParams* GetExtraParams(int Index)
        {
        	return (wxsSizerExtraParams*)GetExtra(Index);
        }

        virtual wxWindow* GetChildPaletteHeader(wxWindow* Parent,int ChildPos);

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
