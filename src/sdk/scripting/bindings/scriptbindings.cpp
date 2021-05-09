/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#if 0
// FIXME (squirrel) Reimplement all binding

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
    #include "configmanager.h"
    #include "editormanager.h"
    #include "scriptingmanager.h"
    #include "uservarmanager.h"
#endif // CB_PRECOMP

#include "sc_utils.h"
#include "sc_typeinfo_all.h"

#include "cbstyledtextctrl.h"

namespace ScriptBindings
{
    SQInteger ConfigManager_Read(HSQUIRRELVM v)
    {
        // this, key, default value
        ExtractParams3<SkipParam, const wxString *, SkipParam> extractor(v);
        if (!extractor.Process("ConfigManager::Read"))
            return extractor.ErrorMessage();

        // FIXME (squirrel) We don't use this for some unknown reason.
        ConfigManager *cfg = Manager::Get()->GetConfigManager("scripts");
        if (cfg == nullptr)
            return sq_throwerror(v, _SC("ConfigManager::Read cannot get manager pointer!"));

        const wxString &key = *extractor.p1;
        const SQObjectType type = sq_gettype(v, 3);
        switch (type)
        {
        case OT_INTEGER:
            sq_pushinteger(v, cfg->ReadInt(key, extractor.GetParamInt(3)));
            return 1;
        case OT_BOOL:
            sq_pushbool(v, cfg->ReadBool(key, extractor.GetParamBool(3)));
            return 1;
        case OT_FLOAT:
            sq_pushfloat(v, cfg->ReadDouble(key, extractor.GetParamFloat(3)));
            return 1;
        case OT_INSTANCE:
            {
                const wxString *defaultValue;
                if (!extractor.ProcessParam(defaultValue, 3, "ConfigManager::Read"))
                    return extractor.ErrorMessage();
                const wxString &result = cfg->Read(key, *defaultValue);
                return ConstructAndReturnInstance(v, result);
            }
        default:
            return sq_throwerror(v, _SC("ConfigManager::Read given unsupported type!"));
        }
    }

    SQInteger ConfigManager_Write(HSQUIRRELVM v)
    {
        // FIXME (squirrel) We don't use this for some unknown reason.
        ConfigManager *cfg = Manager::Get()->GetConfigManager("scripts");
        if (cfg == nullptr)
            return sq_throwerror(v, _SC("ConfigManager::Write cannot get manager pointer!"));

        const int numArgs = sq_gettop(v);
        if (numArgs == 4)
        {
            // this, key, value, ignoreEmpty
            ExtractParams4<SkipParam, const wxString*, const wxString*, bool> extractor(v);
            if (!extractor.Process("ConfigManager::Write"))
                return extractor.ErrorMessage();
            cfg->Write(*extractor.p1, *extractor.p2, extractor.p3);
            return 0;
        }
        else
        {
            // this, key, value
            ExtractParams3<SkipParam, const wxString *, SkipParam> extractor(v);
            if (!extractor.Process("ConfigManager::Write"))
                return extractor.ErrorMessage();

            const wxString &key = *extractor.p1;
            const SQObjectType type = sq_gettype(v, 3);
            switch (type)
            {
            case OT_INTEGER:
                cfg->Write(key, int(extractor.GetParamInt(3)));
                return 0;
            case OT_BOOL:
                cfg->Write(key, extractor.GetParamBool(3));
                return 0;
            case OT_FLOAT:
                cfg->Write(key, extractor.GetParamFloat(3));
                return 0;
            case OT_INSTANCE:
                {
                    const wxString *value;
                    if (!extractor.ProcessParam(value, 3, "ConfigManager::Read"))
                        return extractor.ErrorMessage();
                    cfg->Write(key, *value);
                    return 0;
                }
            default:
                ;
            }
            return sq_throwerror(v, _SC("ConfigManager::Read given unsupported type!"));
        }
    }

    template<void (ProjectFile::*func)(const wxString &)>
    SQInteger ProjectFile_SingleWxStringParam(HSQUIRRELVM v)
    {
        // this, targetName
        ExtractParams2<ProjectFile*, const wxString*> extractor(v);
        if (!extractor.Process("ProjectFile_SingleWxStringParam"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(*extractor.p1);
        return 0;
    }

    SQInteger ProjectFile_RenameBuildTarget(HSQUIRRELVM v)
    {
        // this, oldTargetName, newTargetName
        ExtractParams3<ProjectFile*, const wxString*, const wxString*> extractor(v);
        if (!extractor.Process("ProjectFile::RenameBuildTarget"))
            return extractor.ErrorMessage();
        extractor.p0->RenameBuildTarget(*extractor.p1, *extractor.p2);
        return 0;
    }

    SQInteger ProjectFile_GetBuildTargets(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const ProjectFile*> extractor(v);
        if (!extractor.Process("ProjectFile::GetBuildTargets"))
            return extractor.ErrorMessage();
        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxArrayString *result = &const_cast<wxArrayString&>(extractor.p0->GetBuildTargets());
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger ProjectFile_GetBaseName(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const ProjectFile*> extractor(v);
        if (!extractor.Process("ProjectFile::GetBaseName"))
            return extractor.ErrorMessage();
        return ConstructAndReturnInstance(v, extractor.p0->GetBaseName());
    }

    SQInteger ProjectFile_GetObjName(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectFile*> extractor(v);
        if (!extractor.Process("ProjectFile::GetObjName"))
            return extractor.ErrorMessage();
        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxString *result = &const_cast<wxString&>(extractor.p0->GetObjName());
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger ProjectFile_GetParentProject(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectFile*> extractor(v);
        if (!extractor.Process("ProjectFile::GetParentProject"))
            return extractor.ErrorMessage();
        cbProject *result = extractor.p0->GetParentProject();
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger ProjectFile_SetUseCustomBuildCommand(HSQUIRRELVM v)
    {
        // this, compilerId, useCustomBuildCommand
        ExtractParams3<ProjectFile*, const wxString *, bool> extractor(v);
        if (!extractor.Process("ProjectFile::SetUseCustomBuildCommand"))
            return extractor.ErrorMessage();
        extractor.p0->SetUseCustomBuildCommand(*extractor.p1, extractor.p2);
        return 0;
    }

    SQInteger ProjectFile_SetCustomBuildCommand(HSQUIRRELVM v)
    {
        // this, compilerId, newBuildCommand
        ExtractParams3<ProjectFile*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("ProjectFile::SetCustomBuildCommand"))
            return extractor.ErrorMessage();
        extractor.p0->SetCustomBuildCommand(*extractor.p1, *extractor.p2);
        return 0;
    }

    SQInteger ProjectFile_GetUseCustomBuildCommand(HSQUIRRELVM v)
    {
        // this, compilerId
        ExtractParams2<ProjectFile*, const wxString *> extractor(v);
        if (!extractor.Process("ProjectFile::GetUseCustomBuildCommand"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->GetUseCustomBuildCommand(*extractor.p1));
        return 1;
    }

    SQInteger ProjectFile_GetCustomBuildCommand(HSQUIRRELVM v)
    {
        // this, compilerId
        ExtractParams2<ProjectFile*, const wxString *> extractor(v);
        if (!extractor.Process("ProjectFile::GetCustomBuildCommand"))
            return extractor.ErrorMessage();
        return ConstructAndReturnInstance(v, extractor.p0->GetCustomBuildCommand(*extractor.p1));
    }

    template<void (CompileOptionsBase::*func)(int)>
    SQInteger CompileOptionsBase_Platform(HSQUIRRELVM v)
    {
        // this, platform(s)
        ExtractParams2<CompileOptionsBase*, SQInteger> extractor(v);
        if (!extractor.Process("CompileOptionsBase_Platform"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(extractor.p1);
        return 0;
    }

    SQInteger CompileOptionsBase_GetPlatforms(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::GetPlatforms"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, extractor.p0->GetPlatforms());
        return 1;
    }

    SQInteger CompileOptionsBase_SupportsCurrentPlatform(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::SupportsCurrentPlatform"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->SupportsCurrentPlatform());
        return 1;
    }

    template<void (CompileOptionsBase::*func)(const wxArrayString&)>
    SQInteger CompileOptionsBase_SetArrayString(HSQUIRRELVM v)
    {
        // this, linkerOpts
        ExtractParams2<CompileOptionsBase*, const wxArrayString *> extractor(v);
        if (!extractor.Process("CompileOptionsBase_SetArrayString"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(*extractor.p1);
        return 0;
    }

    template<const wxArrayString& (CompileOptionsBase::*func)() const>
    SQInteger CompileOptionsBase_GetArrayString(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase_GetArrayString"))
            return extractor.ErrorMessage();

        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxArrayString *result = &const_cast<wxArrayString&>((extractor.p0->*func)());
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    template<void (CompileOptionsBase::*func)(const wxString &)>
    SQInteger CompileOptionsBase_StringParam(HSQUIRRELVM v)
    {
        // this, option
        ExtractParams2<CompileOptionsBase*, const wxString *> extractor(v);
        if (!extractor.Process("CompileOptionsBase_AddOption"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(*extractor.p1);
        return 0;
    }

    template<void (CompileOptionsBase::*func)(const wxString&, const wxString&)>
    SQInteger CompileOptionsBase_ReplaceOption(HSQUIRRELVM v)
    {
        // this, option, new_option
        ExtractParams3<CompileOptionsBase*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("CompileOptionsBase_ReplaceOption"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(*extractor.p1, *extractor.p2);
        return 0;
    }

    SQInteger CompileOptionsBase_SetLinkerExecutable(HSQUIRRELVM v)
    {
        // this, option (actual type LinkerExecutableOption)
        ExtractParams2<CompileOptionsBase*, SQInteger> extractor(v);
        if (!extractor.Process("CompileOptionsBase::SetLinkerExecutable"))
            return extractor.ErrorMessage();

        if (extractor.p1 < int32_t(LinkerExecutableOption::First) ||
            extractor.p1 >= int32_t(LinkerExecutableOption::Last))
        {
            return sq_throwerror(v, _SC("CompileOptionsBase::SetLinkerExecutable: option value out of range!"));
        }
        extractor.p0->SetLinkerExecutable(LinkerExecutableOption(extractor.p1));
        return 0;
    }

    SQInteger CompileOptionsBase_GetLinkerExecutable(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::GetLinkerExecutable"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, int32_t(extractor.p0->GetLinkerExecutable()));
        return 1;
    }

    SQInteger CompileOptionsBase_GetAlwaysRunPostBuildSteps(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::GetAlwaysRunPostBuildSteps"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->GetAlwaysRunPostBuildSteps());
        return 1;
    }

    SQInteger CompileOptionsBase_SetAlwaysRunPostBuildSteps(HSQUIRRELVM v)
    {
        // this, always
        ExtractParams2<CompileOptionsBase*, bool> extractor(v);
        if (!extractor.Process("CompileOptionsBase::SetAlwaysRunPostBuildSteps"))
            return extractor.ErrorMessage();
        extractor.p0->SetAlwaysRunPostBuildSteps(extractor.p1);
        return 0;
    }

    SQInteger CompileOptionsBase_SetVar(HSQUIRRELVM v)
    {
        // this, key, value, onlyIfExists
        ExtractParams4<CompileOptionsBase*, const wxString*, const wxString*, bool> extractor(v);
        if (!extractor.Process("CompileOptionsBase::SetVar"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->SetVar(*extractor.p1, *extractor.p2, extractor.p3));
        return 1;
    }

    SQInteger CompileOptionsBase_GetVar(HSQUIRRELVM v)
    {
        // this, key
        ExtractParams2<const CompileOptionsBase*, const wxString*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::GetVar"))
            return extractor.ErrorMessage();

        const wxString &result = extractor.p0->GetVar(*extractor.p1);
        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        return ConstructAndReturnNonOwnedPtr(v, &const_cast<wxString&>(result));
    }

    SQInteger CompileOptionsBase_UnsetVar(HSQUIRRELVM v)
    {
        // this, key
        ExtractParams2<CompileOptionsBase*, const wxString*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::UnsetVar"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->UnsetVar(*extractor.p1));
        return 1;
    }

    SQInteger CompileOptionsBase_UnsetAllVars(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::UnsetAllVars"))
            return extractor.ErrorMessage();
        extractor.p0->UnsetAllVars();
        return 0;
    }

    SQInteger CompileTargetBase_SetTargetFilenameGenerationPolicy(HSQUIRRELVM v)
    {
        // this, prefix, extension (type is TargetFilenameGenerationPolicy)
        ExtractParams3<CompileTargetBase*, SQInteger, SQInteger> extractor(v);
        if (!extractor.Process("CompileTargetBase::SetTargetFilenameGenerationPolicy"))
            return extractor.ErrorMessage();
        extractor.p0->SetTargetFilenameGenerationPolicy(TargetFilenameGenerationPolicy(extractor.p1),
                                                        TargetFilenameGenerationPolicy(extractor.p2));
        return 0;
    }

    SQInteger CompileTargetBase_GetTargetFilenameGenerationPolicy(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const CompileTargetBase*> extractor(v);
        if (!extractor.Process("CompileTargetBase::GetTargetFilenameGenerationPolicy"))
            return extractor.ErrorMessage();

        TargetFilenameGenerationPolicy prefixOut, extensionOut;
        extractor.p0->GetTargetFilenameGenerationPolicy(prefixOut, extensionOut);

        // Create a table with two slots "prefix" and "extension". This emulates multiple returns.
        sq_newtableex(v, 2);

        sq_pushstring(v, _SC("prefix"), -1);
        sq_pushinteger(v, int(prefixOut));
        sq_newslot(v, -3, SQFalse);

        sq_pushstring(v, _SC("extension"), -1);
        sq_pushinteger(v, int(extensionOut));
        sq_newslot(v, -3, SQFalse);
        return 1;
    }

    template<typename Type, const wxString& (Type::*func)() const>
    SQInteger Generic_GetCString(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const Type*> extractor(v);
        if (!extractor.Process("Generic_GetCString"))
            return extractor.ErrorMessage();
        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxString *result = &const_cast<wxString&>((extractor.p0->*func)());
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    template<typename Type, void (Type::*func)(const wxString&)>
    SQInteger Generic_SetString(HSQUIRRELVM v)
    {
        // this, string
        ExtractParams2<Type*, const wxString *> extractor(v);
        if (!extractor.Process("Generic_SetString"))
            return extractor.ErrorMessage();

        (extractor.p0->*func)(*extractor.p1);
        return 0;
    }

    template<typename Type, wxString (Type::*func)()>
    SQInteger Generic_GetString(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<Type*> extractor(v);
        if (!extractor.Process("Generic_GetString"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, (extractor.p0->*func)());
    }

    template<typename Type, wxString (Type::*func)() const>
    SQInteger Generic_GetStringConst(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const Type*> extractor(v);
        if (!extractor.Process("Generic_GetStringConst"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, (extractor.p0->*func)());
    }

    template<typename Type, bool (Type::*func)() const>
    SQInteger Generic_GetBool(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const Type*> extractor(v);
        if (!extractor.Process("Generic_GetBool"))
            return extractor.ErrorMessage();
        sq_pushbool(v, (extractor.p0->*func)());
        return 1;
    }

    template<typename Type, bool (Type::*func)()>
    SQInteger Generic_DoSomethingGetBool(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<Type*> extractor(v);
        if (!extractor.Process("Generic_DoSomethingGetBool"))
            return extractor.ErrorMessage();
        sq_pushbool(v, (extractor.p0->*func)());
        return 1;
    }

    template<typename Type, void (Type::*func)(bool)>
    SQInteger Generic_SetBool(HSQUIRRELVM v)
    {
        // this, value
        ExtractParams2<Type*, bool> extractor(v);
        if (!extractor.Process("Generic_SetBool"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(extractor.p1);
        return 0;
    }

    SQInteger CompileTargetBase_GetOptionRelation(HSQUIRRELVM v)
    {
        // this, type (OptionsRelationType)
        ExtractParams2<const CompileTargetBase*, SQInteger> extractor(v);
        if (!extractor.Process("CompileTargetBase::GetOptionRelation"))
            return extractor.ErrorMessage();
        if (extractor.p1 < ortCompilerOptions || extractor.p1 >= ortLast)
            return sq_throwerror(v, _SC("CompileTargetBase::GetOptionRelation: parameter type out of range!"));

        const OptionsRelation result = extractor.p0->GetOptionRelation(OptionsRelationType(extractor.p1));
        sq_pushinteger(v, int(result));
        return 1;
    }

    SQInteger CompileTargetBase_SetOptionRelation(HSQUIRRELVM v)
    {
        // this, type (OptionsRelationType), rel (OptionsRelation)
        ExtractParams3<CompileTargetBase*, SQInteger, SQInteger> extractor(v);
        if (!extractor.Process("CompileTargetBase::SetOptionRelation"))
            return extractor.ErrorMessage();
        if (extractor.p1 < ortCompilerOptions || extractor.p1 >= ortLast)
            return sq_throwerror(v, _SC("CompileTargetBase::SetOptionRelation: parameter 'type' out of range!"));
        if (extractor.p2 < orUseParentOptionsOnly || extractor.p2 > orAppendToParentOptions)
            return sq_throwerror(v, _SC("CompileTargetBase::SetOptionRelation: parameter 'rel' out of range!"));

        extractor.p0->SetOptionRelation(OptionsRelationType(extractor.p1),
                                        OptionsRelation(extractor.p2));
        return 0;
    }

    SQInteger CompileTargetBase_SetTargetType(HSQUIRRELVM v)
    {
        // this, pt (TargetType)
        ExtractParams2<CompileTargetBase*, SQInteger> extractor(v);
        if (!extractor.Process("CompileTargetBase::SetTargetType"))
            return extractor.ErrorMessage();
        if (extractor.p1 < ttExecutable || extractor.p1 > ttNative)
            return sq_throwerror(v, _SC("CompileTargetBase::SetTargetType: parameter 'pt' out of range!"));

        extractor.p0->SetTargetType(TargetType(extractor.p1));
        return 0;
    }

    SQInteger CompileTargetBase_GetTargetType(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const CompileTargetBase*> extractor(v);
        if (!extractor.Process("CompileTargetBase::GetTargetType"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, int(extractor.p0->GetTargetType()));
        return 1;
    }

    SQInteger CompileTargetBase_GetMakeCommandFor(HSQUIRRELVM v)
    {
        // this, cmd (type MakeCommand)
        ExtractParams2<const CompileTargetBase*, SQInteger> extractor(v);
        if (!extractor.Process("CompileTargetBase::GetMakeCommandFor"))
            return extractor.ErrorMessage();
        if (extractor.p1 < mcClean || extractor.p1 >= mcLast)
            return sq_throwerror(v, _SC("CompileTargetBase::GetMakeCommandFor: parameter 'cmd' out of range!"));
        return ConstructAndReturnInstance(v, extractor.p0->GetMakeCommandFor(MakeCommand(extractor.p1)));
    }

    SQInteger CompileTargetBase_SetMakeCommandFor(HSQUIRRELVM v)
    {
        // this, cmd (type MakeCommand), make
        ExtractParams3<CompileTargetBase*, SQInteger, const wxString *> extractor(v);
        if (!extractor.Process("CompileTargetBase::SetMakeCommandFor"))
            return extractor.ErrorMessage();
        if (extractor.p1 < mcClean || extractor.p1 >= mcLast)
            return sq_throwerror(v, _SC("CompileTargetBase::GetMakeCommandFor: parameter 'cmd' out of range!"));
        extractor.p0->SetMakeCommandFor(MakeCommand(extractor.p1), *extractor.p2);
        return 0;
    }

    SQInteger ProjectBuildTarget_GetParentProject(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectBuildTarget*> extractor(v);
        if (!extractor.Process("ProjectBuildTarget::GetParentProject"))
            return extractor.ErrorMessage();
        return ConstructAndReturnNonOwnedPtr(v, extractor.p0->GetParentProject());
    }

    template<typename ClassType>
    SQInteger Generic_GetFilesCount(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ClassType*> extractor(v);
        if (!extractor.Process("Generic_GetFilesCount"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, extractor.p0->GetFilesCount());
        return 1;
    }

    template<typename ClassType>
    SQInteger Generic_GetFile(HSQUIRRELVM v)
    {
        // this, index
        ExtractParams2<ClassType*, SQInteger> extractor(v);
        if (!extractor.Process("Generic_GetFile"))
            return extractor.ErrorMessage();
        ProjectFile *file = extractor.p0->GetFile(extractor.p1);
        return ConstructAndReturnNonOwnedPtrOrNull(v, file);
    }

    SQInteger cbProject_CloseAllFiles(HSQUIRRELVM v)
    {
        // this, dontSave
        ExtractParams2<cbProject*, bool> extractor(v);
        if (!extractor.Process("cbProject::CloseAllFiles"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->CloseAllFiles(extractor.p1));
        return 1;
    }

    SQInteger cbProject_GetFileByFilename(HSQUIRRELVM v)
    {
        // this, filename, isRelative, isUnixFilename
        ExtractParams4<cbProject*, const wxString*, bool, bool> extractor(v);
        if (!extractor.Process("cbProject::GetFileByFilename"))
            return extractor.ErrorMessage();
        ProjectFile *file = extractor.p0->GetFileByFilename(*extractor.p1, extractor.p2,
                                                            extractor.p3);
        return ConstructAndReturnNonOwnedPtrOrNull(v, file);
    }

    SQInteger cbProject_RemoveFile(HSQUIRRELVM v)
    {
        // FIXME (squirrel) Add support for pf being nullptr
        // this, pf
        ExtractParams2<cbProject*, ProjectFile *> extractor(v);
        if (!extractor.Process("cbProject::RemoveFile"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->RemoveFile(extractor.p1));
        return 1;
    }

    struct ParamIntOrWxString
    {
        bool Parse(ExtractParamsBase &extractor, int stackIndex, const char *funcStr)
        {
            switch (sq_gettype(extractor.GetVM(), stackIndex))
            {
                case OT_INTEGER:
                    intValue = extractor.GetParamInt(stackIndex);
                    isInt = true;
                    return true;
                case OT_INSTANCE:
                {
                    if (!extractor.ProcessParam(strValue, stackIndex, funcStr))
                    {
                        extractor.ErrorMessage();
                        return false;
                    }
                    isInt = false;
                    return true;
                }
                default:
                    return false;
            }
        }

        bool isInt;
        union {
            int intValue;
            const wxString *strValue;
        };
    };

    SQInteger cbProject_AddFile(HSQUIRRELVM v)
    {
        // FIXME (squirrel) Add support for default parameters in ExtractParamsN
        // this, targetName or targetIndex, filename, compile=true, link=true, weight=50
        ExtractParamsBase extractor(v);
        if (!extractor.CheckNumArguments(3, 6, "cbProject::AddFile"))
            return extractor.ErrorMessage();

        cbProject *self;
        if (!extractor.ProcessParam(self, 1, "cbProject::AddFile"))
            return extractor.ErrorMessage();
        const wxString *filename;
        if (!extractor.ProcessParam(filename, 3, "cbProject::AddFile"))
            return extractor.ErrorMessage();
        bool compile = true;
        const int numArgs = sq_gettop(v);
        if (numArgs >= 4)
        {
            if (!extractor.ProcessParam(compile, 4, "cbProject::AddFile"))
                return extractor.ErrorMessage();
        }
        bool link = true;
        if (numArgs >= 5)
        {
            if (!extractor.ProcessParam(link, 5, "cbProject::AddFile"))
                return extractor.ErrorMessage();
        }
        SQInteger weight = 50;
        if (numArgs == 6)
        {
            if (!extractor.ProcessParam(weight, 6, "cbProject::AddFile"))
                return extractor.ErrorMessage();
        }

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::AddFile"))
            return sq_throwerror(v, _SC("cbProject::AddFile: Expects an integer or wxString as first argument!"));

        ProjectFile *result;
        if (targetNameOrIndex.isInt)
            result = self->AddFile(targetNameOrIndex.intValue, *filename, compile, link, weight);
        else
            result = self->AddFile(*targetNameOrIndex.strValue, *filename, compile, link, weight);
        return ConstructAndReturnNonOwnedPtrOrNull(v, result);
    }

    SQInteger cbProject_GetBuildTargetsCount(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<cbProject*> extractor(v);
        if (!extractor.Process("cbProject::GetBuildTargetsCount"))
            return extractor.ErrorMessage();

        sq_pushinteger(v, extractor.p0->GetBuildTargetsCount());
        return 1;
    }

    SQInteger cbProject_GetBuildTarget(HSQUIRRELVM v)
    {
        // this, index or targetName
        ExtractParams2<cbProject*, SkipParam> extractor(v);
        if (!extractor.Process("cbProject::GetBuildTarget"))
            return extractor.ErrorMessage();

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::GetBuildTarget"))
            return sq_throwerror(v, _SC("cbProject::GetBuildTarget: Expects an integer or wxString as first argument!"));

        ProjectBuildTarget *result;
        if (targetNameOrIndex.isInt)
            result = extractor.p0->GetBuildTarget(targetNameOrIndex.intValue);
        else
            result = extractor.p0->GetBuildTarget(*targetNameOrIndex.strValue);
        if (result)
        {
            // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
            return ConstructAndReturnNonOwnedPtr(v, result);
        }
        else
        {
            // FIXME (squirrel) Write a test for this.
            sq_pushnull(v);
            return 1;
        }
    }

    SQInteger cbProject_AddBuildTarget(HSQUIRRELVM v)
    {
        // this, targetName
        ExtractParams2<cbProject*, const wxString*> extractor(v);
        if (!extractor.Process("cbProject::AddBuildTarget"))
            return extractor.ErrorMessage();

        ProjectBuildTarget *result = extractor.p0->AddBuildTarget(*extractor.p1);
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger cbProject_RenameBuildTarget(HSQUIRRELVM v)
    {
        // this, oldTargetName or index, newTargetName
        ExtractParams3<cbProject*, SkipParam, const wxString*> extractor(v);
        if (!extractor.Process("cbProject::RenameBuildTarget"))
            return extractor.ErrorMessage();

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::RenameBuildTarget"))
            return sq_throwerror(v, _SC("cbProject::RenameBuildTarget: Expects an integer or wxString as first argument!"));

        if (targetNameOrIndex.isInt)
        {
            sq_pushbool(v, extractor.p0->RenameBuildTarget(targetNameOrIndex.intValue,
                                                           *extractor.p2));
        }
        else
        {
            sq_pushbool(v, extractor.p0->RenameBuildTarget(*targetNameOrIndex.strValue,
                                                           *extractor.p2));
        }
        return 1;
    }

    SQInteger cbProject_DuplicateBuildTarget(HSQUIRRELVM v)
    {
        // this, targetName or index, newTargetName
        ExtractParams3<cbProject*, SkipParam, const wxString*> extractor(v);
        if (!extractor.Process("cbProject::DuplicateBuildTarget"))
            return extractor.ErrorMessage();

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::DuplicateBuildTarget"))
            return sq_throwerror(v, _SC("cbProject::DuplicateBuildTarget: Expects an integer or wxString as first argument!"));

        ProjectBuildTarget *result;
        if (targetNameOrIndex.isInt)
            result = extractor.p0->DuplicateBuildTarget(targetNameOrIndex.intValue, *extractor.p2);
        else
            result = extractor.p0->DuplicateBuildTarget(*targetNameOrIndex.strValue, *extractor.p2);
        return ConstructAndReturnNonOwnedPtrOrNull(v, result);
    }

    SQInteger cbProject_RemoveBuildTarget(HSQUIRRELVM v)
    {
        // this, targetName or index
        ExtractParams2<cbProject*, SkipParam> extractor(v);
        if (!extractor.Process("cbProject::RemoveBuildTarget"))
            return extractor.ErrorMessage();

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::RemoveBuildTarget"))
            return sq_throwerror(v, _SC("cbProject::RemoveBuildTarget: Expects an integer or wxString as first argument!"));

        bool result;
        if (targetNameOrIndex.isInt)
            result = extractor.p0->RemoveBuildTarget(targetNameOrIndex.intValue);
        else
            result = extractor.p0->RemoveBuildTarget(*targetNameOrIndex.strValue);
        sq_pushbool(v, result);
        return 1;
    }

    SQInteger cbProject_ExportTargetAsProject(HSQUIRRELVM v)
    {
        // this, targetName or index
        ExtractParams2<cbProject*, SkipParam> extractor(v);
        if (!extractor.Process("cbProject::ExportTargetAsProject"))
            return extractor.ErrorMessage();

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::ExportTargetAsProject"))
            return sq_throwerror(v, _SC("cbProject::ExportTargetAsProject: Expects an integer or wxString as first argument!"));

        bool result;
        if (targetNameOrIndex.isInt)
            result = extractor.p0->ExportTargetAsProject(targetNameOrIndex.intValue);
        else
            result = extractor.p0->ExportTargetAsProject(*targetNameOrIndex.strValue);
        sq_pushbool(v, result);
        return 1;
    }

    SQInteger cbProject_BuildTargetValid(HSQUIRRELVM v)
    {
        // this, name, virtuals_too
        ExtractParams3<const cbProject*, const wxString *, bool> extractor(v);
        if (!extractor.Process("cbProject::BuildTargetValid"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->BuildTargetValid(*extractor.p1, extractor.p2));
        return 1;
    }

    SQInteger cbProject_GetFirstValidBuildTargetName(HSQUIRRELVM v)
    {
        // this, virtuals_too
        ExtractParams2<const cbProject*, bool> extractor(v);
        if (!extractor.Process("cbProject::GetFirstValidBuildTargetName"))
            return extractor.ErrorMessage();

        const wxString &result = extractor.p0->GetFirstValidBuildTargetName(extractor.p1);
        return ConstructAndReturnInstance(v, result);
    }

    SQInteger cbProject_SetActiveBuildTarget(HSQUIRRELVM v)
    {
        // this, name
        ExtractParams2<cbProject*, const wxString*> extractor(v);
        if (!extractor.Process("cbProject::SetActiveBuildTarget"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->SetActiveBuildTarget(*extractor.p1));
        return 1;
    }

    SQInteger cbProject_SelectTarget(HSQUIRRELVM v)
    {
        // this, initial, evenIfOne
        ExtractParams3<cbProject*, SQInteger, bool> extractor(v);
        if (!extractor.Process("cbProject::SelectTarget"))
            return extractor.ErrorMessage();

        sq_pushinteger(v, extractor.p0->SelectTarget(extractor.p1, extractor.p2));
        return 1;
    }

    SQInteger cbProject_GetCurrentlyCompilingTarget(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<cbProject*> extractor(v);
        if (!extractor.Process("cbProject::GetCurrentlyCompilingTarget"))
            return extractor.ErrorMessage();

        return ConstructAndReturnNonOwnedPtrOrNull(v, extractor.p0->GetCurrentlyCompilingTarget());
    }

    SQInteger cbProject_SetCurrentlyCompilingTarget(HSQUIRRELVM v)
    {
        // FIXME (squirrel) Implement something like a wrapper, which makes it easier to handle the
        //  ProjectBuildTarget being nullptr. Currently ProcessParams requires this to be non-null
        // and so we cannot use it.

        // this, bt
        ExtractParams2<cbProject*, SkipParam/*ProjectBuildTarget**/> extractor(v);
        if (!extractor.Process("cbProject::SetCurrentlyCompilingTarget"))
            return extractor.ErrorMessage();

        ProjectBuildTarget *target = nullptr;
        if (sq_gettype(v, 2) != OT_NULL)
        {
            if (!extractor.ProcessParam(target, 2, "cbProject::SetCurrentlyCompilingTarget"))
                return extractor.ErrorMessage();
        }

        extractor.p0->SetCurrentlyCompilingTarget(target);
        return 0;
    }

    SQInteger cbProject_GetModeForPCH(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const cbProject*> extractor(v);
        if (!extractor.Process("cbProject::GetModeForPCH"))
            return extractor.ErrorMessage();

        sq_pushinteger(v, extractor.p0->GetModeForPCH());
        return 1;
    }

    SQInteger cbProject_SetModeForPCH(HSQUIRRELVM v)
    {
        // this, mode
        ExtractParams2<cbProject*, SQInteger> extractor(v);
        if (!extractor.Process("cbProject::SetModeForPCH"))
            return extractor.ErrorMessage();

        if (extractor.p1 < pchSourceDir || extractor.p1 > pchSourceFile)
            return sq_throwerror(v, _SC("cbProject::SetModeForPCH: The value of 'mode' parameter is out of range!"));
        extractor.p0->SetModeForPCH(PCHMode(extractor.p1));
        return 0;
    }

    SQInteger cbProject_ShowNotes(HSQUIRRELVM v)
    {
        // this, nonEmptyONly, editable
        ExtractParams3<cbProject*, bool, bool> extractor(v);
        if (!extractor.Process("cbProject::ShowNotes"))
            return extractor.ErrorMessage();

        extractor.p0->ShowNotes(extractor.p1, extractor.p2);
        return 0;
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

    static wxString FindFullExtensionPathForNode(const TiXmlNode *node)
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
        ExtractParams2<cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionListNodes"))
            return extractor.ErrorMessage();
        const wxString &extension = *extractor.p1;

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, extension);
        if (queryResult.element == nullptr)
        {
            // FIXME (squirrel) Modify the API to not throw errors if something is missing.
            // Return null probably?
            if (queryResult.errorStr.empty())
                return ConstructAndReturnInstance(v, wxArrayString());
            else
                return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        wxArrayString result;
        int index = 0;
        for (const TiXmlNode *child = queryResult.element->FirstChild();
             child;
             child = child->NextSibling())
        {
            wxString msg = extension + wxT("/") + wxString(child->Value(), wxConvUTF8);
            msg << wxT('[') << index << wxT(']');
            result.Add(msg);
            ++index;
        }
        return ConstructAndReturnInstance(v, result);
    }

    /// Squirrel function would expect a cbProject and an extension string. It will return a
    /// wxArrayString object.
    SQInteger cbProject_ExtensionListNodeAttributes(HSQUIRRELVM v)
    {
        ExtractParams2<cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionListNodeAttributes"))
            return extractor.ErrorMessage();

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
                return ConstructAndReturnInstance(v, wxArrayString());
            else
                return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        wxArrayString result;
        for (const TiXmlAttribute *attr = queryResult.element->FirstAttribute();
             attr;
             attr = attr->Next())
        {
            result.Add(wxString(attr->Name(), wxConvUTF8));
        }
        return ConstructAndReturnInstance(v, result);
    }

    /// Squirrel function would expect a cbProject, an extension string and attribute name. It will
    /// return a wxString object.
    SQInteger cbProject_ExtensionGetNodeAttribute(HSQUIRRELVM v)
    {
        ExtractParams3<cbProject*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionGetNodeAttribute"))
            return extractor.ErrorMessage();

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
                return ConstructAndReturnInstance(v, wxString());
            else
                return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        wxString result;
        const char *attributeValue = queryResult.element->Attribute(extractor.p2->utf8_str().data());
        if (attributeValue)
            result = wxString(attributeValue, wxConvUTF8);
        return ConstructAndReturnInstance(v, result);
    }

    /// Squirrel function would expect a cbProject, an extension string, attribute name and
    /// attribute value.
    SQInteger cbProject_ExtensionSetNodeAttribute(HSQUIRRELVM v)
    {
        ExtractParams4<cbProject*, const wxString *, const wxString *, const wxString*> extractor(v);
        if (!extractor.Process("cbProject::ExtensionSetNodeAttribute"))
            return extractor.ErrorMessage();

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extractor.p1->wx_str());
            }
            return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        queryResult.element->SetAttribute(extractor.p2->utf8_str().data(),
                                          extractor.p3->utf8_str().data());
        extractor.p0->SetModified(true);
        return 0;
    }

    /// Squirrel function would expect a cbProject, an extension string, attribute name.
    SQInteger cbProject_ExtensionRemoveNodeAttribute(HSQUIRRELVM v)
    {
        ExtractParams3<cbProject*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionRemoveNodeAttribute"))
            return extractor.ErrorMessage();

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extractor.p1->wx_str());
            }
            return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        queryResult.element->RemoveAttribute(extractor.p2->utf8_str().data());
        extractor.p0->SetModified(true);
        return 0;
    }

    /// Squirrel function would expect a cbProject, an extension string and node name.
    /// It will return the extension of the newly created node, so it could be used in other
    /// node calls.
    SQInteger cbProject_ExtensionAddNode(HSQUIRRELVM v)
    {
        ExtractParams3<cbProject*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionAddNode"))
            return extractor.ErrorMessage();

        // FIXME (squirrel) Simplify this. This code is too repetitive.
        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extractor.p1->wx_str());
            }
            return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        TiXmlNode *newNode = queryResult.element->InsertEndChild(TiXmlElement(cbU2C(*extractor.p2)));
        const wxString resultExtension = FindFullExtensionPathForNode(newNode);
        extractor.p0->SetModified(true);
        return ConstructAndReturnInstance(v, resultExtension);
    }

    /// Squirrel function would expect a cbProject and extension string.
    SQInteger cbProject_ExtensionRemoveNode(HSQUIRRELVM v)
    {
        ExtractParams2<cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionRemoveNode"))
            return extractor.ErrorMessage();

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extractor.p1->wx_str());
            }
            return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        TiXmlNode *parent = queryResult.element->Parent();
        parent->RemoveChild(queryResult.element);
        extractor.p0->SetModified(true);
        return 0;
    }

    SQInteger cbProject_DefineVirtualBuildTarget(HSQUIRRELVM v)
    {
        // this, alias, targets
        ExtractParams3<cbProject*, const wxString *, const wxArrayString *> extractor(v);
        if (!extractor.Process("cbProject::DefineVirtualBuildTarget"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->DefineVirtualBuildTarget(*extractor.p1, *extractor.p2));
        return 1;
    }

    SQInteger cbProject_HasVirtualBuildTarget(HSQUIRRELVM v)
    {
        // this, alias
        ExtractParams2<const cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::HasVirtualBuildTarget"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->HasVirtualBuildTarget(*extractor.p1));
        return 1;
    }

    SQInteger cbProject_RemoveVirtualBuildTarget(HSQUIRRELVM v)
    {
        ExtractParams2<cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::RemoveVirtualBuildTarget"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->RemoveVirtualBuildTarget(*extractor.p1));
        return 1;
    }

    SQInteger cbProject_GetVirtualBuildTargets(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const cbProject*> extractor(v);
        if (!extractor.Process("cbProject::GetVirtualBuildTargets"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, extractor.p0->GetVirtualBuildTargets());
    }

    SQInteger cbProject_GetVirtualBuildTargetGroup(HSQUIRRELVM v)
    {
        // this, alias
        ExtractParams2<const cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::GetVirtualBuildTargetGroup"))
            return extractor.ErrorMessage();

        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxArrayString *result = &const_cast<wxArrayString&>(extractor.p0->GetVirtualBuildTargetGroup(*extractor.p1));
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger cbProject_GetExpandedVirtualBuildTargetGroup(HSQUIRRELVM v)
    {
        // this, alias
        ExtractParams2<const cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::GetExpandedVirtualBuildTargetGroup"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, extractor.p0->GetExpandedVirtualBuildTargetGroup(*extractor.p1));
    }

    SQInteger cbProject_CanAddToVirtualBuildTarget(HSQUIRRELVM v)
    {
        // this, alias, target
        ExtractParams3<cbProject*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::CanAddToVirtualBuildTarget"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->CanAddToVirtualBuildTarget(*extractor.p1, *extractor.p2));
        return 1;
    }

    SQInteger ProjectManager_NewProject(HSQUIRRELVM v)
    {
        // this, filename
        ExtractParams2<ProjectManager*, const wxString *> extractor(v);
        if (!extractor.Process("ProjectManager::NewProject"))
            return extractor.ErrorMessage();
        return ConstructAndReturnNonOwnedPtr(v, extractor.p0->NewProject(*extractor.p1));
    }

    SQInteger ProjectManager_AddFileToProject(HSQUIRRELVM v)
    {
        // this, filename, project, target
        ExtractParams4<ProjectManager*, const wxString *, cbProject *, SQInteger> extractor(v);
        if (!extractor.Process("ProjectManager::AddFileToProject"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, extractor.p0->AddFileToProject(*extractor.p1, extractor.p2, extractor.p3));
        return 1;
    }

    SQInteger ProjectManager_CloseWorkspace(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectManager*> extractor(v);
        if (!extractor.Process("ProjectManager::CloseWorkspace"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->CloseWorkspace());
        return 1;
    }

    SQInteger ProjectManager_CloseProject(HSQUIRRELVM v)
    {
        // this, project, dontsave, refresh
        ExtractParams4<ProjectManager*, cbProject *, bool, bool> extractor(v);
        if (!extractor.Process("ProjectManager::CloseProject"))
            return extractor.ErrorMessage();
        const bool result = extractor.p0->CloseProject(extractor.p1, extractor.p2, extractor.p3);
        sq_pushbool(v, result);
        return 1;
    }

    SQInteger ProjectManager_RebuildTree(HSQUIRRELVM v)
    {
        // this, project, dontsave, refresh
        ExtractParams1<ProjectManager*> extractor(v);
        if (!extractor.Process("ProjectManager::RebuildTree"))
            return extractor.ErrorMessage();
        extractor.p0->GetUI().RebuildTree();
        return 0;
    }

    SQInteger EditorBase_Close(HSQUIRRELVM v)
    {
        ExtractParams1<EditorBase*> extractor(v);
        if (!extractor.Process("EditorBase::Close"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->Close());
        return 1;
    }

    SQInteger cbEditor_SetText(HSQUIRRELVM v)
    {
        ExtractParams2<cbEditor*, const wxString *> extractor(v);
        if (!extractor.Process("cbEditor::SetText"))
            return extractor.ErrorMessage();

        extractor.p0->GetControl()->SetText(*extractor.p1);
        return 0;
    }

    SQInteger EditorManager_New(HSQUIRRELVM v)
    {
        ExtractParams2<EditorManager*, const wxString *> extractor(v);
        if (!extractor.Process("EditorManager::New"))
            return extractor.ErrorMessage();
        cbEditor *result = extractor.p0->New(*extractor.p1);
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger UserVariableManager_Exists(HSQUIRRELVM v)
    {
        // this, variable
        ExtractParams2<UserVariableManager*, const wxString *> extractor(v);
        if (!extractor.Process("UserVariableManager::Exists"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->Exists(*extractor.p1));
        return 1;
    }

    SQInteger ScriptingManager_RegisterScriptMenu(HSQUIRRELVM v)
    {
        // this, menuPath, scriptOrFunc, isFunction
        ExtractParams4<ScriptingManager*, const wxString *, const wxString*, bool> extractor(v);
        if (!extractor.Process("ScriptingManager::RegisterScriptMenu"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->RegisterScriptMenu(*extractor.p1, *extractor.p2, extractor.p3));
        return 1;
    }

    template<>
    MembersType<PluginInfo> FindMembers<PluginInfo>::members{};
    template<>
    MembersType<ProjectFile> FindMembers<ProjectFile>::members{};

    void Register_Constants(HSQUIRRELVM v);
    void Unregister_Constants(HSQUIRRELVM v);
    void Register_Globals(HSQUIRRELVM v);
    void Register_wxTypes(HSQUIRRELVM v);
    void Register_Dialog(HSQUIRRELVM v);
    void Register_ProgressDialog(HSQUIRRELVM v);
    void Register_UtilDialogs(HSQUIRRELVM v);
    void Register_IO(HSQUIRRELVM v);
    void Register_ScriptPlugin(HSQUIRRELVM v);

    void RegisterBindings(HSQUIRRELVM v)
    {
        Register_wxTypes(v);
        Register_Constants(v);
        Register_Globals(v);
//        Register_IO(); // IO is enabled, but just for harmless functions
//        Register_Dialog();
//        Register_ProgressDialog();
//        Register_UtilDialogs();

        PreserveTop preserveTop(v);
        sq_pushroottable(v);

        // FIXME (squirrel) Add a way to prevent instances from being constructed for a particular
        // class type.

        // FIXME (squirrel) We have the class name in the typeinfo there is no need to specify it,
        // manually. And I guess the base class name is also redundant.

        {
            // Register ConfigManager
            const SQInteger classDecl = CreateClassDecl<ConfigManager>(v, _SC("ConfigManager"));
            BindMethod(v, _SC("Read"), ConfigManager_Read, _SC("ConfigManager::Read"));
            BindMethod(v, _SC("Write"), ConfigManager_Write, _SC("ConfigManager::Write"));

            BindDefaultInstanceCmp<ConfigManager>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register ProjectFile
            const SQInteger classDecl = CreateClassDecl<ProjectFile>(v, _SC("ProjectFile"));
            BindMethod(v, _SC("AddBuildTarget"),
                       ProjectFile_SingleWxStringParam<&ProjectFile::AddBuildTarget>,
                       _SC("ProjectFile::AddBuildTarget"));
            BindMethod(v, _SC("RenameBuildTarget"), ProjectFile_RenameBuildTarget,
                       _SC("ProjectFile::RenameBuildTarget"));
            BindMethod(v, _SC("RemoveBuildTarget"),
                       ProjectFile_SingleWxStringParam<&ProjectFile::RemoveBuildTarget>,
                       _SC("ProjectFile::RemoveBuildTarget"));
            BindMethod(v, _SC("GetBuildTargets"), ProjectFile_GetBuildTargets,
                       _SC("ProjectFile::GetBuildTargets"));
            BindMethod(v, _SC("GetBaseName"), ProjectFile_GetBaseName,
                       _SC("ProjectFile::GetBaseName"));
            BindMethod(v, _SC("GetObjName"), ProjectFile_GetObjName,
                       _SC("ProjectFile::GetObjName"));
            BindMethod(v, _SC("SetObjName"),
                       ProjectFile_SingleWxStringParam<&ProjectFile::SetObjName>,
                       _SC("ProjectFile::SetObjName"));
            BindMethod(v, _SC("GetParentProject"), ProjectFile_GetParentProject,
                       _SC("ProjectFile::GetParentProject"));
            BindMethod(v, _SC("SetUseCustomBuildCommand"), ProjectFile_SetUseCustomBuildCommand,
                       _SC("ProjectFile::SetUseCustomBuildCommand"));
            BindMethod(v, _SC("SetCustomBuildCommand"), ProjectFile_SetCustomBuildCommand,
                       _SC("ProjectFile::SetCustomBuildCommand"));
            BindMethod(v, _SC("GetUseCustomBuildCommand"), ProjectFile_GetUseCustomBuildCommand,
                       _SC("ProjectFile::GetUseCustomBuildCommand"));
            BindMethod(v, _SC("GetCustomBuildCommand"), ProjectFile_GetCustomBuildCommand,
                       _SC("ProjectFile::GetCustomBuildCommand"));

            MembersType<ProjectFile> &members = BindMembers<ProjectFile>(v);
            addMemberRef(members, _SC("file"), &ProjectFile::file);
            addMemberRef(members, _SC("relativeFilename"), &ProjectFile::relativeFilename);
            addMemberRef(members, _SC("relativeToCommonTopLevelPath"),
                         &ProjectFile::relativeToCommonTopLevelPath);
            addMemberBool(members, _SC("compile"), &ProjectFile::compile);
            addMemberBool(members, _SC("link"), &ProjectFile::link);
            addMemberUInt(members, _SC("weight"), &ProjectFile::weight);
            addMemberRef(members, _SC("compilerVar"), &ProjectFile::compilerVar);
            addMemberRef(members, _SC("buildTargets"), &ProjectFile::buildTargets);

            BindDefaultInstanceCmp<ProjectFile>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register CompileOptionsBase
            const SQInteger classDecl = CreateClassDecl<CompileOptionsBase>(v, _SC("CompileOptionsBase"));
            BindMethod(v, _SC("AddPlatform"),
                       CompileOptionsBase_Platform<&CompileOptionsBase::AddPlatform>,
                       _SC("CompileOptionsBase::AddPlatform"));
            BindMethod(v, _SC("RemovePlatform"),
                       CompileOptionsBase_Platform<&CompileOptionsBase::RemovePlatform>,
                       _SC("CompileOptionsBase::RemovePlatform"));
            BindMethod(v, _SC("SetPlatforms"),
                       CompileOptionsBase_Platform<&CompileOptionsBase::SetPlatforms>,
                       _SC("CompileOptionsBase::SetPlatforms"));
            BindMethod(v, _SC("GetPlatforms"), CompileOptionsBase_GetPlatforms,
                       _SC("CompileOptionsBase::GetPlatforms"));
            BindMethod(v, _SC("SupportsCurrentPlatform"),
                         CompileOptionsBase_SupportsCurrentPlatform,
                         _SC("CompileOptionsBase::SupportsCurrentPlatform"));
            BindMethod(v, _SC("SetLinkerOptions"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetLinkerOptions>,
                       _SC("CompileOptionsBase::SetLinkerOptions"));
            BindMethod(v, _SC("SetLinkLibs"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetLinkLibs>,
                       _SC("CompileOptionsBase::SetLinkLibs"));
            BindMethod(v, _SC("SetLinkerExecutable"), CompileOptionsBase_SetLinkerExecutable,
                       _SC("CompileOptionsBase::SetLinkerExecutable"));
            BindMethod(v, _SC("GetLinkerExecutable"), CompileOptionsBase_GetLinkerExecutable,
                       _SC("CompileOptionsBase::GetLinkerExecutable"));
            BindMethod(v, _SC("SetCompilerOptions"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetCompilerOptions>,
                       _SC("CompileOptionsBase::SetCompilerOptions"));
            BindMethod(v, _SC("SetResourceCompilerOptions"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetResourceCompilerOptions>,
                       _SC("CompileOptionsBase::SetResourceCompilerOptions"));
            BindMethod(v, _SC("SetIncludeDirs"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetIncludeDirs>,
                       _SC("CompileOptionsBase::SetIncludeDirs"));
            BindMethod(v, _SC("SetResourceIncludeDirs"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetResourceIncludeDirs>,
                       _SC("CompileOptionsBase::SetResourceIncludeDirs"));
            BindMethod(v, _SC("SetLibDirs"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetLibDirs>,
                       _SC("CompileOptionsBase::SetLibDirs"));
            BindMethod(v, _SC("SetCommandsBeforeBuild"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetCommandsBeforeBuild>,
                       _SC("CompileOptionsBase::SetCommandsBeforeBuild"));
            BindMethod(v, _SC("SetCommandsAfterBuild"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetCommandsAfterBuild>,
                       _SC("CompileOptionsBase::SetCommandsAfterBuild"));
            BindMethod(v, _SC("GetLinkerOptions"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetLinkerOptions>,
                       _SC("CompileOptionsBase::GetLinkerOptions"));
            BindMethod(v, _SC("GetLinkLibs"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetLinkLibs>,
                       _SC("CompileOptionsBase::GetLinkLibs"));
            BindMethod(v, _SC("GetCompilerOptions"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetCompilerOptions>,
                       _SC("CompileOptionsBase::GetCompilerOptions"));
            BindMethod(v, _SC("GetResourceCompilerOptions"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetResourceCompilerOptions>,
                       _SC("CompileOptionsBase::GetResourceCompilerOptions"));
            BindMethod(v, _SC("GetIncludeDirs"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetIncludeDirs>,
                       _SC("CompileOptionsBase::GetIncludeDirs"));
            BindMethod(v, _SC("GetResourceIncludeDirs"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetResourceIncludeDirs>,
                       _SC("CompileOptionsBase::GetResourceIncludeDirs"));
            BindMethod(v, _SC("GetLibDirs"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetLibDirs>,
                       _SC("CompileOptionsBase::GetLibDirs"));
            BindMethod(v, _SC("GetCommandsBeforeBuild"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetCommandsBeforeBuild>,
                       _SC("CompileOptionsBase::GetCommandsBeforeBuild"));
            BindMethod(v, _SC("GetCommandsAfterBuild"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetCommandsAfterBuild>,
                       _SC("CompileOptionsBase::GetCommandsAfterBuild"));
            BindMethod(v, _SC("GetModified"),
                       Generic_GetBool<CompileOptionsBase, &CompileOptionsBase::GetModified>,
                       _SC("CompileOptionsBase::GetModified"));
            BindMethod(v, _SC("SetModified"),
                       Generic_SetBool<CompileOptionsBase, &CompileOptionsBase::SetModified>,
                       _SC("CompileOptionsBase::SetModified"));
            BindMethod(v, _SC("AddLinkerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddLinkerOption>,
                       _SC("CompileOptionsBase::AddLinkerOption"));
            BindMethod(v, _SC("AddLinkLib"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddLinkLib>,
                       _SC("CompileOptionsBase::AddLinkLib"));
            BindMethod(v, _SC("AddCompilerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddCompilerOption>,
                       _SC("CompileOptionsBase::AddCompilerOption"));
            BindMethod(v, _SC("AddResourceCompilerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddResourceCompilerOption>,
                       _SC("CompileOptionsBase::AddResourceCompilerOption"));
            BindMethod(v, _SC("AddIncludeDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddIncludeDir>,
                       _SC("CompileOptionsBase::AddIncludeDir"));
            BindMethod(v, _SC("AddResourceIncludeDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddResourceIncludeDir>,
                       _SC("CompileOptionsBase::AddResourceIncludeDir"));
            BindMethod(v, _SC("AddLibDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddLibDir>,
                       _SC("CompileOptionsBase::AddLibDir"));
            BindMethod(v, _SC("AddCommandsBeforeBuild"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddCommandsBeforeBuild>,
                       _SC("CompileOptionsBase::AddCommandsBeforeBuild"));
            BindMethod(v, _SC("AddCommandsAfterBuild"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddCommandsAfterBuild>,
                       _SC("CompileOptionsBase::AddCommandsAfterBuild"));
            BindMethod(v, _SC("ReplaceLinkerOption"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceLinkerOption>,
                       _SC("CompileOptionsBase::ReplaceLinkerOption"));
            BindMethod(v, _SC("ReplaceLinkLib"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceLinkLib>,
                       _SC("CompileOptionsBase::ReplaceLinkLib"));
            BindMethod(v, _SC("ReplaceCompilerOption"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceCompilerOption>,
                       _SC("CompileOptionsBase::ReplaceCompilerOption"));
            BindMethod(v, _SC("ReplaceResourceCompilerOption"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceResourceCompilerOption>,
                       _SC("CompileOptionsBase::ReplaceResourceCompilerOption"));
            BindMethod(v, _SC("ReplaceIncludeDir"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceIncludeDir>,
                       _SC("CompileOptionsBase::ReplaceIncludeDir"));
            BindMethod(v, _SC("ReplaceResourceIncludeDir"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceResourceIncludeDir>,
                       _SC("CompileOptionsBase::ReplaceResourceIncludeDir"));
            BindMethod(v, _SC("ReplaceLibDir"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceLibDir>,
                       _SC("CompileOptionsBase::ReplaceLibDir"));
            BindMethod(v, _SC("ReplaceLibDir"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceLibDir>,
                       _SC("CompileOptionsBase::ReplaceLibDir"));
            BindMethod(v, _SC("RemoveLinkerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveLinkerOption>,
                       _SC("CompileOptionsBase::RemoveLinkerOption"));
            BindMethod(v, _SC("RemoveLinkLib"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveLinkLib>,
                       _SC("CompileOptionsBase::RemoveLinkLib"));
            BindMethod(v, _SC("RemoveCompilerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveCompilerOption>,
                       _SC("CompileOptionsBase::RemoveCompilerOption"));
            BindMethod(v, _SC("RemoveCompilerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveCompilerOption>,
                       _SC("CompileOptionsBase::RemoveCompilerOption"));
            BindMethod(v, _SC("RemoveIncludeDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveIncludeDir>,
                       _SC("CompileOptionsBase::RemoveIncludeDir"));
            BindMethod(v, _SC("RemoveResourceCompilerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveResourceCompilerOption>,
                       _SC("CompileOptionsBase::RemoveResourceCompilerOption"));
            BindMethod(v, _SC("RemoveResourceIncludeDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveResourceIncludeDir>,
                       _SC("CompileOptionsBase::RemoveResourceIncludeDir"));
            BindMethod(v, _SC("RemoveLibDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveLibDir>,
                       _SC("CompileOptionsBase::RemoveLibDir"));
            BindMethod(v, _SC("RemoveCommandsBeforeBuild"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveCommandsBeforeBuild>,
                       _SC("CompileOptionsBase::RemoveCommandsBeforeBuild"));
            BindMethod(v, _SC("RemoveCommandsAfterBuild"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveCommandsAfterBuild>,
                       _SC("CompileOptionsBase::RemoveCommandsAfterBuild"));
            BindMethod(v, _SC("GetAlwaysRunPostBuildSteps"),
                       CompileOptionsBase_GetAlwaysRunPostBuildSteps,
                       _SC("CompileOptionsBase::GetAlwaysRunPostBuildSteps"));
            BindMethod(v, _SC("SetAlwaysRunPostBuildSteps"),
                       CompileOptionsBase_SetAlwaysRunPostBuildSteps,
                       _SC("CompileOptionsBase::SetAlwaysRunPostBuildSteps"));
            BindMethod(v, _SC("SetBuildScripts"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetBuildScripts>,
                       _SC("CompileOptionsBase::SetBuildScripts"));
            BindMethod(v, _SC("GetBuildScripts"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetBuildScripts>,
                       _SC("CompileOptionsBase::GetBuildScripts"));
            BindMethod(v, _SC("AddBuildScript"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddBuildScript>,
                       _SC("CompileOptionsBase::AddBuildScript"));
            BindMethod(v, _SC("RemoveBuildScript"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveBuildScript>,
                       _SC("CompileOptionsBase::RemoveBuildScript"));
            BindMethod(v, _SC("SetVar"), CompileOptionsBase_SetVar,
                       _SC("CompileOptionsBase::SetVar"));
            BindMethod(v, _SC("GetVar"), CompileOptionsBase_GetVar,
                       _SC("CompileOptionsBase::GetVar"));
            BindMethod(v, _SC("UnsetVar"), CompileOptionsBase_UnsetVar,
                       _SC("CompileOptionsBase::UnsetVar"));
            BindMethod(v, _SC("UnsetAllVars"), CompileOptionsBase_UnsetAllVars,
                       _SC("CompileOptionsBase::UnsetAllVars"));

            BindDefaultInstanceCmp<CompileOptionsBase>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register CompileTargetBase
            const SQInteger classDecl = CreateClassDecl<CompileTargetBase>(v, _SC("CompileTargetBase"), _SC("CompileOptionsBase"));
            BindMethod(v, _SC("SetTargetFilenameGenerationPolicy"),
                       CompileTargetBase_SetTargetFilenameGenerationPolicy,
                       _SC("CompileTargetBase::SetTargetFilenameGenerationPolicy"));
            // FIXME (squirrel) Add this to the wiki!
            BindMethod(v, _SC("GetTargetFilenameGenerationPolicy"),
                       CompileTargetBase_GetTargetFilenameGenerationPolicy,
                       _SC("CompileTargetBase::GetTargetFilenameGenerationPolicy"));
            BindMethod(v, _SC("GetFilename"),
                       Generic_GetCString<CompileTargetBase, &CompileTargetBase::GetFilename>,
                       _SC("CompileTargetBase::GetFilename"));
            BindMethod(v, _SC("GetTitle"),
                       Generic_GetCString<CompileTargetBase, &CompileTargetBase::GetTitle>,
                       _SC("CompileTargetBase::GetTitle"));
            BindMethod(v, _SC("SetTitle"), Generic_SetString<CompileTargetBase, &CompileTargetBase::SetTitle>,
                       _SC("CompileTargetBase::SetTitle"));
            BindMethod(v, _SC("SetOutputFilename"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetOutputFilename>,
                       _SC("CompileTargetBase::SetOutputFilename"));
            BindMethod(v, _SC("SetWorkingDir"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetWorkingDir>,
                       _SC("CompileTargetBase::SetWorkingDir"));
            BindMethod(v, _SC("SetObjectOutput"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetObjectOutput>,
                       _SC("CompileTargetBase::SetObjectOutput"));
            BindMethod(v, _SC("SetDepsOutput"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetDepsOutput>,
                       _SC("CompileTargetBase::SetDepsOutput"));
            BindMethod(v, _SC("GetOptionRelation"), CompileTargetBase_GetOptionRelation,
                       _SC("CompileTargetBase::GetOptionRelation"));
            BindMethod(v, _SC("SetOptionRelation"), CompileTargetBase_SetOptionRelation,
                       _SC("CompileTargetBase::SetOptionRelation"));
            BindMethod(v, _SC("GetWorkingDir"),
                       Generic_GetString<CompileTargetBase, &CompileTargetBase::GetWorkingDir>,
                       _SC("CompileTargetBase::GetWorkingDir"));
            BindMethod(v, _SC("GetObjectOutput"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetObjectOutput>,
                       _SC("CompileTargetBase::GetObjectOutput"));
            BindMethod(v, _SC("GetDepsOutput"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetDepsOutput>,
                       _SC("CompileTargetBase::GetDepsOutput"));
            BindMethod(v, _SC("GetOutputFilename"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetOutputFilename>,
                       _SC("CompileTargetBase::GetOutputFilename"));
            BindMethod(v, _SC("SuggestOutputFilename"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::SuggestOutputFilename>,
                       _SC("CompileTargetBase::SuggestOutputFilename"));
            BindMethod(v, _SC("GetExecutableFilename"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetExecutableFilename>,
                       _SC("CompileTargetBase::GetExecutableFilename"));
            BindMethod(v, _SC("GetDynamicLibFilename"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetDynamicLibFilename>,
                       _SC("CompileTargetBase::GetDynamicLibFilename"));
            BindMethod(v, _SC("GetDynamicLibDefFilename"),
                       Generic_GetString<CompileTargetBase, &CompileTargetBase::GetDynamicLibDefFilename>,
                       _SC("CompileTargetBase::GetDynamicLibDefFilename"));
            BindMethod(v, _SC("GetStaticLibFilename"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetStaticLibFilename>,
                       _SC("CompileTargetBase::GetStaticLibFilename"));
            BindMethod(v, _SC("GetBasePath"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetBasePath>,
                       _SC("CompileTargetBase::GetBasePath"));
            BindMethod(v, _SC("SetTargetType"), CompileTargetBase_SetTargetType,
                       _SC("CompileTargetBase::SetTargetType"));
            BindMethod(v, _SC("GetTargetType"), CompileTargetBase_GetTargetType,
                         _SC("CompileTargetBase::GetTargetType"));
            BindMethod(v, _SC("GetExecutionParameters"),
                       Generic_GetCString<CompileTargetBase, &CompileTargetBase::GetExecutionParameters>,
                       _SC("CompileTargetBase::GetExecutionParameters"));
            BindMethod(v, _SC("SetExecutionParameters"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetExecutionParameters>,
                       _SC("CompileTargetBase::SetExecutionParameters"));
            BindMethod(v, _SC("GetHostApplication"),
                       Generic_GetCString<CompileTargetBase, &CompileTargetBase::GetHostApplication>,
                       _SC("CompileTargetBase::GetHostApplication"));
            BindMethod(v, _SC("SetHostApplication"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetHostApplication>,
                       _SC("CompileTargetBase::SetHostApplication"));
            BindMethod(v, _SC("SetCompilerID"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetCompilerID>,
                       _SC("CompileTargetBase::SetCompilerID"));
            BindMethod(v, _SC("GetCompilerID"),
                       Generic_GetCString<CompileTargetBase, &CompileTargetBase::GetCompilerID>,
                       _SC("CompileTargetBase::GetCompilerID"));
            BindMethod(v, _SC("GetMakeCommandFor"), CompileTargetBase_GetMakeCommandFor,
                       _SC("CompileTargetBase::GetMakeCommandFor"));
            BindMethod(v, _SC("SetMakeCommandFor"), CompileTargetBase_SetMakeCommandFor,
                       _SC("CompileTargetBase::SetMakeCommandFor"));
            BindMethod(v, _SC("MakeCommandsModified"),
                       Generic_GetBool<CompileTargetBase, &CompileTargetBase::MakeCommandsModified>,
                       _SC("CompileTargetBase::MakeCommandsModified"));

            BindDefaultInstanceCmp<CompileTargetBase>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register ProjectBuildTarget
            const SQInteger classDecl = CreateClassDecl<ProjectBuildTarget>(v, _SC("ProjectBuildTarget"), _SC("CompileTargetBase"));
            BindMethod(v, _SC("GetParentProject"), ProjectBuildTarget_GetParentProject,
                       _SC("ProjectBuildTarget::GetParentProject"));
            BindMethod(v, _SC("GetFullTitle"),
                       Generic_GetStringConst<ProjectBuildTarget, &ProjectBuildTarget::GetFullTitle>,
                       _SC("ProjectBuildTarget::GetFullTitle"));
            BindMethod(v, _SC("GetExternalDeps"),
                       Generic_GetCString<ProjectBuildTarget, &ProjectBuildTarget::GetExternalDeps>,
                       _SC("ProjectBuildTarget::GetExternalDeps"));
            BindMethod(v, _SC("SetExternalDeps"),
                       Generic_SetString<ProjectBuildTarget, &ProjectBuildTarget::SetExternalDeps>,
                       _SC("ProjectBuildTarget::SetExternalDeps"));
            BindMethod(v, _SC("SetAdditionalOutputFiles"),
                       Generic_SetString<ProjectBuildTarget, &ProjectBuildTarget::SetAdditionalOutputFiles>,
                       _SC("ProjectBuildTarget::SetAdditionalOutputFiles"));
            BindMethod(v, _SC("GetAdditionalOutputFiles"),
                       Generic_GetCString<ProjectBuildTarget, &ProjectBuildTarget::GetAdditionalOutputFiles>,
                       _SC("ProjectBuildTarget::GetAdditionalOutputFiles"));
            BindMethod(v, _SC("GetIncludeInTargetAll"),
                       Generic_GetBool<ProjectBuildTarget, &ProjectBuildTarget::GetIncludeInTargetAll>,
                       _SC("ProjectBuildTarget::GetIncludeInTargetAll"));
            BindMethod(v, _SC("SetIncludeInTargetAll"),
                       Generic_SetBool<ProjectBuildTarget, &ProjectBuildTarget::SetIncludeInTargetAll>,
                       _SC("ProjectBuildTarget::SetIncludeInTargetAll"));
            BindMethod(v, _SC("GetCreateDefFile"),
                       Generic_GetBool<ProjectBuildTarget, &ProjectBuildTarget::GetCreateDefFile>,
                       _SC("ProjectBuildTarget::GetCreateDefFile"));
            BindMethod(v, _SC("SetCreateDefFile"),
                       Generic_SetBool<ProjectBuildTarget, &ProjectBuildTarget::SetCreateDefFile>,
                       _SC("ProjectBuildTarget::SetCreateDefFile"));
            BindMethod(v, _SC("GetCreateStaticLib"),
                       Generic_GetBool<ProjectBuildTarget, &ProjectBuildTarget::GetCreateStaticLib>,
                       _SC("ProjectBuildTarget::GetCreateStaticLib"));
            BindMethod(v, _SC("SetCreateStaticLib"),
                       Generic_SetBool<ProjectBuildTarget, &ProjectBuildTarget::SetCreateStaticLib>,
                       _SC("ProjectBuildTarget::SetCreateStaticLib"));
            BindMethod(v, _SC("GetUseConsoleRunner"),
                       Generic_GetBool<ProjectBuildTarget, &ProjectBuildTarget::GetUseConsoleRunner>,
                       _SC("ProjectBuildTarget::GetUseConsoleRunner"));
            BindMethod(v, _SC("SetUseConsoleRunner"),
                       Generic_SetBool<ProjectBuildTarget, &ProjectBuildTarget::SetUseConsoleRunner>,
                       _SC("ProjectBuildTarget::SetUseConsoleRunner"));

            BindMethod(v, _SC("GetFilesCount"), Generic_GetFilesCount<ProjectBuildTarget>,
                       _SC("ProjectBuildTarget::GetFilesCount"));
            BindMethod(v, _SC("GetFile"), Generic_GetFile<ProjectBuildTarget>,
                       _SC("ProjectBuildTarget::GetFile"));

            BindDefaultInstanceCmp<ProjectBuildTarget>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register cbProject
            const SQInteger classDecl = CreateClassDecl<cbProject>(v, _SC("cbProject"), _SC("CompileTargetBase"));
            BindMethod(v, _SC("GetModified"), Generic_GetBool<cbProject, &cbProject::GetModified>,
                       _SC("cbProject::GetModified"));
            BindMethod(v, _SC("SetModified"), Generic_SetBool<cbProject, &cbProject::SetModified>,
                       _SC("cbProject::SetModified"));
            BindMethod(v, _SC("GetMakefile"), Generic_GetCString<cbProject, &cbProject::GetMakefile>,
                       _SC("cbProject::GetMakefile"));
            BindMethod(v, _SC("SetMakefile"), Generic_SetString<cbProject, &cbProject::SetMakefile>,
                       _SC("cbProject::SetMakefile"));
            BindMethod(v, _SC("IsMakefileCustom"), Generic_GetBool<cbProject, &cbProject::IsMakefileCustom>,
                       _SC("cbProject::IsMakefileCustom"));
            BindMethod(v, _SC("SetMakefileCustom"), Generic_SetBool<cbProject, &cbProject::SetMakefileCustom>,
                       _SC("cbProject::SetMakefileCustom"));
            BindMethod(v, _SC("CloseAllFiles"), cbProject_CloseAllFiles,
                       _SC("cbProject::CloseAllFiles"));
            BindMethod(v, _SC("SaveAllFiles"),
                       Generic_DoSomethingGetBool<cbProject, &cbProject::SaveAllFiles>,
                       _SC("cbProject::SaveAllFiles"));
            BindMethod(v, _SC("Save"),
                       Generic_DoSomethingGetBool<cbProject, &cbProject::Save>,
                       _SC("cbProject::Save"));
            // SaveAs is deemed unsafe, so it is not bound
            BindMethod(v, _SC("SaveLayout"),
                       Generic_DoSomethingGetBool<cbProject, &cbProject::SaveLayout>,
                       _SC("cbProject::SaveLayout"));
            BindMethod(v, _SC("LoadLayout"),
                       Generic_DoSomethingGetBool<cbProject, &cbProject::LoadLayout>,
                       _SC("cbProject::LoadLayout"));
            BindMethod(v, _SC("GetCommonTopLevelPath"),
                       Generic_GetStringConst<cbProject, &cbProject::GetCommonTopLevelPath>,
                       _SC("cbProject::GetCommonTopLevelPath"));
            BindMethod(v, _SC("GetFilesCount"), Generic_GetFilesCount<cbProject>,
                       _SC("cbProject::GetFilesCount"));
            BindMethod(v, _SC("GetFile"), Generic_GetFile<cbProject>, _SC("cbProject::GetFile"));
            BindMethod(v, _SC("GetFileByFilename"), cbProject_GetFileByFilename,
                       _SC("cbProject::GetFileByFilename"));
            BindMethod(v, _SC("RemoveFile"), cbProject_RemoveFile, _SC("cbProject::RemoveFile"));
            BindMethod(v, _SC("AddFile"), cbProject_AddFile, _SC("cbProject::AddFile"));
            BindMethod(v, _SC("GetBuildTargetsCount"), cbProject_GetBuildTargetsCount,
                       _SC("cbProject::GetBuildTargetsCount"));
            BindMethod(v, _SC("GetBuildTarget"), cbProject_GetBuildTarget,
                       _SC("cbProject::GetBuildTarget"));
            BindMethod(v, _SC("AddBuildTarget"), cbProject_AddBuildTarget,
                       _SC("cbProject::AddBuildTarget"));
            BindMethod(v, _SC("RenameBuildTarget"), cbProject_RenameBuildTarget,
                       _SC("cbProject::RenameBuildTarget"));
            BindMethod(v, _SC("DuplicateBuildTarget"), cbProject_DuplicateBuildTarget,
                       _SC("cbProject::DuplicateBuildTarget"));
            BindMethod(v, _SC("RemoveBuildTarget"), cbProject_RemoveBuildTarget,
                       _SC("cbProject::RemoveBuildTarget"));
            BindMethod(v, _SC("ExportTargetAsProject"), cbProject_ExportTargetAsProject,
                       _SC("cbProject::ExportTargetAsProject"));
            BindMethod(v, _SC("BuildTargetValid"), cbProject_BuildTargetValid,
                       _SC("cbProject::BuildTargetValid"));
            BindMethod(v, _SC("GetFirstValidBuildTargetName"),
                       cbProject_GetFirstValidBuildTargetName,
                       _SC("cbProject::GetFirstValidBuildTargetName"));
            BindMethod(v, _SC("SetDefaultExecuteTarget"),
                       Generic_SetString<cbProject, &cbProject::SetDefaultExecuteTarget>,
                       _SC("cbProject::SetDefaultExecuteTarget"));
            BindMethod(v, _SC("GetDefaultExecuteTarget"),
                       Generic_GetCString<cbProject, &cbProject::GetDefaultExecuteTarget>,
                       _SC("cbProject::GetDefaultExecuteTarget"));
            BindMethod(v, _SC("SetActiveBuildTarget"), cbProject_SetActiveBuildTarget,
                       _SC("cbProject::SetActiveBuildTarget"));
            BindMethod(v, _SC("GetActiveBuildTarget"),
                       Generic_GetCString<cbProject, &cbProject::GetActiveBuildTarget>,
                       _SC("cbProject::GetActiveBuildTarget"));
            BindMethod(v, _SC("SelectTarget"), cbProject_SelectTarget,
                       _SC("cbProject::SelectTarget"));
            BindMethod(v, _SC("GetCurrentlyCompilingTarget"), cbProject_GetCurrentlyCompilingTarget,
                       _SC("cbProject::GetCurrentlyCompilingTarget"));
            BindMethod(v, _SC("SetCurrentlyCompilingTarget"), cbProject_SetCurrentlyCompilingTarget,
                       _SC("cbProject::SetCurrentlyCompilingTarget"));
            BindMethod(v, _SC("GetModeForPCH"), cbProject_GetModeForPCH,
                       _SC("cbProject::GetModeForPCH"));
            BindMethod(v, _SC("SetModeForPCH"), cbProject_SetModeForPCH,
                       _SC("cbProject::SetModeForPCH"));
            BindMethod(v, _SC("SetExtendedObjectNamesGeneration"),
                       Generic_SetBool<cbProject, &cbProject::SetExtendedObjectNamesGeneration>,
                       _SC("cbProject::SetExtendedObjectNamesGeneration"));
            BindMethod(v, _SC("GetExtendedObjectNamesGeneration"),
                       Generic_GetBool<cbProject, &cbProject::GetExtendedObjectNamesGeneration>,
                       _SC("cbProject::GetExtendedObjectNamesGeneration"));
            BindMethod(v, _SC("SetNotes"), Generic_SetString<cbProject, &cbProject::SetNotes>,
                       _SC("cbProject::SetNotes"));
            BindMethod(v, _SC("GetNotes"), Generic_GetCString<cbProject, &cbProject::GetNotes>,
                       _SC("cbProject::GetNotes"));
            BindMethod(v, _SC("SetShowNotesOnLoad"),
                       Generic_SetBool<cbProject, &cbProject::SetShowNotesOnLoad>,
                       _SC("cbProject::SetShowNotesOnLoad"));
            BindMethod(v, _SC("GetShowNotesOnLoad"),
                       Generic_GetBool<cbProject, &cbProject::GetShowNotesOnLoad>,
                       _SC("cbProject::GetShowNotesOnLoad"));
            BindMethod(v, _SC("SetCheckForExternallyModifiedFiles"),
                       Generic_SetBool<cbProject, &cbProject::SetCheckForExternallyModifiedFiles>,
                       _SC("cbProject::SetCheckForExternallyModifiedFiles"));
            BindMethod(v, _SC("GetCheckForExternallyModifiedFiles"),
                       Generic_GetBool<cbProject, &cbProject::GetCheckForExternallyModifiedFiles>,
                       _SC("cbProject::GetCheckForExternallyModifiedFiles"));
            BindMethod(v, _SC("ShowNotes"), cbProject_ShowNotes, _SC("cbProject::ShowNotes"));
            BindMethod(v, _SC("AddToExtensions"),
                       Generic_SetString<cbProject, &cbProject::AddToExtensions>,
                       _SC("cbProject::AddToExtensions"));
            BindMethod(v, _SC("ExtensionListNodes"), cbProject_ExtensionListNodes,
                       _SC("cbProject::ExtensionListNodes"));
            BindMethod(v, _SC("ExtensionListNodeAttributes"), cbProject_ExtensionListNodeAttributes,
                       _SC("cbProject::ExtensionListNodeAttributes"));
            BindMethod(v, _SC("ExtensionGetNodeAttribute"),
                       cbProject_ExtensionGetNodeAttribute,
                       _SC("cbProject::ExtensionGetNodeAttribute"));
            BindMethod(v, _SC("ExtensionSetNodeAttribute"), cbProject_ExtensionSetNodeAttribute,
                       _SC("cbProject::ExtensionSetNodeAttribute"));
            BindMethod(v, _SC("ExtensionRemoveNodeAttribute"),
                       cbProject_ExtensionRemoveNodeAttribute,
                       _SC("cbProject::ExtensionRemoveNodeAttribute"));
            BindMethod(v, _SC("ExtensionAddNode"), cbProject_ExtensionAddNode,
                       _SC("cbProject::ExtensionAddNode"));
            BindMethod(v, _SC("ExtensionRemoveNode"), cbProject_ExtensionRemoveNode,
                       _SC("cbProject::ExtensionRemoveNode"));
            BindMethod(v, _SC("DefineVirtualBuildTarget"), cbProject_DefineVirtualBuildTarget,
                       _SC("cbProject::DefineVirtualBuildTarget"));
            BindMethod(v, _SC("HasVirtualBuildTarget"), cbProject_HasVirtualBuildTarget,
                       _SC("cbProject::HasVirtualBuildTarget"));
            BindMethod(v, _SC("RemoveVirtualBuildTarget"), cbProject_RemoveVirtualBuildTarget,
                       _SC("cbProject::RemoveVirtualBuildTarget"));
            BindMethod(v, _SC("GetVirtualBuildTargets"), cbProject_GetVirtualBuildTargets,
                       _SC("cbProject::GetVirtualBuildTargets"));
            BindMethod(v, _SC("GetVirtualBuildTargetGroup"), cbProject_GetVirtualBuildTargetGroup,
                       _SC("cbProject::GetVirtualBuildTargetGroup"));
            BindMethod(v, _SC("GetExpandedVirtualBuildTargetGroup"),
                       cbProject_GetExpandedVirtualBuildTargetGroup,
                       _SC("cbProject::GetExpandedVirtualBuildTargetGroup"));
            BindMethod(v, _SC("CanAddToVirtualBuildTarget"), cbProject_CanAddToVirtualBuildTarget,
                       _SC("cbProject::CanAddToVirtualBuildTarget"));

            BindDefaultInstanceCmp<cbProject>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register ProjectManager
            const SQInteger classDecl = CreateClassDecl<ProjectManager>(v, _SC("ProjectManager"));
            BindMethod(v, _SC("NewProject"), ProjectManager_NewProject,
                       _SC("ProjectManager::NewProject"));
            BindMethod(v, _SC("AddFileToProject"), ProjectManager_AddFileToProject,
                       _SC("ProjectManager::AddFileToProject"));
            BindMethod(v, _SC("CloseWorkspace"), ProjectManager_CloseWorkspace,
                       _SC("ProjectManager::CloseWorkspace"));
            BindMethod(v, _SC("CloseProject"), ProjectManager_CloseProject,
                       _SC("ProjectManager::CloseProject"));
            BindMethod(v, _SC("RebuildTree"), ProjectManager_RebuildTree,
                       _SC("ProjectManager::RebuildTree"));

            BindDefaultInstanceCmp<ProjectManager>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register EditorBase
            const SQInteger classDecl = CreateClassDecl<EditorBase>(v, _SC("EditorBase"));
            BindMethod(v, _SC("Close"), EditorBase_Close, _SC("EditorBase::Close"));

            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register cbEditor
            const SQInteger classDecl = CreateClassDecl<cbEditor>(v, _SC("cbEditor"),
                                                                  _SC("EditorBase"));
            BindMethod(v, _SC("SetText"), cbEditor_SetText, _SC("cbEditor::SetText"));

            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register EditorManager
            const SQInteger classDecl = CreateClassDecl<EditorManager>(v, _SC("EditorManager"));
            BindMethod(v, _SC("New"), EditorManager_New, _SC("EditorManager::New"));

            BindDefaultInstanceCmp<EditorManager>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register UserVariableManager
            const SQInteger classDecl = CreateClassDecl<UserVariableManager>(v, _SC("UserVariableManager"));
            BindMethod(v, _SC("Exists"), UserVariableManager_Exists, _SC("UserVariableManager::Exists"));

            BindDefaultInstanceCmp<UserVariableManager>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register ScriptingManager
            const SQInteger classDecl = CreateClassDecl<ScriptingManager>(v, _SC("ScriptingManager"));
            BindMethod(v, _SC("RegisterScriptMenu"), ScriptingManager_RegisterScriptMenu,
                       _SC("ScriptingManager::RegisterScriptMenu"));

            BindDefaultInstanceCmp<ScriptingManager>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register CompilerFactory
            const SQInteger classDecl = CreateClassDecl<CompilerFactory>(v, _SC("CompilerFactory"));

            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register PluginInfo
            const SQInteger classDecl = CreateClassDecl<PluginInfo>(v, _SC("PluginInfo"));
            BindEmptyCtor<PluginInfo>(v);

            MembersType<PluginInfo> &members = BindMembers<PluginInfo>(v);
            addMemberRef(members, _SC("name"), &PluginInfo::name);
            addMemberRef(members, _SC("title"), &PluginInfo::title);
            addMemberRef(members, _SC("version"), &PluginInfo::version);
            addMemberRef(members, _SC("description"), &PluginInfo::description);
            addMemberRef(members, _SC("author"), &PluginInfo::author);
            addMemberRef(members, _SC("authorEmail"), &PluginInfo::authorEmail);
            addMemberRef(members, _SC("authorWebsite"), &PluginInfo::authorWebsite);
            addMemberRef(members, _SC("thanksTo"), &PluginInfo::thanksTo);
            addMemberRef(members, _SC("license"), &PluginInfo::license);

            BindDefaultInstanceCmp<PluginInfo>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register FileTreeData
            const SQInteger classDecl = CreateClassDecl<FileTreeData>(v, _SC("FileTreeData"));

            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        sq_pop(v, 1); // Pop root table.

        // called last because it needs a few previously registered types
        Register_ScriptPlugin(v);
    }

    void UnregisterBindings(HSQUIRRELVM v)
    {
        Unregister_Constants(v);
    }
} // namespace ScriptBindings
#endif // 0
