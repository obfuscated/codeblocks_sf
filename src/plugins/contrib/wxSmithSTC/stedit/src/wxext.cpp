///////////////////////////////////////////////////////////////////////////////
// File:        wxext.cpp
// Purpose:     wxWidgets extensions
// Author:      Troels K
// Created:     2009-11-11
// RCS-ID:
// Copyright:   (c) John Labenski, Troels K
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include <wx/stedit/stedefs.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <wx/cmdline.h>
#include <wx/clipbrd.h>

#include "wxext.h"

#ifndef WXPRECOMP
    #include <wx/settings.h>
    #include <wx/app.h>
    #include <wx/choicdlg.h>
#endif // WXPRECOMP

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(AcceleratorArray)
WX_DEFINE_OBJARRAY(FileNameArray)

const wxSize wxIconSize_System     (wxSystemSettings::GetMetric(wxSYS_ICON_X     ), wxSystemSettings::GetMetric(wxSYS_ICON_Y     ));
const wxSize wxIconSize_SystemSmall(wxSystemSettings::GetMetric(wxSYS_SMALLICON_X), wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y));
//const wxSize wxIconSize_CustomSmall(16, 16);

bool wxGetExeFolder(wxFileName* filename)
{
   filename->Assign(wxStandardPaths::Get().GetExecutablePath());
   filename->SetFullName(wxEmptyString);
   return filename->IsOk();
}

bool wxLocale_Init(wxLocale* locale, const wxString& exetitle, enum wxLanguage lang)
{
   wxFileName filename;
   wxGetExeFolder(&filename);
   filename.AppendDir(wxT("locale"));
   wxLocale::AddCatalogLookupPathPrefix(filename.GetFullPath());

   bool ok = locale->Init(lang);
   if (ok)
   {
      locale->AddCatalog(exetitle, (enum wxLanguage)locale->GetLanguage(), wxEmptyString);
   }
   return ok;
}

bool wxLocale_Find(const wxString& str, enum wxLanguage* lang)
{
   const size_t len = str.Length();
   for (int i = wxLANGUAGE_UNKNOWN + 1; i < wxLANGUAGE_USER_DEFINED; i++)
   {
      const wxLanguageInfo* info = wxLocale::GetLanguageInfo(i);
      if (   info
          && (   (0 == str.CmpNoCase(info->CanonicalName))
              || (0 == str.CmpNoCase(info->CanonicalName.Left(len)))
             )
          )
      {
         *lang = (enum wxLanguage)i;
         return true;
      }
   }
   return false;
}

bool wxLocale_GetSupportedLanguages(LanguageArray* array)
{
   wxFileName filename;
   wxGetExeFolder(&filename);
   filename.AppendDir(wxT("locale"));
   wxDir dir;
   bool ok = dir.Open(filename.GetFullPath());
   if (ok)
   {
      const enum wxLanguage default_lang = wxLANGUAGE_ENGLISH;
      wxString str;
      array->Add(default_lang);
      for (bool cont = dir.GetFirst(&str, wxEmptyString, wxDIR_DIRS);
           cont;
           cont = dir.GetNext(&str))
      {
         enum wxLanguage lang;
         if (   wxLocale_Find(str, &lang)
             && (lang != default_lang))
         {
            array->Add(lang);
         }
      }
   }
   return ok;
}

bool wxLocale_SingleChoice(const LanguageArray& array, enum wxLanguage* lang)
{
   wxArrayString as;
   for (size_t i = 0; i < array.GetCount(); i++)
   {
      enum wxLanguage temp = (enum wxLanguage)array.Item(i);
      as.Add(wxLocale::GetLanguageName(temp));
   }
   int index = wxGetSingleChoiceIndex(wxT("Language"), wxMessageBoxCaption, as);
   bool ok = (index != wxNOT_FOUND);
   if (ok)
   {
      *lang = (enum wxLanguage)array.Item(index);
   }
   return ok;
}

#if wxUSE_ACCEL

wxAcceleratorEntry wxGetStockAcceleratorEx(wxWindowID id)
{
    wxAcceleratorEntry ret;

    #define STOCKITEM(stockid, flags, keycode)      \
        case stockid:                               \
            ret.Set(flags, keycode, stockid);       \
            break;

    switch (id)
    {
        STOCKITEM(wxID_PRINT,                wxACCEL_CMD,'P')
        STOCKITEM(wxID_PREVIEW,              wxACCEL_CMD | wxACCEL_SHIFT,'P')
        STOCKITEM(wxID_SAVEAS,               wxACCEL_CMD | wxACCEL_SHIFT,'S')
        STOCKITEM(wxID_SELECTALL,            wxACCEL_CMD,'A')
        STOCKITEM(wxID_UNDO,                 wxACCEL_CMD,'Z')
        STOCKITEM(wxID_REDO,                 wxACCEL_CMD,'Y')
        //STOCKITEM(wxID_PREFERENCES,          wxACCEL_CMD,'T')
        STOCKITEM(wxID_ICONIZE_FRAME,        wxACCEL_ALT,WXK_FULLSCREEN)
        STOCKITEM(wxID_REFRESH,              wxACCEL_NORMAL, WXK_F5)
        STOCKITEM(wxID_PROPERTIES,           wxACCEL_ALT,WXK_RETURN)
        STOCKITEM(wxID_BACKWARD,             wxACCEL_ALT , WXK_LEFT)
        STOCKITEM(wxID_FORWARD,              wxACCEL_ALT , WXK_RIGHT)
        STOCKITEM(wxID_HELP,                 wxACCEL_NORMAL, WXK_HELP)
        STOCKITEM(wxID_REPLACE,              wxACCEL_CMD,'H')
        STOCKITEM(wxID_EXIT,                 wxACCEL_CMD, 'Q')
        STOCKITEM(wxID_CLOSE,                wxACCEL_CMD, 'W')
        STOCKITEM(wxID_CLEAR,                wxACCEL_NORMAL, WXK_DELETE)
        STOCKITEM(wxID_ABOUT,                wxACCEL_SHIFT, WXK_HELP)
        default:
            ret = wxGetStockAccelerator(id);
            break;
    }

    #undef STOCKITEM

#if (wxVERSION_NUMBER >= 2902)
    wxASSERT(ret.IsOk());
#else
    // trac.wxwidgets.org/ticket/12444
    // trac.wxwidgets.org/ticket/12445
#endif
    return ret;
}

void wxSetAcceleratorTable(wxWindow* wnd, const AcceleratorArray& array)
{
   const size_t count = array.GetCount();
   wxAcceleratorEntry* temp = new wxAcceleratorEntry[count];
   for (size_t i = 0; i < count; i++)
   {
      temp[i] = array.Item(i);
   }
   wxAcceleratorTable accel(count, temp);
   wnd->SetAcceleratorTable(accel);
   delete [] temp;
}

#endif // wxUSE_ACCEL

wxString wxMenuItem_GetText(const wxMenuItem* item)
{
   wxString str = item->GetItemLabel();
#ifdef __WXGTK__
   str.Replace(wxString(wxT('_')), wxString(wxT('&')));
#endif
   return str;
}

#define ACCELSTR_SEP "   "

static bool wxMenuItem_SetAccelText(wxMenuItem* item, const wxString& accel, bool append = true)
{
   wxString str = wxMenuItem_GetText(item);
   wxString ch_sep = wxT("\t");
   const int sep = str.Find(ch_sep);

   if (wxNOT_FOUND == sep)
   {
   }
   else if (append)
   {
      ch_sep = wxT(ACCELSTR_SEP);
   }
   else
   {
      str.Truncate(sep);
   }
   item->SetItemLabel(wxString::Format(wxT("%s%s%s"),
      str.wx_str(),
      ch_sep.wx_str(),
      accel.wx_str()));
   return true;
}

static wxString wxGetAccelText(int flags, int keyCode)
{
   // wxAcceleratorEntry.ToString() produces silly text
   wxString str;
#if (wxVERSION_NUMBER >= 2901)
   const wxChar sep = '+'; // the new wx default
#else
   const wxChar sep = '-'; // the old annoying wx default
#endif
#if 0 && (wxVERSION_NUMBER >= 2800)
   wxAcceleratorEntry entry(flags, keyCode);
   str = entry.ToString(); // doesn't work (wxIsalnum(WXK_F2)), silly text (WXK_NUMPAD_ADD)
#else
   if (flags & wxACCEL_CTRL)
   {
      if (str.Length()) str+=sep;
      str+=_("Ctrl");
   }
   if (flags & wxACCEL_ALT)
   {
      if (str.Length()) str+=sep;
      str+=_("Alt");
   }
   if (flags & wxACCEL_SHIFT)
   {
      if (str.Length()) str+=sep;
      str+=_("Shift");
   }
   if (str.Length()) str+=sep;
   switch (keyCode)
   {
      case WXK_INSERT         : str+=_("Insert" ); break;
      case WXK_PAGEUP         : str+=_("PgUp"   ); break;
      case WXK_PAGEDOWN       : str+=_("PgDn"   ); break;
      case WXK_HOME           : str+=_("Home"   ); break;
      case WXK_END            : str+=_("End"    ); break;
      case WXK_RETURN         : str+=_("Return" ); break;
      case WXK_DELETE         : str+=_("Del" ); break;
      case WXK_NUMPAD_ADD     : str+=_("Num+"   ); break;
      case WXK_NUMPAD_SUBTRACT: str+=_("Num-"   ); break;
      default:
         if ( (keyCode >= WXK_F1) && (keyCode <= WXK_F24) )
         {
            str+=wxString::Format(wxT("F%d"), keyCode - WXK_F1 + 1);
         }
         else
         {
            wxASSERT(keyCode >= ' ');
            str+=(wxChar)keyCode;
         }
         break;
   }
#endif
   return str;
}

wxString wxGetStockLabelEx(wxWindowID id, long flags)
{
#define STOCKITEM(stockid, label) \
     case stockid:                 \
         stockLabel = label;       \
         break;

   wxString stockLabel;
   switch (id)
   {
      STOCKITEM(wxID_ABOUT,     _("&About..."))   // + ellipsis
      STOCKITEM(wxID_OPEN,      _("&Open..."))    // + ellipsis
      STOCKITEM(wxID_SELECTALL, _("Select &All")) // + ampersand
      STOCKITEM(wxID_FIND,      _("&Find..."))    // + ellipsis
      STOCKITEM(wxID_REPLACE,   _("Rep&lace...")) // + ellipsis
      STOCKITEM(wxID_REVERT,    _("Re&vert..."))
      STOCKITEM(wxID_PROPERTIES, _("Proper&ties...")) // + ellipsis
      default:
         break;
   }
#undef STOCKITEM
   if (stockLabel.Length())
   {
       if ( !(flags & wxSTOCK_WITH_MNEMONIC) )
       {
           stockLabel = wxStripMenuCodes(stockLabel);
       }
#if (wxVERSION_NUMBER >= 2901)
       if (flags & wxSTOCK_WITHOUT_ELLIPSIS)
       {
           wxString baseLabel;
           if ( stockLabel.EndsWith(wxT("..."), &baseLabel) )
               stockLabel = baseLabel;
       }
#else
       // handled below
#endif
   }
   else
   {
      stockLabel = wxGetStockLabel(id, flags);
   }
#if (wxVERSION_NUMBER < 2901)
   if (flags & wxSTOCK_WITHOUT_ELLIPSIS)
   {
      wxString baseLabel;
      if ( stockLabel.EndsWith(wxT("..."), &baseLabel) )
         stockLabel = baseLabel;
   }
#endif
   return stockLabel;
}

static wxString wxGetAccelText(const wxAcceleratorEntry& accel)
{
   return wxGetAccelText(accel.GetFlags(), (enum wxKeyCode)accel.GetKeyCode());
}

void wxMenu_SetAccelText(wxMenuBar* menubar, const AcceleratorArray& accel)
{
   size_t count = menubar->GetMenuCount();
   for (size_t j = 0; j < count; j++)
   {
      wxMenu* menu = menubar->GetMenu(j);
      wxMenu_SetAccelText(menu, accel);
   }
}

wxString wxToolBarTool_MakeShortHelp(const wxString& rstr, const AcceleratorArray& accel, int id)
{
   wxString str = rstr;
   if (accel.GetCount() && str.Length())
   {
      wxString strAccel;

      for (size_t i = 0; i < accel.GetCount(); i++)
      {
         const wxAcceleratorEntry& element = accel.Item(i);
         if (element.GetCommand() == id)
         {
            if (strAccel.Length()) strAccel+=wxT(ACCELSTR_SEP);
            strAccel+=wxGetAccelText(element);
         }
      }
      if (strAccel.Length())
      {
         str+=wxString::Format(wxT(" (%s)"), strAccel.wx_str());
      }
   }
   return str;
}

static bool wxMenuItem_SetAccelText(wxMenuItem* item, const wxAcceleratorEntry& entry)
{
   return wxMenuItem_SetAccelText(item, wxGetAccelText(entry));
}

static void wxMenu_SetAccelText(wxMenu* menu, const wxAcceleratorEntry& accel)
{
   wxMenuItemList& list = menu->GetMenuItems();

   for (wxMenuItemList::iterator it = list.begin();
        it != list.end();
        it++)
   {
      wxMenuItem* item = *it;
      if (item->IsSubMenu())
      {
         wxMenu_SetAccelText(item->GetSubMenu(), accel);
      }
      else if (item->GetId() == accel.GetCommand())
      {
         wxMenuItem_SetAccelText(item, accel);
      }
   }
}

void wxMenu_SetAccelText(wxMenu* menu, const AcceleratorArray& array)
{
   for (size_t i = 0; i < array.GetCount(); i++)
   {
      const wxAcceleratorEntry& accel = array.Item(i);
      wxMenu_SetAccelText(menu, accel);
   }
}

/////////////////////////////////////////////////////////////////////////////
// wxPreviewFrameEx

wxPreviewFrameEx::wxPreviewFrameEx(wxPrintPreviewBase* preview,
                wxWindow *parent,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name) 
   : wxPreviewFrame(preview, parent, title, pos, size, style, name)
{
}

BEGIN_EVENT_TABLE(wxPreviewFrameEx, wxPreviewFrame)
#if (wxVERSION_NUMBER < 2900)
    EVT_CHAR_HOOK(wxPreviewFrameEx::OnKeyDown)
#endif
END_EVENT_TABLE()

bool wxPreviewFrameEx::Destroy()
{
   bool ok = base::Destroy();
   if (ok && GetParent())
   {
      GetParent()->Raise();
   }
   return ok;
}

#if (wxVERSION_NUMBER < 2900)
// trac.wxwidgets.org/ticket/8570
void wxPreviewFrameEx::OnKeyDown(wxKeyEvent& event)
{
   switch (event.GetKeyCode())
   {
      case WXK_ESCAPE:
         Close();
         break;
      default:
         event.Skip();
         break;
   }
}
#endif

void wxCommandLineUsage(wxWindow* parent)
{
    wxCmdLineParser parser;
    wxTheApp->OnInitCmdLine(parser);
#if (wxVERSION_NUMBER >= 2900)
    // GetUsageString() is public
    wxMessageBox(parser.GetUsageString(), wxTheApp->GetAppDisplayName(), wxOK | wxICON_INFORMATION, parent);
#elif defined(__WXMSW__)
    // GetUsageString() is private, sigh
    parser.Usage();
    wxUnusedVar(parent);
#else
   // Usage() goes to console, sigh
    wxUnusedVar(parent);
#endif
}

void wxFrame_SetInitialPosition(wxFrame* wnd, const wxPoint& pos, const wxSize& size, int margin_pct)
{
   if (size == wxDefaultSize)
   {
      wxRect rect = wxGetClientDisplayRect();
      wxSize size(
         (rect.width  * (100 - margin_pct*2))/100, 
         (rect.height * (100 - margin_pct*2))/100);
         
      wnd->SetSize(size);
   }
   if (pos == wxDefaultPosition)
   {
      wnd->Center();
   }
}

void wxFrame_ClonePosition(wxFrame* wnd, wxWindow* otherwindow /*= NULL*/)
{
   otherwindow = otherwindow ? wxGetTopLevelParent(otherwindow) : wxTheApp->GetTopWindow();
   wxFrame* topframe = wxStaticCast(otherwindow, wxFrame);
   if (topframe->IsMaximized())
   {
      wnd->Maximize();
   }
   else if (topframe->IsFullScreen())
   {
      wnd->Maximize();
   }
   else
   {
      wxRect rect = topframe->GetScreenRect();
      wnd->SetSize(rect);
   }
}

#if wxUSE_CLIPBOARD
bool wxClipboard_IsAvailable(const enum wxDataFormatId* array, size_t array_count)
{
   wxClipboard* clipboard = wxTheClipboard;
   bool open = clipboard->IsOpened();
   bool ok = open || clipboard->Open();
   if (ok)
   {
      size_t i;
      for (i = 0; i < array_count; i++)
      {
         if (clipboard->IsSupported(wxDataFormat(array[i]))) break;
      }
      ok = (i != array_count);
   }
   if (!open) clipboard->Close();
   return ok;
}
#endif

#if wxUSE_DATAOBJ && wxUSE_CLIPBOARD
bool wxClipboard_Get(wxString* str)
{
    bool ok = false;
#if wxUSE_DATAOBJ
    wxClipboard* clipboard = wxTheClipboard;
    ok = clipboard && clipboard->Open();
    if (ok)
    {
        wxTextDataObject temp;
        clipboard->UsePrimarySelection(false);

        ok = clipboard->GetData(temp);
        if (ok && str)
        {
            str->operator=(temp.GetText());
        }
        clipboard->Close();
    }
#endif
    return ok;
}
#endif
