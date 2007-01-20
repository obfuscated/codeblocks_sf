#ifndef WXSITEMFACTORY_H
#define WXSITEMFACTORY_H

#include "wxsiteminfo.h"
#include "../wxscodinglang.h"

#include <wx/string.h>
#include <wx/hashmap.h>

#include <configmanager.h>

class wxsItem;
class wxsItemResData;

/** \brief Class managing creation, destruction and enumeration of supported
 *         items
 *
 * All public functinos are static which means that this class is something
 * like singleton but using static members for global operations.
 * Each instance of this class does represent one wxsItem - provides it's info
 * and is able to build this item.
 * Addind new supported item to factory is done by deriving other class from
 * this one and writing it's
 */
class wxsItemFactory
{
	public:

        /** \brief Creating item with given name */
        static wxsItem* Build(const wxString& ClassName,wxsItemResData* Data);

        /** \brief Returning info for item with given name
         *  \param ClassName name of item's class
         *  \return Pointer to info or NULL if there's no such item
         */
        static const wxsItemInfo* GetInfo(const wxString& ClassName);

        /** \brief Getting onfo of first item */
        static const wxsItemInfo* GetFirstInfo();

        /** \brief Continuing getting item infos */
        static const wxsItemInfo* GetNextInfo();

    protected:

        /** \brief Ctor - can be accessed from derived classes only */
		wxsItemFactory(const wxsItemInfo* Info);

		/** \brief Extra ctor for templates - uses class name from param.
		 *         It may be used when Info is going to be created
		 *         inside constructor.
		 */
        wxsItemFactory(const wxsItemInfo* Info,const wxString& ClassName);

		/** \brief Dctor */
		virtual ~wxsItemFactory();

        /** \brief Building item */
        virtual wxsItem* OnBuild(wxsItemResData* Data) = 0;

	private:

        WX_DECLARE_STRING_HASH_MAP(wxsItemFactory*,ItemMapT);

        /** \brief Function for getting global item's map */
        static ItemMapT& ItemMap();

        const wxsItemInfo* m_Info;          ///< \brief Info of item handled by this instance
        static ItemMapT::iterator m_Iter;   ///< \brief Iterator used for GetFirstInfo / GetNextInfo
};

/** \brief Template for easy registering of new items */
template<class T> class wxsRegisterItem: public wxsItemFactory
{
    public:

        /** \brief Publically available info - may be accessed from item */
        wxsItemInfo Info;

        /** \brief Ctor - bitmaps are generated from wxBitmap classes */
        wxsRegisterItem(
            const wxString& ClassName,
            wxsItemType Type,
            const wxString& License,
            const wxString& Author,
            const wxString& Email,
            const wxString& Site,
            const wxString& Category,
            long Priority,
            const wxString& DefaultVarName,
            long Languages,
            unsigned short VerHi,
            unsigned short VerLo,
            const wxBitmap& Bmp32,
            const wxBitmap& Bmp16
            ): wxsItemFactory(&Info,ClassName)
        {
            Info.ClassName = ClassName;
            Info.Type = Type;
            Info.License = License;
            Info.Author = Author;
            Info.Email = Email;
            Info.Site = Site;
            Info.Category = Category;
            Info.Priority = Priority;
            Info.DefaultVarName = DefaultVarName;
            Info.Languages = Languages;
            Info.VerHi = VerHi;
            Info.VerLo = VerLo;
            Info.Icon32 = Bmp32.GetSubBitmap(wxRect(0,0,Bmp32.GetWidth(),Bmp32.GetHeight()));
            Info.Icon16 = Bmp16.GetSubBitmap(wxRect(0,0,Bmp16.GetWidth(),Bmp16.GetHeight()));

            // TODO: Build resource tree entry
        }

        /** \brief Ctor - bitmaps are loaded from files */
        wxsRegisterItem(
            const wxString& ClassName,
            wxsItemType Type,
            const wxString& License,
            const wxString& Author,
            const wxString& Email,
            const wxString& Site,
            const wxString& Category,
            long Priority,
            const wxString& DefaultVarName,
            long Languages,
            unsigned short VerHi,
            unsigned short VerLo,
            const wxString& Bmp32FileName,
            const wxString& Bmp16FileName
            ): wxsItemFactory(&Info,ClassName)
        {
            Info.ClassName = ClassName;
            Info.Type = Type;
            Info.License = License;
            Info.Author = Author;
            Info.Email = Email;
            Info.Site = Site;
            Info.Category = Category;
            Info.Priority = Priority;
            Info.DefaultVarName = DefaultVarName;
            Info.Languages = Languages;
            Info.VerHi = VerHi;
            Info.VerLo = VerLo;

            wxString DataPath = ConfigManager::GetDataFolder() + _T("/images/wxsmith/");
            Info.Icon32.LoadFile(DataPath+Bmp32FileName,wxBITMAP_TYPE_ANY);
            Info.Icon16.LoadFile(DataPath+Bmp16FileName,wxBITMAP_TYPE_ANY);

            // TODO: Build resource tree entry
        }

        /** \brief Ctor for built-in items from wxWidgets - sets most of data fields to default */
        wxsRegisterItem(
            const wxString& ClassNameWithoutWx,
            wxsItemType Type,
            const wxString& Category,
            long Priority): wxsItemFactory(&Info,_T("wx") + ClassNameWithoutWx)
        {
            Info.ClassName = _T("wx") + ClassNameWithoutWx;
            Info.Type = Type;
            Info.License = _("wxWidgets license");
            Info.Author = _("wxWidgets team");
            Info.Email = _T("");
            Info.Site = _T("www.wxwidgets.org");
            Info.Category = Category;
            Info.Priority = Priority;
            Info.DefaultVarName = ClassNameWithoutWx;
            Info.Languages = wxsCPP;
            Info.VerHi = 2;
            Info.VerLo = 6;

            wxString DataPath = ConfigManager::GetDataFolder() + _T("/images/wxsmith/");
            Info.Icon32.LoadFile(DataPath+Info.ClassName+_T(".png"),wxBITMAP_TYPE_PNG);
            Info.Icon16.LoadFile(DataPath+Info.ClassName+_T("16.png"),wxBITMAP_TYPE_PNG);

            // TODO: Build resource tree entry
        }

    protected:

        virtual wxsItem* OnBuild(wxsItemResData* Data)
        {
            return new T(Data);
        }
};

#endif
