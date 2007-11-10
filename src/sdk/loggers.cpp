#include "loggers.h"
#include <cbexception.h>
#include <wx/listctrl.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>

TextCtrlLogger::TextCtrlLogger(bool fixedPitchFont)
	: control(0), fixed(fixedPitchFont)
{
}

bool TextCtrlLogger::IsEmpty()
{
	if (control)
		return control->GetValue().IsEmpty();
	return true;
}

void TextCtrlLogger::CopyContentsToClipboard(bool selectionOnly)
{
    if (!IsEmpty() && wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(selectionOnly ? control->GetStringSelection() : control->GetValue()));
        wxTheClipboard->Close();
    }
}

void TextCtrlLogger::UpdateSettings()
{
	if (!control)
		return;
	
	control->SetBackgroundColour(*wxWHITE);

	int size = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), platform::macosx ? 10 : 8);

	wxFont default_font(size, fixed ? wxFONTFAMILY_MODERN : wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont bold_font(default_font);
	wxFont italic_font(default_font);

	bold_font.SetWeight(wxFONTWEIGHT_BOLD);

	wxFont bigger_font(bold_font);
	bigger_font.SetPointSize(size + 2);

	wxFont small_font(default_font);
	small_font.SetPointSize(size - 4);

	italic_font.SetStyle(wxFONTSTYLE_ITALIC);

	// might try alternatively
	//italic_font.SetStyle(wxFONTSTYLE_SLANT);

	for(unsigned int i = 0; i < num_levels; ++i)
	{
		style[i].SetFont(default_font);
		style[i].SetAlignment(wxTEXT_ALIGNMENT_DEFAULT);
		style[i].SetTextColour(*wxBLACK);
		style[i].SetBackgroundColour(*wxWHITE);

		// is it necessary to do that?
		//style[i].SetFlags(...);
	}

	style[caption].SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
	bigger_font.SetUnderlined(true);
	style[caption].SetFont(bigger_font);

	style[success].SetTextColour(*wxBLUE);

	style[warning].SetFont(italic_font);

	style[error].SetFont(bold_font);
	style[error].SetTextColour(*wxRED);

	style[critical].SetFont(bold_font);
	style[critical].SetTextColour(*wxWHITE);
	style[critical].SetBackgroundColour(*wxRED);
	style[spacer].SetFont(small_font);
};

void TextCtrlLogger::Append(const wxString& msg, Logger::level lv)
{
	if(!control)
		return;

	::temp_string.assign(msg);
	::temp_string.append(_T("\n"));

	if(lv == caption)
	{
		control->SetDefaultStyle(style[info]);
		control->AppendText(::newline_string);

		control->SetDefaultStyle(style[lv]);
		control->AppendText(::temp_string);

		control->SetDefaultStyle(style[spacer]);
		control->AppendText(::newline_string);
	}
	else
	{
		control->SetDefaultStyle(style[lv]);
		control->AppendText(::temp_string);
	}
};


void TextCtrlLogger::Clear()
{
	if(control)
		control->Clear();
};

wxWindow* TextCtrlLogger::CreateControl(wxWindow* parent)
{
	if (!control)
		control = new wxTextCtrl(parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_NOHIDESEL);
	return control;
};


void TimestampTextCtrlLogger::Append(const wxString& msg, Logger::level lv)
{
	if(!control)
		return;

	wxDateTime timestamp(wxDateTime::UNow());
	::temp_string.Printf(_T("[%2.2d:%2.2d:%2.2d.%3.3d] %s\n"), timestamp.GetHour(), timestamp.GetMinute(), timestamp.GetSecond(), timestamp.GetMillisecond(), msg.c_str());

	control->SetDefaultStyle(style[lv]);
	control->AppendText(::temp_string);
};

ListCtrlLogger::ListCtrlLogger(const wxArrayString& titles, const wxArrayInt& widths, bool fixedPitchFont)
	: control(0), fixed(fixedPitchFont),
	titles(titles), widths(widths)
{
	cbAssert(titles.GetCount() == widths.GetCount());
};

bool ListCtrlLogger::IsEmpty()
{
	if (control)
		return control->GetItemCount() == 0;
	return true;
}

void ListCtrlLogger::CopyContentsToClipboard(bool selectionOnly)
{
	if (!control)
		return;
	
    if (!IsEmpty() && wxTheClipboard->Open())
    {
    	wxString text;
    	if (selectionOnly)
    	{
			int i = control->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			text = GetItemAsText(i);
    	}
    	else
    	{
			for (int i = 0; i < control->GetItemCount(); ++i)
			{
				text << GetItemAsText(i);

				if (platform::windows)
					text << _T('\r'); // Add CR for Windows clipboard
				text << _T('\n');
			}
    	}
        wxTheClipboard->SetData(new wxTextDataObject(text));
        wxTheClipboard->Close();
    }
}

wxString ListCtrlLogger::GetItemAsText(long item)
{
    wxString text;

    wxListItem info;
    info.m_itemId = item;
    info.m_mask = wxLIST_MASK_TEXT;

	for (size_t i = 0; i < titles.GetCount(); ++i)
	{
		info.m_col = i;
		control->GetItem(info);
		text << info.m_text << _T('|');
	}
	return text;
}

void ListCtrlLogger::UpdateSettings()
{
	if (!control)
		return;
		
	int size = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), platform::macosx ? 10 : 8);
	wxFont default_font(size, fixed ? wxFONTFAMILY_MODERN : wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont bold_font(default_font);
	wxFont italic_font(default_font);

	bold_font.SetWeight(wxFONTWEIGHT_BOLD);

	wxFont bigger_font(bold_font);
	bigger_font.SetPointSize(size + 2);

	wxFont small_font(default_font);
	small_font.SetPointSize(size - 4);

	italic_font.SetStyle(wxFONTSTYLE_ITALIC);

	for(unsigned int i = 0; i < num_levels; ++i)
	{
		style[i].font = default_font;
		style[i].colour = *wxBLACK;
	}

	style[caption].font = bigger_font;
	style[success].colour = *wxBLUE;

	style[warning].font = italic_font;
	style[error].colour = wxColour(0x00, 0x00, 0xa0); // navy blue

	style[error].colour = *wxRED;

	style[critical].font = bold_font;
	style[critical].colour = wxColour(0x0a, 0x00, 0x00); // maroon

	style[spacer].font = small_font;
}

void ListCtrlLogger::Append(const wxString& msg, Logger::level lv)
{
	if (!control)
		return;
	
	int idx = control->GetItemCount();
	
	control->Freeze();
	control->InsertItem(idx, msg);
	control->SetItemFont(idx, style[lv].font);
	control->SetItemTextColour(idx, style[lv].colour);
	control->Thaw();
}

void ListCtrlLogger::Append(const wxArrayString& colValues, Logger::level lv)
{
	if (!control)
		return;
	
	if (colValues.GetCount() == 0 || colValues.GetCount() > titles.GetCount())
		return;

	control->Freeze();
	Append(colValues[0], lv);
	int idx = control->GetItemCount() - 1;
	for (size_t i = 1; i < colValues.GetCount(); ++i)
	{
		control->SetItem(idx, i, colValues[i]);
	}
	control->Thaw();
}

size_t ListCtrlLogger::GetItemsCount()
{
	if (control)
		return control->GetItemCount();
	return 0;
}

void ListCtrlLogger::Clear()
{
	if(control)
		control->DeleteAllItems();
};

wxWindow* ListCtrlLogger::CreateControl(wxWindow* parent)
{
	if (control)
		return control;
	
	control = new wxListCtrl(parent, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	for (size_t i = 0; i < titles.GetCount(); ++i)
	{
		control->InsertColumn(i, titles[i], wxLIST_FORMAT_LEFT, widths[i]);
	}
	return control;
};
