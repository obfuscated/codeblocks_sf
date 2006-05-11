#ifndef WXSSIZER_H
#define WXSSIZER_H

#include "wxsparent.h"

/** \brief Structure containing additional parameters for each widget insidee sizer */
class wxsSizerExtra: public wxsPropertyContainer
{
    public:
        long Proportion;                ///< \brief Proportion param (see wxW documentation for details)
        long Flags;                     ///< \brief Sizer item flags
        long Border;                    ///< \brief Size of additional border
        bool BorderInDU;                ///< \brief Switch between pixel / dialog units
//        wxSizeData MinSize;             ///< \brief Minimal size
//        wxSizeData Ratio;               ///< \brief Ratio

        wxsSizerExtra():
            Proportion(1),
            Flags(wxsSizerFlagsProperty::AlignCenterHorizontal|
                  wxsSizerFlagsProperty::AlignCenterVertical|
                  wxsSizerFlagsProperty::BorderTop|
                  wxsSizerFlagsProperty::BorderBottom|
                  wxsSizerFlagsProperty::BorderLeft|
                  wxsSizerFlagsProperty::BorderRight),
            Border(5),
            BorderInDU(false)
        {
            ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));
            Proportion = cfg->ReadInt  ( _T("/defsizer/proportion"), Proportion );
            Flags      = cfg->ReadInt  ( _T("/defsizer/flags"),      Flags );
            Border     = cfg->ReadInt  ( _T("/defsizer/border"),     Border );
            BorderInDU = cfg->ReadBool ( _T("/defsizer/borderdu"),   BorderInDU );
        }

        wxString AllParamsCode(const wxString& WindowParent,wxsCodingLang Language);

    protected:
        virtual void EnumProperties(long Flags);
};


/** \brief Base class for sizers
 *
 * \note sizers doesn't have identifier by default. But because of some
 *       extra information it requires, random identifier will be generated
 *       for it when editing in source mode.
 */
class wxsSizer: public wxsParent
{
    public:

        /** \brief Ctor */
        wxsSizer(wxsWindowRes* Resource);

    protected:

        /** \brief Function generating sizer object used in preview
         *
         * Sizer created here will be used to generate previews. Adding items
         * into sizer are handled automatically. Binding sizer into container
         * must be done in container.
         */
        virtual wxSizer* BuildSizerPreview(wxWindow* Parent) = 0;

        /** \brief Function building code genearating sizer
         *
         * This function must append code generating sizer to the end of Code
         * param. Adding items into sizer is handled automatically.
         */
        virtual void BuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language) = 0;

        /** \brief Function building code
         *
         * Code is created using BuindSizerCreatingCode function. It
         * automatically adds all children into sizer.
         */
        virtual void BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);

        /** \brief Function building preview
         *
         * Preview is generated using BuildSizerPreview function. All child
         * items are added and sizer is binded to parent object automatically.
         *
         * When there's no exact mode, there's additional panel on which
         * guidelines are drawn.
         */
         wxObject* DoBuildPreview(wxWindow* Parent,bool Exact);

        /** \brief Funciton creating additional data
         *
         * There's additional data configuring widget inside sizer - it does
         * define placement, borders and some other flags.
         */
        virtual wxsPropertyContainer* BuildExtra();

        /** \brief Function adding additional QPP child panel */
        virtual void AddChildQPP(wxsItem* Child,wxsAdvQPP* QPP);

        /** \brief Disabling identifier in sizers */
        virtual long GetPropertiesFlags() { return wxsItem::GetPropertiesFlags() & ~wxsFLId; }

    private:

        /** \brief Custom child loading function - needed to support Spacer exception */
        virtual bool XmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        /** \brief Custom child writing function - needed to support Spacer exception */
        virtual bool XmlWriteChild(int Index,TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        /** \brief Name of extra object node will be returned here */
        virtual wxString XmlGetExtraObjectClass();

};

#endif
