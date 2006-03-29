#include "wxsstdmanager.h"

#include "wxsframe.h"
#include "wxsdialog.h"
#include "wxspanelr.h"

#include "wxsboxsizer.h"
#include "wxsflexgridsizer.h"
#include "wxsgridsizer.h"
#include "wxsstaticboxsizer.h"
#include "wxsspacer.h"

#include "wxsbutton.h"
#include "wxscalendarctrl.h"
#include "wxscheckbox.h"
#include "wxschecklistbox.h"
#include "wxschoice.h"
#include "wxscombobox.h"
#include "wxsdatepickerctrl.h"
#include "wxsgauge.h"
#include "wxsgenericdirctrl.h"
#include "wxslistbox.h"
#include "wxslistctrl.h"
#include "wxsradiobox.h"
#include "wxsradiobutton.h"
#include "wxsscrollbar.h"
#include "wxsslider.h"
#include "wxsspinbutton.h"
#include "wxsspinctrl.h"
#include "wxsstaticbitmap.h"
#include "wxsstaticbox.h"
#include "wxsstaticline.h"
#include "wxsstatictext.h"
#include "wxstextctrl.h"
#include "wxstogglebutton.h"
#include "wxstreectrl.h"

#include <wx/xrc/xmlres.h>
#include <configmanager.h>
#include <wx/fs_zip.h>

namespace
{

// Creating array of item infos
#define ITEM(name) &(name::Info),
    static wxsItemInfo* Infos[] =
    {
        #include "list.inc"
    };
    static int InfosCnt = sizeof(Infos) / sizeof(Infos[0]);
#undef ITEM

// Creating enum type connecting item types with numbers
#define ITEM(name) name##Number,
    enum ItemNumbers
    {
        minus_one = -1,
        #include "list.inc"
    };
#undef ITEM

}

wxsStdManagerT::wxsStdManagerT()
{
}

wxsStdManagerT::~wxsStdManagerT()
{
    for ( int i=0; i<InfosCnt; i++ )
    {
        if ( Infos[i]->Icon32 ) delete Infos[i]->Icon32;
        if ( Infos[i]->Icon16 ) delete Infos[i]->Icon16;
    }
}

bool wxsStdManagerT::Initialize()
{
    wxString resPath = ConfigManager::GetDataFolder();

    for ( int i=0; i<InfosCnt; i++ )
    {
        wxString FileName32 = resPath + _T("/images/wxsmith/") + Infos[i]->Name + _T(".png");
        wxString FileName16 = resPath + _T("/images/wxsmith/") + Infos[i]->Name + _T("16.png");

        Infos[i]->Icon16 = NULL;
        Infos[i]->Icon32 = NULL;

        if ( wxFileName::FileExists(FileName16) )
        {
            wxBitmap* Bmp = new wxBitmap(FileName16,wxBITMAP_TYPE_PNG);
            if ( Bmp->Ok() )
            {
                Infos[i]->Icon16 = Bmp;
            }
            else
            {
                delete Bmp;
            }
        }

        if ( wxFileName::FileExists(FileName32) )
        {
            wxBitmap* Bmp = new wxBitmap(FileName32,wxBITMAP_TYPE_PNG);
            if ( Bmp->Ok() )
            {
                Infos[i]->Icon32 = Bmp;
            }
            else
            {
                delete Bmp;
            }
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
    return InfosCnt;
}

const wxsItemInfo& wxsStdManagerT::GetItemInfo(int Number)
{
    return *Infos[Number];
}

wxsItem* wxsStdManagerT::ProduceItem(int Number,wxsWindowRes* Res)
{
    #define ITEM(name) case name##Number: return new name(Res);
        switch ( Number )
        {
            #include "list.inc"
        }
    #undef ITEM

    return NULL;
}

wxsStdManagerT wxsStdManager;
