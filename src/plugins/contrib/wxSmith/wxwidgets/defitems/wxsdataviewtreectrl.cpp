/*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* This code is based in part on wxsimagecombobox from the wxSmithImage plug-in,
* copyright Ron Collins and released under the GPL.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsdataviewtreectrl.h"
#include <wx/dataview.h>

namespace
{
    wxsRegisterItem<wxsDataViewTreeCtrl> Reg(_T("DataViewTreeCtrl"), wxsTWidget, _T("Standard"), 276);

    WXS_ST_BEGIN(wxsDataViewTreeCtrlStyles, _T("wxDV_DEFAULT_STYLE"))
    WXS_ST_CATEGORY("wxDataViewTreeCtrl")
    WXS_ST(wxDV_SINGLE)
    WXS_ST(wxDV_MULTIPLE)
    WXS_ST(wxDV_ROW_LINES)
    WXS_ST(wxDV_HORIZ_RULES)
    WXS_ST(wxDV_VERT_RULES)
    WXS_ST(wxDV_VARIABLE_LINE_HEIGHT)
    WXS_ST(wxDV_NO_HEADER)
    WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsDataViewTreeCtrlEvents)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_SELECTION_CHANGED, wxDataViewEvent, RowSelected)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_ACTIVATED, wxDataViewEvent, ItemActivated)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_START_EDITING, wxDataViewEvent, ItemStartEditing)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_EDITING_STARTED, wxDataViewEvent, ItemEditingStarted)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_EDITING_DONE, wxDataViewEvent, ItemEditingDone)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_COLLAPSING, wxDataViewEvent, ItemCollapsing)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_COLLAPSED, wxDataViewEvent, ItemCollapsed)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_EXPANDING, wxDataViewEvent, ItemExpanding)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_EXPANDED, wxDataViewEvent, ItemExpanded)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_VALUE_CHANGED, wxDataViewEvent, ItemValueChanged)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEvent, ItemContextMenu)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_COLUMN_HEADER_CLICK, wxDataViewEvent, HeaderClick)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, wxDataViewEvent, HeaderRightClick)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_COLUMN_SORTED, wxDataViewEvent, ColSorted)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_COLUMN_REORDERED, wxDataViewEvent, ColReordered)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_BEGIN_DRAG, wxDataViewEvent, ItemBeginDrag)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_DROP_POSSIBLE, wxDataViewEvent, ItemDropPossible)
    WXS_EVI_STD_TYPE(EVT_DATAVIEW_ITEM_DROP, wxDataViewEvent, ItemDrop)

    WXS_EV_END()
}

wxsDataViewTreeCtrl::wxsDataViewTreeCtrl(wxsItemResData *Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsDataViewTreeCtrlEvents,
        wxsDataViewTreeCtrlStyles)
{

}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsDataViewTreeCtrl::OnBuildCreatingCode()
{

    switch(GetLanguage())
    {
        case wxsCPP:
            {
                AddHeader(_T("<wx/dataview.h>"), GetInfo().ClassName, 0);
                AddHeader(_T("<wx/dataview.h>"), _T("wxDataViewTreeCtrl"), 0);
                Codef(_T("%C(%W, %I, %P, %S, %T, %V);\n"));

                // assign the image-list -- AFTER the image list has been built
                BuildSetupWindowCode();
                return;
            }

        case wxsUnknownLanguage: // fall-through
        default:
            {
                wxsCodeMarks::Unknown(_T("wxsDataViewTreeCtrl::OnBuildCreatingCode"), GetLanguage());
            }
    }
}

/*! \brief    Build the control preview.
 *
 * \param parent wxWindow*    The parent window.
 * \param flags long          The control flags.
 * \return wxObject*          The constructed control.
 *
 */
wxObject *wxsDataViewTreeCtrl::OnBuildPreview(wxWindow *Parent, long Flags)
{
    wxDataViewCtrl *preview = new wxDataViewCtrl(Parent, GetId(), Pos(Parent), Size(Parent), Style());

    return SetupWindow(preview, Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsDataViewTreeCtrl::OnEnumWidgetProperties(cb_unused long Flags)
{

}

/*! \brief Find all tools that are image lists and return their names.
 *
 * \param aNames wxArrayString&
 * \return void
 *
 */
void wxsDataViewTreeCtrl::FindAllImageLists(wxArrayString &aNames)
{

}

/*! \brief .Update the list of combo items to send to the wsxImageTreeEditorDlg.
 *
 * \return void
 *
 */
void wxsDataViewTreeCtrl::UpdateTreeItemList()
{

}
