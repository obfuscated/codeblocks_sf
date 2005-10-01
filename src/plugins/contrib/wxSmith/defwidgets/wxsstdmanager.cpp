#include "wxsstdmanager.h"

#include "wxsbutton.h"
#include "wxsframe.h"
#include "wxspanel.h"
#include "wxsdialog.h"
#include "wxsgridsizer.h"
#include "wxsstatictext.h"
#include "wxscheckbox.h"
#include "wxstogglebutton.h"
#include "wxscombobox.h"
#include "wxslistbox.h"
#include "wxsspacer.h"
#include "wxsboxsizer.h"
#include "wxsstaticboxsizer.h"
#include "wxsflexgridsizer.h"
#include "wxstextctrl.h"
#include "wxsgauge.h"
#include "wxsradiobutton.h"
#include "wxsscrollbar.h"
#include "wxsspinbutton.h"
#include "wxsspinctrl.h"
#include "wxstreectrl.h"
#include "wxsradiobox.h"
#include "wxsdatepickerctrl.h"
#include "wxsstaticline.h"
#include "wxssplitterwindow.h"
#include "wxsnotebook.h"
#include "wxslistbook.h"


#include <wx/xrc/xmlres.h>
#include <configmanager.h>
#include <wx/fs_zip.h>

/******************************************************************************/
/* Infos for standard widgets                                                 */
/******************************************************************************/

static const wxString DefLicence     = _("wxWidgets License");
static const wxString DefAuthor      = _("wxWidgets Team");
static const wxString DefAuthorEmail = _T("");
static const wxString DefAuthorSite  = _("www.wxwidgets.org");
static const wxString DefCategory    = _("Standard");
static const wxString DefSizerCat    = _("Layout");


#define Entry2Headers(Name,Link,Header1,Header2)        \
    {   _T("wx") _T(#Name),                             \
        DefLicence,                                     \
        DefAuthor,                                      \
        DefAuthorEmail,                                 \
        DefAuthorSite,                                  \
        _T("http://www.wxwidgets.org/manuals/2.6.1/") _T(Link),  \
        DefCategory,                                    \
        _T(#Name),                                      \
        false,                                          \
        false,                                          \
        false,                                          \
        2, 6,                                           \
        NULL,                                           \
        &wxsStdManager,                                 \
        wxs##Name##Id,                                  \
        0,                                              \
        wxs##Name##Styles,                              \
        wxs##Name##Events,                              \
        _T(Header1),                                    \
        _T(Header2),                                    \
        wxsWidgetInfo::exNone                           \
    },

#define Entry(Name,Link,Header)                         \
    Entry2Headers(Name,Link,Header,"")

#define WindowEntry(Name,Link,Header)                   \
    {   _T("wx") _T(#Name),                             \
        DefLicence,                                     \
        DefAuthor,                                      \
        DefAuthorEmail,                                 \
        DefAuthorSite,                                  \
        _T("http://www.wxwidgets.org/manuals/2.6.1/") _T(Link),  \
        _T(""),                                         \
        _T(""),                                         \
        false,                                          \
        false,                                          \
        false,                                          \
        2, 6,                                           \
        NULL,                                           \
        &wxsStdManager,                                 \
        wxs##Name##Id,                                  \
        0,                                              \
        wxs##Name##Styles,                              \
        wxs##Name##Events,                              \
        _T(Header),                                     \
        _T(""),                                         \
        wxsWidgetInfo::exNone                           \
    },

#define SpacerEntry()                                   \
    {   _T("Spacer"),                                   \
        DefLicence,                                     \
        DefAuthor,                                      \
        DefAuthorEmail,                                 \
        DefAuthorSite,                                  \
        _T("http://www.wxwidgets.org/manuals/2.6.1/wx_wxsizer.html#wxsizeradd"),    \
        DefSizerCat,                                    \
        _T(""),                                         \
        false,                                          \
        false,                                          \
        true,                                           \
        2, 6,                                           \
        NULL,                                           \
        &wxsStdManager,                                 \
        wxsSpacerId,                                    \
        0,                                              \
        NULL,                                           \
        NULL,                                           \
        _T("<wx/sizer.h>"),                             \
        _T(""),                                         \
        wxsWidgetInfo::exNone                           \
    },
        

#define SizerEntry(Name,Link,Header)                            \
    {   _T("wx") _T(#Name),                                     \
        DefLicence,                                             \
        DefAuthor,                                              \
        DefAuthorEmail,                                         \
        DefAuthorSite,                                          \
        _T("http://www.wxwidgets.org/manuals/2.6.1/") _T(Link), \
        DefSizerCat,                                            \
        _T(#Name),                                              \
        true,                                                   \
        true,                                                   \
        false,                                                  \
        2, 6,                                                   \
        NULL,                                                   \
        &wxsStdManager,                                         \
        wxs##Name##Id,                                          \
        0,                                                      \
        NULL,                                                   \
        NULL,                                                   \
        _T(Header),                                             \
        _T(""),                                                 \
        wxsWidgetInfo::exNone                                   \
    },
        

static wxsWidgetInfo StdInfos[] =
{
    { _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), false,
      false, false, 0, 0, NULL, NULL, wxsNoneId, 0, NULL, NULL, _T(""), _T(""), 
      wxsWidgetInfo::exNone },  // NONE
    
    SizerEntry(GridSizer,"wx_wxgridsizer.html#wxgridsizer","<wx/sizer.h>")
    SizerEntry(BoxSizer,"wx_wxboxsizer.html#wxboxsizer","<wx/sizer.h>")
    SizerEntry(StaticBoxSizer,"wx_wxstaticboxsizer.html#wxstaticboxsizer","<wx/sizer.h>")
    SizerEntry(FlexGridSizer,"wx_wxflexgridsizer.html#wxflexgridsizer","<wx/sizer.h>")
    SpacerEntry()
 
    Entry(Button,      "wx_wxbutton.htm#wxbutton","<wx/button.h>")
    Entry(ToggleButton,"wx_wxtogglebutton.html#wxtogglebutton","<wx/tglbtn.h>")
    Entry(CheckBox,    "wx_wxcheckbox.html#wxcheckbox","<wx/checkbox.h>")
    Entry(StaticText,  "wx_wxstatictext.html#wxstatictext","<wx/stattext.h>")
    Entry(ComboBox,    "wx_wxcombobox.html#wxcombobox","<wx/combobox.h>")
    Entry(ListBox,     "wx_wxlistbox.html#wxlistbox","<wx/listbox.h>")
    Entry(Panel,       "wx_wxpanel.html#wxpanel","<wx/panel.h>")
    Entry(TextCtrl,    "wx_wxtextctrl.html#wxtextctrl","<wx/textctrl.h>")
    Entry(Gauge,       "wx_wxgauge.html#wxgauge","<wx/gauge.h>")
    Entry(RadioButton,   "wx_wxradiobutton.html#wxradiobutton","<wx/radiobut.h>")   
    Entry(ScrollBar,     "wx_wxscrollbar.html#wxscrollbar","<wx/scrolbar.h>")
    Entry(SpinButton,    "wx_wxspinbutton.html#wxspinbutton","<wx/spinbutt.h>")
    Entry(SpinCtrl,      "wx_wxspinctrl.html#wxspinctrl","<wx/spinctrl.h>")
    Entry(TreeCtrl,      "wx_wxtreectrl.html#wxtreectrl","<wx/treectrl.h>")
    Entry(RadioBox,      "wx_wxradiobox.html#wxradiobox","<wx/radiobox.h>")
    Entry2Headers(DatePickerCtrl,"wx_wxdatepickerctrl.html#wxdatepickerctrl","<wx/datectrl.h>","<wx/dateevt.h>")
    Entry(StaticLine,    "wx_wxstaticline.html#wxstaticline","<wx/statline.h>")
    Entry(SplitterWindow,"wx_wxsplitterwindow.html#wxsplitterwindow","<wx/splitter.h>")
    Entry(Notebook,      "wx_wxnotebook.html#wxnotebook","<wx/notebook.h>")
    Entry(Listbook,      "wx_wxlistbook.html#wxlistbook","<wx/listbook.h>")
    
    WindowEntry(Dialog,"wx_wxdialog.html#wxdialog","<wx/dialog.h>")
    WindowEntry(Frame, "wx_wxframe.html#wxframe","<wx/frame.h>")
    WindowEntry(Panelr,"wx_wxpanel.html#wxpanel","<wx/panel.h>")
    
};

static const int StdInfosCnt = sizeof(StdInfos) / sizeof(StdInfos[0]);


wxsStdManagerT::wxsStdManagerT()
{
}

wxsStdManagerT::~wxsStdManagerT()
{
    for ( int i=0; i<StdInfosCnt; i++ )
    {
        if ( StdInfos[i].Icon )
        {
            delete StdInfos[i].Icon;
            StdInfos[i].Icon = NULL;
        }
    }
}

bool wxsStdManagerT::Initialize()
{
    wxString resPath = ConfigManager::Get()->Read(_T("data_path"), wxEmptyString);
    for ( int i=1; i<StdInfosCnt; i++ )
    {
        wxString FileName = resPath + _T("/images/wxsmith/") + StdInfos[i].Name + _T(".png");
        wxBitmap* Bmp = new wxBitmap;
        if ( wxFileName::FileExists(FileName) )
        {
            Bmp->LoadFile(FileName,wxBITMAP_TYPE_PNG);
                
            if ( Bmp->Ok() )
            {
                StdInfos[i].Icon = Bmp;
            }
            else
            {
                StdInfos[i].Icon = NULL;
                delete Bmp;
            }
        }
        else
        {
            StdInfos[i].Icon = NULL;
        }
    }
    return true;
}

int wxsStdManagerT::GetCount()
{
    return wxsStdIdCount;
}
        
/** Getting widget's info */
const wxsWidgetInfo* wxsStdManagerT::GetWidgetInfo(int Number)
{
    if ( Number < 0 || Number >= StdInfosCnt ) Number = wxsNoneId;
    assert ( StdInfos[Number].Id == Number );
    return &StdInfos[Number];
}

/** Getting new widget */
wxsWidget* wxsStdManagerT::ProduceWidget(int Id,wxsWindowRes* Res)
{
    switch ( Id )
    {
        case wxsGridSizerId:        return new wxsGridSizer(this,Res);
        case wxsBoxSizerId:         return new wxsBoxSizer(this,Res);
        case wxsStaticBoxSizerId:   return new wxsStaticBoxSizer(this,Res);
        case wxsFlexGridSizerId:    return new wxsFlexGridSizer(this,Res);
        case wxsButtonId:           return new wxsButton(this,Res);
        case wxsCheckBoxId:         return new wxsCheckBox(this,Res);
        case wxsStaticTextId:       return new wxsStaticText(this,Res);
        case wxsToggleButtonId:     return new wxsToggleButton(this,Res);
        case wxsComboBoxId:         return new wxsComboBox(this,Res);
        case wxsListBoxId:          return new wxsListBox(this,Res);
        case wxsPanelId:            return new wxsPanel(this,Res);
        case wxsTextCtrlId:         return new wxsTextCtrl(this,Res);
        case wxsGaugeId:            return new wxsGauge(this,Res);
        case wxsSpacerId:           return new wxsSpacer(this,Res);
        case wxsDialogId:           return new wxsDialog(this,Res);
        case wxsFrameId:            return new wxsFrame(this,Res);
        case wxsPanelrId:           return new wxsPanelr(this,Res);
        case wxsRadioButtonId:      return new wxsRadioButton(this,Res);
        case wxsScrollBarId:        return new wxsScrollBar(this,Res);
        case wxsSpinButtonId:       return new wxsSpinButton(this,Res);
        case wxsSpinCtrlId:         return new wxsSpinCtrl(this,Res);
        case wxsTreeCtrlId:         return new wxsTreeCtrl(this,Res);
        case wxsRadioBoxId:         return new wxsRadioBox(this,Res);
        case wxsDatePickerCtrlId:   return new wxsDatePickerCtrl(this,Res);
        case wxsStaticLineId:       return new wxsStaticLine(this,Res);
        case wxsSplitterWindowId:   return new wxsSplitterWindow(this,Res);
        case wxsNotebookId:         return new wxsNotebook(this,Res);
        case wxsListbookId:         return new wxsListbook(this,Res);
    }
    
    return NULL;
}

/** Killing widget */
void wxsStdManagerT::KillWidget(wxsWidget* Widget)
{
    if ( Widget ) delete Widget;
}

wxsStdManagerT wxsStdManager;
