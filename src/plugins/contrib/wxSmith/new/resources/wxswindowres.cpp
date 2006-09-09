#include "wxswindowres.h"

#include "../wxsglobals.h"
#include "../wxsitem.h"
#include "../wxsparent.h"
#include "../wxsresourcetree.h"
#include "../wxsproject.h"
#include "../wxsitemfactory.h"
#include "../wxsextresmanager.h"
#include "../wxseditor.h"
#include "../wxscoder.h"
#include "../wxspredefinedids.h"
#include "../editors/wxswindoweditor.h"
#include <manager.h>
#include <editormanager.h>
#include <messagemanager.h>
#include <wxFlatNotebook.h>
#include <sstream>

namespace
{
    const wxChar* EmptySource =
    _T("#include \"$(Include)\"\n")
    _T("\n")
    _T("BEGIN_EVENT_TABLE($(ClassName),$(BaseClassName))\n")
    // TODO: Remove event table code when sure that Connect does work
    //       perfectly
    _T("\t") wxsBHeader("EventTable","$(ClassName)") _T("\n")
    _T("\t") wxsBEnd() _T("\n")
    _T("END_EVENT_TABLE()\n")
    _T("\n")
    _T("$(ClassName)::$(ClassName)(wxWindow* parent,wxWindowID id)\n")
    _T("{\n")
    _T("\t") wxsBHeader("Initialize","$(ClassName)") _T("\n")
    _T("\t") wxsBEnd() _T("\n")
    _T("}\n")
    _T("\n")
    _T("$(ClassName)::~$(ClassName)()\n")
    _T("{\n")
    _T("}\n");

    const wxChar* EmptyHeader =
    _T("#ifndef $(Guard)\n")
    _T("#define $(Guard)\n")
    _T("\n")
    wxsBHeader("Headers","$(ClassName)") _T("\n")
    wxsBEnd() _T("\n")
    _T("\n")
    _T("class $(ClassName): public $(BaseClassName)\n")
    _T("{\n")
    _T("\tpublic:\n")
    _T("\n")
    _T("\t\t$(ClassName)(wxWindow* parent,wxWindowID id = -1);\n")
    _T("\t\tvirtual ~$(ClassName)();\n")
    _T("\n")
    _T("\t\t") wxsBHeader("Identifiers","$(ClassName)") _T("\n")
    _T("\t\t") wxsBEnd() _T("\n")
    _T("\n")
    _T("\tprotected:\n")
    _T("\n")
    _T("\t\t") wxsBHeader("Handlers","$(ClassName)") _T("\n")
    _T("\t\t") wxsBEnd() _T("\n")
    _T("\n")
    _T("\t\t") wxsBHeader("Declarations","$(ClassName)") _T("\n")
    _T("\t\t") wxsBEnd() _T("\n")
    _T("\n")
    _T("\tprivate:\n")
    _T("\n")
    _T("\t\tDECLARE_EVENT_TABLE()\n")
    _T("};\n")
    _T("\n")
    _T("#endif\n");
}

wxsWindowRes::wxsWindowRes(wxsProject* Project):
    wxsResource(Project),
    RootItem(NULL),
    RootSelection(NULL),
    Preview(NULL)
{
}

wxsWindowRes::~wxsWindowRes()
{
    EditClose();
    if ( RootItem )
    {
        wxsKILL(RootItem);
    }
}

const wxString& wxsWindowRes::GetResourceName()
{
     return ClassName;
}

void wxsWindowRes::BuildTree(wxTreeCtrl* Tree,wxTreeItemId ParentId)
{
    SetTreeItemId(Tree->AppendItem(ParentId,ClassName,-1,-1,new wxsResourceTreeData(this)));
}

bool wxsWindowRes::LoadConfiguration(TiXmlElement* Element)
{
    wxASSERT_MSG( GetProject() != NULL,
        _T("Can not call wxsWindowRes::LoadConfiguration when there's no project") );

    ClassName = FixFileName(cbC2U(Element->Attribute("class")));
    WxsFile   = FixFileName(cbC2U(Element->Attribute("wxs_file")));
    SrcFile   = FixFileName(cbC2U(Element->Attribute("src_file")));
    HFile     = FixFileName(cbC2U(Element->Attribute("header_file")));
    XrcFile   = FixFileName(cbC2U(Element->Attribute("xrc_file")));

    BasePropsFilter = !strcmp(Element->Attribute("edit_mode"),"Source") ?
            wxsFLSource : wxsFLMixed;

    // Checking of existance of required files
    return
        wxFile::Exists(GetProject()->GetInternalFileName(WxsFile)) &&
        wxFile::Exists(GetProject()->GetProjectFileName(SrcFile)) &&
        wxFile::Exists(GetProject()->GetProjectFileName(HFile)) &&
        ( BasePropsFilter==wxsFLSource ||
          wxFile::Exists(GetProject()->GetProjectFileName(XrcFile)));
}

bool wxsWindowRes::SaveConfiguration(TiXmlElement* Element)
{
    wxASSERT_MSG( GetProject() != NULL,
        _T("Can not call wxsWindowRes::SaveConfiguration when there's no project") );

    Element->SetAttribute("class",cbU2C(ClassName));
    Element->SetAttribute("wxs_file",cbU2C(WxsFile));
    Element->SetAttribute("src_file",cbU2C(SrcFile));
    Element->SetAttribute("header_file",cbU2C(HFile));
    if ( BasePropsFilter==wxsFLMixed )
    {
        Element->SetAttribute("xrc_file",cbU2C(XrcFile));
        Element->SetAttribute("edit_mode","Xrc");
    }
    else
    {
        Element->SetAttribute("edit_mode","Source");
    }
    return true;
}

void wxsWindowRes::BindExternalResource(const wxString& FileName,const wxString& _ClassName)
{
    ClassName = _ClassName;
    WxsFile = _T("");
    SrcFile = _T("");
    HFile = _T("");
    // We do not fix file name since it's global path
    XrcFile = FileName;
    BasePropsFilter = wxsFLFile;
}

bool wxsWindowRes::UsingXRC()
{
    return BasePropsFilter != wxsFLSource;
}

wxsEditor* wxsWindowRes::CreateEditor()
{
    // Loading resource
    if ( !GetRootItem() && !LoadResource() )
    {
        wxMessageBox(_("Error while loading resource"));
        return NULL;
    }

    // Checking existance of root item
    wxASSERT_MSG(RootItem!=NULL,_T("Did not create root window item during loading"));

    // Building tree for resource
	RootItem->BuildItemTree(wxsTREE(),GetTreeItemId());

	// Creating new editor
    wxsWindowEditor* Edit = new wxsWindowEditor(Manager::Get()->GetEditorManager()->GetNotebook(),this);
    return Edit;
}

void wxsWindowRes::EditorClosed()
{
    wxsTREE()->UnselectAll();
    wxsTREE()->DeleteChildren(GetTreeItemId());
    if ( RootItem )
    {
        wxsKILL(RootItem);
        RootItem = NULL;
    }

	if ( !GetProject() )
	{
	    wxsEXTRES()->ResClosed(this);
	}
	Modified = false;
}

bool wxsWindowRes::LoadResource()
{
    if ( GetEditor() )
    {
        DBGLOG(_T("Internal wxSmith error: Can not load resource when editor is opened"));
        return false;
    }

	if ( RootItem )
	{
	    wxsKILL(RootItem);
	}

	RootItem = BuildRootItem();
    if ( !RootItem )
    {
        DBGLOG(_T("Internal wxSmith error: Did not create root resource item"));
        return false;
    }

    bool RequireUpdate = false;     // Will be set to true if convertin from old format (duplcated structure) to new one (separate structure and extra info)

    // Clearing modified flag
    Modified = false;

	wxString GlobalXRC = GetProject() ? GetProject()->GetProjectFileName(XrcFile) : XrcFile;
	wxString GlobalWXS = GetProject() ? GetProject()->GetInternalFileName(WxsFile) : WxsFile;

    if ( GetBasePropsFilter() == wxsFLFile )
    {
        TiXmlDocument DocXRC;

        // File mode - we fetch XRC structure only
        if ( !DocXRC.LoadFile(cbU2C(GlobalXRC)) )
        {
            LOG(_("wxSmith: Couldn't load '%s' file."),GlobalXRC.c_str());
            LOG(_("wxSmith:    (Either file can not be opened or it has not valid XML structure)."));
            return false;
        }
        TiXmlElement* Structure = TiXmlHandle(&DocXRC).FirstChildElement("resource").FirstChildElement("object").Element();
        if ( !Structure )
        {
            LOG(_("wxSmith: Invalid XRC file structure - no \"object\" node in '%s'."),GlobalXRC.c_str());
            return false;
        }
        if ( cbC2U(Structure->Attribute("class")) != GetRootItemClass() )
        {
            LOG(_("wxSmith: Invalid XRC file structure - root object is not of type '%s' in '%s'."),GetRootItemClass().c_str(),GlobalXRC.c_str());
            return false;
        }

        return RootItem->XmlRead(Structure,true,false);
    }
    else if ( GetBasePropsFilter() == wxsFLMixed )
    {
        TiXmlDocument DocWXS;
        TiXmlDocument DocXRC;

        // Mixed mode - wxs can contain extra data only or structure and extra
        if ( !DocWXS.LoadFile(cbU2C(GlobalWXS)) )
        {
            LOG(_("wxSmith: Couldn't load '%s' file."),GlobalWXS.c_str());
            LOG(_("wxSmith:    (Either file can not be opened or it has not valid XML structure)."));
            return false;
        }
        TiXmlElement* Extra = TiXmlHandle(&DocWXS).FirstChildElement("resource").FirstChildElement("extra").Element();
        if ( Extra && (cbC2U(Extra->Attribute("class")) != GetRootItemClass()) )
        {
            Extra = NULL;
        }
        TiXmlElement* Structure = TiXmlHandle(&DocXRC).FirstChildElement("resource").FirstChildElement("object").Element();
        if ( Structure )
        {
            // TODO: Show some notification dialog
            LOG(_("wxSmith warning: Resource '%s' is using obsolete wxs format, it will be updated."),ClassName.c_str());
            RequireUpdate = true;
        }
        if ( !Structure && !Extra )
        {
            LOG(_("wxSmith warning: Did not found extra data for '%s' resource, some data may be lost"),ClassName.c_str());
        }

        // XRC file can additionally handle structure but without extra informations
        // if WXS file contains structure, it has higher priority
        if ( !Structure )
        {
            if ( !DocXRC.LoadFile(cbU2C(GlobalXRC)) )
            {
                LOG(_("wxSmith: Couldn't load '%s' file."),GlobalXRC.c_str());
                LOG(_("wxSmith:    (Either file can not be opened or it has not valid XML structure)."));
                return false;
            }
            Structure = TiXmlHandle(&DocXRC).FirstChildElement("resource").FirstChildElement("object").Element();
            if ( !Structure )
            {
                LOG(_("wxSmith: Invalid XRC file structure - no \"object\" node in '%s'."),GlobalXRC.c_str());
                return false;
            }
            if ( cbC2U(Structure->Attribute("class")) != GetRootItemClass() )
            {
                LOG(_("wxSmith: Invalid XRC file structure - root object is not of type '%s' in '%s'."),GetRootItemClass().c_str(),GlobalXRC.c_str());
                return false;
            }
        }

        if ( Extra )
        {
            // Reading structure from Structure node
            RootItem->XmlRead(Structure,true,false);

            // Reading additional extra info.
            // Ths will be done per widget because additional extra data
            // is flatten.
            // First we have to create map ids->items

            IdToItemMapT Map;
            CollectIdMap(Map,RootItem);

            // Now reading all enteries from extra data
            for ( TiXmlElement* Child = Extra->FirstChildElement("object");
                  Child != NULL; Child = Child->NextSiblingElement("object") )
            {
                wxString Id = cbC2U(Child->Attribute("name"));
                IdToItemMapT::iterator i = Map.find(Id);
                if ( i != Map.end() && i->second!=NULL )
                {
                    i->second->XmlRead(Extra,false,true);
                }
            }
        }
        else
        {
            RootItem->XmlRead(Structure,true,true);
        }
    }
    else if ( GetBasePropsFilter() == wxsFLSource )
    {
        TiXmlDocument DocWXS;

        // Source mode - extra and structure is inside WXS
        if ( !DocWXS.LoadFile(cbU2C(GlobalWXS)) )
        {
            LOG(_("wxSmith: Couldn't load '%s' file."),GlobalWXS.c_str());
            LOG(_("wxSmith:    (Either file can not be opened or it has not valid XML structure)."));
            return false;
        }
        TiXmlElement* Structure = TiXmlHandle(&DocWXS).FirstChildElement("resource").FirstChildElement("object").Element();
        if ( !Structure )
        {
            LOG(_("wxSmith: Invalid WXS file structure - no \"object\" node in '%s'."),GlobalWXS.c_str());
            return false;
        }
        if ( cbC2U(Structure->Attribute("class")) != GetRootItemClass() )
        {
            LOG(_("wxSmith: Invalid WXS file structure - root object is not of type '%s' in '%s'."),GetRootItemClass().c_str(),GlobalWXS.c_str());
            return false;
        }

        RootItem->XmlRead(Structure,true,true);
    }

    if ( RequireUpdate )
    {
        Modified = true;
        SaveResource();
    }

    Modified = false;
    return true;
}

void wxsWindowRes::SaveResource()
{
    if ( !GetModified() ) return;
    if ( !RootItem ) return;

    wxString GlobalXRC = GetProject() ? GetProject()->GetProjectFileName(XrcFile) : XrcFile;
	wxString GlobalWXS = GetProject() ? GetProject()->GetInternalFileName(WxsFile) : WxsFile;

    if ( GetBasePropsFilter() == wxsFLFile )
    {
        TiXmlDocument Doc;
        TiXmlElement* Obj = Doc.InsertEndChild(TiXmlElement("resource"))->InsertEndChild(TiXmlElement("object"))->ToElement();
        Obj->SetAttribute("class",cbU2C(GetRootItemClass()));
        RootItem->XmlWrite(Obj,true,false);
        Doc.SaveFile(cbU2C(GlobalXRC));
    }
    else if ( GetBasePropsFilter() == wxsFLMixed )
    {
        TiXmlDocument DocXRC;
        TiXmlDocument DocWXS;
        TiXmlElement* Obj = DocXRC.InsertEndChild(TiXmlElement("resource"))->InsertEndChild(TiXmlElement("object"))->ToElement();
        Obj->SetAttribute("class",cbU2C(GetRootItemClass()));
        TiXmlElement* Ext = DocWXS.InsertEndChild(TiXmlElement("resource"))->InsertEndChild(TiXmlElement("extra"))->ToElement();
        Ext->SetAttribute("class",cbU2C(GetRootItemClass()));
        RootItem->XmlWrite(Obj,true,false);
        SaveChildrenExtra(RootItem,Ext);
        DocXRC.SaveFile(cbU2C(GlobalXRC));
        DocWXS.SaveFile(cbU2C(GlobalWXS));
    }
    else if ( GetBasePropsFilter() == wxsFLSource )
    {
        TiXmlDocument Doc;
        TiXmlElement* Obj = Doc.InsertEndChild(TiXmlElement("resource"))->InsertEndChild(TiXmlElement("object"))->ToElement();
        Obj->SetAttribute("class",cbU2C(GetRootItemClass()));
        RootItem->XmlWrite(Obj,true,true);
        Doc.SaveFile(cbU2C(GlobalWXS));
    }

    if ( GetEditor() )
    {
        GetEditor()->SetModified(false);
    }

    Modified = false;
}

void wxsWindowRes::SaveChildrenExtra(wxsItem* Item,TiXmlElement* Element)
{
    if ( !Item ) return;

    if ( !Item->GetIdName().empty() )
    {
        TiXmlElement* ChildExtra = Element->InsertEndChild(TiXmlElement("object"))->ToElement();
        ChildExtra->SetAttribute("name",cbU2C(Item->GetIdName()));
        Item->XmlWrite(ChildExtra,false,true);
    }

    wxsParent* Parent = Item->ToParent();
    if ( Parent )
    {
        int Count = Parent->GetChildCount();
        for ( int i=0; i<Count; i++ )
        {
            SaveChildrenExtra(Parent->GetChild(i),Element);
        }
    }
}

void wxsWindowRes::CollectIdMap(IdToItemMapT& Map,wxsItem* Item,bool WithRoot)
{
    if ( WithRoot )
    {
        if ( (Item->GetPropertiesFlags() & wxsFLId ) &&
             !Item->GetIdName().empty() )
        {
            Map[Item->GetIdName()] = Item;
        }
    }

    wxsParent* Parent = Item->ToParent();
    if ( Parent )
    {
        int Count = Parent->GetChildCount();
        for ( int i=0; i<Count; i++ )
        {
            CollectIdMap(Map,Parent->GetChild(i),true);
        }
    }
}

wxString wxsWindowRes::GetXmlData()
{
    TiXmlDocument Doc;
    TiXmlElement* Obj = Doc.InsertEndChild(TiXmlElement("resource"))->InsertEndChild(TiXmlElement("object"))->ToElement();
    Obj->SetAttribute("class",cbU2C(GetRootItemClass()));
    RootItem->XmlWrite(Obj,true,true);

    #ifdef TIXML_USE_STL
        std::ostringstream buffer;
        buffer << Doc;
        return cbC2U(buffer.str().c_str());
    #else
        TiXmlOutStream buffer;
        buffer << Doc;
        return cbC2U(buffer.c_str());
    #endif
}

bool wxsWindowRes::SetXmlData(const wxString& Data)
{
    TiXmlDocument Doc;
	Doc.Parse(cbU2C(Data));
    if ( Doc.Error() )
    {
        DBGLOG(_T("wxSmith: Error loading Xml data -> ") + cbC2U(Doc.ErrorDesc()));
    	return false;
    }

    TiXmlElement* XmlElem = Doc.FirstChildElement("resource");
    if ( !XmlElem )
    {
        DBGLOG(_T("wxSmith: No \"resource\" node in Xml data"));
    	return false;
    }
    TiXmlElement* ObjElem = XmlElem->FirstChildElement("object");
    if ( !XmlElem )
    {
        DBGLOG(_T("wxSmith: No \"object\" node in Xml data"));
    	return false;
    }

    if ( cbC2U(ObjElem->Attribute("class")) != GetRootItemClass() )
    {
        DBGLOG(_T("wxSmith: Invalid main object class"));
        return false;
    }

	if ( RootItem )
	{
	    wxsKILL(RootItem);
	    RootItem = BuildRootItem();
	    if ( !RootItem )
	    {
            DBGLOG(_T("Internal wxSmith error: Did not create root resource item"));
            return false;
	    }
	}

    RootItem->XmlRead(ObjElem,true,true);
    return true;
}

void wxsWindowRes::ShowPreview()
{
    if ( !Preview )
    {
        Preview = BuildPreview();
    }
    if ( !Preview )
    {
        return;
    }
    Preview->Show();
    Preview->Raise();
}

void wxsWindowRes::HidePreview()
{
    if ( !Preview )
    {
        return;
    }
    delete Preview;
    Preview = NULL;
}

bool wxsWindowRes::IsPreview()
{
    return Preview != NULL;
}

void wxsWindowRes::NotifyPreviewClosed()
{
    Preview = NULL;
}

bool wxsWindowRes::CreateNewResource(TiXmlElement* Element)
{
    if ( !GetProject() ) return false;

    // Forcing resource to load configuration
    LoadConfiguration(Element);

    // Generating guard macro name

    wxString FName = wxFileName(HFile).GetFullName();
    FName.MakeUpper();
    wxString Guard;

    for ( int i=0; i<(int)FName.Length(); i++ )
    {
        wxChar ch = FName.GetChar(i);
        if ( ( ch < _T('A') || ch > _T('Z') ) &&
             ( ch < _T('0') || ch > _T('9') ) ) Guard.Append(_T('_'));
        else Guard.Append(ch);
    }

    // Generating valid include path

    wxFileName IncludeFN(GetProject()->GetProjectFileName(HFile));
    IncludeFN.MakeRelativeTo(
        wxFileName(GetProject()->GetProjectFileName(SrcFile)).GetPath() );
    wxString Include = IncludeFN.GetFullPath(wxPATH_UNIX);

    wxString Content = EmptyHeader;
    Content.Replace(_T("$(Guard)"),Guard,true);
    Content.Replace(_T("$(ClassName)"),ClassName,true);
    Content.Replace(_T("$(BaseClassName)"),GetRootItemClass(),true);

    wxFile File(GetProject()->GetProjectFileName(HFile),wxFile::write);
    if ( !File.IsOpened() ) return false;
    // There's no risk that some unicode characters will be found,
    // we can use default encoding
    File.Write(Content);
    File.Close();

    Content = EmptySource;
    Content.Replace(_T("$(Include)"),Include,true);
    Content.Replace(_T("$(ClassName)"),ClassName,true);
    Content.Replace(_T("$(BaseClassName)"),GetRootItemClass(),true);
    File.Open(GetProject()->GetProjectFileName(SrcFile),wxFile::write);
    if ( !File.IsOpened() ) return false;
    File.Write(Content);
    File.Close();

    // Generating new root item
    RootItem = BuildRootItem();
    return true;
}

void wxsWindowRes::NotifyChange(wxsItem* Changed)
{
    // Regenerating source code
    RebuildCode();

    // Applying modified state
    if ( GetEditor() )
    {
        ((wxsWindowEditor*)GetEditor())->NotifyChange(Changed);
    }
    else
    {
        SetModified();
    }
}

void wxsWindowRes::RebuildCode()
{
    if ( !GetProject() ) return;

    switch ( GetLanguage() )
    {
        case wxsCPP:
        {

        //------------------------------------------------------
        // Generating initializing code and global declarations
        //------------------------------------------------------

            wxString CodeHeader = wxString::Format(wxsBHeaderF("Initialize"),GetClassName().c_str());
            wxString Code = _T("\n");
            wxString GlobalVarsCode = _T("\n");

            // Creating local and global declarations
            if ( BuildDeclarations(RootItem,Code,GlobalVarsCode) )
            {
                Code.Append(_T('\n'));
            }

            // Creating window-generating code
            if ( GetBasePropsFilter() == wxsFLSource )
            {
                // Adding code building the structure
                RootItem->BuildCreatingCode(Code,_T("parent"),wxsCPP);
            }
            else if ( GetBasePropsFilter() == wxsFLMixed )
            {
                // Removing all local declarations
                Code = _T("\n");
                Code << BuildXrcLoadingCode();

                // Fetching items from built resource
                FetchXmlBuiltItems(RootItem,Code);
            }

            // Adding code for event handlers
            AddEventHandlers(RootItem,Code);

            wxsADDCODE(
                GetProject()->GetProjectFileName(SrcFile),
                CodeHeader,
                wxsBEnd(),
                Code);

            CodeHeader.Printf(wxsBHeaderF("Declarations"),GetClassName().c_str());
            wxsADDCODE(
                GetProject()->GetProjectFileName(HFile),
                CodeHeader,
                wxsBEnd(),
                GlobalVarsCode);

        //---------------------------------
        // Generating Identifiers
        //---------------------------------

            CodeHeader.Printf(wxsBHeaderF("Identifiers"),GetClassName().c_str());
            Code = _T('\n');
            if ( GetBasePropsFilter() == wxsFLSource )
            {
                wxArrayString IdsArray;
                BuildIdsArray(RootItem,IdsArray);
                if ( IdsArray.Count() )
                {
                    Code.Append(_T("enum Identifiers\n{"));
                    IdsArray.Sort();
                    wxString Previous = _T("");
                    bool First = true;
                    for ( size_t i = 0; i<IdsArray.Count(); ++i )
                    {
                        if ( IdsArray[i] != Previous )
                        {
                            Previous = IdsArray[i];
                            Code.Append( _T("\n\t") );
                            Code.Append( Previous );
                            if ( First )
                            {
                                Code.Append( _T(" = 0x1000") );
                                First = false;
                            }
                            if ( i < IdsArray.Count() - 1 )
                            {
                                Code.Append( _T(',') );
                            }
                        }
                    }
                    Code.Append( _T("\n};\n") );
                }
            }

            wxsADDCODE(
                GetProject()->GetProjectFileName(HFile),
                CodeHeader,
                wxsBEnd(),
                Code);

        //---------------------------------
        // Generating Includes
        //---------------------------------

            wxArrayString DeclHeaders;
            wxArrayString DefHeaders;
            BuildDeclArrays(RootItem,DeclHeaders,DefHeaders);
            DefHeaders.Add(_T("<wx/intl.h>"));
            if ( GetBasePropsFilter() == wxsFLMixed )
            {
                DefHeaders.Add(_T("<wx/xrc/xmlres.h>"));
            }

            // TODO: Split these and put into files separately
            for ( size_t i = DefHeaders.Count(); i-->0; )
            {
                DeclHeaders.Add(DefHeaders[i]);
            }

            DeclHeaders.Sort();
            CodeHeader.Printf(wxsBHeaderF("Headers"),GetClassName().c_str());
            Code = _T("\n");
            wxString Previous = _T("");
            for ( size_t i = 0; i<DeclHeaders.Count(); i++ )
            {
                if ( DeclHeaders[i] != Previous )
                {
                    Previous = DeclHeaders[i];
                    Code << _T("#include ") << Previous << _T("\n");
                }
            }

            wxsADDCODE(
                GetProject()->GetProjectFileName(HFile),
                CodeHeader,
                wxsBEnd(),
                Code);


        //---------------------------------
        // Clearing outdated event array
        //---------------------------------

            CodeHeader.Printf(wxsBHeaderF("EventTable"),ClassName.c_str());
            wxsADDCODE(
                GetProject()->GetProjectFileName(SrcFile),
                CodeHeader,
                wxsBEnd(),
                _T("\n"));

            return;
        }
    }

    wxsLANGMSG(wxsWindowRes::RebuildCode,GetLanguage());

}

bool wxsWindowRes::BuildDeclarations(wxsItem* Item,wxString& Code,wxString& GlobalCode)
{
    wxsParent* Parent = Item->ToParent();
    if ( !Parent ) return false;
    bool Result = false;

	int ChildCnt = Parent->GetChildCount();
	for ( int i=0; i<ChildCnt; i++ )
	{
		wxsItem* Child = Parent->GetChild(i);

		if ( Child->GetPropertiesFlags() & wxsFLVariable )
        {
            if ( Child->GetIsMember() )
            {
                Child->BuildDeclarationCode(GlobalCode,GetLanguage());
            }
            else
            {
                Result = true;
                Child->BuildDeclarationCode(Code,GetLanguage());
            }
		}

		if ( BuildDeclarations(Child,Code,GlobalCode) )
		{
		    Result = true;
		}
	}
	return Result;
}

void wxsWindowRes::AddEventHandlers(wxsItem* Item,wxString& Code)
{
    wxsEvents& Events = Item->GetEvents();
    Events.GenerateBindingCode(Code,UsingXRC(),GetLanguage());

    wxsParent* Parent = Item->ToParent();
    if ( Parent )
    {
        int Cnt = Parent->GetChildCount();
        for ( int i=0; i<Cnt; i++ )
        {
            AddEventHandlers(Parent->GetChild(i),Code);
        }
    }
}

void wxsWindowRes::BuildIdsArray(wxsItem* Item,wxArrayString& Array)
{
    wxsParent* Parent = Item->ToParent();
    if ( !Parent ) return;

	int Cnt = Parent->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsItem* Child = Parent->GetChild(i);
		if ( Child->GetPropertiesFlags() & wxsFLId )
		{
		    const wxString& Name = Child->GetIdName();

		    if ( !wxsPredefinedIDs::Check(Name) )
		    {
                Array.Add(Name);
		    }
		}
		BuildIdsArray(Child,Array);
	}
}

void wxsWindowRes::BuildDeclArrays(wxsItem* Item,wxArrayString& DeclHeaders,wxArrayString& DefHeaders)
{
    Item->EnumDeclFiles(DeclHeaders,DefHeaders,GetLanguage());

    wxsParent* Parent = Item->ToParent();
    if ( !Parent ) return;

	int Cnt = Parent->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
	    BuildDeclArrays(Parent->GetChild(i),DeclHeaders,DefHeaders);
	}
}

void wxsWindowRes::FetchXmlBuiltItems(wxsItem* Item,wxString& Code)
{
    wxsParent* Parent = Item->ToParent();
    if ( !Parent ) return;

	int Cnt = Parent->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsItem* Child = Parent->GetChild(i);
		if ( (Child->GetPropertiesFlags() & (wxsFLVariable|wxsFLId)) == (wxsFLVariable|wxsFLId) && Child->GetIsMember() )
		{
		    const wxString& VarName = Child->GetVarName();
		    const wxString& IdName = Child->GetIdName();

            Code << VarName << _T(" = (") << Child->GetInfo().Name <<
                    _T("*)FindWindow(XRCID(") + IdName + _T("));\n");
		}
		FetchXmlBuiltItems(Child,Code);
	}
}

void wxsWindowRes::SetModified(bool modified)
{
    Modified = modified;
}

wxString wxsWindowRes::GetType()
{
    return GetRootItemClass();
}

bool wxsWindowRes::UsingFile(const wxString& FileName)
{
    if ( !GetProject() )
    {
        return FileName == XrcFile;
    }

    if ( BasePropsFilter == wxsFLMixed )
    {
        wxFileName FN(FileName);
        if ( FN.MakeRelativeTo(GetProject()->GetProjectPath()) )
        {
            if ( FN.GetFullPath(wxPATH_UNIX) == XrcFile ) return true;
        }
    }

    wxFileName FN(FileName);
    if ( FN.MakeRelativeTo(GetProject()->GetInternalPath()) )
    {
        if ( FN.GetFullPath(wxPATH_UNIX) == WxsFile ) return true;
    }

    return false;
}

void wxsWindowRes::SelectionChanged(wxsItem* ChangedItem)
{
    RootSelection = ChangedItem;

    if ( !RootSelection || !RootSelection->GetIsSelected() )
    {
        RootSelection = NULL;

        // Need to find other selection
        FindFirstSelection(GetRootItem());

        if ( !RootSelection )
        {
            // There's no selection at all, we just select root item
            RootSelection = GetRootItem();
            GetRootItem()->SetIsSelected(true);
        }
    }

    // Notifying editor if there's one
    if ( GetEditor() )
    {
        ((wxsWindowEditor*)GetEditor())->SelectionChanged();
    }

    // Notifying resource browser
    wxsTREE()->SelectionChanged(RootSelection);

    // Showing new selection in property browser
    RootSelection->ShowInPropertyGrid();

}

void wxsWindowRes::FindFirstSelection(wxsItem* Item)
{
    if ( Item->GetIsSelected() )
    {
        RootSelection = Item;
        return;
    }

    wxsParent* Parent = Item->ToParent();
    if ( Parent )
    {
        for( int i=0; i<Parent->GetChildCount(); i++ )
        {
            FindFirstSelection(Parent->GetChild(i));
            if ( RootSelection!=NULL ) return;
        }
    }
}

inline wxString wxsWindowRes::FixFileName(wxString FileName)
{
    if ( FileName.empty() ) return FileName;
    wxFileName FN(FileName);
    if ( FN.IsAbsolute() ) return FileName;
    return FN.GetFullPath(wxPATH_UNIX);
}
