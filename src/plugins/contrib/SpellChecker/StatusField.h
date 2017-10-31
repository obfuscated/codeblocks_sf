#ifndef STATUSFIELD_H
#define STATUSFIELD_H

#include <wx/panel.h>


class wxStaticBitmap;
class wxStaticText;
class SpellCheckerConfig;
class SpellCheckerPlugin;

class SpellCheckerStatusField : public wxPanel
{
public:
    SpellCheckerStatusField(wxWindow* parent, SpellCheckerPlugin *plugin, SpellCheckerConfig *sccfg);
    virtual ~SpellCheckerStatusField();

    //void SetLanguage(const wxString &lang);
    void Update();
private:
    wxStaticBitmap *m_bitmap;
    wxStaticText   *m_text;
    SpellCheckerConfig *m_sccfg;
    SpellCheckerPlugin *m_plugin;

private:
    void OnSize(wxSizeEvent &event);
    void OnRightUp(wxMouseEvent &event);
    void OnSelect(wxCommandEvent &event);
    void OnEditPersonalDictionary(wxCommandEvent &event);

    void DoSize();
};

#endif // STATUSFIELD_H
