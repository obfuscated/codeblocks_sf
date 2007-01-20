#ifndef WXSCORRECTOR_H
#define WXSCORRECTOR_H

#include <wx/string.h>
#include <wx/hashmap.h>
#include <wx/hashset.h>

class wxsItemResData;
class wxsItem;

/** \brief This class is responsible for correcting data in item resource.
 *
 * This class takes care about variable names and identifiers. It may also
 * do some additional checks in future. Base rule is that there can be no items
 * in resource with same variable name nor there may not be two items with same
 * identifier
 */
class wxsCorrector
{
    public:

        /** \brief Ctor */
        wxsCorrector(wxsItemResData* Data);

        /** \brief Correcting data right after resource load
         *
         * This function scans all items searching for duplicates of
         * variable names and identifiers. For duplicates, new values are
         * created. Also empty fields are filled with new items.
         *
         * \return false if all data was correct, false otherwise
         */
        bool GlobalCheck();

        /** \brief Correcting data after change of item properties
         *
         * This function should be called when variable name or identifier
         * may have changed in one item.
         */
        void AfterChange(wxsItem* Item);

        /** \brief Preparing new item before pasting into resource.
         *
         * This function updates all variable names and identifier in
         * given tree subnode.
         */
        void BeforePaste(wxsItem* Item);

        /** \brief Notifying inish of pasting item */
        void AfterPaste(wxsItem* Item);

        /** \brief Clearing current vars/ids cache */
        void ClearCache();

    private:

        /** \brief Rebuilding content of Vars and Ids */
        void RebuildSets();

        /** \brief Generating new variable name for given item */
        void SetNewVarName(wxsItem* Item);

        /** \brief Generating new identifier for given item */
        void SetNewIdName(wxsItem* Item);

        /** \brief Function checking and fixing if necessary variable name */
        bool FixVarName(wxString& Name);

        /** \brief Function checking and fixing if necessary identifier */
        bool FixIdName(wxString& Id);

        void RebuildSetsReq(wxsItem* Item,wxsItem* Exclude);
        bool FixAfterLoadCheckNames(wxsItem* Item);
        bool FillEmpty(wxsItem* Item);
        void BeforePasteReq(wxsItem* Item);

        WX_DECLARE_HASH_SET(wxString,wxStringHash,wxStringEqual,wxStringSet);

        wxsItemResData* m_Data; ///< \brief Data object using this corrector
        wxStringSet m_Vars;     ///< \brief set of currently used variable names
        wxStringSet m_Ids;      ///< \brief set of currently used identifiers
        bool m_NeedRebuild;     ///< \brief flag indicating when Vars and Ids contain valid data
};

#endif
