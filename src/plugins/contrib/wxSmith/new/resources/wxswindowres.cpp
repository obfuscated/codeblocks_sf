#include "wxswindowres.h"

#include "../wxsglobals.h"
#include "../wxsitem.h"
#include "../wxsparent.h"
#include "../wxsresourcetree.h"
#include "../wxsproject.h"
#include "../wxsitemfactory.h"
#include "../wxsextresmanager.h"
#include "../wxseditor.h"
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

    ClassName = cbC2U(Element->Attribute("class"));
    WxsFile   = cbC2U(Element->Attribute("wxs_file"));
    SrcFile   = cbC2U(Element->Attribute("src_file"));
    HFile     = cbC2U(Element->Attribute("header_file"));
    XrcFile   = cbC2U(Element->Attribute("xrc_file"));

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

    // Clearing undo buffer and adding new position from current entry

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
    wxString Include = IncludeFN.GetFullPath();

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

//void wxsWindowRes::NotifyChange()
//{
//    // Regenerating source code
//	UpdateWidgetsVarNameId();
//	RebuildCode();
//
//    // Applying modified state
//    if ( GetEditor() )
//    {
//    	// Must process inside editor (updating titile)
//    	GetEditor()->SetModified();
//    }
//    else
//    {
//        SetModified();
//    }
//
//    // Storing change inside undo buffer
//
//    if ( GetEditor() )
//    {
//    	((wxsWindowEditor*)GetEditor())->GetUndoBuff()->StoreChange();
//    }
//}
//
//void wxsWindowRes::RebuildCode()
//{
//    if ( !GetProject() ) return;
//
////------------------------------
//// Generating initializing code
////------------------------------
//
//	wxString CodeHeader = wxString::Format(wxsBHeaderF("Initialize"),GetClassName().c_str());
//	wxString Code = CodeHeader + _T("\n");
//
//	// Creating local and global declarations
//	wxString GlobalCode;
//	bool WasDeclaration = false;
//	AddDeclarationsReq(RootWidget,Code,GlobalCode,WasDeclaration);
//	if ( WasDeclaration )
//	{
//		Code.Append(_T('\n'));
//	}
//
//	// Creating window-generating code
//
//    if ( GetEditMode() == wxsREMSource )
//    {
//        // Generating producing code
//        wxsCodeGen Gen(RootWidget,false);
//        Code.Append(Gen.GetCode());
//    }
//    else if ( GetEditMode() == wxsREMMixed )
//    {
//    	// Writing new Xrc file
//        TiXmlDocument* Doc = GenerateXrc();
//        if ( Doc )
//        {
//            Doc->SaveFile(_C(GetProject()->GetProjectFileName(XrcFile)));
//            delete Doc;
//        }
//
//        // No local variables - clearing the code
//        Code = CodeHeader;
//        Code.Append(_T('\n'));
//        Code.Append(GetXrcLoadingCode());
//        Code.Append(_T('\n'));
//
//    	// Loading all controls
//    	GenXrcFetchingCode(Code,RootWidget);
//    }
//
//	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(SrcFile),CodeHeader,Code);
//
////---------------------------------
//// Generating variable declarations
////---------------------------------
//
//	CodeHeader.Printf(wxsBHeaderF("Declarations"),GetClassName().c_str());
//	Code = CodeHeader + _T("\n") + GlobalCode;
//	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(HFile),CodeHeader,Code);
//
////---------------------------------
//// Generating Identifiers
////---------------------------------
//
//    CodeHeader.Printf(wxsBHeaderF("Identifiers"),GetClassName().c_str());
//    Code = CodeHeader;
//    Code.Append(_T('\n'));
//    if ( GetEditMode() == wxsREMSource )
//    {
//        wxArrayString IdsArray;
//        BuildIdsArray(RootWidget,IdsArray);
//        if ( IdsArray.Count() )
//        {
//            Code.Append(_T("enum Identifiers\n{"));
//            IdsArray.Sort();
//            wxString Previous = _T("");
//            bool First = true;
//            for ( size_t i = 0; i<IdsArray.Count(); ++i )
//            {
//                if ( IdsArray[i] != Previous )
//                {
//                    Previous = IdsArray[i];
//                    Code.Append( _T("\n\t") );
//                    Code.Append( Previous );
//                    if ( First )
//                    {
//                        Code.Append( _T(" = 0x1000") );
//                        First = false;
//                    }
//                    if ( i < IdsArray.Count() - 1 )
//                    {
//                        Code.Append( _T(',') );
//                    }
//                }
//            }
//            Code.Append( _T("\n};\n") );
//        }
//    }
//    wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(HFile),CodeHeader,Code);
//
////---------------------------------
//// Generating Includes
////---------------------------------
//
//	wxArrayString HeadersArray;
//	BuildHeadersArray(RootWidget,HeadersArray);
//	HeadersArray.Add(_T("<wx/intl.h>"));
//	if ( GetEditMode() == wxsREMMixed )
//	{
//		HeadersArray.Add(_T("<wx/xrc/xmlres.h>"));
//	}
//	HeadersArray.Sort();
//	CodeHeader.Printf(wxsBHeaderF("Headers"),GetClassName().c_str());
//	Code = CodeHeader;
//	wxString Previous = _T("");
//	for ( size_t i = 0; i<HeadersArray.Count(); i++ )
//	{
//		if ( HeadersArray[i] != Previous )
//        {
//        	Previous = HeadersArray[i];
//            Code.Append(_T("\n#include "));
//            Code.Append(Previous);
//        }
//	}
//	Code.Append(_T('\n'));
//	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(HFile),CodeHeader,Code);
//	UpdateEventTable();
//}
//
//void wxsWindowRes::AddDeclarationsReq(wxsWidget* Widget,wxString& LocalCode,wxString& GlobalCode,bool& WasLocal)
//{
//	if ( !Widget ) return;
//
//	static wxsCodeParams Params;
//
//	int Count = Widget->GetChildCount();
//	for ( int i=0; i<Count; i++ )
//	{
//		wxsWidget* Child = Widget->GetChild(i);
//		if ( Child->GetBPType() & bptVariable )
//        {
//		    // Only VarName should be used when fetching declaration code
//            Params.VarName = Child->BaseProperties.VarName;
//            wxString Decl = Child->GetDeclarationCode(Params);
//            if ( Decl.Length() )
//            {
//                bool Member = Child->BaseProperties.IsMember;
//                wxString& Code = Member ? GlobalCode : LocalCode;
//                Code.Append(Decl);
//                Code.Append('\n');
//                WasLocal |= !Member;
//            }
//		}
//		AddDeclarationsReq(Child,LocalCode,GlobalCode,WasLocal);
//	}
//}
//
//void wxsWindowRes::UpdateWidgetsVarNameId()
//{
//    StrMap NamesMap;
//    StrMap IdsMap;
//
//    CreateSetsReq(NamesMap,IdsMap,RootWidget);
//   	UpdateWidgetsVarNameIdReq(NamesMap,IdsMap,RootWidget);
//}
//
//void wxsWindowRes::UpdateWidgetsVarNameIdReq(StrMap& NamesMap, StrMap& IdsMap, wxsWidget* Widget)
//{
//	int Cnt = Widget->GetChildCount();
//	for ( int i=0; i<Cnt; i++ )
//	{
//		wxsWidget* Child = Widget->GetChild(i);
//
//        wxsBaseProperties& Params = Child->BaseProperties;
//
//        bool UpdateVar = ( Child->GetBPType() & bptVariable ) &&
//                         ( Params.VarName.Length() == 0 );
//        bool UpdateId  = ( Child->GetBPType() & bptId ) &&
//                         ( Params.IdName.Length() == 0 );
//        if ( UpdateVar || UpdateId )
//        {
//            wxString NameBase = Child->GetInfo().DefaultVarName;
//            wxString Name;
//            wxString IdBase = Child->GetInfo().DefaultVarName;
//            IdBase.MakeUpper();
//            wxString Id;
//            int Index = 1;
//            do
//            {
//                Name.Printf(_T("%s%d"),NameBase.c_str(),Index);
//                Id.Printf(_T("ID_%s%d"),IdBase.c_str(),Index++);
//            }
//            while ( ( UpdateVar && NamesMap.find(Name) != NamesMap.end() ) ||
//                    ( UpdateId  && IdsMap.find(Id)     != IdsMap.end() ) );
//
//            if ( UpdateVar )
//            {
//                Params.VarName = Name;
//                NamesMap[Name] = Child;
//            }
//            if ( UpdateId )
//            {
//                Params.IdName = Id;
//                IdsMap[Id] = Child;
//            }
//            if ( UpdateVar || UpdateId )
//            {
//            	SetModified();
//            }
//        }
//
//		UpdateWidgetsVarNameIdReq(NamesMap,IdsMap,Child);
//	}
//}
//
//void wxsWindowRes::CreateSetsReq(StrMap& NamesMap, StrMap& IdsMap, wxsWidget* Widget, wxsWidget* Without)
//{
//	int Cnt = Widget->GetChildCount();
//	for ( int i=0; i<Cnt; i++ )
//	{
//		wxsWidget* Child = Widget->GetChild(i);
//
//		if ( Child != Without )
//		{
//            if ( Child->BaseProperties.VarName.Length() )
//            {
//                NamesMap[Child->BaseProperties.VarName.c_str()] = Child;
//            }
//
//            if ( Child->BaseProperties.IdName.Length() )
//            {
//                IdsMap[Child->BaseProperties.IdName.c_str()] = Child;
//            }
//		}
//
//		CreateSetsReq(NamesMap,IdsMap,Child,Without);
//	}
//}
//
//bool wxsWindowRes::CheckBaseProperties(bool Correct,wxsWidget* Changed)
//{
//    StrMap NamesMap;
//    StrMap IdsMap;
//
//    if ( Changed == NULL )
//    {
//    	// Will check all widgets
//    	return CheckBasePropertiesReq(RootWidget,Correct,NamesMap,IdsMap);
//    }
//
//    // Creating sets of names and ids
//   	CreateSetsReq(NamesMap,IdsMap,RootWidget,Changed);
//
//   	// Checkign and correcting changed widget
//   	return CorrectOneWidget(NamesMap,IdsMap,Changed,Correct);
//}
//
//bool wxsWindowRes::CheckBasePropertiesReq(wxsWidget* Widget,bool Correct,StrMap& NamesMap,StrMap& IdsMap)
//{
//	bool Result = true;
//	int Cnt = Widget->GetChildCount();
//	for ( int i=0; i<Cnt; ++i )
//	{
//		wxsWidget* Child = Widget->GetChild(i);
//
//		if ( !CorrectOneWidget(NamesMap,IdsMap,Child,Correct) )
//		{
//			if ( !Correct ) return false;
//			Result = false;
//		}
//
//		NamesMap[Child->BaseProperties.VarName] = Child;
//		IdsMap[Child->BaseProperties.IdName] = Child;
//
//		if ( ! CheckBasePropertiesReq(Child,Correct,NamesMap,IdsMap) )
//		{
//			if ( !Correct ) return false;
//			Result = false;
//		}
//	}
//
//	return Result;
//}
//
//bool wxsWindowRes::CorrectOneWidget(StrMap& NamesMap,StrMap& IdsMap,wxsWidget* Changed,bool Correct)
//{
//	bool Valid = true;
//
//    // Validating variable name
//
//    if ( Changed->GetBPType() & bptVariable )
//    {
//    	wxString& VarName = Changed->BaseProperties.VarName;
//    	wxString Corrected;
//    	VarName.Trim(true);
//    	VarName.Trim(false);
//
//    	// first validating produced name
//
//    	if ( VarName.Length() == 0 )
//    	{
//    		if ( !Correct )
//    		{
//    			wxMessageBox(_("Item must have variable name"));
//    			return false;
//    		}
//    		else
//    		{
//    		    DBGLOG(_T("wxSmith: Widget has empty variable name"));
//    		}
//
//   			// Creating new unique name
//
//   			const wxString& Prefix = Changed->GetInfo().DefaultVarName;
//   			for ( int i=1;; ++i )
//   			{
//   				Corrected.Printf(_T("%s%d"),Prefix.c_str(),i);
//   				if ( NamesMap.find(Corrected) == NamesMap.end() ) break;
//   			}
//
//    		Valid = false;
//    	}
//    	else
//    	{
//    		// Validating name as C++ ideentifier
//            if ( wxString(_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
//                          _T("abcdefghijklmnopqrstuvwxyz")
//                          _T("_") ).Find(VarName.GetChar(0)) == -1 )
//            {
//            	if ( !Correct )
//            	{
//            		wxMessageBox(wxString::Format(_("Invalid character: '%c' in variable name"),VarName.GetChar(0)));
//            		return false;
//            	}
//            	else
//            	{
//            	    DBGLOG(_T("wxSmith: Variable name : \"%s\" is not a valid c++ identifier (invalid character \"%c\" at position %d)"),VarName.c_str(),VarName.GetChar(0),0);
//            	}
//                Valid = false;
//            }
//            else
//            {
//            	Corrected.Append(VarName.GetChar(0));
//            }
//
//            for ( size_t i=1; i<VarName.Length(); ++i )
//            {
//                if ( wxString(_T("0123456789")
//                              _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
//                              _T("abcdefghijklmnopqrstuvwxyz")
//                              _T("_") ).Find(VarName.GetChar(i)) == -1 )
//                {
//                    if ( !Correct )
//                    {
//                        wxMessageBox(wxString::Format(_("Invalid character: '%c' in variable name"),VarName.GetChar(i)));
//                        return false;
//                    }
//                    else
//                    {
//                        DBGLOG(_T("wxSmith: Variable name : \"%s\" is not a valid c++ identifier (invalid character \"%c\" at position %d)"),VarName.c_str(),VarName.GetChar(i),i);
//                    }
//                    Valid = false;
//                }
//                else
//                {
//                    Corrected.Append(VarName.GetChar(i));
//                }
//            }
//
//            // Searching for another widget with same name
//            if ( NamesMap.find(Corrected) != NamesMap.end() )
//            {
//            	if ( !Correct )
//            	{
//            		wxMessageBox(wxString::Format(_("Item with variable name '%s' already exists"),Corrected.c_str()));
//            		return false;
//            	}
//            	else
//            	{
//            	    DBGLOG(_T("wxSmith: Duplicated variable name: \"%s\""),VarName.c_str());
//            	}
//
//            	// Generating new unique name
//
//                const wxString& Prefix = Changed->GetInfo().DefaultVarName;
//                for ( int i=1;; ++i )
//                {
//                    Corrected.Printf(_T("%s%d"),Prefix.c_str(),i);
//                    if ( NamesMap.find(Corrected) == NamesMap.end() ) break;
//                }
//
//            	Valid = false;
//            }
//    	}
//
//        if ( Correct )
//        {
//        	VarName = Corrected;
//        }
//    }
//
//    if ( Changed->GetBPType() & bptId )
//    {
//    	wxString& IdName = Changed->BaseProperties.IdName;
//    	wxString Corrected;
//    	IdName.Trim(true);
//    	IdName.Trim(false);
//
//    	// first validating produced name
//
//    	if ( IdName.Length() == 0 )
//    	{
//    		if ( !Correct )
//    		{
//    			wxMessageBox(_("Item must have identifier"));
//    			return false;
//    		}
//    		else
//    		{
//    		    DBGLOG(_T("wxSmith: Empty identifier"));
//    		}
//
//   			// Creating new unique name
//
//   			wxString Prefix = Changed->GetInfo().DefaultVarName;
//   			Prefix.UpperCase();
//   			for ( int i=1;; ++i )
//   			{
//   				Corrected.Printf(_T("ID_%s%d"),Prefix.c_str(),i);
//   				if ( IdsMap.find(Corrected) == IdsMap.end() ) break;
//   			}
//
//    		Valid = false;
//    	}
//    	else
//    	{
//    	    long IdValue;
//    	    if ( !IdName.ToLong(&IdValue,0) )
//    	    {
//                // Validating id as C++ identifier
//                if ( wxString(_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
//                              _T("abcdefghijklmnopqrstuvwxyz")
//                              _T("_") ).Find(IdName.GetChar(0)) == -1 )
//                {
//                    if ( !Correct )
//                    {
//                        wxMessageBox(wxString::Format(_("Invalid character: '%c' in id name"),IdName.GetChar(0)));
//                        return false;
//                    }
//                    else
//                    {
//                        DBGLOG(_T("wxSmith: Identifier name : \"%s\" is not a valid c++ name (invalid character \"%c\" at position %d)"),IdName.c_str(),IdName.GetChar(0),0);
//                    }
//                    Valid = false;
//                }
//                else
//                {
//                    Corrected.Append(IdName.GetChar(0));
//                }
//
//                for ( size_t i=1; i<IdName.Length(); ++i )
//                {
//                    if ( wxString(_T("0123456789")
//                                  _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
//                                  _T("abcdefghijklmnopqrstuvwxyz")
//                                  _T("_") ).Find(IdName.GetChar(i)) == -1 )
//                    {
//                        if ( !Correct )
//                        {
//                            wxMessageBox(wxString::Format(_("Invalid character: '%c' in id name"),IdName.GetChar(i)));
//                            return false;
//                        }
//                        else
//                        {
//                            DBGLOG(_T("wxSmith: Identifier name : \"%s\" is not a valid c++ name (invalid character \"%c\" at position %d)"),IdName.c_str(),IdName.GetChar(i),i);
//                        }
//                        Valid = false;
//                    }
//                    else
//                    {
//                        Corrected.Append(IdName.GetChar(i));
//                    }
//                }
//
//                // Searching for another widget with same name
//
//                bool Predefined = false;
//                for ( int i=0; i<wxsPredefinedIdsCount; i++ )
//                {
//                    if ( wxsPredefinedIds[i] == Corrected )
//                    {
//                        Predefined = true;
//                        break;
//                    }
//                }
//
//                if ( (!Predefined) &&
//                     ( IdsMap.find(Corrected) != IdsMap.end() ) )
//                {
//                    if ( !Correct )
//                    {
//                        wxMessageBox(wxString::Format(_("Item with identifier '%s' already exists"),Corrected.c_str()));
//                        return false;
//                    }
//                    else
//                    {
//                        DBGLOG(_T("wxSmith: Duplicated identifier name: \"%s\""),IdName.c_str());
//                    }
//
//                    // Generating new unique name
//
//                    wxString Prefix = Changed->GetInfo().DefaultVarName;
//                    Prefix.UpperCase();
//                    for ( int i=1;; ++i )
//                    {
//                        Corrected.Printf(_T("ID_%s%d"),Prefix.c_str(),i);
//                        if ( IdsMap.find(Corrected) == IdsMap.end() ) break;
//                    }
//
//                    Valid = false;
//                }
//    	    }
//    	    else
//    	    {
//    	        if ( GetEditMode() != wxsREMSource )
//    	        {
//    	            if ( IdName != _T("-1") )
//    	            {
//                        if ( !Correct )
//                        {
//                            wxMessageBox(wxString::Format(_("XRC allow only -1 value instead of identifier name."),Corrected.c_str()));
//                            return false;
//                        }
//                        else
//                        {
//                            DBGLOG(_T("wxSmith: Invalid numeric id value: \"%s\""),IdName.c_str());
//                        }
//
//                        IdName = _T("-1");
//                        Valid = false;
//    	            }
//    	        }
//    	    }
//
//    	}
//
//    	if ( Correct )
//    	{
//    		IdName = Corrected;
//    	}
//    }
//
//    if ( !Valid && Correct ) SetModified();
//
//	return Valid;
//}
//
//void wxsWindowRes::BuildIdsArray(wxsWidget* Widget,wxArrayString& Array)
//{
//	int Cnt = Widget->GetChildCount();
//	for ( int i=0; i<Cnt; i++ )
//	{
//		wxsWidget* Child = Widget->GetChild(i);
//		if ( Child->GetBPType() & bptId )
//		{
//		    const wxString& Name = Child->BaseProperties.IdName;
//		    long Value;
//		    bool Predefined = Name.ToLong(&Value,0);
//		    if ( !Predefined )
//		    {
//		        for ( int i=0; i<wxsPredefinedIdsCount; i++ )
//                {
//                    if ( Name == wxsPredefinedIds[i] )
//                    {
//                        Predefined = true;
//                        break;
//                    }
//                }
//		    }
//		    if ( !Predefined )
//		    {
//                Array.Add(Child->BaseProperties.IdName);
//		    }
//		}
//		BuildIdsArray(Child,Array);
//	}
//}
//
//void wxsWindowRes::BuildHeadersArray(wxsWidget* Widget,wxArrayString& Array)
//{
//	Array.Add(Widget->GetInfo().HeaderFile);
//	if ( Widget->GetInfo().ExtHeaderFile.Length() )
//	{
//        Array.Add(Widget->GetInfo().ExtHeaderFile);
//	}
//	int Cnt = Widget->GetChildCount();
//	for ( int i=0; i<Cnt; i++ )
//	{
//		wxsWidget* Child = Widget->GetChild(i);
//		BuildHeadersArray(Child,Array);
//	}
//}
//
//void wxsWindowRes::UpdateEventTable()
//{
//    if ( !GetProject() ) return;
//
//	wxString CodeHeader;
//	CodeHeader.Printf(wxsBHeaderF("EventTable"),ClassName.c_str());
//	wxString Code = CodeHeader;
//	Code.Append(_T('\n'));
//	CollectEventTableEnteries(Code,RootWidget);
//	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(GetSourceFile()),CodeHeader,Code);
//    // Applying modified state
//    if ( GetEditor() )
//    {
//    	// Must process inside editor (updating titile)
//    	GetEditor()->SetModified();
//    }
//    else
//    {
//        SetModified();
//    }
//
//    // Storing change inside undo buffer
//    if ( GetEditor() )
//    {
//    	((wxsWindowEditor*)GetEditor())->GetUndoBuff()->StoreChange();
//    }
//}
//
//void wxsWindowRes::CollectEventTableEnteries(wxString& Code,wxsWidget* Widget)
//{
//	int Cnt = Widget->GetChildCount();
//    Code += Widget->GetEvents()->GetArrayEnteries();
//	for ( int i=0; i<Cnt; i++ )
//	{
//		wxsWidget* Child = Widget->GetChild(i);
//		CollectEventTableEnteries(Code,Child);
//	}
//}
//
//void wxsWindowRes::GenXrcFetchingCode(wxString& Code,wxsWidget* Widget)
//{
//	int Cnt = Widget->GetChildCount();
//	for ( int i=0; i<Cnt; i++ )
//	{
//		wxsWidget* Child = Widget->GetChild(i);
//		if ( ( Child->GetBPType() & bptVariable ) &&
//		     ( Child->BaseProperties.IsMember ) )
//		{
//			Code.Append(Child->BaseProperties.VarName);
//			Code.Append(_T(" = XRCCTRL(*this,\""));
//			Code.Append(Child->BaseProperties.IdName);
//			Code.Append(_T("\","));
//			Code.Append(Child->GetInfo().Name);
//			Code.Append(_T(");\n"));
//		}
//		GenXrcFetchingCode(Code,Child);
//	}
//}
//
//TiXmlDocument* wxsWindowRes::GenerateXrc()
//{
//	int EMStore = GetEditMode();
//	SetEditMode(wxsREMFile);
//	TiXmlDocument* Generated = GenerateXml();
//	SetEditMode(EMStore);
//	return Generated;
//}
//

void wxsWindowRes::SetModified(bool modified)
{
    Modified = modified;
}

//bool wxsWindowRes::ChangeRootWidget(wxsWidget* NewRoot,bool DeletePrevious)
//{
//	if ( !NewRoot ) return false;
//	// New root must be of the same type as current
//	if ( RootWidget->GetInfo().Name != NewRoot->GetInfo().Name ) return false;
//    wxsBlockSelectEvents();
//	RootWidget->KillTree(wxsTREE());
//	if ( GetEditor() )
//	{
//		((wxsWindowEditor*)GetEditor())->KillPreview();
//	}
//	if ( DeletePrevious ) wxsFACTORY()->Kill(RootWidget);
//	RootWidget = NewRoot;
//    wxTreeCtrl* Tree = wxsTREE();
//    Tree->SelectItem(GetTreeItemId());
//    if ( GetEditor() )
//    {
//        GetRootWidget()->BuildTree(Tree,GetTreeItemId());
//        ((wxsWindowEditor*)GetEditor())->BuildPreview();
//    }
//    RebuildCode();
//    wxsBlockSelectEvents(false);
//	return true;
//}
//
//void wxsWindowRes::OnSelect()
//{
//    wxsSelectWidget(GetRootWidget());
//}

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
            if ( FN.GetFullPath() == XrcFile ) return true;
        }
    }

    wxFileName FN(FileName);
    if ( FN.MakeRelativeTo(GetProject()->GetInternalPath()) )
    {
        if ( FN.GetFullPath() == WxsFile ) return true;
    }

    return false;
}
