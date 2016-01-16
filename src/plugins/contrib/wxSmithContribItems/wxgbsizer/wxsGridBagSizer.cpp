/* wxGridBagSizer implementation for wxSmith
 * Created by Stéphane Coquelin (sun.wu.kong@free.fr)
 *
 * Most of this file consist on copy / paste and adapt to fit to wxGridBagSizer.
 * I have used wxssizer.cpp and wxsflexgridsizer.cpp to create this file.
 *
 * Known problems :
 *    - two items whith the same position in the sizer
 *    - only test on windows
 *    - I could have done a better work but I'm not the best in C/C++
 *    - warnings while compiling the xpm files
 **************************************************************************************************/
#include "wxsGridBagSizer.h"
#include "images/wxGridBagSizer16.xpm"
#include "images/wxGridBagSizer32.xpm"

namespace
{
	class wxsSizerPreview: public wxPanel
	{
		public:
			wxsSizerPreview(wxWindow* Parent): wxPanel(Parent,-1,wxDefaultPosition,wxDefaultSize, wxTAB_TRAVERSAL)
			{
				InheritAttributes();
				Connect(wxID_ANY,wxEVT_PAINT,(wxObjectEventFunction)&wxsSizerPreview::OnPaint);
			}

		private:
			void OnPaint(wxPaintEvent& event)
			{
				// Drawing additional border around te panel
				wxPaintDC DC(this);
				int W, H;
				GetSize(&W,&H);
				DC.SetBrush(*wxTRANSPARENT_BRUSH);
				DC.SetPen(*wxRED_PEN);
				DC.DrawRectangle(0,0,W,H);
			}
	};

	wxArrayInt GetArray(const wxString& String,bool* Valid = 0)
	{
		wxStringTokenizer Tokens(String,_T(","));
		wxArrayInt Array;
		if ( Valid )
		{
			*Valid = true;
		}

		while ( Tokens.HasMoreTokens() )
		{
			long Value;
			wxString Token = Tokens.GetNextToken();
			Token.Trim(true);
			Token.Trim(false);
			if ( !Token.ToLong(&Value) && Valid )
			{
				*Valid = false;
			}
			Array.Add((int)Value);
		}

		return Array;
	}

	bool FixupList(wxString& List)
	{
		bool Ret;
		wxArrayInt Array = GetArray(List,&Ret);
		List.Clear();
		for ( size_t i=0; i<Array.Count(); i++ )
		{
			List.Append(wxString::Format(_T("%d"),Array[i]));
			if ( i < Array.Count() - 1 )
			{
				List.Append(_T(','));
			}
		}
		return Ret;
	}

	wxsRegisterItem<wxsGridBagSizer> Reg(
		_T("wxGridBagSizer"),
		wxsTSizer, _T("wxWindows"), _T("Sun Wu Kong"),
		_T("sun.wu.kong@free.fr"), _T("http://sun.wu.kong.free.fr"),
		_T("Layout"), 80, _T("GridBagSizer"),
		wxsCPP, 1, 0,
		wxBitmap(wxGridBagSizer32_xpm), wxBitmap(wxGridBagSizer16_xpm),
		false);
}





wxsGridBagSizerExtra::wxsGridBagSizerExtra():wxsSizerExtra(), colspan(1), rowspan(1), col(-1), row(-1)
{
	ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));
	rowspan = cfg->ReadInt(_T("/defsizer/rowspan"), rowspan);
	colspan = cfg->ReadInt(_T("/defsizer/colspan"), colspan);
	col = cfg->ReadInt(_T("/defsizer/col"), col);
	row = cfg->ReadInt(_T("/defsizer/row"), row);
}
void wxsGridBagSizerExtra::OnEnumProperties(long Flags)
{
	static const int Priority = 100;
	WXS_LONG_P(wxsGridBagSizerExtra, colspan, _("Colspan"), _T("colspan"), 1, Priority);
	WXS_LONG_P(wxsGridBagSizerExtra, rowspan, _("Rowspan"), _T("rowspan"), 1, Priority);
	WXS_LONG_P(wxsGridBagSizerExtra, col, _("Col"), _T("col"), -1, Priority);
	WXS_LONG_P(wxsGridBagSizerExtra, row, _("Row"), _T("row"), -1, Priority);
	wxsSizerExtra::OnEnumProperties(Flags);
}
wxString wxsGridBagSizerExtra::AllParamsCode(wxsCoderContext* Ctx)
{
	switch ( Ctx->m_Language )
	{
		case wxsCPP:
		{
			wxString str;
			str = wxString::Format(_("wxGBPosition(%ld, %ld), "), row, col);
			if (rowspan == 1 && colspan == 1)
				str += _("wxDefaultSpan, ");
			else
				str += wxString::Format(_("wxGBSpan(%ld, %ld), "), rowspan, colspan);
			str += wxsSizerFlagsProperty::GetString(Flags) + _T(", ") << Border.GetPixelsCode(Ctx);
			return str;
		}
		default:
			wxsCodeMarks::Unknown(_T("wxsGridBagSizerExtra::AllParamsCode"),Ctx->m_Language);
	}
	return wxEmptyString;
}







wxsGridBagSizer::wxsGridBagSizer(wxsItemResData* Data):wxsSizer(Data,&Reg.Info)
{
}
wxsPropertyContainer* wxsGridBagSizer::OnBuildExtra()
{
	return new wxsGridBagSizerExtra();
}
void wxsGridBagSizer::OnEnumSizerProperties(long Flags)
{
	FixupList(GrowableCols);
	FixupList(GrowableRows);
	WXS_DIMENSION(wxsGridBagSizer,VGap,_("V-Gap"),_("V-Gap in dialog units"),_T("vgap"),0,false);
	WXS_DIMENSION(wxsGridBagSizer,HGap,_("H-Gap"),_("H,y-Gap in dialog units"),_T("hgap"),0,false);
	WXS_SHORT_STRING(wxsGridBagSizer,GrowableCols,_("Growable cols"),_T("growablecols"),_T(""),false);
	WXS_SHORT_STRING(wxsGridBagSizer,GrowableRows,_("Growable rows"),_T("growablerows"),_T(""),false);
	FixupList(GrowableCols);
	FixupList(GrowableRows);
}
wxGridBagSizer* wxsGridBagSizer::OnBuildSizerPreview(wxWindow* Parent)
{
	wxGridBagSizer* Sizer = new wxGridBagSizer(VGap.GetPixels(Parent),HGap.GetPixels(Parent));

	wxArrayInt Cols = GetArray(GrowableCols);
	for ( size_t i=0; i<Cols.Count(); i++ )
	{
		Sizer->AddGrowableCol(Cols[i]);
	}

	wxArrayInt Rows = GetArray(GrowableRows);
	for ( size_t i=0; i<Rows.Count(); i++ )
	{
		Sizer->AddGrowableRow(Rows[i]);
	}
	return Sizer;
}
wxObject* wxsGridBagSizer::OnBuildPreview(wxWindow* Parent,long Flags)
{
	wxWindow* NewParent = Parent;

	if ( !(Flags & pfExact) )
	{
		NewParent = new wxsSizerPreview(Parent);
	}

	wxGridBagSizer* Sizer = OnBuildSizerPreview(NewParent);
	int Count = GetChildCount();
	for ( int i=0; i<Count; i++ )
	{
		wxsItem* Child = GetChild(i);
		wxsGridBagSizerExtra* Extra = (wxsGridBagSizerExtra*)GetChildExtra(i);

		// Reset span in case of wrong values
		if (Extra->colspan < 1) Extra->colspan = 1;
		if (Extra->rowspan < 1) Extra->rowspan = 1;

		// How many rows / cols ?
		int nbRows = 0;
		int nbCols = 0;
		for (int j = 0 ; j < Count ; j++)
		{
			wxsGridBagSizerExtra* ExtraRows = (wxsGridBagSizerExtra*)GetChildExtra(j);

			if (nbRows < ExtraRows->row + ExtraRows->rowspan)
				nbRows = ExtraRows->row + ExtraRows->rowspan;

			if (nbCols < ExtraRows->col + ExtraRows->colspan)
				nbCols = ExtraRows->col + ExtraRows->colspan;
		}

		// Set the position in the sizer in case of wrong values
		if (Extra->col < 0 && Extra->row < 0)
		{
			Extra->col = 0;
			Extra->row = nbRows;
		}
		if (Extra->col < 0) Extra->col = nbCols;
		if (Extra->row < 0) Extra->row = nbRows;

		// We pass either Parent passed to current BuildPreview function
		// or pointer to additional parent currently created
		wxObject* ChildPreview = Child->BuildPreview(NewParent,Flags);
		if ( !ChildPreview ) continue;

		wxSizer* ChildAsSizer = wxDynamicCast(ChildPreview,wxSizer);
		wxWindow* ChildAsWindow = wxDynamicCast(ChildPreview,wxWindow);
		wxGBSizerItem* ChildAsItem = wxDynamicCast(ChildPreview,wxGBSizerItem);
		if ( ChildAsSizer )
		{
			Sizer->Add(ChildAsSizer,
			wxGBPosition(Extra->row, Extra->col), wxGBSpan(Extra->rowspan, Extra->colspan),
			wxsSizerFlagsProperty::GetWxFlags(Extra->Flags),
			Extra->Border.GetPixels(Parent));
		}
		else if ( ChildAsWindow )
		{
			Sizer->Add(ChildAsWindow,
			wxGBPosition(Extra->row, Extra->col), wxGBSpan(Extra->rowspan, Extra->colspan),
			wxsSizerFlagsProperty::GetWxFlags(Extra->Flags),
			Extra->Border.GetPixels(Parent));
		}
		else if ( ChildAsItem )
		{
			ChildAsItem->SetProportion(Extra->Proportion);
			ChildAsItem->SetFlag(wxsSizerFlagsProperty::GetWxFlags(Extra->Flags));
			ChildAsItem->SetBorder(Extra->Border.GetPixels(Parent));
			ChildAsItem->SetSpan(wxGBSpan(Extra->rowspan, Extra->colspan));
			ChildAsItem->SetPos(wxGBPosition(Extra->row, Extra->col));
			Sizer->Add(ChildAsItem);
		}
	}

	if ( !(Flags & pfExact) )
	{
		NewParent->SetSizer(Sizer);
		if ( !GetChildCount() )
		{
			// Setting custom size for childless sizer to prevent
			// zero-size items
			#if wxCHECK_VERSION(2,8,0)
			NewParent->SetInitialSize(wxSize(20,20));
			#else
			NewParent->SetBestFittingSize(wxSize(20,20));
			#endif
			NewParent->SetSizeHints(20,20);
			NewParent->SetSize(wxSize(20,20));
		}
		else
		{
			Sizer->Fit(NewParent);
			Sizer->SetSizeHints(NewParent);
		}
		return NewParent;
	}
	return Sizer;
}
void wxsGridBagSizer::OnBuildSizerCreatingCode()
{
	switch ( GetLanguage() )
	{
		case wxsCPP:
		{
			AddHeader(_T("<wx/gbsizer.h>"),GetInfo().ClassName,hfInPCH);
			Codef(_T("%C(%s, %s);\n"),
				#if wxCHECK_VERSION(2, 9, 0)
				VGap.GetPixelsCode(GetCoderContext()).wx_str(),
				HGap.GetPixelsCode(GetCoderContext()).wx_str());
				#else
				VGap.GetPixelsCode(GetCoderContext()).c_str(),
				HGap.GetPixelsCode(GetCoderContext()).c_str());
				#endif

			wxArrayInt Cols = GetArray(GrowableCols);
			for ( size_t i=0; i<Cols.Count(); i++ )
			{
				Codef(_T("%AAddGrowableCol(%d);\n"),Cols[i]);
			}

			wxArrayInt Rows = GetArray(GrowableRows);
			for ( size_t i=0; i<Rows.Count(); i++ )
			{
				Codef(_T("%AAddGrowableRow(%d);\n"),Rows[i]);
			}

			return;
		}

		default:
		{
			wxsCodeMarks::Unknown(_T("wxsGridBagSizer::OnBuildSizerCreatingCode"),GetLanguage());
		}
	}
}
void wxsGridBagSizer::OnBuildCreatingCode()
{
	OnBuildSizerCreatingCode();

	bool UnknownLang = false;
	int Count = GetChildCount();
	for ( int i=0; i<Count; i++ )
	{
		wxsItem* Child = GetChild(i);
		wxsGridBagSizerExtra* Extra = (wxsGridBagSizerExtra*)GetChildExtra(i);

		// Using same parent as we got, sizer is not a parent window
		Child->BuildCode(GetCoderContext());

		switch ( Child->GetType() )
		{
			case wxsTWidget:
			case wxsTContainer:
			case wxsTSizer:
				switch ( GetLanguage() )
				{
					case wxsCPP:
					{
						Codef(_T("%AAdd(%o, %s);\n"),i,Extra->AllParamsCode(GetCoderContext()).wx_str());
						break;
					}
					default:
					{
						UnknownLang = true;
					}
				}
				break;

			case wxsTSpacer:
				// Spacer is responsible for adding itself into sizer
				break;

			default:
				break;
		}
	}

	if ( UnknownLang )
	{
		wxsCodeMarks::Unknown(_T("wxsGridBagSizer::OnBuildCreatingCode"),GetLanguage());
	}
}
