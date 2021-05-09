/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#if 0

#include <sdk_precomp.h>
#ifndef CB_PRECOMP
    #include <settings.h>
    #include <manager.h>
    #include <logmanager.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <projectmanager.h>
    #include <macrosmanager.h>
    #include <compilerfactory.h>
    #include <cbproject.h>
    #include <cbeditor.h>
    #include <globals.h>
#endif
#include "cbstyledtextctrl.h"

#include <cbexception.h>
#include "sc_base_types.h"

namespace ScriptBindings
{
    extern void Register_Constants();
    extern void Register_Globals();
    extern void Register_wxTypes();
    extern void Register_Dialog();
    extern void Register_ProgressDialog();
    extern void Register_UtilDialogs();
    extern void Register_IO();
    extern void Register_ScriptPlugin();

    SQInteger ConfigManager_Read(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 3)
        {
            wxString key = *SqPlus::GetInstance<wxString,false>(v, 2);
            if (sa.GetType(3) == OT_INTEGER)
                return sa.Return((SQInteger)Manager::Get()->GetConfigManager(_T("scripts"))->ReadInt(key, sa.GetInt(3)));
            else if (sa.GetType(3) == OT_BOOL)
                return sa.Return(Manager::Get()->GetConfigManager(_T("scripts"))->ReadBool(key, sa.GetBool(3)));
            else if (sa.GetType(3) == OT_FLOAT)
                return sa.Return((float)Manager::Get()->GetConfigManager(_T("scripts"))->ReadDouble(key, sa.GetFloat(3)));
            else
            {
                wxString val = *SqPlus::GetInstance<wxString,false>(v, 3);
                wxString ret = Manager::Get()->GetConfigManager(_T("scripts"))->Read(key, val);
                return SqPlus::ReturnCopy(v, ret);
            }
        }
        return sa.ThrowError("Invalid arguments to \"ConfigManager::Read\"");
    }
    SQInteger ConfigManager_Write(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 3)
        {
            wxString key = *SqPlus::GetInstance<wxString,false>(v, 2);
            if (sa.GetType(3) == OT_INTEGER)
            {
                Manager::Get()->GetConfigManager(_T("scripts"))->Write(key, (int)sa.GetInt(3));
                return SQ_OK;
            }
            else if (sa.GetType(3) == OT_BOOL)
            {
                Manager::Get()->GetConfigManager(_T("scripts"))->Write(key, (bool)sa.GetBool(3));
                return SQ_OK;
            }
            else if (sa.GetType(3) == OT_FLOAT)
            {
                Manager::Get()->GetConfigManager(_T("scripts"))->Write(key, sa.GetFloat(3));
                return SQ_OK;
            }
            else
            {
                Manager::Get()->GetConfigManager(_T("scripts"))->Write(key, *SqPlus::GetInstance<wxString,false>(v, 3));
                return SQ_OK;
            }
        }
        else if (paramCount == 4)
        {
            wxString key = *SqPlus::GetInstance<wxString,false>(v, 2);
            wxString val = *SqPlus::GetInstance<wxString,false>(v, 3);
            if (sa.GetType(4) == OT_BOOL)
            {
                Manager::Get()->GetConfigManager(_T("scripts"))->Write(key, val, sa.GetBool(4));
                return SQ_OK;
            }
        }
        return sa.ThrowError("Invalid arguments to \"ConfigManager::Write\"");
    }
    SQInteger EditorManager_GetBuiltinEditor(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbEditor* ed = nullptr;
            if (sa.GetType(2) == OT_INTEGER)
                ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(sa.GetInt(2));
            else
                ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(*SqPlus::GetInstance<wxString,false>(v, 2));
            SqPlus::Push(v, ed);
            return 1;
        }
        return sa.ThrowError("Invalid arguments to \"EditorManager::GetBuiltinEditor\"");
    }
    SQInteger EditorManager_Open(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbEditor* ed = Manager::Get()->GetEditorManager()->Open(*SqPlus::GetInstance<wxString,false>(v, 2));
            SqPlus::Push(v, ed);
            return 1;
        }
        return sa.ThrowError("Invalid arguments to \"EditorManager::Open\"");
    }
    SQInteger EditorManager_Close(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            if (sa.GetType(2) == OT_INTEGER)
                return sa.Return(Manager::Get()->GetEditorManager()->Close(sa.GetInt(2)));
            else
                return sa.Return(Manager::Get()->GetEditorManager()->Close(*SqPlus::GetInstance<wxString,false>(v, 2)));
        }
        return sa.ThrowError("Invalid arguments to \"EditorManager::Close\"");
    }
    SQInteger EditorManager_Save(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            if (sa.GetType(2) == OT_INTEGER)
                return sa.Return(Manager::Get()->GetEditorManager()->Save(sa.GetInt(2)));
            else
                return sa.Return(Manager::Get()->GetEditorManager()->Save(*SqPlus::GetInstance<wxString,false>(v, 2)));
        }
        return sa.ThrowError("Invalid arguments to \"EditorManager::Save\"");
    }
    SQInteger cbProject_RemoveFile(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbProject* prj = SqPlus::GetInstance<cbProject,false>(v, 1);
            if (sa.GetType(2) == OT_INTEGER)
                return sa.ThrowError("Invalid arguments to \"cbProject::RemoveFile\"");
            else
                return sa.Return(prj->RemoveFile(SqPlus::GetInstance<ProjectFile,false>(v, 2)));
        }
        return sa.ThrowError("Invalid arguments to \"cbProject::RemoveFile\"");
    }
    SQInteger cbProject_AddFile(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount >= 3)
        {
            cbProject* prj = SqPlus::GetInstance<cbProject,false>(v, 1);
            wxString str = *SqPlus::GetInstance<wxString,false>(v, 3);
            bool b1 = paramCount >= 4 ? sa.GetBool(4) : true;
            bool b2 = paramCount >= 5 ? sa.GetBool(5) : true;
            int i = paramCount == 6 ? sa.GetInt(6) : 50;
            ProjectFile* pf = nullptr;
            if (sa.GetType(2) == OT_INTEGER)
                pf = prj->AddFile(sa.GetInt(2), str, b1, b2, i);
            else
                pf = prj->AddFile(*SqPlus::GetInstance<wxString,false>(v, 2), str, b1, b2, i);
            SqPlus::Push(v, pf);
            return 1;
        }
        return sa.ThrowError("Invalid arguments to \"cbProject::AddFile\"");
    }
    SQInteger cbProject_GetBuildTarget(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbProject* prj = SqPlus::GetInstance<cbProject,false>(v, 1);
            ProjectBuildTarget* bt = nullptr;
            if (sa.GetType(2) == OT_INTEGER)
                bt = prj->GetBuildTarget(sa.GetInt(2));
            else
                bt = prj->GetBuildTarget(*SqPlus::GetInstance<wxString,false>(v, 2));
            SqPlus::Push(v, bt);
            return 1;
        }
        return sa.ThrowError("Invalid arguments to \"cbProject::GetBuildTarget\"");
    }
    SQInteger cbProject_RenameBuildTarget(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 3)
        {
            cbProject* prj = SqPlus::GetInstance<cbProject,false>(v, 1);
            if (sa.GetType(2) == OT_INTEGER)
                return sa.Return(prj->RenameBuildTarget(sa.GetInt(2), *SqPlus::GetInstance<wxString,false>(v, 3)));
            else
                return sa.Return(prj->RenameBuildTarget(*SqPlus::GetInstance<wxString,false>(v, 2), *SqPlus::GetInstance<wxString,false>(v, 3)));
        }
        return sa.ThrowError("Invalid arguments to \"cbProject::RenameBuildTarget\"");
    }
    SQInteger cbProject_DuplicateBuildTarget(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 3)
        {
            cbProject* prj = SqPlus::GetInstance<cbProject,false>(v, 1);
            ProjectBuildTarget* bt = nullptr;
            if (sa.GetType(2) == OT_INTEGER)
                bt = prj->DuplicateBuildTarget(sa.GetInt(2), *SqPlus::GetInstance<wxString,false>(v, 3));
            else
                bt = prj->DuplicateBuildTarget(*SqPlus::GetInstance<wxString,false>(v, 2), *SqPlus::GetInstance<wxString,false>(v, 3));
            SqPlus::Push(v, bt);
            return 1;
        }
        return sa.ThrowError("Invalid arguments to \"cbProject::DuplicateBuildTarget\"");
    }
    SQInteger cbProject_RemoveBuildTarget(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbProject* prj = SqPlus::GetInstance<cbProject,false>(v, 1);
            if (sa.GetType(2) == OT_INTEGER)
                return sa.Return(prj->RemoveBuildTarget(sa.GetInt(2)));
            else
                return sa.Return(prj->RemoveBuildTarget(*SqPlus::GetInstance<wxString,false>(v, 2)));
        }
        return sa.ThrowError("Invalid arguments to \"cbProject::RemoveBuildTarget\"");
    }
    SQInteger cbProject_ExportTargetAsProject(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbProject* prj = SqPlus::GetInstance<cbProject,false>(v, 1);
            if (sa.GetType(2) == OT_INTEGER)
                return sa.Return(prj->ExportTargetAsProject(sa.GetInt(2)));
            else
                return sa.Return(prj->ExportTargetAsProject(*SqPlus::GetInstance<wxString,false>(v, 2)));
        }
        return sa.ThrowError("Invalid arguments to \"cbProject::ExportTargetAsProject\"");
    }

    struct FindXmlElementResult
    {
        TiXmlElement *element = nullptr;
        wxString errorStr;
    };

    static FindXmlElementResult FindExtensionElement(cbProject *project, const wxString &query)
    {
        TiXmlNode *extensionNode = project->GetExtensionsNode();
        if (!extensionNode)
            return FindXmlElementResult();
        TiXmlElement *currentElem = extensionNode->ToElement();
        if (!currentElem)
            return FindXmlElementResult();

        // Note: This is slow!
        const wxArrayString names = GetArrayFromString(query, wxT("/"), false);
        for (const wxString &name : names)
        {
            const wxString::size_type openBracePos = name.find_first_of(wxT("[("));
            if (openBracePos != wxString::npos)
            {
                if (name[openBracePos] == wxT('['))
                {
                    const wxString::size_type closeBracePos = name.find(wxT(']'), openBracePos + 1);
                    if (closeBracePos == wxString::npos || closeBracePos != name.length() - 1)
                    {
                        FindXmlElementResult result;
                        result.errorStr.Printf(wxT("Invalid index format in '%s'!"), name.wx_str());
                        return result;
                    }

                    const wxString nameStripped = name.substr(0, openBracePos);
                    long lIndex;
                    const wxString indexStr = name.substr(openBracePos + 1,
                                                          closeBracePos - openBracePos - 1);
                    if (!indexStr.ToLong(&lIndex))
                    {
                        FindXmlElementResult result;
                        result.errorStr.Printf(wxT("Can't convert '%s' to integer!"),
                                               indexStr.wx_str());
                        return result;
                    }

                    const int index = int(lIndex);

                    int currentIndex = -1;
                    for (TiXmlNode *child = currentElem->FirstChild();
                         child;
                         child = child->NextSibling())
                    {
                        TiXmlElement *childElement = child->ToElement();
                        if (wxString(childElement->Value(), wxConvUTF8) != nameStripped)
                            continue;
                        ++currentIndex;
                        if (currentIndex == index)
                        {
                            currentElem = childElement;
                            break;
                        }
                    }
                    if (currentIndex != index)
                        return FindXmlElementResult();
                }
                else if (name[openBracePos] == wxT('('))
                {
                    const wxString::size_type closeBracePos = name.find(wxT(')'), openBracePos + 1);
                    if (closeBracePos == wxString::npos || closeBracePos != name.length() - 1)
                    {
                        FindXmlElementResult result;
                        result.errorStr.Printf(wxT("Invalid attribute format in '%s'!"),
                                               name.wx_str());
                        return result;
                    }

                    const wxString nameStripped = name.substr(0, openBracePos);
                    const wxString attributeStr = name.substr(openBracePos + 1,
                                                           closeBracePos - openBracePos - 1);
                    const wxString::size_type equalPos = attributeStr.find(wxT('='));
                    if (equalPos == wxString::npos)
                    {
                        FindXmlElementResult result;
                        result.errorStr.Printf(wxT("Invalid attribute format in '%s'!"),
                                               attributeStr.wx_str());
                        return result;
                    }

                    const std::string attributeName = wxString(attributeStr.substr(0, equalPos)).utf8_str().data();
                    const std::string attributeValue = wxString(attributeStr.substr(equalPos + 1)).utf8_str().data();
                    for (TiXmlNode *child = currentElem->FirstChild();
                         child;
                         child = child->NextSibling())
                    {
                        TiXmlElement *childElement = child->ToElement();
                        if (wxString(childElement->Value(), wxConvUTF8) != nameStripped)
                            continue;

                        const char *value = childElement->Attribute(attributeName.c_str());
                        if (value != nullptr && attributeValue == value)
                        {
                            currentElem = childElement;
                            break;
                        }
                    }
                }
            }
            else
                currentElem = currentElem->FirstChildElement(name.utf8_str().data());

            if (!currentElem)
                return FindXmlElementResult();
        }

        FindXmlElementResult result;
        result.element = currentElem;
        return result;
    }

    wxString FindFullExtensionPathForNode(const TiXmlNode *node)
    {
        if (!node)
            return wxString();

        struct StackItem
        {
            const char *name;
            int index;
        };
        std::vector<StackItem> extensionStack;

        while (node)
        {
            const char *nodeValue = node->ToElement()->Value();
            if (strcmp(nodeValue, "Extensions") == 0)
                break;

            int index = 0;
            // Find index by going back through the siblings and matching the names.
            for (const TiXmlNode *sibling = node->PreviousSibling();
                 sibling;
                 sibling = sibling->PreviousSibling())
            {
                const char *value = sibling->ToElement()->Value();
                if (strcmp(nodeValue, value) == 0)
                    index++;
            }

            StackItem item;
            item.name = nodeValue;
            item.index = index;
            extensionStack.push_back(item);

            node = node->Parent();
        }

        wxString result;
        for (std::vector<StackItem>::reverse_iterator it = extensionStack.rbegin();
             it != extensionStack.rend();
             ++it)
        {
            if (!result.empty())
                result << wxT('/');
            result << wxString(it->name, wxConvUTF8) << wxT('[') << it->index << wxT(']');
        }
        return result;
    }

    /// Squirrel function would expect a cbProject and an extension string. It will return a
    /// wxArrayString object.
    SQInteger cbProject_ExtensionListNodes(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount != 2)
            return sa.ThrowError("Invalid arguments to \"cbProject::ExtensionListNodes\"");

        cbProject *project = SqPlus::GetInstance<cbProject, false>(v, 1);
        if (!project)
            return sa.ThrowError("Invalid project argument to \"cbProject::ExtensionListNodes\"");
        const wxString *extension = SqPlus::GetInstance<wxString, false>(v, 2);
        if (!extension)
            return sa.ThrowError("Invalid extension argument to \"cbProject::ExtensionListNodes\"");

        FindXmlElementResult queryResult = FindExtensionElement(project, *extension);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
                return SqPlus::ReturnCopy(v, wxArrayString());
            else
                return sa.ThrowError(queryResult.errorStr.utf8_str().data());
        }

        wxArrayString result;
        int index = 0;
        for (const TiXmlNode *child = queryResult.element->FirstChild();
             child;
             child = child->NextSibling())
        {
            wxString msg = *extension + wxT("/") + wxString(child->Value(), wxConvUTF8);
            msg << wxT('[') << index << wxT(']');
            result.Add(msg);
            ++index;
        }
        return SqPlus::ReturnCopy(v, result);
    }

    /// Squirrel function would expect a cbProject and an extension string. It will return a
    /// wxArrayString object.
    SQInteger cbProject_ExtensionListNodeAttributes(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount != 2)
            return sa.ThrowError("Invalid arguments to \"cbProject::ExtensionListNodeAttributes\"");

        cbProject *project = SqPlus::GetInstance<cbProject, false>(v, 1);
        if (!project)
            return sa.ThrowError("Invalid project argument to \"cbProject::ExtensionListNodeAttributes\"");
        const wxString *extension = SqPlus::GetInstance<wxString, false>(v, 2);
        if (!extension)
            return sa.ThrowError("Invalid extension argument to \"cbProject::ExtensionListNodeAttributes\"");

        FindXmlElementResult queryResult = FindExtensionElement(project, *extension);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
                return SqPlus::ReturnCopy(v, wxArrayString());
            else
                return sa.ThrowError(queryResult.errorStr.utf8_str().data());
        }

        wxArrayString result;
        for (const TiXmlAttribute *attr = queryResult.element->FirstAttribute();
             attr;
             attr = attr->Next())
        {
            result.Add(wxString(attr->Name(), wxConvUTF8));
        }
        return SqPlus::ReturnCopy(v, result);
    }

    /// Squirrel function would expect a cbProject, an extension string and attribute name. It will
    /// return a wxString object.
    SQInteger cbProject_ExtensionGetNodeAttribute(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount != 3)
            return sa.ThrowError("Invalid arguments to \"cbProject::ExtensionGetNodeAttribute\"");

        cbProject *project = SqPlus::GetInstance<cbProject, false>(v, 1);
        if (!project)
            return sa.ThrowError("Invalid project argument to \"cbProject::ExtensionGetNodeAttribute\"");
        const wxString *extension = SqPlus::GetInstance<wxString, false>(v, 2);
        if (!extension)
            return sa.ThrowError("Invalid extension argument to \"cbProject::ExtensionGetNodeAttribute\"");
        const wxString *attributeName = SqPlus::GetInstance<wxString, false>(v, 3);
        if (!attributeName)
            return sa.ThrowError("Invalid attribute name argument to \"cbProject::ExtensionGetNodeAttribute\"");

        FindXmlElementResult queryResult = FindExtensionElement(project, *extension);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
                return SqPlus::ReturnCopy(v, wxString());
            else
                return sa.ThrowError(queryResult.errorStr.utf8_str().data());
        }

        wxString result;
        const char *attributeValue = queryResult.element->Attribute(attributeName->utf8_str().data());
        if (attributeValue)
            result = wxString(attributeValue, wxConvUTF8);
        return SqPlus::ReturnCopy(v, result);
    }

    /// Squirrel function would expect a cbProject, an extension string, attribute name and
    /// attribute value.
    SQInteger cbProject_ExtensionSetNodeAttribute(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount != 4)
            return sa.ThrowError("Invalid arguments to \"cbProject::ExtensionSetNodeAttribute\"");

        cbProject *project = SqPlus::GetInstance<cbProject, false>(v, 1);
        if (!project)
            return sa.ThrowError("Invalid project argument to \"cbProject::ExtensionSetNodeAttribute\"");
        const wxString *extension = SqPlus::GetInstance<wxString, false>(v, 2);
        if (!extension)
            return sa.ThrowError("Invalid extension argument to \"cbProject::ExtensionSetNodeAttribute\"");
        const wxString *attributeName = SqPlus::GetInstance<wxString, false>(v, 3);
        if (!attributeName)
            return sa.ThrowError("Invalid attribute name argument to \"cbProject::ExtensionSetNodeAttribute\"");
        const wxString *attributeValue = SqPlus::GetInstance<wxString, false>(v, 4);
        if (!attributeName)
            return sa.ThrowError("Invalid attribute value argument to \"cbProject::ExtensionSetNodeAttribute\"");

        FindXmlElementResult queryResult = FindExtensionElement(project, *extension);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extension->wx_str());
            }
            return sa.ThrowError(queryResult.errorStr.utf8_str().data());
        }

        queryResult.element->SetAttribute(attributeName->utf8_str().data(),
                                          attributeValue->utf8_str().data());
        project->SetModified(true);
        return sa.Return();
    }

    /// Squirrel function would expect a cbProject, an extension string, attribute name.
    SQInteger cbProject_ExtensionRemoveNodeAttribute(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount != 3)
            return sa.ThrowError("Invalid arguments to \"cbProject::ExtensionRemoveNodeAttribute\"");

        cbProject *project = SqPlus::GetInstance<cbProject, false>(v, 1);
        if (!project)
            return sa.ThrowError("Invalid project argument to \"cbProject::ExtensionRemoveNodeAttribute\"");
        const wxString *extension = SqPlus::GetInstance<wxString, false>(v, 2);
        if (!extension)
            return sa.ThrowError("Invalid extension argument to \"cbProject::ExtensionRemoveNodeAttribute\"");
        const wxString *attributeName = SqPlus::GetInstance<wxString, false>(v, 3);
        if (!attributeName)
            return sa.ThrowError("Invalid attribute name argument to \"cbProject::ExtensionRemoveNodeAttribute\"");

        FindXmlElementResult queryResult = FindExtensionElement(project, *extension);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extension->wx_str());
            }
            return sa.ThrowError(queryResult.errorStr.utf8_str().data());
        }

        queryResult.element->RemoveAttribute(attributeName->utf8_str().data());
        project->SetModified(true);
        return sa.Return();
    }

    /// Squirrel function would expect a cbProject, an extension string and node name.
    /// It will return the extension of the newly created node, so it could be used in other
    /// node calls.
    SQInteger cbProject_ExtensionAddNode(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount != 3)
            return sa.ThrowError("Invalid arguments to \"cbProject::ExtensionAddNode\"");

        cbProject *project = SqPlus::GetInstance<cbProject, false>(v, 1);
        if (!project)
            return sa.ThrowError("Invalid project argument to \"cbProject::ExtensionAddNode\"");
        const wxString *extension = SqPlus::GetInstance<wxString, false>(v, 2);
        if (!extension)
            return sa.ThrowError("Invalid extension argument to \"cbProject::ExtensionAddNode\"");
        const wxString *nodeName = SqPlus::GetInstance<wxString, false>(v, 3);
        if (!nodeName)
            return sa.ThrowError("Invalid node name argument to \"cbProject::ExtensionAddNode\"");

        FindXmlElementResult queryResult = FindExtensionElement(project, *extension);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extension->wx_str());
            }
            return sa.ThrowError(queryResult.errorStr.utf8_str().data());
        }

        TiXmlNode *newNode = queryResult.element->InsertEndChild(TiXmlElement(cbU2C(*nodeName)));
        const wxString resultExtension = FindFullExtensionPathForNode(newNode);
        project->SetModified(true);
        return SqPlus::ReturnCopy(v, resultExtension);
    }

    /// Squirrel function would expect a cbProject and extension string.
    SQInteger cbProject_ExtensionRemoveNode(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount != 2)
            return sa.ThrowError("Invalid arguments to \"cbProject::ExtensionRemoveNode\"");

        cbProject *project = SqPlus::GetInstance<cbProject, false>(v, 1);
        if (!project)
            return sa.ThrowError("Invalid project argument to \"cbProject::ExtensionRemoveNode\"");
        const wxString *extension = SqPlus::GetInstance<wxString, false>(v, 2);
        if (!extension)
            return sa.ThrowError("Invalid extension argument to \"cbProject::ExtensionRemoveNode\"");

        FindXmlElementResult queryResult = FindExtensionElement(project, *extension);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extension->wx_str());
            }
            return sa.ThrowError(queryResult.errorStr.utf8_str().data());
        }

        TiXmlNode *parent = queryResult.element->Parent();
        parent->RemoveChild(queryResult.element);
        project->SetModified(true);
        return sa.Return();
    }

    SQInteger ProjectManager_AddFileToProject(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 4)
        {
            if (sa.GetType(4) == OT_INTEGER)
            {
                wxString fname = *SqPlus::GetInstance<wxString,false>(v, 2);
                cbProject* prj = SqPlus::GetInstance<cbProject,false>(v, 3);
                int idx = sa.GetInt(4);
                return sa.Return((SQInteger)Manager::Get()->GetProjectManager()->AddFileToProject(fname, prj, idx));
            }
        }
        return sa.ThrowError("Invalid arguments to \"ProjectManager::AddFileToProject\"");
    }

    SQInteger ProjectManager_GetProjectCount(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int count = sa.GetParamCount();
        if (count != 1)
            return sa.ThrowError("Invalid arguments to \"ProjectManager::GetProjectCount\"");
        else
        {
            ProjectManager *manager = SqPlus::GetInstance<ProjectManager, false>(v, 1);
            int project_count = manager->GetProjects()->GetCount();
            return sa.Return((SQInteger)project_count);
        }
    }

    SQInteger ProjectManager_GetProject(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int count = sa.GetParamCount();
        if (count != 2)
            return sa.ThrowError("Invalid arguments to \"ProjectManager::GetProject\"");
        else
        {
            ProjectManager *manager = SqPlus::GetInstance<ProjectManager, false>(v, 1);
            int index = sa.GetInt(2);
            int project_count = manager->GetProjects()->GetCount();
            if(index >= project_count)
                return sa.ThrowError("Index out of bounds in \"ProjectManager::GetProject\"");
            else
            {
                cbProject *project = (*manager->GetProjects())[index];
                SqPlus::Push(v, project);
                return 1;
            }
        }
    }
    SQInteger ProjectManager_RebuildTree(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 1)
        {
            ProjectManager *manager = SqPlus::GetInstance<ProjectManager, false>(v, 1);
            if (manager)
            {
                manager->GetUI().RebuildTree();
                return sa.Return();
            }
            return sa.ThrowError("'this' is NULL!?! (type of ProjectManager*)");
        }
        return sa.ThrowError("Invalid arguments to \"ProjectManager::RebuildTree\"");
    }

    SQInteger cbEditor_SetText(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbEditor* self = SqPlus::GetInstance<cbEditor,false>(v, 1);
            if (self)
            {
                self->GetControl()->SetText(*SqPlus::GetInstance<wxString,false>(v, 2));
                return sa.Return();
            }
            return sa.ThrowError("'this' is NULL!?! (type of cbEditor*)");
        }
        return sa.ThrowError("Invalid arguments to \"cbEditor::SetText\"");
    }
    SQInteger cbEditor_GetText(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 1)
        {
            cbEditor* self = SqPlus::GetInstance<cbEditor,false>(v, 1);
            if (self)
            {
                wxString str = self->GetControl()->GetText();
                return SqPlus::ReturnCopy(v, str);
            }
            return sa.ThrowError("'this' is NULL!?! (type of cbEditor*)");
        }
        return sa.ThrowError("Invalid arguments to \"cbEditor::GetText\"");
    }
    SQInteger CompilerFactory_GetCompilerIndex(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
            return sa.Return((SQInteger)CompilerFactory::GetCompilerIndex(*SqPlus::GetInstance<wxString,false>(v, 2)));
        return sa.ThrowError("Invalid arguments to \"CompilerFactory::GetCompilerIndex\"");
    }

    wxString CompilerFactory_GetCompilerIDByName(const wxString &name)
    {
        Compiler *compiler = CompilerFactory::GetCompilerByName(name);
        return (compiler ? compiler->GetID() : wxString(wxEmptyString));
    }

    void RegisterBindings()
    {
        if (!SquirrelVM::GetVMPtr())
            cbThrow(_T("Scripting engine not initialized!?"));

        Register_wxTypes();
        Register_Constants();
        Register_Globals();
        Register_IO(); // IO is enabled, but just for harmless functions
        Register_Dialog();
        Register_ProgressDialog();
        Register_UtilDialogs();

        SqPlus::SQClassDef<ConfigManager>("ConfigManager").
                staticFuncVarArgs(&ConfigManager_Read, "Read", "*").
                staticFuncVarArgs(&ConfigManager_Write, "Write", "*");

        SqPlus::SQClassDef<ProjectFile>("ProjectFile").
                func(&ProjectFile::AddBuildTarget, "AddBuildTarget").
                func(&ProjectFile::RenameBuildTarget, "RenameBuildTarget").
                func(&ProjectFile::RemoveBuildTarget, "RemoveBuildTarget").
                func(&ProjectFile::GetBuildTargets, "GetBuildTargets").
                func(&ProjectFile::GetBaseName, "GetBaseName").
                func(&ProjectFile::GetObjName, "GetObjName").
                func(&ProjectFile::SetObjName, "SetObjName").
                func(&ProjectFile::GetParentProject, "GetParentProject").
                func(&ProjectFile::SetUseCustomBuildCommand, "SetUseCustomBuildCommand").
                func(&ProjectFile::SetCustomBuildCommand, "SetCustomBuildCommand").
                func(&ProjectFile::GetUseCustomBuildCommand, "GetUseCustomBuildCommand").
                func(&ProjectFile::GetCustomBuildCommand, "GetCustomBuildCommand").
                var(&ProjectFile::file, "file").
                var(&ProjectFile::relativeFilename, "relativeFilename").
                var(&ProjectFile::relativeToCommonTopLevelPath, "relativeToCommonTopLevelPath").
                var(&ProjectFile::compile, "compile").
                var(&ProjectFile::link, "link").
                var(&ProjectFile::weight, "weight").
                var(&ProjectFile::compilerVar, "compilerVar").
                var(&ProjectFile::buildTargets, "buildTargets");

        SqPlus::SQClassDef<CompileOptionsBase>("CompileOptionsBase").
                func(&CompileOptionsBase::AddPlatform, "AddPlatform").
                func(&CompileOptionsBase::RemovePlatform, "RemovePlatform").
                func(&CompileOptionsBase::SetPlatforms, "SetPlatforms").
                func(&CompileOptionsBase::GetPlatforms, "GetPlatforms").
                func(&CompileOptionsBase::SupportsCurrentPlatform, "SupportsCurrentPlatform").
                func(&CompileOptionsBase::SetLinkerOptions, "SetLinkerOptions").
                func(&CompileOptionsBase::SetLinkLibs, "SetLinkLibs").
                func(&CompileOptionsBase::SetLinkerExecutable, "SetLinkerExecutable").
                func(&CompileOptionsBase::GetLinkerExecutable, "GetLinkerExecutable").
                func(&CompileOptionsBase::SetCompilerOptions, "SetCompilerOptions").
                func(&CompileOptionsBase::SetResourceCompilerOptions, "SetResourceCompilerOptions").
                func(&CompileOptionsBase::SetIncludeDirs, "SetIncludeDirs").
                func(&CompileOptionsBase::SetResourceIncludeDirs, "SetResourceIncludeDirs").
                func(&CompileOptionsBase::SetLibDirs, "SetLibDirs").
                func(&CompileOptionsBase::SetCommandsBeforeBuild, "SetCommandsBeforeBuild").
                func(&CompileOptionsBase::SetCommandsAfterBuild, "SetCommandsAfterBuild").
                func(&CompileOptionsBase::GetLinkerOptions, "GetLinkerOptions").
                func(&CompileOptionsBase::GetLinkLibs, "GetLinkLibs").
                func(&CompileOptionsBase::GetCompilerOptions, "GetCompilerOptions").
                func(&CompileOptionsBase::GetResourceCompilerOptions, "GetResourceCompilerOptions").
                func(&CompileOptionsBase::GetIncludeDirs, "GetIncludeDirs").
                func(&CompileOptionsBase::GetResourceIncludeDirs, "GetResourceIncludeDirs").
                func(&CompileOptionsBase::GetLibDirs, "GetLibDirs").
                func(&CompileOptionsBase::GetCommandsBeforeBuild, "GetCommandsBeforeBuild").
                func(&CompileOptionsBase::GetCommandsAfterBuild, "GetCommandsAfterBuild").
                func(&CompileOptionsBase::GetModified, "GetModified").
                func(&CompileOptionsBase::SetModified, "SetModified").
                func(&CompileOptionsBase::AddLinkerOption, "AddLinkerOption").
                func(&CompileOptionsBase::AddLinkLib, "AddLinkLib").
                func(&CompileOptionsBase::AddCompilerOption, "AddCompilerOption").
                func(&CompileOptionsBase::AddResourceCompilerOption, "AddResourceCompilerOption").
                func(&CompileOptionsBase::AddIncludeDir, "AddIncludeDir").
                func(&CompileOptionsBase::AddResourceIncludeDir, "AddResourceIncludeDir").
                func(&CompileOptionsBase::AddLibDir, "AddLibDir").
                func(&CompileOptionsBase::AddCommandsBeforeBuild, "AddCommandsBeforeBuild").
                func(&CompileOptionsBase::AddCommandsAfterBuild, "AddCommandsAfterBuild").
                func(&CompileOptionsBase::ReplaceLinkerOption, "ReplaceLinkerOption").
                func(&CompileOptionsBase::ReplaceLinkLib, "ReplaceLinkLib").
                func(&CompileOptionsBase::ReplaceCompilerOption, "ReplaceCompilerOption").
                func(&CompileOptionsBase::ReplaceResourceCompilerOption, "ReplaceResourceCompilerOption").
                func(&CompileOptionsBase::ReplaceIncludeDir, "ReplaceIncludeDir").
                func(&CompileOptionsBase::ReplaceResourceIncludeDir, "ReplaceResourceIncludeDir").
                func(&CompileOptionsBase::ReplaceLibDir, "ReplaceLibDir").
                func(&CompileOptionsBase::RemoveLinkerOption, "RemoveLinkerOption").
                func(&CompileOptionsBase::RemoveLinkLib, "RemoveLinkLib").
                func(&CompileOptionsBase::RemoveCompilerOption, "RemoveCompilerOption").
                func(&CompileOptionsBase::RemoveIncludeDir, "RemoveIncludeDir").
                func(&CompileOptionsBase::RemoveResourceCompilerOption, "RemoveResourceCompilerOption").
                func(&CompileOptionsBase::RemoveResourceIncludeDir, "RemoveResourceIncludeDir").
                func(&CompileOptionsBase::RemoveLibDir, "RemoveLibDir").
                func(&CompileOptionsBase::RemoveCommandsBeforeBuild, "RemoveCommandsBeforeBuild").
                func(&CompileOptionsBase::RemoveCommandsAfterBuild, "RemoveCommandsAfterBuild").
                func(&CompileOptionsBase::GetAlwaysRunPostBuildSteps, "GetAlwaysRunPostBuildSteps").
                func(&CompileOptionsBase::SetAlwaysRunPostBuildSteps, "SetAlwaysRunPostBuildSteps").
                func(&CompileOptionsBase::SetBuildScripts, "SetBuildScripts").
                func(&CompileOptionsBase::GetBuildScripts, "GetBuildScripts").
                func(&CompileOptionsBase::AddBuildScript, "AddBuildScript").
                func(&CompileOptionsBase::RemoveBuildScript, "RemoveBuildScript").
                func(&CompileOptionsBase::SetVar, "SetVar").
                func(&CompileOptionsBase::GetVar, "GetVar").
                func(&CompileOptionsBase::UnsetVar, "UnsetVar").
                func(&CompileOptionsBase::UnsetAllVars, "UnsetAllVars");

        SqPlus::SQClassDef<CompileTargetBase>("CompileTargetBase", "CompileOptionsBase").
                func(&CompileTargetBase::SetTargetFilenameGenerationPolicy, "SetTargetFilenameGenerationPolicy").
//                func(&CompileTargetBase::GetTargetFilenameGenerationPolicy, "GetTargetFilenameGenerationPolicy"). // not exposed because its args are non-const references
                func(&CompileTargetBase::GetFilename, "GetFilename").
                func(&CompileTargetBase::GetTitle, "GetTitle").
                func(&CompileTargetBase::SetTitle, "SetTitle").
                func(&CompileTargetBase::SetOutputFilename, "SetOutputFilename").
                func(&CompileTargetBase::SetWorkingDir, "SetWorkingDir").
                func(&CompileTargetBase::SetObjectOutput, "SetObjectOutput").
                func(&CompileTargetBase::SetDepsOutput, "SetDepsOutput").
                func(&CompileTargetBase::GetOptionRelation, "GetOptionRelation").
                func(&CompileTargetBase::SetOptionRelation, "SetOptionRelation").
                func(&CompileTargetBase::GetWorkingDir, "GetWorkingDir").
                func(&CompileTargetBase::GetObjectOutput, "GetObjectOutput").
                func(&CompileTargetBase::GetDepsOutput, "GetDepsOutput").
                func(&CompileTargetBase::GetOutputFilename, "GetOutputFilename").
                func(&CompileTargetBase::SuggestOutputFilename, "SuggestOutputFilename").
                func(&CompileTargetBase::GetExecutableFilename, "GetExecutableFilename").
                func(&CompileTargetBase::GetDynamicLibFilename, "GetDynamicLibFilename").
                func(&CompileTargetBase::GetDynamicLibDefFilename, "GetDynamicLibDefFilename").
                func(&CompileTargetBase::GetStaticLibFilename, "GetStaticLibFilename").
                func(&CompileTargetBase::GetBasePath, "GetBasePath").
                func(&CompileTargetBase::SetTargetType, "SetTargetType").
                func(&CompileTargetBase::GetTargetType, "GetTargetType").
                func(&CompileTargetBase::GetExecutionParameters, "GetExecutionParameters").
                func(&CompileTargetBase::SetExecutionParameters, "SetExecutionParameters").
                func(&CompileTargetBase::GetHostApplication, "GetHostApplication").
                func(&CompileTargetBase::SetHostApplication, "SetHostApplication").
                func(&CompileTargetBase::SetCompilerID, "SetCompilerID").
                func(&CompileTargetBase::GetCompilerID, "GetCompilerID").
                func(&CompileTargetBase::GetMakeCommandFor, "GetMakeCommandFor").
                func(&CompileTargetBase::SetMakeCommandFor, "SetMakeCommandFor").
                func(&CompileTargetBase::MakeCommandsModified, "MakeCommandsModified");

        SqPlus::SQClassDef<ProjectBuildTarget>("ProjectBuildTarget", "CompileTargetBase").
                func<const cbProject* (ProjectBuildTarget::*)() const>(&ProjectBuildTarget::GetParentProject, "GetParentProject").
                func(&ProjectBuildTarget::GetFullTitle, "GetFullTitle").
                func(&ProjectBuildTarget::GetExternalDeps, "GetExternalDeps").
                func(&ProjectBuildTarget::SetExternalDeps, "SetExternalDeps").
                func(&ProjectBuildTarget::SetAdditionalOutputFiles, "SetAdditionalOutputFiles").
                func(&ProjectBuildTarget::GetAdditionalOutputFiles, "GetAdditionalOutputFiles").
                func(&ProjectBuildTarget::GetIncludeInTargetAll, "GetIncludeInTargetAll").
                func(&ProjectBuildTarget::SetIncludeInTargetAll, "SetIncludeInTargetAll").
                func(&ProjectBuildTarget::GetCreateDefFile, "GetCreateDefFile").
                func(&ProjectBuildTarget::SetCreateDefFile, "SetCreateDefFile").
                func(&ProjectBuildTarget::GetCreateStaticLib, "GetCreateStaticLib").
                func(&ProjectBuildTarget::SetCreateStaticLib, "SetCreateStaticLib").
                func(&ProjectBuildTarget::GetUseConsoleRunner, "GetUseConsoleRunner").
                func(&ProjectBuildTarget::SetUseConsoleRunner, "SetUseConsoleRunner").
                func(&ProjectBuildTarget::GetFilesCount, "GetFilesCount").
                func(&ProjectBuildTarget::GetFile, "GetFile");

        SqPlus::SQClassDef<cbProject>("cbProject", "CompileTargetBase").
                func(&cbProject::GetModified, "GetModified").
                func(&cbProject::SetModified, "SetModified").
                func(&cbProject::GetMakefile, "GetMakefile").
                func(&cbProject::SetMakefile, "SetMakefile").
                func(&cbProject::IsMakefileCustom, "IsMakefileCustom").
                func(&cbProject::SetMakefileCustom, "SetMakefileCustom").
                func(&cbProject::CloseAllFiles, "CloseAllFiles").
                func(&cbProject::SaveAllFiles, "SaveAllFiles").
                func(&cbProject::Save, "Save").
//                func(&cbProject::SaveAs, "SaveAs"). // *UNSAFE*
                func(&cbProject::SaveLayout, "SaveLayout").
                func(&cbProject::LoadLayout, "LoadLayout").
//                func(&cbProject::ShowOptions, "ShowOptions").
                func(&cbProject::GetCommonTopLevelPath, "GetCommonTopLevelPath").
                func(&cbProject::GetFilesCount, "GetFilesCount").
                func(&cbProject::GetFile, "GetFile").
                func(&cbProject::GetFileByFilename, "GetFileByFilename").
                staticFuncVarArgs(&cbProject_RemoveFile, "RemoveFile", "*").
                staticFuncVarArgs(&cbProject_AddFile, "AddFile", "*").
                func(&cbProject::GetBuildTargetsCount, "GetBuildTargetsCount").
                staticFuncVarArgs(&cbProject_GetBuildTarget, "GetBuildTarget", "*").
                func(&cbProject::AddBuildTarget, "AddBuildTarget").
                staticFuncVarArgs(&cbProject_RenameBuildTarget, "RenameBuildTarget", "*").
                staticFuncVarArgs(&cbProject_DuplicateBuildTarget, "DuplicateBuildTarget", "*").
                staticFuncVarArgs(&cbProject_RemoveBuildTarget, "RemoveBuildTarget", "*").
                staticFuncVarArgs(&cbProject_ExportTargetAsProject, "ExportTargetAsProject", "*").
                func(&cbProject::BuildTargetValid, "BuildTargetValid").
                func(&cbProject::GetFirstValidBuildTargetName, "GetFirstValidBuildTargetName").
                func(&cbProject::SetDefaultExecuteTarget, "SetDefaultExecuteTarget").
                func(&cbProject::GetDefaultExecuteTarget, "GetDefaultExecuteTarget").
                func(&cbProject::SetActiveBuildTarget, "SetActiveBuildTarget").
                func(&cbProject::GetActiveBuildTarget, "GetActiveBuildTarget").
                func(&cbProject::SelectTarget, "SelectTarget").
                func<const ProjectBuildTarget* (cbProject::*)() const>(&cbProject::GetCurrentlyCompilingTarget, "GetCurrentlyCompilingTarget").
                func(&cbProject::SetCurrentlyCompilingTarget, "SetCurrentlyCompilingTarget").
                func(&cbProject::GetModeForPCH, "GetModeForPCH").
                func(&cbProject::SetModeForPCH, "SetModeForPCH").
                func(&cbProject::SetExtendedObjectNamesGeneration, "SetExtendedObjectNamesGeneration").
                func(&cbProject::GetExtendedObjectNamesGeneration, "GetExtendedObjectNamesGeneration").
                func(&cbProject::SetNotes, "SetNotes").
                func(&cbProject::GetNotes, "GetNotes").
                func(&cbProject::SetShowNotesOnLoad, "SetShowNotesOnLoad").
                func(&cbProject::GetShowNotesOnLoad, "GetShowNotesOnLoad").
                func(&cbProject::SetCheckForExternallyModifiedFiles, "SetCheckForExternallyModifiedFiles").
                func(&cbProject::GetCheckForExternallyModifiedFiles, "GetCheckForExternallyModifiedFiles").
                func(&cbProject::ShowNotes, "ShowNotes").
                func(&cbProject::AddToExtensions, "AddToExtensions").
                staticFuncVarArgs(cbProject_ExtensionListNodes, "ExtensionListNodes", "*").
                staticFuncVarArgs(cbProject_ExtensionListNodeAttributes, "ExtensionListNodeAttributes", "*").
                staticFuncVarArgs(cbProject_ExtensionGetNodeAttribute, "ExtensionGetNodeAttribute", "*").
                staticFuncVarArgs(cbProject_ExtensionSetNodeAttribute, "ExtensionSetNodeAttribute", "*").
                staticFuncVarArgs(cbProject_ExtensionRemoveNodeAttribute, "ExtensionRemoveNodeAttribute", "*").
                staticFuncVarArgs(cbProject_ExtensionAddNode, "ExtensionAddNode", "*").
                staticFuncVarArgs(cbProject_ExtensionRemoveNode, "ExtensionRemoveNode", "*").
                func(&cbProject::DefineVirtualBuildTarget, "DefineVirtualBuildTarget").
                func(&cbProject::HasVirtualBuildTarget, "HasVirtualBuildTarget").
                func(&cbProject::RemoveVirtualBuildTarget, "RemoveVirtualBuildTarget").
                func(&cbProject::GetVirtualBuildTargets, "GetVirtualBuildTargets").
                func(&cbProject::GetVirtualBuildTargetGroup, "GetVirtualBuildTargetGroup").
                func(&cbProject::GetExpandedVirtualBuildTargetGroup, "GetExpandedVirtualBuildTargetGroup").
                func(&cbProject::CanAddToVirtualBuildTarget, "CanAddToVirtualBuildTarget").
                func(&cbProject::SetTitle, "SetTitle");


        SqPlus::SQClassDef<ProjectManager>("ProjectManager").
                func(&ProjectManager::GetDefaultPath, "GetDefaultPath").
                func(&ProjectManager::SetDefaultPath, "SetDefaultPath").
                func(&ProjectManager::GetActiveProject, "GetActiveProject").
                staticFuncVarArgs(&ProjectManager_GetProjectCount, "GetProjectCount", "*").
                staticFuncVarArgs(&ProjectManager_GetProject, "GetProject", "*").
                func(&ProjectManager::SetProject, "SetProject").
                func(&ProjectManager::LoadWorkspace, "LoadWorkspace").
                func(&ProjectManager::SaveWorkspace, "SaveWorkspace").
                func(&ProjectManager::SaveWorkspaceAs, "SaveWorkspaceAs").
                func(&ProjectManager::CloseWorkspace, "CloseWorkspace").
                func(&ProjectManager::IsOpen, "IsOpen").
                func(&ProjectManager::LoadProject, "LoadProject").
                func(&ProjectManager::SaveProject, "SaveProject").
                func(&ProjectManager::SaveProjectAs, "SaveProjectAs").
                func(&ProjectManager::SaveActiveProject, "SaveActiveProject").
                func(&ProjectManager::SaveActiveProjectAs, "SaveActiveProjectAs").
                func(&ProjectManager::SaveAllProjects, "SaveAllProjects").
                func(&ProjectManager::CloseProject, "CloseProject").
                func(&ProjectManager::CloseActiveProject, "CloseActiveProject").
                func(&ProjectManager::CloseAllProjects, "CloseAllProjects").
                func(&ProjectManager::NewProject, "NewProject").
                staticFuncVarArgs(&ProjectManager_AddFileToProject, "AddFileToProject", "*").
//                func(&ProjectManager::AskForBuildTargetIndex, "AskForBuildTargetIndex").
                func(&ProjectManager::AddProjectDependency, "AddProjectDependency").
                func(&ProjectManager::RemoveProjectDependency, "RemoveProjectDependency").
                func(&ProjectManager::ClearProjectDependencies, "ClearProjectDependencies").
                func(&ProjectManager::RemoveProjectFromAllDependencies, "RemoveProjectFromAllDependencies").
                func(&ProjectManager::GetDependenciesForProject, "GetDependenciesForProject").
//                func(&ProjectManager::ConfigureProjectDependencies, "ConfigureProjectDependencies");
                staticFuncVarArgs(&ProjectManager_RebuildTree, "RebuildTree", "*");

        SqPlus::SQClassDef<EditorBase>("EditorBase").
                func(&EditorBase::GetFilename, "GetFilename").
                func(&EditorBase::SetFilename, "SetFilename").
                func(&EditorBase::GetShortName, "GetShortName").
                func(&EditorBase::GetModified, "GetModified").
                func(&EditorBase::SetModified, "SetModified").
                func(&EditorBase::GetTitle, "GetTitle").
                func(&EditorBase::SetTitle, "SetTitle").
                func(&EditorBase::Activate, "Activate").
                func(&EditorBase::Close, "Close").
                func(&EditorBase::Save, "Save").
                func(&EditorBase::IsBuiltinEditor, "IsBuiltinEditor").
                func(&EditorBase::ThereAreOthers, "ThereAreOthers").
                func(&EditorBase::GotoLine, "GotoLine").
                func(&EditorBase::Undo, "Undo").
                func(&EditorBase::Redo, "Redo").
                func(&EditorBase::Cut, "Cut").
                func(&EditorBase::Copy, "Copy").
                func(&EditorBase::Paste, "Paste").
                func(&EditorBase::CanUndo, "CanUndo").
                func(&EditorBase::CanRedo, "CanRedo").
                func(&EditorBase::CanPaste, "CanPaste").
                func(&EditorBase::IsReadOnly, "IsReadOnly").
                func(&EditorBase::HasSelection, "HasSelection");

        SqPlus::SQClassDef<cbEditor>("cbEditor", "EditorBase").
                func(&cbEditor::SetEditorTitle, "SetEditorTitle").
                func(&cbEditor::GetProjectFile, "GetProjectFile").
                func(&cbEditor::Save, "Save").
                func(&cbEditor::SaveAs, "SaveAs").
                func(&cbEditor::FoldAll, "FoldAll").
                func(&cbEditor::UnfoldAll, "UnfoldAll").
                func(&cbEditor::ToggleAllFolds, "ToggleAllFolds").
                func(&cbEditor::FoldBlockFromLine, "FoldBlockFromLine").
                func(&cbEditor::UnfoldBlockFromLine, "UnfoldBlockFromLine").
                func(&cbEditor::ToggleFoldBlockFromLine, "ToggleFoldBlockFromLine").
                func(&cbEditor::GetLineIndentInSpaces, "GetLineIndentInSpaces").
                func(&cbEditor::GetLineIndentString, "GetLineIndentString").
                func(&cbEditor::Touch, "Touch").
                func(&cbEditor::Reload, "Reload").
                func(&cbEditor::Print, "Print").
                func(&cbEditor::AutoComplete, "AutoComplete").
                func(&cbEditor::AddBreakpoint, "AddBreakpoint").
                func(&cbEditor::RemoveBreakpoint, "RemoveBreakpoint").
                func(&cbEditor::ToggleBookmark, "ToggleBookmark").
                func(&cbEditor::HasBookmark, "HasBookmark").
                func(&cbEditor::GotoNextBookmark, "GotoNextBookmark").
                func(&cbEditor::GotoPreviousBookmark, "GotoPreviousBookmark").
                func(&cbEditor::ClearAllBookmarks, "ClearAllBookmarks").
                func(&cbEditor::ToggleBreakpoint, "ToggleBreakpoint").
                func(&cbEditor::HasBreakpoint, "HasBreakpoint").
                func(&cbEditor::GotoNextBreakpoint, "GotoNextBreakpoint").
                func(&cbEditor::GotoPreviousBreakpoint, "GotoPreviousBreakpoint").


                // these are not present in cbEditor; included to help scripts edit text
                staticFuncVarArgs(&cbEditor_SetText, "SetText", "*").
                staticFuncVarArgs(&cbEditor_GetText, "GetText", "*");

        SqPlus::SQClassDef<EditorManager>("EditorManager").
                func(&EditorManager::New, "New").
                staticFuncVarArgs(&EditorManager_Open, "Open").
                func(&EditorManager::IsBuiltinOpen, "IsBuiltinOpen").
                staticFuncVarArgs(&EditorManager_GetBuiltinEditor, "GetBuiltinEditor", "*").
                func(&EditorManager::GetBuiltinActiveEditor, "GetBuiltinActiveEditor").
                func(&EditorManager::GetActiveEditor, "GetActiveEditor").
                func(&EditorManager::ActivateNext, "ActivateNext").
                func(&EditorManager::ActivatePrevious, "ActivatePrevious").
                func(&EditorManager::SwapActiveHeaderSource, "SwapActiveHeaderSource").
                func(&EditorManager::CloseActive, "CloseActive").
                staticFuncVarArgs(&EditorManager_Close, "Close", "*").
                func(&EditorManager::CloseAll, "CloseAll").
                staticFuncVarArgs(&EditorManager_Save, "Save", "*").
                func(&EditorManager::SaveActive, "SaveActive").
                func(&EditorManager::SaveAs, "SaveAs").
                func(&EditorManager::SaveActiveAs, "SaveActiveAs").
                func(&EditorManager::SaveAll, "SaveAll");
        //        func(&EditorManager::ShowFindDialog, "ShowFindDialog");

        SqPlus::SQClassDef<UserVariableManager>("UserVariableManager").
                func(&UserVariableManager::Exists, "Exists");

        SqPlus::SQClassDef<ScriptingManager>("ScriptingManager").
                func(&ScriptingManager::RegisterScriptMenu, "RegisterScriptMenu");

        typedef bool(*CF_INHERITSFROM)(const wxString&, const wxString&); // CompilerInheritsFrom

        SqPlus::SQClassDef<CompilerFactory>("CompilerFactory").
                staticFunc(&CompilerFactory::IsValidCompilerID, "IsValidCompilerID").
                staticFuncVarArgs(&CompilerFactory_GetCompilerIndex, "GetCompilerIndex", "*").
                staticFunc(&CompilerFactory::GetDefaultCompilerID, "GetDefaultCompilerID").
                staticFunc(&CompilerFactory::GetCompilerVersionString, "GetCompilerVersionString").
                staticFunc<CF_INHERITSFROM>(&CompilerFactory::CompilerInheritsFrom, "CompilerInheritsFrom").
                staticFunc(CompilerFactory_GetCompilerIDByName, "GetCompilerIDByName");

        SqPlus::SQClassDef<PluginInfo>("PluginInfo").
            emptyCtor().
            var(&PluginInfo::name, "name").
            var(&PluginInfo::title, "title").
            var(&PluginInfo::version, "version").
            var(&PluginInfo::description, "description").
            var(&PluginInfo::author, "author").
            var(&PluginInfo::authorEmail, "authorEmail").
            var(&PluginInfo::authorWebsite, "authorWebsite").
            var(&PluginInfo::thanksTo, "thanksTo").
            var(&PluginInfo::license, "license");

        SqPlus::SQClassDef<FileTreeData>("FileTreeData").
            func(&FileTreeData::GetKind, "GetKind").
            func(&FileTreeData::GetProject, "GetProject").
            func(&FileTreeData::GetFileIndex, "GetFileIndex").
            func(&FileTreeData::GetProjectFile, "GetProjectFile").
            func(&FileTreeData::GetFolder, "GetFolder").
            func(&FileTreeData::SetKind, "SetKind").
            func(&FileTreeData::SetProject, "SetProject").
            func(&FileTreeData::SetFileIndex, "SetFileIndex").
            func(&FileTreeData::SetProjectFile, "SetProjectFile").
            func(&FileTreeData::SetFolder, "SetFolder");

        // called last because it needs a few previously registered types
        Register_ScriptPlugin();
    }
} // namespace ScriptBindings


#else

#include <sdk_precomp.h>
#ifndef CB_PRECOMP
#endif // CB_PRECOMP

#include "sc_utils.h"

namespace ScriptBindings
{
    void Register_Constants(HSQUIRRELVM v);
    void Register_Globals(HSQUIRRELVM v);
    void Register_wxTypes(HSQUIRRELVM v);
    void Register_Dialog(HSQUIRRELVM v);
    void Register_ProgressDialog(HSQUIRRELVM v);
    void Register_UtilDialogs(HSQUIRRELVM v);
    void Register_IO(HSQUIRRELVM v);
    void Register_ScriptPlugin(HSQUIRRELVM v);

    void RegisterBindings(HSQUIRRELVM vm)
    {
        Register_wxTypes(vm);
        Register_Constants(vm);
        Register_Globals(vm);
//        Register_IO(); // IO is enabled, but just for harmless functions
//        Register_Dialog();
//        Register_ProgressDialog();
//        Register_UtilDialogs();
    }
} // namespace ScriptBindings
#endif // 0
