/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "manager.h"
    #include "logmanager.h"
    #include "globals.h"
#endif

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
    LogManager* pMsg = Manager::Get()->GetLogManager();
    if (!pMsg)
        return false;

//    pMsg->DebugLog(_T("Reading template file %s"), filename.c_str());

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
            pMsg->DebugLog(_T("Not a valid Code::Blocks template file..."));
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
            m_Name = cbC2U(node->Attribute("name"));
        if (node->Attribute("title"))
            m_Title = cbC2U(node->Attribute("title"));
        if (node->Attribute("category"))
            m_Category = cbC2U(node->Attribute("category"));
        if (node->Attribute("bitmap"))
            m_Bitmap = cbC2U(node->Attribute("bitmap"));

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
    m_Notice = cbC2U(node->Attribute("value"));
    while (m_Notice.Replace(_T("  "), _T(" ")))
        ;
    m_Notice.Replace(_T("\t"), _T(""));
    m_NoticeMsgType = cbC2U(node->Attribute("value")) == _T("0") ? wxICON_INFORMATION : wxICON_WARNING;
}

void ProjectTemplateLoader::DoFileSet(TiXmlElement* parentNode)
{
    TiXmlElement* node = parentNode->FirstChildElement("FileSet");
    while (node)
    {
        FileSet fs;
        if (node->Attribute("name"))
            fs.name = cbC2U(node->Attribute("name"));
        if (node->Attribute("title"))
            fs.title = cbC2U(node->Attribute("title"));

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
            fsf.source = cbC2U(node->Attribute("source"));
        if (node->Attribute("destination"))
            fsf.destination = cbC2U(node->Attribute("destination"));
        if (node->Attribute("targets"))
            fsf.targets = cbC2U(node->Attribute("targets"));

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
        to.useDefaultCompiler = true;

        if (node->Attribute("name"))
            to.name = cbC2U(node->Attribute("name"));

        if (!to.name.IsEmpty())
        {
            TiXmlElement* tmpnode = node->FirstChildElement("Notice");
            if (tmpnode)
            {
                to.notice << _T("\n") << cbC2U(tmpnode->Attribute("value"));
                while (to.notice.Replace(_T("  "), _T(" ")))
                    ;
                to.notice.Replace(_T("\t"), _T(""));
                to.noticeMsgType = strncmp(tmpnode->Attribute("value"), "0", 1) == 0 ? wxICON_INFORMATION : wxICON_WARNING;
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
    if (node)
    {
        if (node->Attribute("file"))
            to.file = cbC2U(node->Attribute("file"));
        if (node->Attribute("useDefaultCompiler"))
            to.useDefaultCompiler = strncmp(node->Attribute("useDefaultCompiler"), "0", 1) != 0;
    }
}

void ProjectTemplateLoader::DoOptionCompiler(TiXmlElement* parentNode, TemplateOption& to)
{
    TiXmlElement* node = parentNode->FirstChildElement("Compiler");
    while (node)
    {
        if (node->Attribute("flag"))
            to.extraCFlags.Add(cbC2U(node->Attribute("flag")));

        node = node->NextSiblingElement("Compiler");
    }
}

void ProjectTemplateLoader::DoOptionLinker(TiXmlElement* parentNode, TemplateOption& to)
{
    TiXmlElement* node = parentNode->FirstChildElement("Linker");
    while (node)
    {
        if (node->Attribute("flag"))
            to.extraLDFlags.Add(cbC2U(node->Attribute("flag")));

        node = node->NextSiblingElement("Linker");
    }
}
