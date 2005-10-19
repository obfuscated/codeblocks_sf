#include "sdk_precomp.h"
#include "manager.h"
#include "messagemanager.h"
#include "projecttemplateloader.h"
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FileSetFileArray);
WX_DEFINE_OBJARRAY(FileSetArray);
WX_DEFINE_OBJARRAY(TemplateOptionArray);

ProjectTemplateLoader::ProjectTemplateLoader()
{
	//ctor
}

ProjectTemplateLoader::~ProjectTemplateLoader()
{
	//dtor
}

bool ProjectTemplateLoader::Open(const wxString& filename)
{
    MessageManager* pMsg = Manager::Get()->GetMessageManager();
    if (!pMsg)
        return false;

    TiXmlDocument doc(filename.mb_str());
    if (!doc.LoadFile())
        return false;

    TiXmlElement* root;

    root = doc.FirstChildElement("CodeBlocks_template_file");
    if (!root)
    {
        // old tag
        root = doc.FirstChildElement("Code::Blocks_template_file");
        if (!root)
        {
            pMsg->DebugLog(_("Not a valid Code::Blocks template file..."));
            return false;
        }
    }

    DoTemplate(root);

    return true;
}

void ProjectTemplateLoader::DoTemplate(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("Template");
    while (node)
    {
        if (node->Attribute("name"))
            m_Name = _U(node->Attribute("name"));
        if (node->Attribute("title"))
            m_Title = _U(node->Attribute("title"));
        if (node->Attribute("category"))
            m_Category = _U(node->Attribute("category"));
        if (node->Attribute("bitmap"))
            m_Bitmap = _U(node->Attribute("bitmap"));

        DoTemplateNotice(node);
        DoFileSet(node);
        DoOption(node);

        node = node->NextSiblingElement("Template");
    }
}

void ProjectTemplateLoader::DoTemplateNotice(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("Notice");
    if (!node)
        return;
    m_Notice = _U(node->Attribute("value"));
    while (m_Notice.Replace(_T("  "), _T(" ")))
        ;
    m_Notice.Replace(_T("\t"), _T(""));
    m_NoticeMsgType = _U(node->Attribute("value")) == _T("0") ? wxICON_INFORMATION : wxICON_WARNING;
}

void ProjectTemplateLoader::DoFileSet(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("FileSet");
    while (node)
    {
        FileSet fs;
        if (node->Attribute("name"))
            fs.name = _U(node->Attribute("name"));
        if (node->Attribute("title"))
            fs.title = _U(node->Attribute("title"));

        if (!fs.name.IsEmpty() && !fs.title.IsEmpty())
        {
            DoFileSetFile(node, fs);
            m_FileSets.Add(fs);
        }

        node = node->NextSiblingElement("FileSet");
    }
}

void ProjectTemplateLoader::DoFileSetFile(TiXmlElement* parentNode, FileSet& fs)
{
    TiXmlElement* node = parentNode->FirstChildElement("File");
    while (node)
    {
        FileSetFile fsf;
        if (node->Attribute("source"))
            fsf.source = _U(node->Attribute("source"));
        if (node->Attribute("destination"))
            fsf.destination = _U(node->Attribute("destination"));
        if (node->Attribute("targets"))
            fsf.targets = _U(node->Attribute("targets"));

        if (!fsf.source.IsEmpty() && !fsf.destination.IsEmpty())
            fs.files.Add(fsf);

        node = node->NextSiblingElement("File");
    }
}

void ProjectTemplateLoader::DoOption(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("Option");
    while (node)
    {
        TemplateOption to;
        if (node->Attribute("name"))
            to.name = _U(node->Attribute("name"));

        if (!to.name.IsEmpty())
        {
            TiXmlElement* tmpnode = node->FirstChildElement("Notice");
            if (tmpnode)
            {
                to.notice << _T("\n") << _U(tmpnode->Attribute("value"));
                while (to.notice.Replace(_T("  "), _T(" ")))
                    ;
                to.notice.Replace(_T("\t"), _T(""));
                to.noticeMsgType = _U(tmpnode->Attribute("value")) == _T("0") ? wxICON_INFORMATION : wxICON_WARNING;
            }

            DoOptionProject(node, to);
            DoOptionCompiler(node, to);
            DoOptionLinker(node, to);
            m_TemplateOptions.Add(to);
        }

        node = node->NextSiblingElement("Option");
    }
}

void ProjectTemplateLoader::DoOptionProject(TiXmlElement* parentNode, TemplateOption& to)
{
    TiXmlElement* node = parentNode->FirstChildElement("Project");
    if (node && node->Attribute("file"))
        to.file = _U(node->Attribute("file"));
}

void ProjectTemplateLoader::DoOptionCompiler(TiXmlElement* parentNode, TemplateOption& to)
{
    TiXmlElement* node = parentNode->FirstChildElement("Compiler");
    while (node)
    {
        if (node->Attribute("flag"))
            to.extraCFlags.Add(_U(node->Attribute("flag")));

        node = node->NextSiblingElement("Compiler");
    }
}

void ProjectTemplateLoader::DoOptionLinker(TiXmlElement* parentNode, TemplateOption& to)
{
    TiXmlElement* node = parentNode->FirstChildElement("Linker");
    while (node)
    {
        if (node->Attribute("flag"))
            to.extraLDFlags.Add(_U(node->Attribute("flag")));

        node = node->NextSiblingElement("Linker");
    }
}
