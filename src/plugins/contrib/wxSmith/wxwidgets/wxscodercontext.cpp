#include "wxscodercontext.h"
#include "wxsflags.h"

using namespace wxsFlags;

void wxsCoderContext::AddHeader(const wxString& Header,const wxString& DeclaredClass,long HeaderFlags)
{
    // Cache some flags
    bool IsLocal   = (HeaderFlags&hfLocal)     != 0;
    bool InPch     = (HeaderFlags&hfInPCH)     != 0;
    bool ForbidFwd = (HeaderFlags&hfForbidFwd) != 0;
    bool AddFwd    = false;

    // Skip some headers when using XRC file
    if ( !(m_Flags & flSource) )
    {
        if ( IsLocal ) return;          // Do not need any local includes because they're used only inside manually building code
        if ( m_Flags&flLocal ) return;  // Skip any headers for items which are not members (they're not used at all)
    }

    // Check if we can use forward declatarion
    if ( (m_Flags&flFwdDeclar) &&
         (m_Flags&flPointer) &&
         !ForbidFwd &&
         !IsLocal &&
         !(m_Flags&flRoot) )
    {
        // We put header in local cpp file when using forward declaration
        IsLocal = true;
        AddFwd = true;
    }

    // Test if we're using PCH
    if ( !(m_Flags & flPchFilter) )
    {
        // If we don't use Pch filter, all headers are added to NonPCH array
        InPch = false;
    }

    // Add to proper array
    wxStringSet& Global       = m_GlobalHeaders;
    wxStringSet& GlobalNonPCH = m_GlobalHeadersNonPCH;
    wxStringSet& Local        = m_LocalHeaders;
    wxStringSet& LocalNonPCH  = m_LocalHeadersNonPCH;
    wxStringSet& FwdDcl       = m_ForwardDeclarations;
    wxStringSet& FwdDclNonPCH = m_ForwardDeclarationsNonPCH;


    if ( IsLocal )
    {
        if ( Global.find(Header)==Global.end() && GlobalNonPCH.find(Header)==GlobalNonPCH.end() )
        {
            // Add header to local include set
            ( InPch ? Local : LocalNonPCH ) . insert(Header);

            // Add foward declaration
            if ( AddFwd )
            {
                ( InPch ? FwdDcl : FwdDclNonPCH ) . insert(DeclaredClass);
            }
        }
    }
    else
    {
        if ( !InPch )
        {
            // Erase if it's included in in-pch list
            Global.erase(Header);

            // Add to non-pch list
            GlobalNonPCH.insert(Header);
        }
        else
        {
            // Add only when header is not in non-pch list yet
            if ( GlobalNonPCH.find(Header) == GlobalNonPCH.end() )
            {
                Global.insert(Header);
            }
        }

        // Erase includes if they were added locally
        Local.erase(Header);
        LocalNonPCH.erase(Header);

        // Erase those classes from forward declarations since we will include files declaring them globally
        FwdDcl.erase(DeclaredClass);
        FwdDclNonPCH.erase(DeclaredClass);
    }
}

void wxsCoderContext::AddDeclaration(const wxString& Declaration)
{
    // Do not add declaration if item is root item
    if ( m_Flags & flRoot ) return;

    // Skip some declarations when using XRC file
    if ( !(m_Flags & flSource) )
    {
        if ( m_Flags & flLocal ) return;    // Skip any headers for items which are not members (they're not used at all)
    }

    if ( m_Flags & flLocal )
    {
        m_LocalDeclarations.insert(Declaration);
    }
    else
    {
        m_GlobalDeclarations.insert(Declaration);
    }
}

void wxsCoderContext::AddXRCFetchingCode(const wxString& Code)
{
    if ( m_Flags & flRoot  ) return;
    if ( m_Flags & flLocal ) return;

    if ( m_Flags & flMixed )
    {
        m_XRCFetchingCode.Append(Code);
    }
}

void wxsCoderContext::AddBuildingCode(const wxString& Code)
{
    if ( m_Flags & flSource )
    {
        m_BuildingCode.Append(Code);
    }
}

void wxsCoderContext::AddEventCode(const wxString& Code)
{
    m_EventsConnectingCode.Append(Code);
}

void wxsCoderContext::AddIdCode(const wxString& Enumeration,const wxString& Initialization)
{
    if ( m_Flags & flRoot ) return;

    if ( m_Flags & flSource )
    {
        if ( m_IdEnumerations.Index(Enumeration) == wxNOT_FOUND )
        {
            m_IdEnumerations.Add(Enumeration);
            m_IdInitializions.Add(Initialization);
        }
    }
}

wxString wxsCoderContext::GetUniqueName(const wxString& Prefix)
{
    wxString& MapEntry = m_Extra[_T("__Counter_of_") + Prefix];
    MapEntry.Append(_T('*'));
    return wxString::Format(_T("%s_%d"),Prefix.c_str(),(int)MapEntry.Length());
}
