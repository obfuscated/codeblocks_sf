#include "../wxsheaders.h"
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
#include "wxsstddialogbuttonsizer.h"
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
#include "wxsscrolledwindow.h"
#include "wxschoicebook.h"
#include "wxscalendarctrl.h"
#include "wxsgenericdirctrl.h"
#include "wxslistctrl.h"
#include "wxschoice.h"
#include "wxsstaticbox.h"
#include "wxshtmlwindow.h"
#include "wxsslider.h"
#include "wxschecklistbox.h"
#include "wxsstaticbitmap.h"
#include "wxscustomwidget.h"
#include "../wxsmith.h"


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
        _T("http://www.wxwidgets.org/manuals/2.6.2/") _T(Link),  \
        DefCategory,                                    \
        _T(#Name),                                      \
        false,                                          \
        false,                                          \
        false,                                          \
        2, 6,                                           \
        NULL,                                           \
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
        _T("http://www.wxwidgets.org/manuals/2.6.2/") _T(Link),  \
        _T(""),                                         \
        _T(""),                                         \
        false,                                          \
        false,                                          \
        false,                                          \
        2, 6,                                           \
        NULL,                                           \
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
        _T("http://www.wxwidgets.org/manuals/2.6.2/wx_wxsizer.html#wxsizeradd"),    \
        DefSizerCat,                                    \
        _T(""),                                         \
        false,                                          \
        false,                                          \
        true,                                           \
        2, 6,                                           \
        NULL,                                           \
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

#define SizerEntry2Headers(Name,Link,Header1,Header2)           \
    {   _T("wx") _T(#Name),                                     \
        DefLicence,                                             \
        DefAuthor,                                              \
        DefAuthorEmail,                                         \
        DefAuthorSite,                                          \
        _T("http://www.wxwidgets.org/manuals/2.6.2/") _T(Link), \
        DefSizerCat,                                            \
        _T(#Name),                                              \
        true,                                                   \
        true,                                                   \
        false,                                                  \
        2, 6,                                                   \
        NULL,                                                   \
        NULL,                                                   \
        &wxsStdManager,                                         \
        wxs##Name##Id,                                          \
        0,                                                      \
        NULL,                                                   \
        NULL,                                                   \
        _T(Header1),                                            \
        _T(Header2),                                            \
        wxsWidgetInfo::exNone                                   \
    },

#define SizerEntry(Name,Link,Header)                            \
    SizerEntry2Headers(Name,Link,Header,"")


#define CustomEntry()                                   \
    {   _T("Custom"),                                   \
        _("Not specified"),                             \
        _T(""),                                         \
        _T(""),                                         \
        _T(""),                                         \
        _T(""),                                         \
        DefCategory,                                    \
        _T("Custom"),                                   \
        false,                                          \
        false,                                          \
        false,                                          \
        0, 0,                                           \
        NULL,                                           \
        NULL,                                           \
        &wxsStdManager,                                 \
        wxsCustomWidgetId,                              \
        0,                                              \
        NULL,                                           \
        wxsCustomWidgetEvents,                          \
        _T(""),                                         \
        _T(""),                                         \
        wxsWidgetInfo::exNone                           \
    },


static wxsWidgetInfo StdInfos[] =
{
    { _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), false,
      false, false, 0, 0, NULL, NULL, NULL, wxsNoneId, 0, NULL, NULL, _T(""), _T(""),
      wxsWidgetInfo::exNone },  // NONE

    SizerEntry(GridSizer,"wx_wxgridsizer.html#wxgridsizer","<wx/sizer.h>")
    SizerEntry(BoxSizer,"wx_wxboxsizer.html#wxboxsizer","<wx/sizer.h>")
    SizerEntry(StaticBoxSizer,"wx_wxstaticboxsizer.html#wxstaticboxsizer","<wx/sizer.h>")
    SizerEntry(FlexGridSizer,"wx_wxflexgridsizer.html#wxflexgridsizer","<wx/sizer.h>")
    SizerEntry2Headers(StdDialogButtonSizer,"wx_wxstddialogbuttonsizer.html#wxstddialogbuttonsizer","<wx/sizer.h>","<wx/button.h>")
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
    Entry(CalendarCtrl,  "wx_wxcalendarctrl.html#wxcalendarctrl","<wx/calctrl.h>")
    Entry(StaticLine,    "wx_wxstaticline.html#wxstaticline","<wx/statline.h>")
    Entry(SplitterWindow,"wx_wxsplitterwindow.html#wxsplitterwindow","<wx/splitter.h>")
    Entry(Notebook,      "wx_wxnotebook.html#wxnotebook","<wx/notebook.h>")
    Entry(Listbook,      "wx_wxlistbook.html#wxlistbook","<wx/listbook.h>")
    Entry(Choicebook,    "wx_wxchoicebook.html#wxchoicebook","<wx/choicebk.h>")
    Entry(ScrolledWindow,"wx_wxscrolledwindow.html#wxscrolledwindow","<wx/scrolwin.h>")
    Entry(GenericDirCtrl,"wx_wxgenericdirctrl.html#wxgenericdirctrl","<wx/dirctrl.h>")
    Entry(ListCtrl,      "wx_wxlistctrl.html#wxlistctrl","<wx/listctrl.h>")
    Entry(Choice,        "wx_wxchoice.html#wxchoice","<wx/choice.h>")
    Entry(StaticBox,     "wx_wxstaticbox.html#wxstaticbox","<wx/statbox.h>")
    Entry2Headers(HtmlWindow,    "wx_wxhtmlwindow.html#wxhtmlwindow","<wx/html/htmlwin.h>","<wx/filesys.h>")
    Entry(Slider,        "wx_wxslider.html#wxslideer","<wx/slider.h>")
    Entry(CheckListBox,  "wx_wxchecklistbox.html#wxchecklistbox","<wx/checklst.h>")
    Entry2Headers(StaticBitmap,  "wx_wxstaticbitmap.html#wxstaticbitmap","<wx/statbmp.h>","<wx/image.h>")
    //Entry(StaticBitmap,  "wx_wxstaticbitmap.html#wxstaticbitmap","<wx/statbmp.h> \n <wx/image.h>")
    CustomEntry()

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
            delete StdInfos[i].Icon16;
            StdInfos[i].Icon16 = NULL;
        }
    }
}

bool wxsStdManagerT::Initialize()
{
    wxString resPath = ConfigManager::GetDataFolder();
    for ( int i=1; i<StdInfosCnt; i++ )
    {
        // Loading 32x32 image

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

        // Loading 16x16 image

        wxString FileName16 = resPath + _T("/images/wxsmith/") + StdInfos[i].Name + _T("16.png");
        wxBitmap* Bmp16 = new wxBitmap;
        if ( wxFileName::FileExists(FileName16) )
        {
            Bmp16->LoadFile(FileName16,wxBITMAP_TYPE_PNG);

            if ( Bmp16->Ok() )
            {
                StdInfos[i].Icon16 = Bmp16;
            }
            else
            {
                StdInfos[i].Icon16 = NULL;
                delete Bmp16;
            }
        }
        else
        {
            StdInfos[i].Icon16 = NULL;
        }

        // Adding image to resource tree

//        if ( StdInfos[i].Icon16 != NULL )
//        {
//            wxImageList* List = wxsTREE()->GetImageList();
//            StdInfos[i].TreeIconId = List->Add(*(StdInfos[i].Icon16));
//        }
//        else
//        {
//            StdInfos[i].TreeIconId = -1;
//        }
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
        #define ITEM(Name) case wxs##Name##Id: return new wxs##Name(this,Res);
        ITEM(GridSizer)
        ITEM(BoxSizer)
        ITEM(StaticBoxSizer)
        ITEM(FlexGridSizer)
        ITEM(StdDialogButtonSizer)
        ITEM(Button)
        ITEM(CheckBox)
        ITEM(StaticText)
        ITEM(ToggleButton)
        ITEM(ComboBox)
        ITEM(ListBox)
        ITEM(Panel)
        ITEM(TextCtrl)
        ITEM(Gauge)
        ITEM(Spacer)
        ITEM(Dialog)
        ITEM(Frame)
        ITEM(Panelr)
        ITEM(RadioButton)
        ITEM(ScrollBar)
        ITEM(SpinButton)
        ITEM(SpinCtrl)
        ITEM(TreeCtrl)
        ITEM(RadioBox)
        ITEM(DatePickerCtrl)
        ITEM(CalendarCtrl)
        ITEM(StaticLine)
        ITEM(SplitterWindow)
        ITEM(Notebook)
        ITEM(Listbook)
        ITEM(Choicebook)
        ITEM(ScrolledWindow)
        ITEM(GenericDirCtrl)
        ITEM(ListCtrl)
        ITEM(Choice)
        ITEM(StaticBox)
        ITEM(HtmlWindow)
        ITEM(Slider)
        ITEM(CheckListBox)
        ITEM(StaticBitmap)
        ITEM(CustomWidget)
        #undef ITEM
    }

    return NULL;
}

/** Killing widget */
void wxsStdManagerT::KillWidget(wxsWidget* Widget)
{
    if ( Widget ) delete Widget;
}

wxsStdManagerT wxsStdManager;
