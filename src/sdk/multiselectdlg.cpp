#include <wx/xrc/xmlres.h>
#include <wx/checklst.h>
#include <wx/stattext.h>

#include "multiselectdlg.h"

MultiSelectDlg::MultiSelectDlg(wxWindow* parent,
                                const wxArrayString& items,
                                bool selectall,
                                const wxString& label,
                                const wxString& title)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgGenericMultiSelect"));

	SetTitle(title);
	XRCCTRL(*this, "lblLabel", wxStaticText)->SetLabel(label);

    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
	for (size_t i = 0; i < items.GetCount(); ++i)
	{
        lst->Append(items[i]);
        lst->Check(i, selectall);
	}
}

MultiSelectDlg::~MultiSelectDlg()
{
	//dtor
}

wxArrayString MultiSelectDlg::GetSelectedStrings()
{
    wxArrayString ret;
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
	for (int i = 0; i < lst->GetCount(); ++i)
	{
        if (lst->IsChecked(i))
            ret.Add(lst->GetString(i));
	}
	return ret;
}

wxArrayInt MultiSelectDlg::GetSelectedIndices()
{
    wxArrayInt ret;
    wxCheckListBox* lst = XRCCTRL(*this, "lstItems", wxCheckListBox);
	for (int i = 0; i < lst->GetCount(); ++i)
	{
        if (lst->IsChecked(i))
            ret.Add(i);
	}
	return ret;
}
