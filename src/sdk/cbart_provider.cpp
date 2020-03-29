#include "sdk.h"

#include "cbart_provider.h"

#ifndef CB_PRECOMP
    #include "globals.h"
    #include "logmanager.h"
#endif // CB_PRECOMP

cbArtProvider::cbArtProvider(const wxString &prefix)
{
    m_prefix = prefix;
    if (!prefix.EndsWith(wxT("/")))
        m_prefix += wxT("/");
}

void cbArtProvider::AddMapping(const wxString &stockId, const wxString &fileName)
{
    m_idToPath[stockId] = Data(fileName, false);
}

void cbArtProvider::AddMappingF(const wxString &stockId, const wxString &fileName)
{
    m_idToPath[stockId] = Data(fileName, true);
}

wxBitmap cbArtProvider::DoCreateBitmap(const wxArtID& id, Manager::UIComponent uiComponent) const
{
    const MapStockIdToPath::const_iterator it = m_idToPath.find(id);
    if (it == m_idToPath.end())
        return wxNullBitmap;

    const int size = Manager::Get()->GetImageSize(uiComponent);
    const double uiScale = Manager::Get()->GetUIScaleFactor(uiComponent);

    wxString filepath = m_prefix;
    if (!it->second.hasFormatting)
        filepath += wxString::Format(wxT("%dx%d/%s"), size, size, it->second.path.wx_str());
    else
        filepath += wxString::Format(it->second.path, size, size);

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
    else if (client == wxT("wxART_TOOLBAR_C"))
        return DoCreateBitmap(id, Manager::UIComponent::Toolbars);

    return wxNullBitmap;
}
