#include "wxspropertycontainer.h"
#include "wxspropertygridmanager.h"

#include "wxsemptyproperty.h"
#include "wxslongproperty.h"
#include "wxsboolproperty.h"

#include <wx/msgdlg.h>
#include <globals.h>

long wxsPropertyContainer::Flags = 0;
bool wxsPropertyContainer::IsRead = false;
TiXmlElement* wxsPropertyContainer::CurrentElement = NULL;
wxsPropertyStream* wxsPropertyContainer::CurrentStream = NULL;
wxMutex wxsPropertyContainer::Mutex;

wxsPropertyContainer::wxsPropertyContainer():
    CurrentQP(NULL),
    BlockChangeCallback(false)
{
}

wxsPropertyContainer::~wxsPropertyContainer()
{
    // Unbinding from grid if there's one associated
    if ( wxsPGRID() )
    {
        wxsPGRID()->UnbindPropertyContainer(this);
    }

    // Unbinding prom quick properties if there's one
    if ( CurrentQP )
    {
        CurrentQP->Container = NULL;
        CurrentQP = NULL;
    }
}

void wxsPropertyContainer::ShowInPropertyGrid()
{
    wxMutexLocker Lock(Mutex);
    if ( !wxsPGRID() ) return;      // We're not sure that PropertyGridManager has been created
    Flags = (GetPropertiesFlags() & ~(flXml|flPropStream)) | flPropGrid;
    wxsPGRID()->SetTargetPage(0);
    wxsPGRID()->Freeze();
    wxsPGRID()->UnbindAll();
    EnumProperties(Flags);
    wxsPGRID()->Thaw();
    Flags = 0;
    wxsPGRID()->SetNewMainContainer(this);
}

void wxsPropertyContainer::XmlRead(TiXmlElement* Element)
{
    wxMutexLocker Lock(Mutex);
    // Just to make sure we will read exactly what we've saved
    TiXmlNode::SetCondenseWhiteSpace(false);
    Flags = (GetPropertiesFlags() & ~(flPropGrid|flPropStream)) | flXml;
    IsRead = true;
    CurrentElement = Element;
    EnumProperties(Flags);
    NotifyPropertyChange(true);
    Flags = 0;
}

void wxsPropertyContainer::XmlWrite(TiXmlElement* Element)
{
    wxMutexLocker Lock(Mutex);
    // Just to make sure we will read exactly what we've saved
    TiXmlNode::SetCondenseWhiteSpace(false);
    Flags = (GetPropertiesFlags() & ~(flPropGrid|flPropStream)) | flXml;
    IsRead = false;
    CurrentElement = Element;
    EnumProperties(Flags);
    Flags = 0;
}

void wxsPropertyContainer::PropStreamRead(wxsPropertyStream* Stream)
{
    wxMutexLocker Lock(Mutex);
    Flags = (GetPropertiesFlags() & ~(flPropGrid|flXml)) | flPropStream;
    IsRead = true;
    CurrentStream = Stream;
    EnumProperties(Flags);
    NotifyPropertyChange(true);
    Flags = 0;
}

void wxsPropertyContainer::PropStreamWrite(wxsPropertyStream* Stream)
{
    wxMutexLocker Lock(Mutex);
    Flags = (GetPropertiesFlags() & ~(flPropGrid|flXml)) | flPropStream;
    IsRead = false;
    CurrentStream = Stream;
    EnumProperties(Flags);

    // Notifying about change since this method could correct some values
    NotifyPropertyChange(true);
    Flags = 0;
}

void wxsPropertyContainer::NotifyPropertyChangeFromPropertyGrid()
{
    if ( BlockChangeCallback ) return;
    BlockChangeCallback = true;

    if ( CurrentQP )
    {
        CurrentQP->Update();
    }

    PropertyChangedHandler();

    BlockChangeCallback = false;
}

void wxsPropertyContainer::NotifyPropertyChangeFromQuickProps()
{
    if ( BlockChangeCallback ) return;
    BlockChangeCallback = true;

    if ( wxsPGRID() )
    {
        wxsPGRID()->Update(this);
    }

    PropertyChangedHandler();

    BlockChangeCallback = false;
}

void wxsPropertyContainer::NotifyPropertyChange(bool CallPropertyChangeHandler)
{
    if ( BlockChangeCallback ) return;
    BlockChangeCallback = true;

    if ( CurrentQP )
    {
        CurrentQP->Update();
    }

    if ( wxsPGRID() )
    {
        wxsPGRID()->Update(this);
    }

    if ( CallPropertyChangeHandler )
    {
        PropertyChangedHandler();
    }
    BlockChangeCallback = false;
}

wxsQuickPropsPanel* wxsPropertyContainer::BuildQuickPropertiesPanel(wxWindow* Parent)
{
    if ( CurrentQP )
    {
        CurrentQP->Container = NULL;
    }

    CurrentQP = CreateQuickProperties(Parent);

    // CurrentQP MUST be currently associated to this container

    return CurrentQP;
}

void wxsPropertyContainer::Property(wxsProperty& Prop,long PropertyFlags)
{
    // Filtering flags
    if ( (Flags & PropertyFlags) != PropertyFlags ) return;

    // Property function works like kind of state machine, where current state
    // is determined by flPropGrid, flXml, flPropStream bits in Flags variable
    // and IsRead one. These are set up in one of public functions: XmlRead,
    // XmlWrite, PropStreamRead, PropStreamWrite, ShowInPropertyGrid
    switch ( Flags & (flPropGrid|flXml|flPropStream) )
    {
        case flPropGrid:
            // Called from ShowInPropertyGrid
            Prop.PGCreate(this,wxsPGRID(),wxsPGRID()->GetRoot());
            break;

        case flXml:
            if ( IsRead )
            {
                // Called from XmlRead
                Prop.XmlRead(this,CurrentElement->FirstChildElement(cbU2C(Prop.GetDataName())));
            }
            else
            {
                // Called from XmlWrite
                TiXmlElement* Element = CurrentElement->InsertEndChild(
                    TiXmlElement(cbU2C(Prop.GetDataName())))->ToElement();

                if ( !Prop.XmlWrite(this,Element) )
                {
                    // Removing useless node, TiXml automatically frees memory
                    CurrentElement->RemoveChild(Element);
                }
            }
            break;

        case flPropStream:
            if ( IsRead )
            {
                // Called from PropStreamRead
                Prop.PropStreamRead(this,CurrentStream);
            }
            else
            {
                // Called from PropStreamWrite
                Prop.PropStreamWrite(this,CurrentStream);
            }
            break;

        default:

            // This can not be done
            wxMessageBox(_T("wxsPropertyContainer::Property() function has been\n")
                         _T("called manually. If you are the Developer,\n")
                         _T("please remove this code."));

    }
}

void wxsPropertyContainer::SubContainer(wxsPropertyContainer* Container,long NewFlags)
{
    if ( !Container ) return;
    long FlagsStore = Flags;
    // Flags will be replaced using NewFlags but bits used internally by wxsPropertyContainer will be left untouched
    Flags = ( Flags    &  (flPropGrid|flXml|flPropStream) ) |   // Leaving old part of data processing type
            ( NewFlags & ~(flPropGrid|flXml|flPropStream) );    // Rest taken from new properties
    Container->EnumProperties(NewFlags);
    Flags = FlagsStore;
}
