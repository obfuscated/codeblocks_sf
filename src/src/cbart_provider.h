#ifndef CODEBLOCKS_ART_PROVIDER_H
#define CODEBLOCKS_ART_PROVIDER_H

#include <wx/artprov.h>
#include <unordered_map>

/// Custom art provider used to make menu item icons look good on HiDPI displays.
/// Currently loads images only from resources.zip file.
/// The size is set at the beginning of execution and cannot change. The requested size is ignored.
/// Use <bitmap stock_id="core/XXX"/> in the xrc file to load images from this provider.
class cbArtProvider : public wxArtProvider
{
public:
    cbArtProvider(const wxString &prefix, int size);
protected:
    wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                          const wxSize &size) override;
private:
    wxString m_prefix;

#if wxCHECK_VERSION(3, 0, 0)
    typedef std::unordered_map<wxString, wxString> MapStockIdToPath;
#else
    struct StringHash
    {
        size_t operator()(const wxString& s) const
        {
            return std::hash<std::wstring>()(s.wc_str());
        }
    };
    typedef std::unordered_map<wxString, wxString, StringHash> MapStockIdToPath;
#endif // wxCHECK_VERSION

    MapStockIdToPath m_idToPath;
};
#endif // CODEBLOCKS_ART_PROVIDER_H
