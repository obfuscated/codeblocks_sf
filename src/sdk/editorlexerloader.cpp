/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "globals.h"
    #include "manager.h"
    #include "messagemanager.h"
    #include <wx/dynarray.h>
#endif

#include "editorcolorset.h"
#include "editorlexerloader.h"

EditorLexerLoader::EditorLexerLoader(EditorColorSet* target)
    : m_pTarget(target)
{
	//ctor
}

EditorLexerLoader::~EditorLexerLoader()
{
	//dtor
}

void EditorLexerLoader::Load(const wxString& filename)
{
    LOGSTREAM << _("Loading ") << wxFileName(filename).GetFullName() << _T('\n');
    TiXmlDocument doc(filename.mb_str());
    if (!doc.LoadFile())
    {
        LOGSTREAM << _("Failed loading ") << filename << _T('\n');
        return;
    }

//    LOGSTREAM << "Parsing lexer file...\n";
    TiXmlElement* root;
    TiXmlElement* lexer;

    root = doc.FirstChildElement("CodeBlocks_lexer_properties");
    if (!root)
    {
        // old tag
        root = doc.FirstChildElement("Code::Blocks_lexer_properties");
        if (!root)
        {
            LOGSTREAM << _("Not a valid Code::Blocks lexer file...\n");
            return;
        }
    }
    lexer = root->FirstChildElement("Lexer");
    if (!lexer)
    {
        LOGSTREAM << _("No 'Lexer' element in file...\n");
        return;
    }

    DoLexer(lexer);
}

void EditorLexerLoader::DoLexer(TiXmlElement* node)
{
    if (!node->Attribute("name") || !node->Attribute("index"))
    {
    	LOGSTREAM << _("No name or index...\n");
        return;
    }

    wxString name = wxString( node->Attribute("name"), wxConvUTF8 );
    int lexer = atol(node->Attribute("index"));
    wxString masks = wxString ( node->Attribute("filemasks"), wxConvUTF8 );
    HighlightLanguage style = m_pTarget->AddHighlightLanguage(lexer, name);
    if (style == HL_NONE)
        return; // wasn't added
    m_pTarget->SetFileMasks(style, masks);
//    LOGSTREAM << "Found lexer: " << name << " (" << style << ")\n";

    DoStyles(style, node);
    DoKeywords(style, node);
    DoSampleCode(style, node);
}

void EditorLexerLoader::DoStyles(HighlightLanguage language, TiXmlElement* node)
{
    TiXmlElement* style = node->FirstChildElement("Style");
    while (style)
    {
        if (style->Attribute("name") && style->Attribute("index"))
        {
            wxString name = wxString ( style->Attribute("name"), wxConvUTF8 );
            wxString index = wxString ( style->Attribute("index"), wxConvUTF8 ); // comma-separated indices
            wxString fg = wxString ( style->Attribute("fg"), wxConvUTF8 );
            wxString bg = wxString ( style->Attribute("bg"), wxConvUTF8 );
            bool bold = style->Attribute("bold") ? atol(style->Attribute("bold")) != 0 : false;
            bool italics = style->Attribute("italics") ? atol(style->Attribute("italics")) != 0 : false;
            bool underlined = style->Attribute("underlined") ? atol(style->Attribute("underlined")) != 0 : false;

            // break-up arrays
            wxArrayString indices = GetArrayFromString(index, _T(","));
            wxArrayString fgarray = GetArrayFromString(fg, _T(","));
            wxArrayString bgarray = GetArrayFromString(bg, _T(","));

            wxColour fgcolor = wxNullColour;
            if (fgarray.GetCount() == 3)
            {
                long R=0, G=0, B=0;
                fgarray[0].ToLong(&R);
                fgarray[1].ToLong(&G);
                fgarray[2].ToLong(&B);
                fgcolor.Set((unsigned char)R,(unsigned char)G,(unsigned char)B);
            }
            wxColour bgcolor = wxNullColour;
            if (bgarray.GetCount() == 3)
            {
                long R=0, G=0, B=0;
                bgarray[0].ToLong(&R);
                bgarray[1].ToLong(&G);
                bgarray[2].ToLong(&B);
                bgcolor.Set((unsigned char)R,(unsigned char)G,(unsigned char)B);
            }

            for (size_t i = 0; i < indices.GetCount(); ++i)
            {
            	if (indices[i].IsEmpty())
                    continue;
                long value = 0;
                indices[i].ToLong(&value);
//                LOGSTREAM << _("Adding style: ") << name << _T("(") << value << _T(")\n");
                m_pTarget->AddOption(language, name, value,
                                    fgcolor,
                                    bgcolor,
                                    bold,
                                    italics,
                                    underlined,
                                    value >= 0);
            }
        }
        style = style->NextSiblingElement("Style");
    }
}

void EditorLexerLoader::DoKeywords(HighlightLanguage language, TiXmlElement* node)
{
    TiXmlElement* keywords = node->FirstChildElement("Keywords");
    if (!keywords)
        return;
    DoSingleKeywordNode(language, keywords, _T("Language"));
    DoSingleKeywordNode(language, keywords, _T("Documentation"));
    DoSingleKeywordNode(language, keywords, _T("User"));
}

void EditorLexerLoader::DoSingleKeywordNode(HighlightLanguage language, TiXmlElement* node, const wxString& nodename)
{
    TiXmlElement* keywords = node->FirstChildElement(nodename.mb_str());
    if (!keywords)
        return;
//    LOGSTREAM << "Found " << nodename << '\n';
    int keyidx = keywords->Attribute("index") ? atol(keywords->Attribute("index")) : -1;
//    LOGSTREAM << "keyidx=" << keyidx << '\n';
    if (keyidx != -1)
        m_pTarget->SetKeywords(language, keyidx, wxString ( keywords->Attribute("value"), wxConvUTF8 ) );
}

void EditorLexerLoader::DoSampleCode(HighlightLanguage language, TiXmlElement* node)
{
    TiXmlElement* sample = node->FirstChildElement("SampleCode");
    if (!sample)
        return;
    wxString code = wxString ( sample->Attribute("value"), wxConvUTF8 );
    if (code.IsEmpty())
        return;
    int breakLine = sample->Attribute("breakpoint_line") ? atol(sample->Attribute("breakpoint_line")) : -1;
    int debugLine = sample->Attribute("debug_line") ? atol(sample->Attribute("debug_line")) : -1;
    int errorLine = sample->Attribute("error_line") ? atol(sample->Attribute("error_line")) : -1;
    m_pTarget->SetSampleCode(language, code, breakLine, debugLine, errorLine);
}
