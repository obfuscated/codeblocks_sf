/**  wxsrichtextctrl.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010 Gary Harris
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <wx/richtext/richtextctrl.h>
#include "wxsrichtextctrl.h"

namespace
{
    wxsRegisterItem<wxsRichTextCtrl> Reg(_T("RichTextCtrl"), wxsTWidget, _T("Standard"), 160);

	// Commented items are currently unimplemented in wx.
	static const long arrAlignmentStates[] = {
//		wxTEXT_ALIGNMENT_DEFAULT,
		wxTEXT_ALIGNMENT_LEFT,
		wxTEXT_ALIGNMENT_CENTRE,
		wxTEXT_ALIGNMENT_RIGHT
//		wxTEXT_ALIGNMENT_JUSTIFIED
	};
	static const wxChar* arrAlignmentNames[]  = {
//		wxT("wxTEXT_ALIGNMENT_DEFAULT"),
		wxT("wxTEXT_ALIGNMENT_LEFT"),
		wxT("wxTEXT_ALIGNMENT_CENTRE"),
		wxT("wxTEXT_ALIGNMENT_RIGHT"),
		NULL
	};

	static const long arrAttributeStates[] = {
		wxTEXT_ATTR_TEXT_COLOUR,
		wxTEXT_ATTR_BACKGROUND_COLOUR,
		wxTEXT_ATTR_FONT_FACE,
		wxTEXT_ATTR_FONT_SIZE,
		wxTEXT_ATTR_FONT_WEIGHT,
		wxTEXT_ATTR_FONT_ITALIC,
		wxTEXT_ATTR_FONT_UNDERLINE,
		wxTEXT_ATTR_FONT,
		wxTEXT_ATTR_ALIGNMENT,
		wxTEXT_ATTR_LEFT_INDENT,
		wxTEXT_ATTR_RIGHT_INDENT,
		wxTEXT_ATTR_TABS,
		// Extra formatting flags not in wxTextAttr
		wxTEXT_ATTR_PARA_SPACING_AFTER,
		wxTEXT_ATTR_PARA_SPACING_BEFORE,
		wxTEXT_ATTR_LINE_SPACING,
		wxTEXT_ATTR_CHARACTER_STYLE_NAME,
		wxTEXT_ATTR_PARAGRAPH_STYLE_NAME,
		wxTEXT_ATTR_LIST_STYLE_NAME,
		wxTEXT_ATTR_BULLET_STYLE,
		wxTEXT_ATTR_BULLET_NUMBER,
		wxTEXT_ATTR_BULLET_TEXT,
		wxTEXT_ATTR_BULLET_NAME,
		wxTEXT_ATTR_URL,
		wxTEXT_ATTR_PAGE_BREAK,
		wxTEXT_ATTR_EFFECTS,
		wxTEXT_ATTR_OUTLINE_LEVEL
	};
	static const wxChar* arrAttributeNames[]  = {
		wxT("wxTEXT_ATTR_TEXT_COLOUR"),
		wxT("wxTEXT_ATTR_BACKGROUND_COLOUR"),
		wxT("wxTEXT_ATTR_FONT_FACE"),
		wxT("wxTEXT_ATTR_FONT_SIZE"),
		wxT("wxTEXT_ATTR_FONT_WEIGHT"),
		wxT("wxTEXT_ATTR_FONT_ITALIC"),
		wxT("wxTEXT_ATTR_FONT_UNDERLINE"),
		wxT("wxTEXT_ATTR_FONT"),
		wxT("wxTEXT_ATTR_ALIGNMENT"),
		wxT("wxTEXT_ATTR_LEFT_INDENT"),
		wxT("wxTEXT_ATTR_RIGHT_INDENT"),
		wxT("wxTEXT_ATTR_TABS"),
		// Extra formatting flags not in wxTEXTAttr
		wxT("wxTEXT_ATTR_PARA_SPACING_AFTER"),
		wxT("wxTEXT_ATTR_PARA_SPACING_BEFORE"),
		wxT("wxTEXT_ATTR_LINE_SPACING"),
		wxT("wxTEXT_ATTR_CHARACTER_STYLE_NAME"),
		wxT("wxTEXT_ATTR_PARAGRAPH_STYLE_NAME"),
		wxT("wxTEXT_ATTR_LIST_STYLE_NAME"),
		wxT("wxTEXT_ATTR_BULLET_STYLE"),
		wxT("wxTEXT_ATTR_BULLET_NUMBER"),
		wxT("wxTEXT_ATTR_BULLET_TEXT"),
		wxT("wxTEXT_ATTR_BULLET_NAME"),
		wxT("wxTEXT_ATTR_URL"),
		wxT("wxTEXT_ATTR_PAGE_BREAK"),
		wxT("wxTEXT_ATTR_EFFECTS"),
		wxT("wxTEXT_ATTR_OUTLINE_LEVEL"),
		NULL
	};

	static const long arrBulletStates[] = {
		wxTEXT_ATTR_BULLET_STYLE_ARABIC,
		wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER,
		wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER,
		wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER,
		wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER,
		wxTEXT_ATTR_BULLET_STYLE_SYMBOL,
//		wxTEXT_ATTR_BULLET_STYLE_BITMAP,
		wxTEXT_ATTR_BULLET_STYLE_PARENTHESES,
		wxTEXT_ATTR_BULLET_STYLE_PERIOD,
		wxTEXT_ATTR_BULLET_STYLE_STANDARD,
		wxTEXT_ATTR_BULLET_STYLE_RIGHT_PARENTHESIS,
		wxTEXT_ATTR_BULLET_STYLE_OUTLINE,
		wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT,
		wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT,
		wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE
	};
	static const wxChar* arrBulletNames[]  = {
		wxT("wxTEXT_ATTR_BULLET_STYLE_ARABIC"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_SYMBOL"),
//		wxT("wxTEXT_ATTR_BULLET_STYLE_BITMAP"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_PARENTHESES"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_PERIOD"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_STANDARD"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_RIGHT_PARENTHESIS"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_OUTLINE"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT"),
		wxT("wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE"),
		NULL
	};

	static const long arrSpacingStates[] = {
		wxTEXT_ATTR_LINE_SPACING_NORMAL,
		wxTEXT_ATTR_LINE_SPACING_HALF,
		wxTEXT_ATTR_LINE_SPACING_TWICE,
	};
	static const wxChar* arrSpacingNames[]  = {
		wxT("wxTEXT_ATTR_LINE_SPACING_NORMAL"),
		wxT("wxTEXT_ATTR_LINE_SPACING_HALF"),
		wxT("wxTEXT_ATTR_LINE_SPACING_TWICE"),
		NULL
	};

	static const long arrEffectStates[] = {
		wxTEXT_ATTR_EFFECT_CAPITALS,
//		wxTEXT_ATTR_EFFECT_SMALL_CAPITALS,
		wxTEXT_ATTR_EFFECT_STRIKETHROUGH
//		wxTEXT_ATTR_EFFECT_DOUBLE_STRIKETHROUGH,
//		wxTEXT_ATTR_EFFECT_SHADOW,
//		wxTEXT_ATTR_EFFECT_EMBOSS,
//		wxTEXT_ATTR_EFFECT_OUTLINE,
//		wxTEXT_ATTR_EFFECT_ENGRAVE,
//		wxTEXT_ATTR_EFFECT_SUPERSCRIPT,
//		wxTEXT_ATTR_EFFECT_SUBSCRIPT
	};
	static const wxChar* arrEffectNames[]  = {
		wxT("wxTEXT_ATTR_EFFECT_CAPITALS"),
//		wxT("wxTEXT_ATTR_EFFECT_SMALL_CAPITALS"),
		wxT("wxTEXT_ATTR_EFFECT_STRIKETHROUGH"),
//		wxT("wxTEXT_ATTR_EFFECT_DOUBLE_STRIKETHROUGH"),
//		wxT("wxTEXT_ATTR_EFFECT_SHADOW"),
//		wxT("wxTEXT_ATTR_EFFECT_EMBOSS"),
//		wxT("wxTEXT_ATTR_EFFECT_OUTLINE"),
//		wxT("wxTEXT_ATTR_EFFECT_ENGRAVE"),
//		wxT("wxTEXT_ATTR_EFFECT_SUPERSCRIPT"),
//		wxT("wxTEXT_ATTR_EFFECT_SUBSCRIPT"),
		NULL
	};

    WXS_ST_BEGIN(wxsRichTextCtrlStyles, _T("wxRE_MULTILINE"))
#ifndef __WXMAC__
		WXS_ST(wxRE_CENTRE_CARET)
#endif
		WXS_ST(wxRE_MULTILINE)
		WXS_ST(wxRE_READONLY)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsRichTextCtrlEvents)
        WXS_EVI(EVT_TEXT, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEvent, Text)
        WXS_EVI(EVT_TEXT_ENTER, wxEVT_COMMAND_TEXT_ENTER, wxCommandEvent, TextEnter)
        WXS_EVI(EVT_TEXT_URL, wxEVT_COMMAND_TEXT_URL, wxTextUrlEvent, TextUrl)
        WXS_EVI(EVT_RICHTEXT_CHARACTER, wxEVT_COMMAND_RICHTEXT_CHARACTER, wxRichTextEvent, RichTextChar)
        WXS_EVI(EVT_RICHTEXT_DELETE, wxEVT_COMMAND_RICHTEXT_DELETE, wxRichTextEvent, RichTextDelete)
        WXS_EVI(EVT_RICHTEXT_RETURN, wxEVT_COMMAND_RICHTEXT_RETURN, wxRichTextEvent, RichTextReturn)
        WXS_EVI(EVT_RICHTEXT_STYLE_CHANGED, wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED, wxRichTextEvent, RichTextSttyleChanged)
        WXS_EVI(EVT_RICHTEXT_STYLESHEET_CHANGED, wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING, wxRichTextEvent, RichTextStylesheetChanged)
        WXS_EVI(EVT_RICHTEXT_STYLESHEET_REPLACING, wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING, wxRichTextEvent, RichTextStylesheetReplacing)
        WXS_EVI(EVT_RICHTEXT_STYLESHEET_REPLACED, wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED, wxRichTextEvent, RichTextStylesheetReplaced)
        WXS_EVI(EVT_RICHTEXT_CONTENT_INSERTED, wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED, wxRichTextEvent, RichTextContentInserted)
        WXS_EVI(EVT_RICHTEXT_CONTENT_DELETED, wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED, wxRichTextEvent, RichTextContentDeleted)
        WXS_EVI(EVT_RICHTEXT_BUFFER_RESET, wxEVT_COMMAND_RICHTEXT_BUFFER_RESET, wxRichTextEvent, RichTextBufferReset)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*	The control's resource data.
 *
 */
wxsRichTextCtrl::wxsRichTextCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsRichTextCtrlEvents,
        wxsRichTextCtrlStyles,
        flVariable | flId | flPosition | flSize | flEnabled | flFocused | flHidden | flColours  | flToolTip | flHelpText | flSubclass | flMinMaxSize | flExtraCode),
    m_sText(_("Text")),
	m_iAlignment(wxTEXT_ALIGNMENT_LEFT),
	m_iAttribute(0),
	m_iBullets(wxTEXT_ATTR_BULLET_STYLE_NONE),
	m_iSpacing(wxTEXT_ATTR_LINE_SPACING_NORMAL),
	m_iEffects(wxTEXT_ATTR_EFFECT_NONE)
{}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsRichTextCtrl::OnBuildCreatingCode()
{
	wxString sFlags;
	bool bClrChanged = false;

    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/richtext/richtextctrl.h>"),GetInfo().ClassName,hfInPCH);

            #if wxCHECK_VERSION(2, 9, 0)
            Codef(_T("%C(%W, %I, %t, %P, %S, %T, %V, %N);\n"), m_sText.wx_str());
            #else
            Codef(_T("%C(%W, %I, %t, %P, %S, %T, %V, %N);\n"), m_sText.c_str());
            #endif

			wxString sAttrName = GetCoderContext()->GetUniqueName(_T("rchtxtAttr"));
			#if wxCHECK_VERSION(2, 9, 0)
            Codef(_T("\twxRichTextAttr %s;\n"), sAttrName.wx_str());
			#else
            Codef(_T("\twxRichTextAttr %s;\n"), sAttrName.c_str());
			#endif
			// Alignment.
			// wxTEXT_ALIGNMENT_LEFT is the default.
			if(m_iAlignment == wxTEXT_ALIGNMENT_CENTRE){
				#if wxCHECK_VERSION(2, 9, 0)
				Codef( _T("%s.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);\n"), sAttrName.wx_str());
				#else
				Codef( _T("%s.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);\n"), sAttrName.c_str());
				#endif
            }
			else if(m_iAlignment == wxTEXT_ALIGNMENT_RIGHT){
				#if wxCHECK_VERSION(2, 9, 0)
				Codef( _T("%s.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);\n"), sAttrName.wx_str());
				#else
				Codef( _T("%s.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);\n"), sAttrName.c_str());
				#endif
            }
			// Attribute flags.
            for(int i = 0;arrAttributeNames[i];i++){
                if(m_iAttribute & arrAttributeStates[i]){
                	sFlags << arrAttributeNames[i] << _T("|");
                }
            }
			if(!sFlags.IsEmpty()){
				sFlags.RemoveLast();

				#if wxCHECK_VERSION(2, 9, 0)
				Codef( _T("%s.SetFlags(%s);\n"), sAttrName.wx_str(), sFlags.wx_str());
				#else
				Codef( _T("%s.SetFlags(%s);\n"), sAttrName.c_str(), sFlags.c_str());
				#endif
            }
			// Bullet flags.
			sFlags.Clear();
            for(int i = 0;arrBulletNames[i];i++){
                if(m_iBullets & arrBulletStates[i]){
                	sFlags << arrBulletNames[i] << _T("|");
                }
            }
			if(!sFlags.IsEmpty()){
				sFlags.RemoveLast();

				#if wxCHECK_VERSION(2, 9, 0)
				Codef( _T("%s.SetBulletStyle(%s);\n"), sAttrName.wx_str(), sFlags.wx_str());
				#else
				Codef( _T("%s.SetBulletStyle(%s);\n"), sAttrName.c_str(), sFlags.c_str());
				#endif
            }
			// Line spacing.
			// wxTEXT_ATTR_LINE_SPACING_NORMAL is the default.
			if(m_iSpacing == wxTEXT_ATTR_LINE_SPACING_HALF){
				#if wxCHECK_VERSION(2, 9, 0)
				Codef( _T("%s.SetLineSpacing(wxTEXT_ATTR_LINE_SPACING_HALF);\n"), sAttrName.wx_str());
				#else
				Codef( _T("%s.SetLineSpacing(wxTEXT_ATTR_LINE_SPACING_HALF);\n"), sAttrName.c_str());
				#endif
            }
			else if(m_iSpacing== wxTEXT_ATTR_LINE_SPACING_TWICE){
				#if wxCHECK_VERSION(2, 9, 0)
				Codef( _T("%s.SetLineSpacing(wxTEXT_ATTR_LINE_SPACING_TWICE);\n"), sAttrName.wx_str());
				#else
				Codef( _T("%s.SetLineSpacing(wxTEXT_ATTR_LINE_SPACING_TWICE);\n"), sAttrName.c_str());
				#endif
            }
			// Text effect flags.
			sFlags.Clear();
            for(int i = 0;arrEffectNames[i];i++){
                if(m_iEffects & arrEffectStates[i]){
                	sFlags << arrEffectNames[i] << _T("|");
                }
            }
			if(!sFlags.IsEmpty()){
				sFlags.RemoveLast();

				#if wxCHECK_VERSION(2, 9, 0)
				Codef( _T("%s.SetTextEffects(%s);\n"), sAttrName.wx_str(), sFlags.wx_str());
				Codef( _T("%s.SetTextEffectFlags(%s);\n"), sAttrName.wx_str(), sFlags.wx_str());
				#else
				Codef( _T("%s.SetTextEffects(%s);\n"), sAttrName.c_str(), sFlags.c_str());
				Codef( _T("%s.SetTextEffectFlags(%s);\n"), sAttrName.c_str(), sFlags.c_str());
				#endif
            }
			// Text colours.
			wxString ss = m_cdTextColour.BuildCode(GetCoderContext());
			if(!ss.IsEmpty()){
				bClrChanged = true;
				#if wxCHECK_VERSION(2, 9, 0)
				Codef( _T("%s.SetTextColour(%s);\n"), sAttrName.wx_str(), ss.wx_str());
				#else
				Codef( _T("%s.SetTextColour(%s);\n"), sAttrName.c_str(), ss.c_str());
				#endif
			}
			ss = m_cdTextBackground.BuildCode(GetCoderContext());
			if(!ss.IsEmpty()){
				bClrChanged = true;
				#if wxCHECK_VERSION(2, 9, 0)
				Codef( _T("%s.SetBackgroundColour(%s);\n"), sAttrName.wx_str(), ss.wx_str());
				#else
				Codef( _T("%s.SetBackgroundColour(%s);\n"), sAttrName.c_str(), ss.c_str());
				#endif
			}
			// Font characteristics.
			wxString sFntName = GetCoderContext()->GetUniqueName(_T("Font"));
			wxString sFnt = m_fdFont.BuildFontCode(sFntName, GetCoderContext());
			if(sFnt.Len() > 0){
				#if wxCHECK_VERSION(2, 9, 0)
				Codef(_T("%s"), sFnt.wx_str());
				Codef( _T("%s.SetFontFaceName(%s.GetFaceName());\n"), sAttrName.wx_str(), sFnt.wx_str());
				Codef( _T("%s.SetFontSize(%s.GetPointSize());\n"), sAttrName.wx_str(), sFnt.wx_str());
				Codef( _T("%s.SetFontStyle(%s.GetStyle());\n"), sAttrName.wx_str(), sFnt.wx_str());
				Codef( _T("%s.SetFontUnderlined(%s.GetUnderlined());\n"), sAttrName.wx_str(), sFnt.wx_str());
				Codef( _T("%s.SetFontWeight(%s.GetWeight());\n"), sAttrName.wx_str(), sFnt.wx_str());
				#else
				Codef(_T("%s"), sFnt.c_str());
				Codef( _T("%s.SetFontFaceName(%s.GetFaceName());\n"), sAttrName.c_str(), sFntName.c_str());
				Codef( _T("%s.SetFontSize(%s.GetPointSize());\n"), sAttrName.c_str(), sFntName.c_str());
				Codef( _T("%s.SetFontStyle(%s.GetStyle());\n"), sAttrName.c_str(), sFntName.c_str());
				Codef( _T("%s.SetFontUnderlined(%s.GetUnderlined());\n"), sAttrName.c_str(), sFntName.c_str());
				Codef( _T("%s.SetFontWeight(%s.GetWeight());\n"), sAttrName.c_str(), sFntName.c_str());
				#endif
			}

			if(m_iAlignment != wxTEXT_ALIGNMENT_LEFT || m_iAttribute != 0 || m_iBullets != wxTEXT_ATTR_BULLET_STYLE_NONE ||
					m_iSpacing != wxTEXT_ATTR_LINE_SPACING_NORMAL || m_iEffects != wxTEXT_ATTR_EFFECT_NONE || bClrChanged || !sFnt.IsEmpty()){
				#if wxCHECK_VERSION(2, 9, 0)
				Codef( _T("%ASetBasicStyle(%s);\n"), sAttrName.wx_str());
				#else
				Codef( _T("%ASetBasicStyle(%s);\n"), sAttrName.c_str());
				#endif
			}

            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsRichTextCtrl::OnBuildCreatingCode"), GetLanguage());
        }
    }
}

/*! \brief	Build the control preview.
 *
 * \param parent wxWindow*	The parent window.
 * \param flags long				The control flags.
 * \return wxObject* 				The constructed control.
 *
 */
wxObject* wxsRichTextCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxRichTextCtrl* Preview = new wxRichTextCtrl(Parent, GetId(), m_sText, Pos(Parent), Size(Parent), Style());
	wxRichTextAttr rchtxtAttr;
	bool bClrChanged = false;

	// Attribute flags.
	rchtxtAttr.SetFlags(m_iAttribute);
	// Alignment.
	// wxTEXT_ALIGNMENT_LEFT is the default.
	if(m_iAlignment == wxTEXT_ALIGNMENT_CENTRE){
		rchtxtAttr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
	}
	else if(m_iAlignment == wxTEXT_ALIGNMENT_RIGHT){
		rchtxtAttr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
	}
	rchtxtAttr.SetFlags(m_iBullets);
	rchtxtAttr.SetLineSpacing(m_iSpacing);
	wxColour cc = m_cdTextColour.GetColour();
	if(cc.IsOk()){
		bClrChanged = true;
		rchtxtAttr.SetTextColour(cc);
	}
	cc = m_cdTextBackground.GetColour();
	if(cc.IsOk()){
		bClrChanged = true;
		rchtxtAttr.SetBackgroundColour(cc);
	}
	rchtxtAttr.SetTextEffects(m_iEffects);
	rchtxtAttr.SetTextEffectFlags(m_iEffects);
	// Font characteristics.
	wxFont fnt = m_fdFont.BuildFont();
	if(fnt.IsOk()){
		rchtxtAttr.SetFontFaceName(fnt.GetFaceName());
		rchtxtAttr.SetFontSize(fnt.GetPointSize());
		rchtxtAttr.SetFontStyle(fnt.GetStyle());
		rchtxtAttr.SetFontUnderlined(fnt.GetUnderlined());
		rchtxtAttr.SetFontWeight(fnt.GetWeight());
	}

	if(m_iAlignment != wxTEXT_ALIGNMENT_LEFT || m_iAttribute != 0 || m_iBullets != wxTEXT_ATTR_BULLET_STYLE_NONE ||
			m_iSpacing != wxTEXT_ATTR_LINE_SPACING_NORMAL || m_iEffects != wxTEXT_ATTR_EFFECT_NONE || bClrChanged || fnt.IsOk()){
		Preview->SetBasicStyle(rchtxtAttr);
	}

    return SetupWindow(Preview, Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long	The control flags.
 * \return void
 *
 */
void wxsRichTextCtrl::OnEnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsRichTextCtrl, m_sText, _("Text"), _T("value"), wxEmptyString, false)
	WXS_FLAGS(wxsRichTextCtrl, m_iAttribute, _("Attributes"), _T("attributes"), arrAttributeStates, arrAttributeNames, 0)
	WXS_FLAGS(wxsRichTextCtrl, m_iBullets, _("Bullet Style"), _T("bullet_style"), arrBulletStates, arrBulletNames, wxTEXT_ATTR_BULLET_STYLE_NONE)
	WXS_ENUM(wxsRichTextCtrl, m_iSpacing, _("Line Spacing"), _T("line_spacing"), arrSpacingStates, arrSpacingNames, wxTEXT_ATTR_LINE_SPACING_NORMAL)
	WXS_ENUM(wxsRichTextCtrl, m_iAlignment, _("Paragraph Alignment"), _T("paragraph_alignment"), arrAlignmentStates, arrAlignmentNames, wxTEXT_ALIGNMENT_LEFT)
	WXS_FLAGS(wxsRichTextCtrl, m_iEffects, _("Text Effects"), _T("text_effects"), arrEffectStates, arrEffectNames, wxTEXT_ATTR_EFFECT_NONE)
	WXS_COLOUR(wxsRichTextCtrl, m_cdTextColour, _("Text Colour"), wxT("text_colour"))
	WXS_COLOUR(wxsRichTextCtrl, m_cdTextBackground, _("Text Background Colour"), wxT("text_background_colour"))
    WXS_FONT(wxsRichTextCtrl, m_fdFont, _("Font"), _T("font"))
}
