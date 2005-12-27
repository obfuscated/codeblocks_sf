/////////////////////////////////////////////////////////////////////////////
// Name:        pg_dox_mainpage.h
// Purpose:     wxPropertyGrid Doxygen Documentation
// Author:      Jaakko Salli
// Modified by:
// Created:     Oct-08-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_PG_DOX_MAINPAGE_H__
#define __WX_PG_DOX_MAINPAGE_H__

/**
    \mainpage wxPropertyGrid Overview

      wxPropertyGrid is a specialized two-column grid for editing properties
    such as strings, numbers, flagsets, fonts, and colours. It allows hierarchial,
    collapsible properties ( via so-called categories that can hold child
    properties), sub-properties, and has strong wxVariant support (for example,
    allows populating from near-arbitrary list of wxVariants).

    Classes:\n
      wxPropertyGrid\n
      wxPropertyGridManager\n
      wxPropertyGridEvent\n

    Header files:\n
      <b>wx/propgrid/propgrid.h:</b> Mandatory when using wxPropertyGrid.\n
      <b>wx/propgrid/advprops.h:</b> For less often used property classes.\n
      <b>wx/propgrid/manager.h:</b> Mandatory when using wxPropertyGridManager.\n
      <b>wx/propgrid/propdev.h:</b> Mandatory when implementing custom property classes.\n

    \ref basics\n
    \ref categories\n
    \ref parentprops\n
    \ref enumandflags\n
    \ref advprops\n
    \ref operations\n
    \ref events\n
    \ref populating\n
    \ref custprop\n
    \ref usage2\n
    \ref compiling\n
    \ref misc\n
    \ref proplist\n
    \ref userhelp\n
    \ref bugspostponed\n
    \ref bugs\n
    \ref issues\n
    \ref todo\n
    \ref notes\n
    \ref newprops\n

    \section basics Creating and Populating wxPropertyGrid

    As seen here, wxPropertyGrid is constructed in the same way as
    other wxWidgets controls:

    \code

    // Necessary header file
    #include <wx/propgrid/propgrid.h>

    ...

        // Assumes code is in frame/dialog constructor

        // Construct wxPropertyGrid control
        wxPropertyGrid* pg = new wxPropertyGrid(
            this, // parent
            PGID, // id
            wxDefaultPosition, // position 
            wxDefaultSize, // size
            // Some specific window styles - for all additional styles, see the documentation
            wxPG_AUTO_SORT | // Automatic sorting after items added
            wxPG_BOLD_MODIFIED | // Modified values are drawn in bold font
            wxPG_SPLITTER_AUTO_CENTER | // Automatically center splitter until user manually adjusts it
            // Default style
            wxPG_DEFAULT_STYLE
            );

    \endcode

      (for complete list of new window styles: @link wndflags Additional Window Styles@endlink)

      wxPropertyGrid is usually populated with lines like this:

    \code
        pg->Append( wxStringProperty(wxT("Label"),wxT("Name"),wxT("Initial Value")) );
    \endcode

    wxStringProperty is a constructor function that creates a property instance of
    a property class "wxStringProperty". Only the first function argument (label)
    is mandatory. When necessary, name defaults to label and initial value to
    default value. If wxPG_LABEL is used as the name argument, then the label is
    automatically used as a name as well (this is more efficient than manually
    defining both as the same). Empty name is also allowed, but in this case the
    property cannot be accessed by its name.

    To demonstrate other common property classes, here's another code snippet:

    \code

        // Add int property 
        pg->Append ( wxIntProperty ( wxT("IntProperty"), wxPG_LABEL, 12345678 ) );

        // Add float property (value type is actually double)
        pg->Append ( wxFloatProperty ( wxT("FloatProperty"), wxPG_LABEL, 12345.678 ) );

        // Add a bool property
        pg->Append ( wxBoolProperty ( wxT("BoolProperty"), wxPG_LABEL, false ) );

        // A string property that can be edited in a separate editor dialog.
        pg->Append ( wxLongStringProperty (wxT("LongStringProperty"),
                                           wxPG_LABEL,
                                           wxT("This is much longer string than the ")
                                           wxT("first one. Edit it by clicking the button.")));

        // String editor with dir selector button.
        pg->Append ( wxDirProperty( wxT("DirProperty"), wxPG_LABEL, ::wxGetUserHome()) );

        // A file selector property.
        pg->Append ( wxFileProperty( wxT("FileProperty"), wxPG_LABEL, wxEmptyString ) );

        // Extra: set wildcard for file property (format same as in wxFileDialog).
        pg->SetPropertyAttribute(wxT("TextFile"),
                                 wxPG_FILE_WILDCARD,
                                 wxT("All files (*.*)|*.*"));

    \endcode

    \section categories Categories

      wxPropertyGrid has a hierarchial property storage and display model, which
    allows property categories to hold child properties and even other
    categories. Other than that, from the programmer's point of view, categories
    can be treated exactly the same as "other" properties. For example, despite
    its name, GetPropertyByName also returns a category by name, and SetPropertyLabel
    also sets label of a category. Note however that sometimes the label of a
    property category may be referred as caption (for example, there is
    SetCaptionForegroundColour method that sets text colour of a property category's label).

      When category is added at the top (i.e. default) level of the hierarchy,
    it becomes a *current category*. This means that all other (non-category)
    properties after it are automatically added to it. You may add
    properties to specific categories by using wxPropertyGrid::Insert or wxPropertyGrid::AppendIn.

      Category code sample:

    \code

        // One way to add category (similar to how other properties are added)
        pg->Append( wxPropertyCategory(wxT("Main")) );

        // All these are added to "Main" category
        pg->Append( wxStringProperty(wxT("Name")) );
        pg->Append( wxIntProperty(wxT("Age"),wxPG_LABEL,25) );
        pg->Append( wxIntProperty(wxT("Height"),wxPG_LABEL,180) );
        pg->Append( wxIntProperty(wxT("Weight")) );

        // Another way
        pg->AppendCategory( wxT("Attributes") );

        // All these are added to "Attributes" category
        pg->Append( wxIntProperty(wxT("Intelligence")) );
        pg->Append( wxIntProperty(wxT("Sensibility")) );
        pg->Append( wxIntProperty(wxT("Agility")) );
        pg->Append( wxIntProperty(wxT("Coordination")) );
        pg->Append( wxIntProperty(wxT("Constitution")) );
        pg->Append( wxIntProperty(wxT("Strength")) );
        pg->Append( wxIntProperty(wxT("Personality")) );
        pg->Append( wxIntProperty(wxT("Appearance")) );
        pg->Append( wxIntProperty(wxT("Bravado")) );
        pg->Append( wxIntProperty(wxT("Willpower")) );

    \endcode

    \section parentprops Parent Properties

      If you want to combine number of properties under single parent (just
    like wxFontProperty combines font attributes), then the easiest way to
    proceed is to use wxParentProperty.

    \remarks
    - wxParentProperty's value type is string.
    - Child property that has children of its own will be embedded in
      braces ([]).

    Sample:

    \code
        wxPGId pid = pg->Append( wxParentProperty(wxT("Car"),wxPG_LABEL) );

        pg->AppendIn( pid, wxStringProperty(wxT("Model")), wxPG_LABEL,
                                            wxT("Lamborghini Diablo SV")) );

        pg->AppendIn( pid, wxIntProperty(wxT("Engine Size (cc)"),
                                         wxPG_LABEL,
                                         5707) );

        wxPGId speedId = pg->AppendIn( pid, wxParentProperty(wxT("Speeds"),wxPG_LABEL) );
        pg->AppendIn( speedId, wxIntProperty(wxT("Max. Speed (mph)"),wxPG_LABEL,300) );
        pg->AppendIn( speedId, wxFloatProperty(wxT("0-100 mph (sec)"),wxPG_LABEL,3.9) );
        pg->AppendIn( speedId, wxFloatProperty(wxT("1/4 mile (sec)"),wxPG_LABEL,8.6) );

        pg->AppendIn( pid, wxIntProperty(wxT("Price ($)"),
                                         wxPG_LABEL,
                                         300000) );

        // Displayed value of "Car" property is now:
        // "Lamborghini Diablo SV; [300; 3.9; 8.6]; 300000"

    \endcode


    \section enumandflags wxEnumProperty and wxFlagsProperty

      wxEnumProperty is used when you want property's (integer) value
    to be selected from a popup list of choices.

      Creating wxEnumProperty is more complex than those described earlier.
    You have to provide list of constant labels, and optionally relevant values
    (if given indexes are not used).

      As of 1.0.0rc1, wxEnumProperty actually stores string and value
    arrays given to it, but in such manner that the same storage can be reused
    for properties that use the exact same array pointers.

    \remarks

    - Value wxPG_INVALID_VALUE (equals 2147483647 equals 2^31-1) is not
      allowed as value.

    - Altough you can use label and value arrays that are temporary
      variables, it is recommended that you make them persistent (i.e. static or
      global) in practical code. <b>Otherwise there is a slight probability that
      property will use wrong set of choices or that the allocation
      of choices becomes very inefficient.</b> Only exception is
      wxPGConstants, which can be used as a temporary variable (see below).

    A very simple example:

    \code

    static const wxChar* gs_array_diet[] = 
        { wxT("Herbivore"), wxT("Carnivore"), wxT("Omnivore"), NULL };

        ...

        pg->Append( wxEnumProperty(wxT("Diet"),
                                   wxPG_LABEL,
                                   gs_array_diet) );

    \endcode

    Now with real values (and more comments):

    \code

        //
        // wxEnumProperty actually stores string.
        static const wxChar* enum_prop_labels[] = { wxT("One Item"),
            wxT("Another Item"), wxT("One More"), wxT("This Is Last"), NULL };

        // This value array would be optional if values matched string indexes
        static long enum_prop_values[] = { 40, 80, 120, 160 };

        // note that the initial value (the last argument) is the actual value,
        // not index or anything like that. Thus, our value selects "Another Item".
        //
        // 0 before value is number of items. If it is 0, like in our example,
        // number of items is calculated, and this requires that the string pointer
        // array is terminated with NULL.
        pg->Append ( wxEnumProperty(wxT("EnumProperty"),wxPG_LABEL,
            enum_prop_labels, enum_prop_values, 0, 80 ) );

    \endcode

    An alternative is to use wxPGConstants class which manages dynamic arrays of strings
    and integers. As of 1.0.0 rc1 wxEnumProperty stores manages arrays given to it,
    including wxPGConstants.

    \code

        // Use basic table from our previous example
        // Can also set/add wxArrayStrings and wxArrayInts directly
        wxPGConstants constants( enum_prop_labels, enum_prop_values );

        // Add extra items
        constants.Add ( wxT("Look, it continues"), 200 );
        constants.Add ( wxT("Even More"), 240 );
        constants.Add ( wxT("And More"), 280 );
        constants.Add ( wxT("True End of the List"), 320 );

        pg->Append ( wxEnumProperty(wxT("EnumProperty2"),
                                    wxPG_LABEL,
                                    constants,
                                    240) );

    \endcode

    You can also get reference to wxEnumProperty's list of choices using
    GetPropertyChoices. This can them be modified or given to further properties
    (in fast and effective manner). There is also AddPropertyChoice which is
    recommended way to append a choice for aproperty.

    \code

        // Continued from our previous example...
        wxPGConstants& choices = pg->GetPropertyChoices(wxT("EnumProperty2"));
        pg->Append( wxEnumProperty(wxT("EnumProperty3"),
                                   wxPG_LABEL,
                                   choices) );

        // This will affect both EnumProperty2 and EnumProperty3 since
        // they both use the same list of choices.
        choices.Add(wxT("Test Item"),360);

    \endcode

    If you want to create your enum properties with simple (label,name,value)
    constructor, then you need to create a new property class using one of the
    supplied macro pairs. See \ref newprops for details.

    wxFlagsProperty is similar:

    \code

        static const wxChar* flags_prop_labels[] = { wxT("wxICONIZE"), 
            wxT("wxCAPTION"), wxT("wxMINIMIZE_BOX"), wxT("wxMAXIMIZE_BOX"), NULL };

        // this value array would be optional if values matched string indexes
        static long flags_prop_values[] = { wxICONIZE, wxCAPTION, wxMINIMIZE_BOX, wxMAXIMIZE_BOX };

        pg->Append( wxFlagsProperty(wxT("FlagsProperty"),
                                    wxPG_LABEL,
                                    flags_prop_labels,
                                    flags_prop_values,
                                    0,
                                    GetWindowStyle()) );

    \endcode

    wxFlagsProperty can use wxPGConstants just the same way as wxEnumProperty
    (and also custom property classes can be created), <b>but currently its
    list of items cannot be modified after it has been created</b> (so please no
    GetPropertyChoices or AddPropertyChoice calls).


    \section advprops Advanced Properties

      This section descbribes the use of less often needed property classes.
    To use them, you need to include <wx/propgrid/advprops.h>.

    \code

    // Necessary extra header file
    #include <wx/propgrid/advprops.h>

    ...

        // wxArrayStringProperty embeds a wxArrayString.
        pg->Append ( wxArrayStringProperty(wxT("Example of ArrayStringProperty"),
                                           wxT("ArrayStringProp")));

        // Image file property. Wildcard is auto-generated from available
        // image handlers, so it is not set this time.
        pg->Append ( wxImageFileProperty(wxT("Example of ImageFileProperty"),
                                         wxT("ImageFileProp")));

        // Font property has sub-properties. Note that we give window's font as
        // initial value.
        pg->Append ( wxFontProperty(wxT("Font"),
                     wxPG_LABEL,
                     GetFont()) );

        // Colour property with arbitrary colour.
        pg->Append ( wxColourProperty(wxT("My Colour 1"),
                                      wxPG_LABEL,
                                      wxColour(242,109,0) ) );

        // System colour property.
        pg->Append ( wxSystemColourProperty (wxT("My SysColour 1"),
                                             wxPG_LABEL,
                                             wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)) );

        // System colour property with custom colour.
        pg->Append ( wxSystemColourProperty (wxT("My SysColour 2"),
                                             wxPG_LABEL,
                                             wxColour(0,200,160) ) );

        // Cursor property
        pg->Append ( wxCursorProperty (wxT("My Cursor"),
                                       wxPG_LABEL,
                                       wxCURSOR_ARROW));

    \endcode


    \section operations Operating with Properties

      All operations on properties should be done via wxPropertyGrid's
    (or wxPropertyGridManager's) methods. Class reference of the base property
    class should only be interesting for those creating custom property classes.

      Property operations, such as SetPropertyValue or DisableProperty,
    all have two versions: one which accepts property id (of type wxPGId) and
    another that accepts property name. Id is faster since it doesn't require
    hash map lookup, but name is often much more convenient.

      You can get property id as Append/Insert return value, or by calling
    GetPropertyByName.

    Example: Setting things about property named "X":

    \code

        // Altough name usually works, id is used here as an example
        wxPGId id = GetPropertyByName(wxT("X"));

        // There are many versions of this method, of which each accept
        // different type of value.
        // NOTE: If type of X is not "long", then this will yield a
        //       run-time error message.
        pg->SetPropertyValue ( wxT("X"), 200 );

        // Setting a string works for all properties - conversion is done
        // automatically.
        pg->SetPropertyValue ( id, wxT("400") );

        // Set new name.
        pg->SetPropertyName ( wxT("X"), wxT("NewNameOfX") );

        // Set new label - we need to use the new name.
        pg->SetPropertyLabel ( wxT("NewNameOfX"), wxT("New Label of X") );

    \endcode

    Example of iterating through all properties (that are not category or
    sub-property items):

    \code

        wxPGId id = pg->GetFirstProperty();

        while ( id.IsOk() )
        {
            // Do something with property id

            ...

            // Get next
            pg->GetNextProperty( id );
        }

	\endcode

    Getting value of selected wxSystemColourProperty (which value type is derived
    from wxObject):

    \code

        wxPGId id = pg->GetSelection();

        if ( id.IsOk() )
        {

            // Get name of property
            const wxString& name = pg->GetPropertyName ( id );

            // If type is not correct, GetColour() method will produce run-time error
            if ( pg->IsPropertyValueType ( id, CLASSINFO(wxColourPropertyValue) ) )
            {
                wxColourPropertyValue* pcolval =
                    wxDynamicCast(pg->GetPropertyValueAsWxObjectPtr(id),
                                  wxColourPropertyValue);

                // Report value
                wxString text;
                if ( pcolval->m_type == wxPG_CUSTOM_COLOUR )
                    text.Printf ( wxT("It is custom colour: (%i,%i,%i)"),
                        (int)pcolval->m_colour.Red(),
                        (int)pcolval->m_colour.Green(),
                        (int)pcolval->m_colour.Blue());
                else
                    text.Printf ( wxT("It is wx system colour (number=%i): (%i,%i,%i)"),
                        (int)pcolval->m_type,
                        (int)pcolval->m_colour.Red(),
                        (int)pcolval->m_colour.Green(),
                        (int)pcolval->m_colour.Blue());

                wxMessageBox ( text );
            }

        }


    \endcode

    \section populating Populating wxPropertyGrid Automatically

    \subsection fromvariants Populating from List of wxVariants

    Example of populating an empty wxPropertyGrid from a values stored
    in an arbitrary list of wxVariants.

    \code

        // This is a static method that initializes *all* builtin type handlers
        // available, including those for wxColour and wxFont. Refers to *all*
        // included properties, so when compiling with static library, this
        // method may increase the executable size significantly.
        pg->InitAllTypeHandlers ();

        // Get contents of the grid as a wxVariant list
        wxVariant all_values = pg->GetPropertyValues();

        // Populate the list with values. If a property with appropriate
        // name is not found, it is created according to the type of variant.
        pg->SetPropertyValues ( my_list_variant );

        // In order to get wxObject ptr from a variant value,
        // wxGetVariantCast(VARIANT,CLASSNAME) macro has to be called.
        // Like this:
        wxVariant v_txcol = pg->GetPropertyValue(wxT("Text Colour"));
        const wxColour& txcol = wxGetVariantCast(v_txcol,wxColour);

	\endcode

    \subsection tofile Saving Population to a Text-based Storage

    \code

    static void WritePropertiesToMyStorage( wxPropertyGrid* pg, wxPGId id, wxMyStorage& f, int depth )
    {
        wxString s;
        wxString s2;

        while ( id.IsOk() )
        {

            // TODO: Save property into text storage using:
            //   wxPropertyGrid::GetPropertyClassName
            //   wxPropertyGrid::GetPropertyName
            //   wxPropertyGrid::GetPropertyLabel
            //   wxPropertyGrid::GetPropertyValueAsString
            //   wxPropertyGrid::GetPropertyChoices
            //   wxPropertyGrid::GetPropertyAttributes

            // Example for adding choices:
            wxPGConstants& choices = pg->GetPropertyChoices(id);
            if ( choices.GetCount() )
            {
                // First add id of the choices list inorder to optimize
                s2.Printf(wxT("\"%X\""),(unsigned int)choices.GetId());
                s.Append(s2);
                f.AddToken(s2);

                size_t i;
                wxArrayString& labels = choices.GetLabels();
                wxArrayInt& values = choices.GetValues();
                if ( values.GetCount() )
                    for ( i=0; i<labels.GetCount(); i++ )
                    {
                        s2.Printf(wxT("\"%s||%i\""),labels[i].c_str(),values[i]);
                        f.AddToken(s2);
                    }
                else
                    for ( i=0; i<labels.GetCount(); i++ )
                    {
                        s2.Printf(wxT("\"%s\""),labels[i].c_str());
                        f.AddToken(s2);
                    }
            }

            // Write children, if any
            wxPGId firstChild = pg->GetFirstChild(id);
            if ( firstChild.IsOk() )
            {
                WritePropertiesToMyStorage( pg, firstChild, f, depth+1 );

                // TODO: Add parent's terminator
            }

            id = pg->GetNextSibling(id);
        }
    }

    ...

        // Then you can use this to store the entire hierarchy
        wxPGId firstChild = pg->GetFirstChild(pg->GetRoot());
        if ( firstChild.IsOk() )
            WritePropertiesToFile(pg,first_child,InstanceOfMyStorage,0);

    \endcode

    For more practical'ish example, see FormMain::OnSaveToFileClick in
    propgridsample.cpp.

    \subsection fromfile Loading Population from a Text-based Storage

    \code

        // Recommended when modifying the grid a lot at once
        pg->Freeze();

        // Necessary if you want a full-page loading
        pg->Clear();

        wxPropertyGridPopulator populator(pg);

        // Store strings from the source here
        wxString s_class;
        wxString s_name;
        wxString s_value;
        wxString s_attr;

        // Each set of choices loaded must have id
        size_t choicesId;

        wxArrayString choiceLabels;
        wxArrayInt choiceValues;

        // Pseudo-code loop to parse the source one "line" at a time
        while ( !source.IsAtEnd() )
        {

            // Clear stuff that doesn't exist at every "line"
            choicesId = 0;
            choiceLabels.Empty();
            choiceValues.Empty();

            // TODO: Load "line" to variables

            // TODO: When a sequence of sibling properties is terminated, call this:
            //   populator.EndChildren();

            // TODO: If had choices, use following code:
            //    if ( choicesId && !populator.HasChoices(choicesId) )
            //    {
            //        populator.AddChoices(choicesId,choiceLabels,choiceValues);
            //    }

            // TODO: Add the property.
            //   (for sake of simplicity we use here default name for properties)
            //    populator.AppendByClass(s_class,
            //                            s_name,
            //                            wxPG_LABEL,
            //                            s_value,
            //                            s_attr,
            //                            choicesId);

            // TODO: When a sequence of sibling properties begins, call this:
            //   populator.BeginChildren();

        }

        pg->Thaw();

    \endcode

    For more practical'ish example, see FormMain::OnLoadFromFileClick in
    propgridsample.cpp.


    \section events Event Handling

    Probably the most important event is the Changed event which occurs when
    value of any property is changed by the user. Use EVT_PG_CHANGED(id,func)
    in your event table to use it.
    For complete list of event types, see wxPropertyGrid class reference.

    The custom event class, wxPropertyGridEvent, has methods to directly
    access the property that triggered the event.

    Here's a small sample:

    \code

    // Portion of an imaginary event table
    BEGIN_EVENT_TABLE(MyForm, wxFrame)

        ...

        // This occurs when a property value changes
        EVT_PG_CHANGED( PGID, MyForm::OnPropertyGridChange )

        ...

    END_EVENT_TABLE()

    void MyForm::OnPropertyGridChange ( wxPropertyGridEvent& event )
    {

        // Get name of changed property
        const wxString& name = event.GetPropertyName();

        // Get resulting value - wxVariant is convenient here.
        wxVariant value = event.GetPropertyValue();

        // Get type identifier
        const wxPGValueType* type = event.GetPropertyValueType();

    }

    \endcode

    \section custprop Custom User Properties (using wxCustomProperty)

    By far the easiest way to have a somewhat customized property
    is to use wxCustomProperty class. No subclassing is necessary -
    property is modified using wxPropertyGrid::SetPropertyAttribute.

    <b>Available Customizations</b> (relevant attribute in parenthesis)

    - May have children.
    - Editor class (wxPG_CUSTOM_EDITOR).
    - Image in front of value (wxPG_CUSTOM_IMAGE,wxPG_CUSTOM_PAINT_CALLBACK).
    - What happens when editor's button is pressed (wxPG_CUSTOM_CALLBACK).

    For more info on attributes, see \ref attrids. In sample application,
    there is a CustomProperty property that has children that can be
    used to modify the property itself.

    <b>Limitations:</b>
    - Currently wxCustomProperty is limited to wxString value type.

    \remarks
    - Since image is not resized to fit, only cropped, you must resize
      the it, if necessary, before setting it as wxPG_CUSTOM_IMAGE
      attribute. Use wxPropertyGrid::GetImageSize to get preferred
      image size (maximum width should be 64).
    - If you need to have property editors other than those supplied,
      please see wxPGEditor docs and source code in propgrid.cpp.

    \code

        //
        // Sample code for adding wxCustomProperty
        //
    
        wxPGId cpId = pg->Append( wxCustomProperty(wxT("Shape"),wxPG_LABEL) );

        // Can have children
        pg->AppendIn( cpId, wxStringProperty(wxT("Label"),
                                             wxT("Shape_Label"),
                                             wxT("Shape 1")) );

        // Set choice editor
        pg->SetPropertyAttribute( cpId, wxPG_CUSTOM_EDITOR, wxPGEditor_Choice );

        wxPGConstants choices;
        choices.Add(wxT("Line"));
        choices.Add(wxT("Triangle"));
        choices.Add(wxT("Rectangle"));
        choices.Add(wxT("Circle"));

        // Set items to choose from
        pg->SetPropertyChoices( cpId, choices );

    \endcode


    \section compiling Compiling wxPropertyGrid

      Actual build process is described in readme.txt, so here is only information
    about compile options. Define these in library project settings, makefile, or if
    you prefer, at the top of propgrid.h.

    <b>wxPG_USE_CUSTOM_CONTROLS:</b> Default is 0. If set to 1, non-wxWindow based custom
    controls are used instead of normal controls. This will provide cleaner, less flickering
    appearance, but combobox may have less features and act a bit differently and textctrl
    has only generic key combinations.

    \section usage2 Using wxPropertyGridManager

    wxPropertyGridManager is an efficient multi-page version of wxPropertyGrid,
    which can optionally have toolbar for mode and page selection, help text box,
    and a compactor button.

    wxPropertyGridManager mirrors most non-visual methods of wxPropertyGrid, some
    identically, some so that they can affect a property on any page, and some
    so that they can only affect selected target page.

    Generally, methods that operate on a property ( such as
    GetPropertyValue, SetPropertyValue, EnableProperty, LimitPropertyEditing, Delete, etc. ),
    work globally (so the given property can exist on any managed page).

    Methods that add properties ( Append, Insert, etc.) or operate on multiple properties
    (such as GetPropertyValues or SetPropertyValues), will work in target page only.
    Use SetTargetPage(index) method to set current target page. Changing a displayed page
    (using SelectPage(index), for example) will automatically set the target page
    to the one displayed.

    Global methods such as ExpandAll generally work on the target page only.

    Visual methods, such as SetCellBackgroundColour and GetNextVisible are only
    available in wxPropertyGrid. Use wxPropertyGridManager::GetGrid() to obtain
    pointer to it.

    wxPropertyGridManager constructor has exact same format as wxPropertyGrid
    constructor, and basicly accepts same extra window style flags (albeit also
    has some extra ones).

    Here's some example code for creating and populating a wxPropertyGridManager:

    \code

        wxPropertyGridManager* pgMan = new wxPropertyGridManager(this, PGID,
            wxDefaultPosition, wxDefaultSize,
            // These and other similar styles are automatically
            // passed to the embedded wxPropertyGrid.
            wxPG_BOLD_MODIFIED|wxPG_SPLITTER_AUTO_CENTER|
            // Include toolbar.
            wxPG_TOOLBAR |
            // Include description box.
            wxPG_DESCRIPTION |
            // Include compactor.
            wxPG_COMPACTOR |
            // Plus defaults.
            wxPGMAN_DEFAULT_STYLE
           );

        // Adding a page sets target page to the one added, so
        // we don't have to call SetTargetPage if we are filling
        // it right after adding.
        pgMan->AddPage(wxT("First Page"));

        pgMan->AppendCategory(wxT("Category A1"));

        // Remember, the next line equals pgman->Append( wxIntProperty(wxT("Number"),wxPG_LABEL,1) );
        pgMan->Append( wxT("Number"),wxPG_LABEL,1 );

        pgMan->Append( wxColourProperty(wxT("Colour"),wxPG_LABEL,*wxWHITE) );

        pgMan->AddPage(wxT("Second Page"));

        pgMan->Append( wxT("Text"),wxPG_LABEL,wxT("(no text)") );

        pgMan->Append( wxFontProperty(wxT("Font"),wxPG_LABEL) );

        // For total safety, finally reset the target page.
        pgMan->SetTargetPage(0);

    \endcode

    \section misc Miscellaneous Topics

    \subsection boolproperty wxBoolProperty

      There are few points about wxBoolProperty that require futher discussion:
      - wxBoolProperty can be shown as either normal combobox, checkbox, or
        a combobox which cycles when double-clicked. Property attribute
        wxPG_BOOL_USE_CHECKBOX sets property to use checkbox, while
        wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING causes it to become double-click
        cycled combo box. For example, if you have a wxFlagsProperty, you can
        set its all items to use double-click cycling using the following:
        \code
            pg->SetPropertyAttribute(wxT("MyFlagsProperty"),wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING,(long)1,wxRECURSE);
        \endcode

      - Default item names for wxBoolProperty are [False,True]. This can be
        changed using wxPropertyGrid::SetBoolChoices(true_choice,false_choice).

    \section proplist Property Descriptions

    Here are descriptions of built-in properties, with attributes
    (see wxPropertyGrid::SetPropertyAttribute) that apply to them.

    \subsection wxPropertyCategory

    <b>Inheritable Class:</b> wxPropertyCategoryClass.

    Not an actual property per se, but a header for a group of properties.

    \subsection wxParentProperty

    Pseudo-property that can have sub-properties inserted under itself.
    Has textctrl editor that allows editing values of all sub-properties
    in a one string. In essence, it is a category that has look and feel
    of a property, and which children can be edited via the textctrl.

    \subsection wxStringProperty

    Simple string property.

    \subsection wxIntProperty

    Like wxStringProperty, but converts text to a signed long integer.

    \subsection wxUIntProperty

    Like wxIntProperty, but displays value as unsigned int. To set
    the prefix used globally, manipulate wxPG_UINT_PREFIX string attribute.
    To set the globally used base, manipulate wxPG_UINT_BASE int
    attribute. Regardless of current prefix, understands (hex) values starting
    with both "0x" and "$".

    \subsection wxFloatProperty

    Like wxStringProperty, but converts text to a double-precision floating point.
    Default float-to-text precision is 6 decimals, but this can be changed
    by modifying wxPG_FLOAT_PRECISION attribute.

    \subsection wxBoolProperty

    Represents a boolean value. wxChoice is used as editor control, by the
    default. wxPG_BOOL_USE_CHECKBOX attribute can be set to 1 inorder to use
    check box instead.

    \subsection wxLongStringProperty

    <b>Inheritable Class:</b> wxLongStringPropertyClass

    Like wxStringProperty, but has a button that triggers a small text editor
    dialog.

    \subsection wxDirProperty

    Like wxLongStringProperty, but the button triggers dir selector instead.

    \subsection wxFileProperty

    <b>Inheritable Class:</b> wxFilePropertyClass

    Like wxLongStringProperty, but the button triggers file selector instead.
    Default wildcard is "All files..." but this can be changed by setting
    wxPG_FILE_WILDCARD attribute (see wxFileDialog for format details).
    Attribute wxPG_FILE_SHOW_FULL_PATH can be set to 0 inorder to show
    only the filename, not the entire path.

    \subsection wxEnumProperty

    <b>Inheritable Class:</b> wxEnumPropertyClass

    Represents a single selection from a list of choices - wxChoice is used
    to edit the value.

    \subsection wxFlagsProperty

    <b>Inheritable Class:</b> wxFlagsPropertyClass

    Represents a bit set that fits in a long integer. wxBoolProperty sub-properties
    are created for editing individual bits. Textctrl is created to manually edit
    the flags as a text; a continous sequence of spaces, commas and semicolons 
    is considered as a flag id separator.
    \remarks
    You cannot change children (flags) of this property directly, instead
    you can user wxPropertyGrid::ReplaceProperty to replace it with one
    that has new items.

    \subsection wxArrayStringProperty

    <b>Inheritable Class:</b> wxArrayStringPropertyClass

    Allows editing of a list of strings in wxTextCtrl and in a separate dialog.

    \subsection wxMultiChoiceProperty

    Allows editing a multiple selection from a list of strings. This is
    property is pretty much built around concept of wxMultiChoiceDialog.

    \subsection wxImageFileProperty

    <b>Inheritable Class:</b> wxImageFilePropertyClass

    Like wxFileProperty, but has thumbnail of the image in front of
    the filename and autogenerates wildcard from available image handlers.

    \subsection wxColourProperty

    Represents wxColour. wxChoice is used to edit the value. Drop-down list
    has few basic colour choices.

    \subsection wxFontProperty

    Represents wxFont. Various sub-properties are used to edit individual
    subvalues.

    \subsection wxSystemColourProperty

    <b>Inheritable Class:</b> wxSystemColourPropertyClass

    Represents wxColour and a system colour index. wxChoice is used to edit
    the value. Drop-down list has color images.

    \subsection wxCursorProperty

    Represents a wxCursor. wxChoice is used to edit the value.
    Drop-down list has cursor images under some (wxMSW) platforms.

    \subsection wxCustomProperty

    <b>Inheritable Class:</b> wxCustomPropertyClass

    A customizable property class with string data type. Value image, Editor class,
    and children can be modified.

    \subsection Additional Sample Properties

    Sample application has following additional examples of custom properties:
    - wxFontDataProperty ( edits wxFontData )
    - wxPointProperty ( edits wxPoint )
    - wxSizeProperty ( edits wxSize )
    - wxAdvImageFileProperty ( like wxImageFileProperty, but also has a drop-down for recent image selection)
    - wxDirsProperty ( edits a wxArrayString consisting of directory strings)
    - wxArrayDoubleProperty ( edits wxArrayDouble )

    \section userhelp Using wxPropertyGrid

    This is a short summary of how a wxPropertyGrid is used (not how it is programmed),
    or, rather, how it <b>should</b> behave in practice.

    - Basic mouse usage is as follows:\n
      - Clicking property label selects it.
      - Clicking property value selects it and focuses to editor control.
      - Clicking category label selects the category.
      - Double-clicking category label selects the category and expands/collapses it. 
      - Double-clicking labels of a property with children expands/collapses it.

    - Keyboard usage is as follows:\n
      When editor control is not focused:\n
      - cursor up - moves to previous visible property\n
      - cursor down - moves to next visible property\n
      - cursor left - if collapsible, collapses, otherwise moves to previous property\n
      - cursor right - if expandable, expands, otherwise moves to next property\n
      - tab (if enabled) - focuses keyboard to the editor control of selected property\n
      When editor control is focused:\n
      - tab - moves to next visible property\n
      - shift-tab - moves to previous visible property\n
      - escape - unfocuses from editor control\n

    - wxTextCtrl / wxCustomTextCtrl based editors only report that value has actually
      changed if text was changed and return was pressed afterwards.

    - In long strings tabs are represented by "\t" and line break by "\n".

    \section bugspostponed Bugs Not Fixed In 1.0.x branch

    This is a list of bugs that have been fixed in the 1.1.0 development branch,
    but not in the 1.0.x stable API branch. Personally I don't have enough time
    and energy to do the necessary code changes and testing for both branches.

    - Size given in constructor or Create does not work.

    - It is not possible to have user properties with name not starting
      with "wx".

    \section bugs Known Bugs
    Any insight on these is more than welcome.
    - wxGTK: Pressing ESC to unfocus an editor control will screw the focusing
      (either focuses back to the editor or moves focus to limbo; works perfectly
      on wxMSW though).
    - I have experienced a complete freeze when toying with the popup of
      wxAdvImageProperty. Visiting another window will end it.
    - wxGTK: in sample application, property text colour is not set correct from
      wxSystemColourProperty. Value is correct, but the resulting text colour
      is sometimes a bit skewed if it is not a shade of gray. This may be GTK's
      attempt to auto-adjust it.
    - wxGTK: Sometimes '...' text on editor button may disappear (maybe
      "invisible font" related thingie). wxAdvImageProperty never seem to get
      the text, while other controls do.
    Following are wxWidgets or platform bugs:
    - wxWidgets <= 2.5.2: No proper uxtheme (WinXP buttons etc). No keyboard
      navigation out of grid. Possibly other nasty problems.
    - wxWidgets <= 2.5.3: No mouse wheel scrolling in drop down lists.
    - wxWidgets <= 2.5.5: wxGTK: No proper static text wrapping, so help box text
      may appear incorrectly.
    - wxMSW: After (real) popup window is shown in wxPropertyGrid, grid's scrollbar
      won't get hilight until another window is visited. This is probably a Windows
      bug.
    - wxGTK+ w/ unicode: image handlers return only the first character of an extension
    - wxGTK+ 1.2: Controls sometimes disappear. They reappear when cursor is moved
      over them.
    - wxGTK+ 1.2: Cannot set (multiple) items for wxListBox (affects
      wxMultiChoiceProperty).

    \section issues Issues
    These are not bugs per se, but miscellaneous cross-platform issues that have been
    resolved in a less-than-satisfactory manner.
    - wxMSW: Toolbar's ToggleTool doesn't disable last item in the same radiogroup. AFAIK,
      there is no way to do that.
    - wxMSW: wxCCustomTextCtrl copy-pasting causes memory leak unless app calls
      wxTheClipboard->Flush() before exiting.
    - Atleast with wxGTK2+Unicode+Debug Mode File Selector dialog may trigger an assertion
      failure (line 1060 in string.cpp with 2.5.3) that can be cancelled
      probably without any ill-effect.

    Following only apply when <b>not</b> using custom controls:
    - Under GTK, EVT_MOTION does not trigger for child control. Causes cursor change
      inconsistencies. Permanent mouse capture is not practical since it causes wxWindow::
      SetCursor to fail (and events cannot be relayed to native controls anyway).
      Easy solution used: Splitter drag detect margin and control do not overlap.
    - When splitter is being dragged, active editor control (button as well under wxGTK)
      is hidden to prevent flickering. This may go unnoticed with some
      controls (wxTextCtrl) but is very noticeable with others (wxChoice).
    - Under MSW, when resizing, editor controls flicker. No easy fix here
      (wxEVT_ONIDLE might be employed).
    - Under GTK 1.2, font may show invisible if it is not bold (so it is forced).
    - Under wxGTK, controls may flicker a bit (actually, a lot) when being shown.

    \section todo Todo
    For a detailed TODO, see propertygrid.cpp (just search for "todo" and you'll find it).

    \section notes Design Notes
    - Currently wxPropertyGridManager uses "easy" way to relay events from embedded
      wxPropertyGrid. That is, the exact same id is used for both.

    - wxHashMap used to access properties by name uses 'const wxChar*' instead of 'wxString'.
      Altough this has somewhat lower performance if used mostly with wxStrings, it is much
      faster if a lot of non-wxString strings are used, since they don't have to be
      recreated as wxString before using with the hashmap.
      If you want to change this behaviour, see propertygrid.h. Comment current version
      (including wxPGNameStr), and uncomment version that uses wxString.
      Note that with unicode, wxString is always used (due to some weird issues).

    - If wxPG_DOUBLE_BUFFER is 1 (default for MSW, GTK and MAC), wxPropertyGrid::OnDrawItems
      composes the image on a wxMemoryDC. This effectively eliminates flicker caused by drawing
      itself (property editor controls are another matter).

    - Under wxMSW, flicker freedom with native editor controls is achieved by creating them
      at 'out of sight' position, then hiding&freezing them, then moving them to the
      correct position, and finally thawing&showing them.

    \section crossplatform Crossplatform Notes

    - GTK1: When showing a dialog you may encounter invisible font!
      Solution: Set parent's font using SetOwnFont instead of SetFont.

    - GTK: Your own control can overdraw wxGTK wxWindow border!

    - wxWindow::SetSizeHints may be necessary to shrink controls below certain treshold,
      but only on some platforms. For example wxMSW might allow any shrinking without
      SetSizeHints call where wxGTK might not.

    - GTK Choice (atleast, maybe other controls as well) likes its items set
      in constructor. Appending them seems to be slower (Freeze+Thaw won't help).
      Even using Append that gets wxArrayString argument may not be good, since it
      may just append every string one at a time.

    \section newprops Creating New Properties

    <b>Note:</b> This section is under construction!

    Each property class represents a specialized value storage for a value type.
    It also nominates an editor class to itself, and implements some helper
    methods to complement the used value type and editor classes.

    Easiest way to create a new property is to use one of the supplied
    macro pairs (see the section below).

    \remarks
    - Code that implements a property generally requires inclusion of
      wx/propgrid/propdev.h.
    - Read wxPGProperty and wxPGPropertyWithChildren class documentation to
      find out what each overriddable method should do.

    \subsection methoda Macro Pairs

    \subsubsection custstringprop String Property with Button

    This custom property will be exactly the same as wxLongStringProperty,
    except that you can specify a custom code to handle what happens
    when the button is pressed.

      In header:
        \code
        WX_PG_DECLARE_STRING_PROPERTY(PROPNAME)
        \endcode

        In source:
        \code

        #include <wx/propgrid/propdev.h>

        WX_PG_IMPLEMENT_STRING_PROPERTY(PROPNAME)
        bool PROPNAMEClass::OnButtonClick ( wxPropertyGrid* propgrid, wxString& value )
        {
            //
            // TODO: Show dialog, read initial string from value. If changed,
            //   store new string to value and return TRUE.
            //
        }
        \endcode

    \subsubsection custflagsprop Custom Flags Property
    Flags property with custom default value and built-in labels/values.

    In header:
        \code
        WX_PG_DECLARE_CUSTOM_FLAGS_PROPERTY(PROPNAME)
        \endcode

    In source:
        \code

        #include <wx/propgrid/propdev.h>

        // LABELS, VALUES and ITEMCOUNT are as in the arguments to wxFlagsProperty
        // constructor. DEFVAL is the new default value (normally it is 0).
        WX_PG_IMPLEMENT_CUSTOM_FLAGS_PROPERTY(PROPNAME,LABELS,VALUES,ITEMCOUNT,DEFAULT_FLAGS)
        \endcode

    The new property class will have simple (label,name,value) constructor.

    \subsubsection custenumprop Custom EnumProperty

    Exactly the same as custom FlagsProperty. Simply replace FLAGS with ENUM in
    macro names to create wxEnumProperty based class instead.

    \subsubsection custarraystringprop Custom ArrayString property

    This type of custom property allows selecting different string delimiter
    (default is '"' on both sides of the string - as in C code), and allows
    adding custom button into the editor dialog.

    In header:
        \code
        WX_PG_DECLARE_ARRAYSTRING_PROPERTY(wxMyArrayStringProperty)
        \endcode

    In source:

        \code

        #include <wx/propgrid/propdev.h>

        // second argument = string delimiter. '"' for C string style (default),
        //    and anything else for str1<delimiter> str2<delimiter> str3 style
        //    (so for example, using ';' would result to str1; str2; str3).
        // third argument = const wxChar* text for the custom button. If NULL
        //    then no button is added.
        WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY(wxMyArrayStringProperty,',',wxT("Browse"))

        bool wxMyArrayStringPropertyClass::OnCustomStringEdit (wxWindow* parent,
                                                               wxString& value)
        {
            //
            // TODO: Show custom editor dialog, read initial string from value.
            //   If changed, store new string to value and return TRUE.
            //
        }

        \endcode

    \subsubsection custcolprop Custom ColourProperty

    wxColourProperty/wxSystemColourProperty that can have custom list of colours
    in dropdown.

    Use version that doesn't have _USES_WXCOLOUR in macro names to have
    wxColourPropertyValue as value type instead of plain wxColour (in this case
    values array might also make sense).

    In header:
        \code
        #include <wx/propgrid/advprops.h>
        WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(wxMyColourProperty)
        \endcode

    In source:

        \code

        #include <wx/propgrid/propdev.h>

        // Colour labels. Last (before NULL, if any) must be Custom.
        static const wxChar* mycolprop_labels[] = {
            wxT("Black"),
            wxT("Blue"),
            wxT("Brown"),
            wxT("Custom"),
            (const wxChar*) NULL
        };

        // Relevant colour values as unsigned longs.
        static unsigned long mycolprop_colours[] = {
            wxPG_COLOUR(0,0,0),
            wxPG_COLOUR(0,0,255),
            wxPG_COLOUR(166,124,81),
            wxPG_COLOUR(0,0,0)
        };

        // Implement property class. Third argument is optional values array,
        // but in this example we are only interested in creating a shortcut
        // for user to access the colour values. Last arg is itemcount, but
        // that's not necessary because our label array is NULL-terminated.
        WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(wxMyColourProperty,
                                                             mycolprop_labels,
                                                             (long*)NULL,
                                                             mycolprop_colours,
                                                             0)
        \endcode

    \subsection declaring Declaring an Arbitrary Property

    To make your property available globally, you need to declare it in a
    header file. Usually you would want to use WX_PG_DECLARE_PROPERTY
    macro to do that (it is in propgrid.h). It has three arguments: PROPNAME,
    T_AS_ARG and DEFVAL. PROPNAME is property NAME, T_AS_ARG is type input
    in function argument list ( "int" for int value type, "const wxString&" for
    wxString value type, etc.), and DEFVAL is default value for that.

    For example:

    \code

        // Declare wxRealPoint Property
        WX_PG_DECLARE_PROPERTY(wxRealPointProperty,const wxRealPoint&,wxRealPoint(0.0,0.0))

    \endcode

    There is also WX_PG_DECLARE_PROPERTY_WITH_DECL which takes an additional declaration
    argument (export ,for example, when exporting from a dll).

    If you want that your property could be inherited from, then you would also
    have to define the class body in the header file. In most cases this is not
    necessary and the class can be defined and implemented completely in the source.

    \subsection designtypes Design Types of Properties

    When speaking from an object design perspective, there are basicly five different
    types of properties:

    <b>Basic</b>: These derive directly from base abstract property class (wxPGProperty).
    By far the most commonly used design type.

    <b>With Children</b>: These derive directly from base abstract parent property class
    (wxPGPropertyWithChildren). Font property and flags property, for example, are
    of this design type.

    <b>Inherited</b>: These are inherited from a working property class but do not
    use the same value type (for example, SystemColour property inherits from
    Enum property but has different value type).

    <b>Simple Inherited</b>: These are inherited from a working property class and
    use the same value type (for example, Cursor property inherits from
    Enum property and uses the same value type).

    <b>Inherited With Subset Value</b>: These are like Simple Inherited but they
    do not use the same value type. However, they *do* use the same value data
    to store a subset of the parent class' value (for example, Colour property
    has subset of SystemColour property's value).

    \subsection implementing Implementing a Property

    First there is class body with WX_PG_DECLARE_PROPERTY_CLASS (or similar) macro,
    constructor, virtual destructor, and declarations for other overridden
    methods. Then comes WX_PG_IMPLEMENT_PROPERTY_CLASS (or similar) macro, and
    after that class method implementations.

    \subsection Tips

    - To get property's parent grid, call GetParentState()->GetGrid()

    \subsection valuetypes Creating New Value Types

    If you want to a property to use a value type that is not among the
    builtin types, then you need to create a new property value type. It is
    quite straightforward, using two macros.

    In header, use WX_PG_DECLARE_VALUE_TYPE(DATATYPE), like this:

    \code
        // Example: Declare value type for wxRealPoint.
        WX_PG_DECLARE_VALUE_TYPE(wxRealPoint)
    \endcode

    If, however, class of your value type does not inherit from
    wxObject, and you need to use it in wxVariant list used as a
    persistent storage (for example, see wxPropertyGrid::GetPropertyValues),
    then use this instead, as it also declares a necessary wxVariantData_DATATYPE
    class.

    \code
        // Example: Declare value type and wxVariantData class for wxRealPoint.
        WX_PG_DECLARE_VALUE_TYPE_VOIDP(wxRealPoint)
    \endcode

    There are also _WITH_DECL versions of both.

    However, there are a few different implement macros to place in
    a source file. Pick one according to the type of type.

    \code

        // For implementing value type for a wxObject based class.
        WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ(TYPE,DEFPROPERTY,DEFVAL)

        // Same as above, except that an instance of TYPE is
        // stored in class. Thus, DEFVAL can be any expression
        // that can be assigned to the type.
        WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ_OWNDEFAULT(TYPE,DEFPROPERTY,DEFVAL)

        // For implementing value type for a non-wxObject based class.
        // Like with ...WXOBJ_OWNDEFAULT macro above, instance of TYPE
        // is stored and DEFVAL can be any expression.
        WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP_SIMPLE(TYPE,DEFPROPERTY,DEFVAL) 

        // Like above, but also implement the wxVariantData class
        // declared with the second type value type declare macro.
        WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(TYPE,DEFPROPERTY,DEFVAL) 

        // Like above, but accepts a custom wxVariantData class.
        WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP_CVD(TYPE,DEFPROPERTY,DEFVAL,VARIANTDATACLASS) 

        // For implementing value type with different default value.
        WX_PG_IMPLEMENT_DERIVED_TYPE(TYPENAME,PARENTVT,DEFVAL)

        // For implementing value type for a native value.
        // Generally should not be used since it is meant for
        // wxString, int, double etc. which are already implemented.
        WX_PG_IMPLEMENT_VALUE_TYPE(TYPE,DEFPROPERTY,TYPESTRING,GETTER,DEFVAL)

    \endcode

    Argument descriptions:

    TYPE - Actual data type represented by the value type, or if
      derived type, any custom name.

    DEFPROPERY - Name of the property that will edit this
      value type by default.

    DEFVAL - Default value for the property.

    TYPENAME - An arbitraty typename for this value type. Applies
      only to the derived type.

    PARENTVT - Name of parent value type, from which this derived
      type inherits from.

    \remarks
    - Your class, which you create value type for, must have a
      copy constructor.

*/

#endif // __WX_PG_DOX_MAINPAGE_H__
