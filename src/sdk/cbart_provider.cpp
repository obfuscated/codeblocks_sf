#include "sdk.h"

#include "cbart_provider.h"

#ifndef CB_PRECOMP
    #include "globals.h"
    #include "logmanager.h"
#endif // CB_PRECOMP

cbArtProvider::cbArtProvider(const wxString &prefix)
{
    m_prefix = prefix;
}

void cbArtProvider::AddMapping(const wxString &stockId, const wxString &fileName)
{
    m_idToPath[stockId] = fileName;

wxBitmap cbArtProvider::DoCreateBitmap(const wxArtID& id, Manager::UIComponent uiComponent) const
{
    const MapStockIdToPath::const_iterator it = m_idToPath.find(id);
    if (it == m_idToPath.end())
        return wxNullBitmap;

    const int size = Manager::Get()->GetImageSize(uiComponent);
    const double uiScale = Manager::Get()->GetUIScaleFactor(uiComponent);

    const wxString filepath = wxString::Format(wxT("%s/%dx%d/%s"), m_prefix.wx_str(), size,
                                               size, it->second.wx_str());
    wxBitmap result = cbLoadBitmapScaled(filepath, wxBITMAP_TYPE_PNG, uiScale);
    if (!result.IsOk())
    {
        const wxString msg = wxString::Format(wxT("cbArtProvider: Cannot load image '%s'"),
                                              filepath.wx_str());
        Manager::Get()->GetLogManager()->LogError(msg);
    }

    return result;
}

wxBitmap cbArtProvider::CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                     cb_unused const wxSize &size)
{
    if (client == wxT("wxART_MENU_C"))
        return DoCreateBitmap(id, Manager::UIComponent::Menus);
    else if (client == wxT("wxART_BUTTON_C"))
        return DoCreateBitmap(id, Manager::UIComponent::Main);

    return wxNullBitmap;
}
