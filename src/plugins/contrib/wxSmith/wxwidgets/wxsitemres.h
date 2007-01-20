#ifndef WXSITEMRES_H
#define WXSITEMRES_H

#include "wxwidgetsres.h"

/** \brief Some abstract interface allowing wxsItemResData to access some resource-specific functinos easily */
class wxsItemResData;
class wxsItemResFunctions
{
    public:

        /** \brief Ctor */
        wxsItemResFunctions() {};

        /** \brief Dctor */
        virtual ~wxsItemResFunctions() {};

        /** \brief Generating exact preview used in editor after pressing preview button */
        virtual wxWindow* OnBuildExactPreview(wxWindow* Parent,wxsItemResData* Data) = 0;
};

/** \brief Base class for resources using item as root element
 *
 * This class implements most of functions in wxsResource and wxWidgetsRes,
 * only need to write this function: \code virtual wxString OnGetAppBuildingCode() \endcode
 */
class wxsItemRes: public wxWidgetsRes, public wxsItemResFunctions
{
    DECLARE_CLASS(wxsItemRes)
    public:

        /** \brief Available edit modes for item resources */
        enum EditMode { File, Source, Mixed };

        /** \brief Ctor */
        wxsItemRes(wxsProject* Owner,const wxString& ResourceType,bool CanBeMain);

        /** \brief Ctor for external resource
         *  \param FileName name of XRC file
         *  \param Object Xml node with XRC resource
         */
        wxsItemRes(const wxString& FileName,const TiXmlElement* XrcElem,const wxString& ResourceType);

        /** \brief Dctor */
        virtual ~wxsItemRes();

        /** \brief Creating new resource and building files if necessarry */
        virtual bool CreateNewResource(
            const wxString& Class,
            const wxString& Src,
            bool GenSrc,
            const wxString& Hdr,
            bool GenHdr,
            const wxString& Xrc,
            bool GenXrc);

        inline const wxString& GetWxsFileName() { return m_WxsFileName; }
        inline const wxString& GetSrcFileName() { return m_SrcFileName; }
        inline const wxString& GetHdrFileName() { return m_HdrFileName; }
        inline const wxString& GetXrcFileName() { return m_XrcFileName; }

        /** \brief Getting current edit mode */
        EditMode GetEditMode();

    protected:

        virtual wxsEditor* OnCreateEditor(wxWindow* Parent);
        virtual bool OnReadConfig(const TiXmlElement* Node);
        virtual bool OnWriteConfig(TiXmlElement* Node);
        virtual bool OnCanHandleFile(const wxString& FileName);
        virtual wxString OnGetDeclarationFile();
        virtual bool OnGetUsingXRC();
        virtual bool OnGetCanBeMain();

    private:

        wxString m_WxsFileName;
        wxString m_SrcFileName;
        wxString m_HdrFileName;
        wxString m_XrcFileName;
        bool     m_CanBeMain;
};

#endif
