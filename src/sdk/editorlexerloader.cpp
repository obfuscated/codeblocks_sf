#include "editorlexerloader.h"
#include "globals.h"
#include "editorcolorset.h"
#include "manager.h"
#include "messagemanager.h"
#include <wx/dynarray.h>

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
    LOGSTREAM << "Loading lexer file " << filename << '\n';
    TiXmlDocument doc(filename.c_str());
    if (!doc.LoadFile())
    {
        LOGSTREAM << "Failed loading " << filename << '\n';
        return;
    }

//    LOGSTREAM << "Parsing lexer file...\n";
    TiXmlElement* root;
    TiXmlElement* lexer;
    
    root = doc.FirstChildElement("Code::Blocks_lexer_properties");
    if (!root)
    {
        LOGSTREAM << "Not a valid Code::Blocks lexer file...\n";
        return;
    }
    lexer = root->FirstChildElement("Lexer");
    if (!lexer)
    {
        LOGSTREAM << "No 'Lexer' element in file...\n";
        return;
    }
    
    DoLexer(lexer);
}

void EditorLexerLoader::DoLexer(TiXmlElement* node)
{
    if (!node->Attribute("name") || !node->Attribute("index"))
    {
    	LOGSTREAM << "No name or index...\n";
        return;
    }

    wxString name = node->Attribute("name");
    int lexer = atol(node->Attribute("index"));
    wxString masks = node->Attribute("filemasks");
    int style = m_pTarget->AddHighlightLanguage(lexer, name);
    m_pTarget->SetFileMasks(style, masks);
//    LOGSTREAM << "Found lexer: " << name << " (" << style << ")\n";

    DoStyles(style, node);
    DoKeywords(style, node);
    DoSampleCode(style, node);
}

void EditorLexerLoader::DoStyles(int language, TiXmlElement* node)
{
    TiXmlElement* style = node->FirstChildElement("Style");
    while (style)
    {
        if (style->Attribute("name") && style->Attribute("index"))
        {
            wxString name = style->Attribute("name");
            wxString index = style->Attribute("index"); // comma-separated indices
            wxString fg = style->Attribute("fg");
            wxString bg = style->Attribute("bg");
            bool bold = style->Attribute("bold") ? atol(style->Attribute("bold")) != 0 : false;
            bool italics = style->Attribute("italics") ? atol(style->Attribute("italics")) != 0 : false;
            bool underlined = style->Attribute("underlined") ? atol(style->Attribute("underlined")) != 0 : false;

            // break-up arrays
            wxArrayString indices = GetArrayFromString(index, ",");
            wxArrayString fgarray = GetArrayFromString(fg, ",");
            wxArrayString bgarray = GetArrayFromString(bg, ",");
            
            wxColour fgcolor = wxNullColour;
            if (fgarray.GetCount() == 3)
                fgcolor.Set(atoi(fgarray[0]), atoi(fgarray[1]), atoi(fgarray[2]));
            wxColour bgcolor = wxNullColour;
            if (bgarray.GetCount() == 3)
                bgcolor.Set(atoi(bgarray[0]), atoi(bgarray[1]), atoi(bgarray[2]));

            for (size_t i = 0; i < indices.GetCount(); ++i)
            {
            	if (indices[i].IsEmpty())
                    continue;
                int value = atol(indices[i]);
//                LOGSTREAM << "Adding style: " << name << "(" << value << ")\n";
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

void EditorLexerLoader::DoKeywords(int language, TiXmlElement* node)
{
    TiXmlElement* keywords = node->FirstChildElement("Keywords");
    if (!keywords)
        return;
    DoSingleKeywordNode(language, keywords, _("Language"));
    DoSingleKeywordNode(language, keywords, _("Documentation"));
    DoSingleKeywordNode(language, keywords, _("User"));
}

void EditorLexerLoader::DoSingleKeywordNode(int language, TiXmlElement* node, const wxString& nodename)
{
    TiXmlElement* keywords = node->FirstChildElement(nodename);
    if (!keywords)
        return;
    LOGSTREAM << "Found " << nodename << '\n';
    int keyidx = keywords->Attribute(_("index")) ? atol(keywords->Attribute(_("index"))) : -1;
    LOGSTREAM << "keyidx=" << keyidx << '\n';
    if (keyidx != -1)
        m_pTarget->SetKeywords(language, keyidx, keywords->Attribute("value"));
}

void EditorLexerLoader::DoSampleCode(int language, TiXmlElement* node)
{
    TiXmlElement* sample = node->FirstChildElement("SampleCode");
    if (!sample)
        return;
    wxString code = sample->Attribute("value");
    if (code.IsEmpty())
        return;
    int breakLine = sample->Attribute("breakpoint_line") ? atol(sample->Attribute("breakpoint_line")) : -1;
    int debugLine = sample->Attribute("debug_line") ? atol(sample->Attribute("debug_line")) : -1;
    int errorLine = sample->Attribute("error_line") ? atol(sample->Attribute("error_line")) : -1;
    m_pTarget->SetSampleCode(language, code, breakLine, debugLine, errorLine);
}
