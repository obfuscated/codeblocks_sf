#ifndef BASEEXPORTER_INCLUDED
#define BASEEXPORTER_INCLUDED

#include <wx/wx.h>
#include <editorcolorset.h>

class BaseExporter
{
  public:
    virtual ~BaseExporter() = 0;
    virtual void Export(const wxString &filename, const wxString &title, const wxMemoryBuffer &styled_text, const EditorColorSet *color_set) = 0;
};

#endif // BASEEXPORTER_INCLUDED
