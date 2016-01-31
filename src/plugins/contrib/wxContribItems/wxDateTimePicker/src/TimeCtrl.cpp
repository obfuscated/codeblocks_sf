#include "TimeCtrl.h"

#if wxUSE_DATEPICKCTRL
#include <wx/datectrl.h>

#include <wx/msw/wrapwin.h>
#include <wx/msw/wrapcctl.h>
#include <wx/app.h>
#include <wx/intl.h>
#include <wx/dcclient.h>
#include <wx/msw/private.h>

#include <wx/datectrl.h>
#include <wx/dynlib.h>


#define _WX_DEFINE_TIME_EVENTS_
#include <wx/dateevt.h>

#ifndef DateTime_GetSystemtime
    #define DateTime_GetSystemtime DateTime_GetSystemTime
#endif

#ifndef DateTime_SetSystemtime
    #define DateTime_SetSystemtime DateTime_SetSystemTime
#endif

static inline void wxFromSystemTime( wxDateTime *dt, const SYSTEMTIME& st)
{
    dt->Set( st.wDay,
            wx_static_cast( wxDateTime::Month, wxDateTime::Jan + st.wMonth - 1),
            st.wYear,
            st.wHour,st.wMinute,st.wSecond);
}

static inline void wxToSystemTime( SYSTEMTIME* st, const wxDateTime& dt)
{

    const wxDateTime::Tm tm( dt.GetTm());

    st->wYear  = (WXWORD)tm.year;
    st->wMonth = (WXWORD)(tm.mon - wxDateTime::Jan +1);
    st->wDay = tm.mday;

    st->wDayOfWeek = tm.mday;
    st->wHour = tm.hour;
    st->wMinute = tm.min;
    st->wSecond = tm.sec;
    st->wMilliseconds = tm.msec;
}

wxTimePickerCtrl::wxTimePickerCtrl(void)
{

}

wxTimePickerCtrl::wxTimePickerCtrl(wxWindow *parent, wxWindowID id, const wxDateTime& dt, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
{
    Create( parent, id, dt, pos, size, style, validator, name);
}

bool wxTimePickerCtrl::Create( wxWindow *parent, wxWindowID id, const wxDateTime& dt, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
{

    // initialize the base class
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    // create the native control
    if ( !MSWCreateControl(DATETIMEPICK_CLASS, wxEmptyString, pos, size) )
        return false;

    if ( dt.IsValid() || (style & wxDP_ALLOWNONE) )
        SetValue(dt);
    else
        SetValue(wxDateTime::Today());

    return true;
}

WXDWORD wxTimePickerCtrl::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD styleMSW = wxDatePickerCtrlBase::MSWGetStyle(style, exstyle);

    styleMSW |= DTS_TIMEFORMAT;

    if ( style & wxDP_ALLOWNONE )
        styleMSW |= DTS_SHOWNONE;

    return styleMSW;
}

// TODO: handle WM_WININICHANGE

// ----------------------------------------------------------------------------
// wxTimePickerCtrl geometry
// ----------------------------------------------------------------------------

wxSize wxTimePickerCtrl::DoGetBestSize() const
{
    wxClientDC dc(wx_const_cast(wxTimePickerCtrl *, this));
    dc.SetFont(GetFont());

    // we can't use FormatDate() here as the CRT doesn't always use the same
    // format as the time picker control
    wxString s;
    for ( int len = 100; ; len *= 2 )
    {
        if ( ::GetTimeFormat
               (
                    LOCALE_USER_DEFAULT,    // the control should use the same
                    0,         // the format used by the control
                    NULL,                   // use current time (we don't care)
                    NULL,                   // no custom format
                    wxStringBuffer(s, len), // output buffer
                    len                     // and its length
               ) )
        {
            // success
            break;
        }

        const DWORD rc = ::GetLastError();
        if ( rc != ERROR_INSUFFICIENT_BUFFER )
        {
            wxLogApiError(_T("GetTimeFormat"), rc);

            // fall back on wxDateTime, what else to do?
            s = wxDateTime::Now().Format();
            break;
        }
    }

    // the control adds a lot of extra space around separators
    s.Replace(_T(","), _T("    ,    "));

    int x, y;
    dc.GetTextExtent(s, &x, &y);

    wxSize best(x + 40 /* margin + arrows */, EDIT_HEIGHT_FROM_CHAR_HEIGHT(y));
    CacheBestSize(best);
    return best;
}

// ----------------------------------------------------------------------------
// wxTimePickerCtrl operations
// ----------------------------------------------------------------------------

void wxTimePickerCtrl::SetValue(const wxDateTime& dt)
{
    wxCHECK_RET( dt.IsValid() || HasFlag(wxDP_ALLOWNONE),
                    _T("this control requires a valid time") );

    SYSTEMTIME st;
    if ( dt.IsValid() )
        wxToSystemTime(&st, dt);
    if ( !DateTime_SetSystemtime(GetHwnd(),
                                 dt.IsValid() ? GDT_VALID : GDT_NONE,
                                 &st) )
    {
        wxLogDebug(_T("DateTime_SetSystemtime() failed"));
    }

    // we need to keep only the date part, times don't make sense for this
    // control (in particular, comparisons with other dates would fail)
    m_date = dt;
    if ( m_date.IsValid() )
        m_date.ResetTime();
}

wxDateTime wxTimePickerCtrl::GetValue() const
{
#ifdef __WXDEBUG__
    wxDateTime dt;
    SYSTEMTIME st;
    if ( DateTime_GetSystemtime(GetHwnd(), &st) == GDT_VALID )
    {
        wxFromSystemTime(&dt, st);
    }

    wxASSERT_MSG( m_date.IsValid() == dt.IsValid() &&
                    (!dt.IsValid() || dt == m_date),
                  _T("bug in wxTimePickerCtrl: m_date not in sync") );
#endif // __WXDEBUG__

    return m_date;
}

void wxTimePickerCtrl::SetRange(const wxDateTime& dt1, const wxDateTime& dt2)
{
    SYSTEMTIME st[2];

    DWORD flags = 0;
    if ( dt1.IsValid() )
    {
        wxToSystemTime(&st[0], dt1);
        flags |= GDTR_MIN;
    }

    if ( dt2.IsValid() )
    {
        wxToSystemTime(&st[1], dt2);
        flags |= GDTR_MAX;
    }

    if ( !DateTime_SetRange(GetHwnd(), flags, st) )
    {
        wxLogDebug(_T("DateTime_SetRange() failed"));
    }
}

bool wxTimePickerCtrl::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    SYSTEMTIME st[2];

    DWORD flags = DateTime_GetRange(GetHwnd(), st);
    if ( dt1 )
    {
        if ( flags & GDTR_MIN )
            wxFromSystemTime(dt1, st[0]);
        else
            *dt1 = wxDateTime::Now();
    }

    if ( dt2 )
    {
        if ( flags & GDTR_MAX )
            wxFromSystemTime(dt2, st[1]);
        else
            *dt2 = wxDateTime::Now();
    }

    return flags != 0;
}

// ----------------------------------------------------------------------------
// wxTimePickerCtrl events
// ----------------------------------------------------------------------------

bool
wxTimePickerCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHDR* hdr = (NMHDR *)lParam;
    switch ( hdr->code )
    {
        case DTN_DATETIMECHANGE:
        {
            NMDATETIMECHANGE *dtch = (NMDATETIMECHANGE *)hdr;
            wxDateTime dt;
            if ( dtch->dwFlags == GDT_VALID )
                wxFromSystemTime(&dt, dtch->st);

            // filter out duplicate DTN_DATETIMECHANGE events which the native
            // control sends us when using wxDP_DROPDOWN style
            if ( (m_date.IsValid() != dt.IsValid()) ||
                    (m_date.IsValid() && dt != m_date) )
            {
                m_date = dt;
                wxDateEvent event(this, dt, wxEVT_DATE_CHANGED);
                if ( GetEventHandler()->ProcessEvent(event) )
                {
                    *result = 0;
                    return true;
                }
            }
            //else: both the old and new values are invalid, nothing changed
        }
        default:
            break;
    }

    return wxDatePickerCtrlBase::MSWOnNotify(idCtrl, lParam, result);
}

#endif // wxUSE_DATEPICKCTRL

