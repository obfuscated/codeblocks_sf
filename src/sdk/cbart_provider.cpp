#include "sdk.h"

#include "cbart_provider.h"

#ifndef CB_PRECOMP
    #include "globals.h"
    #include "logmanager.h"
#endif // CB_PRECOMP

cbArtProvider::cbArtProvider(const wxString &prefix, int size)
{
    m_prefix = wxString::Format(wxT("%s/%dx%d/"), prefix.wx_str(), size, size);
}

void cbArtProvider::AddMapping(const wxString &stockId, const wxString &fileName)
{
    m_idToPath[stockId] = fileName;
}

wxBitmap cbArtProvider::CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                     const wxSize &size)
{
    if (client != wxT("wxART_MENU_C"))
        return wxNullBitmap;

    MapStockIdToPath::const_iterator it = m_idToPath.find(id);
    if (it == m_idToPath.end())
    {
        wxString msg = wxString::Format(wxT("cbArtProvider: Unknown id '%s'"), id.wx_str());
        Manager::Get()->GetLogManager()->LogError(msg);
        return wxNullBitmap;
    }

    const wxString filepath = m_prefix + it->second;
    wxBitmap result = cbLoadBitmap(filepath);
    if (!result.IsOk())
    {
        wxString msg = wxString::Format(wxT("cbArtProvider: Cannot load image '%s'"),
                                        filepath.wx_str());
        Manager::Get()->GetLogManager()->LogError(msg);
    }

    return result;
}
