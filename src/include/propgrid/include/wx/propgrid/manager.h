/////////////////////////////////////////////////////////////////////////////
// Name:        manager.h
// Purpose:     wxPropertyGridManager
// Author:      Jaakko Salli
// Modified by:
// Created:     Jan-14-2005
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_MANAGER_H_
#define _WX_PROPGRID_MANAGER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "manager.cpp"
#endif

#include <wx/propgrid/propgrid.h>

// -----------------------------------------------------------------------

extern WXDLLIMPEXP_PG const wxChar *wxPropertyGridManagerNameStr;

// -----------------------------------------------------------------------

// This is for mirroring wxPropertyGrid methods with ease.
// Needs to be in hear because of inlines.
#define wxPG_IMPLEMENT_PGMAN_METHOD_NORET1(NAME,AT1) \
wxPG_IPAM_DECL void wxPropertyGridManager::NAME ( wxPGId id, AT1 _av1_ ) \
{ \
    wxPGProperty* p = wxPGIdToPtr(id); \
    wxPropertyGridState* pState = p->GetParentState(); \
    wxASSERT ( pState != (wxPropertyGridState*) NULL ); \
    if ( pState == m_propGrid.m_pState ) m_propGrid.NAME(id,_av1_); \
    else pState->NAME(p,_av1_); \
} \
wxPG_IPAM_DECL void wxPropertyGridManager::NAME ( wxPGNameStr name, AT1 _av1_ ) \
{ \
    wxPropertyGridState* pState; \
    wxPGId id = GetPropertyByName2(name,&pState); \
    wxASSERT ( pState != (wxPropertyGridState*) NULL ); \
    if ( pState == m_propGrid.m_pState ) m_propGrid.NAME(id,_av1_); \
    else pState->NAME(wxPGIdToPtr(id),_av1_); \
}

class wxPropertyGridPageData;

#define wxPG_IPAM_DECL inline

/** \class wxPropertyGridManager
    \ingroup classes
    \brief
    wxPropertyGridManager is an efficient multi-page version of wxPropertyGrid,
    which can optionally have toolbar for mode and page selection, help text box,
    and a compactor button.
    Use window flags to select which ones to include.

    <h4>Derived from</h4>

    wxPropertyContainerMethods\n
    wxWindow\n
    wxEvtHandler\n
    wxObject\n

    <h4>Include files</h4>

    <wx/propgrid/manager.h>

    <h4>Window styles</h4>

    @link wndflags Additional Window Styles@endlink

    <h4>Event handling</h4>

    To process input from a propertygrid control, use these event handler macros to
    direct input to member functions that take a wxPropertyGridEvent argument.

    <table>
    <tr><td>EVT_PG_SELECTED (id, func)</td><td>Property is selected.</td></tr>
    <tr><td>EVT_PG_CHANGED (id, func)</td><td>Property value is modified.</td></tr>
    <tr><td>EVT_PG_HIGHLIGHTED (id, func)</td><td>Mouse moves over property. Event's property is NULL if hovered on area that is not a property.</td></tr>
    <tr><td>EVT_PG_PAGE_CHANGED (id, func)</td><td>User changed page in manager.</td></tr>
    </table>

    \sa @link wxPropertyGridEvent wxPropertyGridEvent@endlink

*/
// BM_MANAGER
class WXDLLIMPEXP_PG wxPropertyGridManager : public wxPanel, public wxPropertyContainerMethods
{
	DECLARE_CLASS(wxPropertyGridManager)

public:

	/** Two step constructor. Call Create when this constructor is called to build up the
	    wxPropertyGridManager.
	*/
    wxPropertyGridManager();

    /** The default constructor. The styles to be used are styles valid for
        the wxWindow.
        \sa @link wndflags Additional Window Styles@endlink
    */
    wxPropertyGridManager(wxWindow *parent, wxWindowID id = -1,
               	      const wxPoint& pos = wxDefaultPosition,
               	      const wxSize& size = wxDefaultSize,
               	      long style = wxPGMAN_DEFAULT_STYLE,
               	      const wxChar* name = wxPropertyGridManagerNameStr);

    /** Destructor */
    virtual ~wxPropertyGridManager();

    /** Creates new property page. Note that the first page is not created
        automatically, and before you have created any pages you are basicly
        in a non-paged mode. When you create the first page, contents
        of the underlying propertygrid is added into it (or this is how it
        will seem to to the user).
        \param label
        A label for the page. This may be shown as a toolbar tooltip etc.
        \param bmp
        Bitmap image for toolbar. If wxNullBitmap is used, then a built-in
        default image is used.
        \retval
        Returns index to the page created.
        \remarks
        If toolbar is used, it is highly recommended that the pages are
        added when the toolbar is not turned off using window style flag
        switching.
    */
    inline int AddPage ( const wxString& label = wxEmptyString, const wxBitmap& bmp = wxNullBitmap )
    {
        return InsertPage (-1,label,bmp);
    }

    /** See wxPropertyGrid::Append. */
    inline wxPGId AppendCategory ( const wxString& label, const wxString& name = wxPG_LABEL )
    {
        wxASSERT ( m_targetState );
        return m_targetState->Append ( new wxPropertyCategoryClass(label,name) );
    }

    /** See wxPropertyGrid::Append. */
    inline wxPGId Append ( wxPGProperty* property )
    {
        wxASSERT ( m_targetState );
        return m_targetState->Append(property);
    }

    inline wxPGId Append ( const wxString& label, const wxString& name, const wxString& value = wxEmptyString )
    {
        wxASSERT ( m_targetState );
        return m_targetState->Append ( wxStringProperty(label,name,value) );
    }

    inline wxPGId Append ( const wxString& label, const wxString& name, int value )
    {
        wxASSERT ( m_targetState );
        return m_targetState->Append ( wxIntProperty(label,name,value) );
    }

    inline wxPGId Append ( const wxString& label, const wxString& name, double value )
    {
        wxASSERT ( m_targetState );
        return m_targetState->Append ( wxFloatProperty(label,name,value) );
    }

    inline wxPGId Append ( const wxString& label, const wxString& name, bool value )
    {
        wxASSERT ( m_targetState );
        return m_targetState->Append ( wxBoolProperty(label,name,value) );
    }

    /** See wxPropertyGrid::AppendIn. */
    inline wxPGId AppendIn ( wxPGId id, wxPGProperty* property )
    {
        wxASSERT ( m_targetState );
        return m_targetState->Insert((wxPGPropertyWithChildren*)(wxPGProperty*)id,-1,property);
    }

    /** See wxPropertyGrid::AppendIn. */
    inline wxPGId AppendIn ( wxPGNameStr name, wxPGProperty* property )
    {
        wxASSERT ( m_targetState );
        return m_targetState->Insert((wxPGPropertyWithChildren*)(wxPGProperty*)m_targetState->BaseGetPropertyByName(name),-1,property);
    }

    void ClearModifiedStatus ( wxPGId id );

    inline void ClearModifiedStatus ()
    {
        m_propGrid.ClearModifiedStatus();
    }

    /** Deletes all properties on given page.
    */
    void ClearPage( int page );

    bool ClearPropertyValue( wxPGId id );
    bool ClearPropertyValue( wxPGNameStr name );

    /** Collapses given item. Returns TRUE if it was collapsable and previously expanded. */
    bool Collapse ( wxPGId id );
    bool Collapse ( wxPGNameStr name );

    /** Collapses all parents on target page. */
    inline void CollapseAll ()
    {
        m_targetState->ExpandAll(0);
    }

    /** Compacts (arg is TRUE) or expands the propertygrid (i.e. low priority
        items are already hidden or shown).
    */
    void Compact ( bool compact );

    /** Two step creation. Whenever the control is created without any parameters,
        use Create to actually create it. Don't access the control's public methods
        before this is called.
        \sa @link wndflags Additional Window Styles@endlink
    */
    bool Create(wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxPGMAN_DEFAULT_STYLE,
                const wxChar* name = wxPropertyGridManagerNameStr);

    /** Deletes a property. */
    void Delete ( wxPGId id );
    void Delete ( wxPGNameStr name );

    /** Disables a property. */
    inline bool DisableProperty ( wxPGId id ) { return EnableProperty (id,FALSE); }

    /** Disables a property. */
    inline bool DisableProperty ( wxPGNameStr name ) { return EnableProperty (name,FALSE); }

    /** Enables or disables (shows/hides) categories according to parameter enable.
        WARNING: Not tested properly, use at your own risk.
    */
    inline bool EnableCategories ( bool enable )
    {
        long fl = m_windowStyle | wxPG_HIDE_CATEGORIES;
        if ( enable ) fl = m_windowStyle & ~(wxPG_HIDE_CATEGORIES);
        SetWindowStyleFlag(m_windowStyle);
        return TRUE;
    }

    /** Enables or disables a property on target page. */
    bool EnableProperty ( wxPGId id, bool enable = TRUE );
    bool EnableProperty ( wxPGNameStr name, bool enable = TRUE );

    /** Selects page, scrolls and/or expands items to ensure that the
        given item is visible. Returns TRUE if something was actually done.
    */
    inline bool EnsureVisible ( wxPGId id )
    {
        return EnsureVisible(id,((wxPGProperty*)id)->GetParentState());
    }

    /** Selects page, scrolls and/or expands items to ensure that the
        given item is visible. Returns TRUE if something was actually done.
    */
    inline bool EnsureVisible ( wxPGNameStr name )
    {
        wxPropertyGridState* pstate;
        wxPGId id = GetPropertyByName2(name,&pstate);
        return EnsureVisible(id,pstate);
    }

    /** Expands given item. Returns TRUE if it was expandable and previously collapsed. */
    bool Expand ( wxPGId id );
    bool Expand ( wxPGNameStr name );

    /** Expands all parents on target page. */
    void ExpandAll ()
    {
        m_targetState->ExpandAll(1);
    }

    /** Returns cell background colour of a category. */
    /*wxColour GetCategoryColour ( wxPGId id )
    {
        return wxPropertyGrid::GetCategoryColour(id);
    }
    wxColour GetCategoryColour ( wxPGNameStr name ) { return GetCategoryColour(GetPropertyByName(name)); }*/

    /** Returns number of children of the root property of the selected page. */
    inline size_t GetChildrenCount ()
    {
        return GetChildrenCount( wxPGId(m_propGrid.m_pState->m_properties) );
    }

    /** Returns number of children of the root property of given page. */
    size_t GetChildrenCount ( int page_index );

    /** Returns number of children for the property. */
    inline size_t GetChildrenCount ( wxPGId id ) const
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return ((wxPGProperty*)id)->GetChildCount();
    }

    /** Returns number of children for the property. */
    inline size_t GetChildrenCount ( wxPGNameStr name ) { return GetChildrenCount( GetPropertyByName(name) ); }

    /** Returns id of first item (in target page), whether it is a category or a property. */
    inline wxPGId GetFirst () const
    {
        return m_targetState->GetFirst();
    }

    /** Returns id of first category (in target page). */
    inline wxPGId GetFirstCategory () const
    {
        return m_targetState->GetFirstCategory ();
    }

    /** Returns id of first property that is not a category (from target page). */
    inline wxPGId GetFirstProperty() const
    {
        return m_targetState->GetFirstProperty();
    }

    /** Returns pointer to the contained wxPropertyGrid. This does not change
        after wxPropertyGridManager has been created, so you can safely obtain
        pointer once and use it for the entire lifetime of the instance.
    */
    inline wxPropertyGrid* GetGrid()
    {
        // FIXME
        //wxASSERT( m_iFlags & wxPG_FL_INITIALIZED );
        return &m_propGrid;
    };

    /** Returns id of last child of given property.
        \remarks
        Returns even sub-properties.
    */
    inline wxPGId GetLastChild ( wxPGId parent )
    {
        wxASSERT ( wxPGIdIsOk(parent) );
        wxPGPropertyWithChildren& p = (wxPGPropertyWithChildren&)parent.GetProperty();
        if ( !p.GetParentingType() || !p.GetCount() ) return wxPGId();
        return wxPGId(p.Last());
    }
    inline wxPGId GetLastChild ( wxPGNameStr name ) { return GetLastChild ( GetPropertyByName(name) ); }

    /** Returns id of next category after a given property (which does not have to be category). */
    inline wxPGId GetNextCategory ( wxPGId id ) const
    {
        wxASSERT ( m_targetState );
        return m_targetState->GetNextCategory(id);
    }

    /** Returns id of next property (from target page). This does <b>not</b> iterate
        to sub-properties or categories.
    */
    inline wxPGId GetNextProperty ( wxPGId id ) const
    {
        wxASSERT ( m_targetState );
        return m_targetState->GetNextProperty(id);
    }

    /** Returns index for a page name. If no match is found, wxNOT_FOUND is returned. */
    int GetPageByName ( const wxChar* name ) const;

    /** Returns index for a relevant propertygrid state. If no match is found,
        wxNOT_FOUND is returned.
    */
    int GetPageByState( wxPropertyGridState* pstate ) const;

    /** Returns number of managed pages. */
    size_t GetPageCount () const { return m_arrPages.GetCount(); }

    /** Returns name of given page. */
    const wxString& GetPageName ( int index ) const;

    /** Returns id of previous property (in target page). */
    inline wxPGId GetPrevProperty ( wxPGId id ) const
    {
        return m_targetState->GetPrevProperty(id);
    }

    /** Returns id of previous item under the same parent. */
    inline wxPGId GetPrevSibling ( wxPGId id )
    {
        return wxPropertyGridState::GetPrevSibling(id);
    }
    inline wxPGId GetPrevSibling ( wxPGNameStr name )
    {
        return wxPropertyGridState::GetPrevSibling(GetPropertyByName(name));
    }

    /** Returns id of property with given label (case-sensitive). If there is no
        property with such label, returned property id is invalid ( i.e. it will return
        FALSE with IsOk method). If there are multiple properties with identical name,
        most recent added is returned.
    */
    wxPGId GetPropertyByLabel ( const wxString& name,
        wxPropertyGridState** ppState = (wxPropertyGridState**)NULL ) const;

    /** Returns id of property's nearest parent category. If no category
        found, returns invalid wxPGId.
    */
    inline wxPGId GetPropertyCategory ( wxPGId id ) const
    {
        return m_propGrid.GetPropertyCategory(id);
    }
    wxPGId GetPropertyCategory ( wxPGNameStr name ) const
    {
        return m_propGrid.GetPropertyCategory(GetPropertyByName(name));
    }

    /** Returns cell background colour of a property. */
    inline wxColour GetPropertyColour ( wxPGId id ) const
    {
        return m_propGrid.GetPropertyColour(id);
    }
    inline wxColour GetPropertyColour ( wxPGNameStr name ) const
    {
        return m_propGrid.GetPropertyColour(GetPropertyByName(name));
    }

    /** Returns help string associated with a property. */
    inline const wxString& GetPropertyHelpString ( wxPGId id ) const
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetHelpString();
    }

    /** Returns help string associated with a property. */
    inline const wxString& GetPropertyHelpString ( wxPGNameStr name ) const
    {
        return GetPropertyHelpString(GetPropertyByNameWithAssert(name));
    }

#if wxPG_USE_VALIDATORS
    /** Returns validator of a property as a reference, which you
        can pass to any number of SetPropertyValidator.
    */
    inline wxPropertyValidator& GetPropertyValidator ( wxPGId id )
    {
        wxASSERT( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetValidator();
    }
    inline wxPropertyValidator& GetPropertyValidator ( wxPGNameStr name )
    {
        return GetPropertyValidator(GetPropertyByName(name));
    }
#endif

    /** Returns a wxVariant list containing wxVariant versions of all
        property values. Order is not guaranteed, but generally it should
        match the visible order in the grid.
        \param flags
        Use wxKEEP_STRUCTURE to retain category structure; each sub
        category will be its own wxList of wxVariant.
        \remarks
        This works on the target page.
    */
    wxVariant GetPropertyValues ( const wxString& listname = wxEmptyString,
        wxPGId baseparent = wxPGId(), long flags = 0 ) const
    {
        return m_targetState->GetPropertyValues(listname,baseparent,flags);
    }

    /** Returns "root property" of the target page. It does not have name, etc.
        and it is not visible. It is only useful for accessing its children.
    */
    wxPGId GetRoot () const { return wxPGIdGen(m_targetState->m_properties); }

    /** Returns index to currently selected page. */
    inline int GetSelectedPage () const { return m_selPage; }

    /** Shortcut for GetGrid()->GetSelection(). */
    inline wxPGId GetSelectedProperty () const
    {
        return m_propGrid.GetSelection();
    }

    /** Synonyme for GetSelectedPage. */
    inline int GetSelection () const { return m_selPage; }

    /** Returns a pointer to the toolbar currently associated with the
        wxPropertyGridManager (if any). */
    inline wxToolBar* GetToolBar() const { return m_pToolbar; }

    inline void InitAllTypeHandlers ()
    {
        wxPropertyGrid::InitAllTypeHandlers();
    }

    /** See wxPropertyGrid::Insert. */
    inline wxPGId Insert ( wxPGId id, int index, wxPGProperty* property )
    {
        wxASSERT ( m_targetState );
        wxPGId res_id = m_targetState->Insert((wxPGPropertyWithChildren*)wxPGIdToPtr(id),index,property);
        if ( m_targetState == m_propGrid.m_pState )
            m_propGrid.DrawItems ( property, (wxPGProperty*) NULL );
        return res_id;
    }

    /** See wxPropertyGrid::Insert. */
    inline wxPGId Insert ( wxPGNameStr name, int index, wxPGProperty* property )
    {
        wxASSERT ( m_targetState );
        wxPGId res_id = m_targetState->Insert((wxPGPropertyWithChildren*)wxPGIdToPtr(m_targetState->BaseGetPropertyByName(name)),index,property);
        if ( m_targetState == m_propGrid.m_pState )
            m_propGrid.DrawItems ( property, (wxPGProperty*) NULL );
        return res_id;
    }

    /** Creates new property page. Note that the first page is not created
        automatically, and before you have created any pages you are basicly
        in a non-paged mode. When you create the first page, contents
        of the underlying propertygrid is added into it (or this is how it
        will seem to to the user).
        \param index
        Add to this position. -1 will add as the last item.
        \param label
        A label for the page. This may be shown as a toolbar tooltip etc.
        \param bmp
        Bitmap image for toolbar. If wxNullBitmap is used, then a built-in
        default image is used.
        \retval
        Returns index to the page created.
    */
    int InsertPage ( int index, const wxString& label, const wxBitmap& bmp = wxNullBitmap );

    /** Returns TRUE if any property on any page has been modified by the user. */
    bool IsAnyModified () const;

    /** Returns TRUE if updating is frozen (ie. Freeze() called but not yet Thaw() ). */
    inline bool IsFrozen () const { return (m_propGrid.m_frozen>0)?TRUE:FALSE; }

    /** Returns TRUE if any property on given page has been modified by the user. */
    bool IsPageModified ( size_t index ) const;

    /** Returns true if property is a category. */
    inline bool IsPropertyCategory ( wxPGId id ) const
    {
        return (wxPGIdToPtr(id)->GetParentingType()>0)?TRUE:FALSE;
    }
    inline bool IsPropertyCategory ( wxPGNameStr name ) { return IsPropertyCategory(GetPropertyByName(name)); }

    /** Disables item's textctrl if other way of editing is available. */
    void LimitPropertyEditing ( wxPGId id, bool limit = TRUE );
    void LimitPropertyEditing ( wxPGNameStr name, bool limit = TRUE );

    virtual void Refresh (bool eraseBackground = true,
                          const wxRect* rect = (const wxRect*) NULL);

    /** Select and displays a given page. */
    void SelectPage ( int index );

    /** Select and displays a given page. */
    inline void SelectPage ( const wxChar* name )
    {
        SelectPage ( GetPageByName(name) );
    }

    /** Select a property. */
    void SelectProperty ( wxPGId id, bool focus = FALSE );
    inline void SelectProperty ( wxPGNameStr name, bool focus = FALSE )
    {
        SelectProperty(GetPropertyByName(name),focus);
    }

    /** Sets label and text in description box.
    */
    void SetDescription ( const wxString& label, const wxString& content );

    /** Sets the current category - Append will add non-categories under this one.
    */
    inline void SetCurrentCategory ( wxPGId id )
    {
        wxPropertyCategoryClass* pc = (wxPropertyCategoryClass*)wxPGIdToPtr(id);
#ifdef __WXDEBUG__
        if ( pc ) wxASSERT ( pc->GetParentingType() > 0 );
#endif
        m_targetState->m_currentCategory = pc;
    }

    /** Sets the current category - Append will add non-categories under this one.
    */
    inline void SetCurrentCategory ( wxPGNameStr name )
    {
        SetCurrentCategory(GetPropertyByName(name));
    }

    /** Sets y coordinate of the description box splitter. */
    void SetDescBoxHeight ( int ht, bool refresh = TRUE );

    /** All properties added/inserted will have given priority by default.
        \param
        priority can be wxPG_HIGH (default) or wxPG_LOW.
    */
    inline void SetDefaultPriority( int priority )
    {
        m_propGrid.SetDefaultPriority(priority);
    }

    /** Same as SetDefaultPriority(wxPG_HIGH). */
    inline void ResetDefaultPriority()
    {
        m_propGrid.ResetDefaultPriority();
    }

    /** Sets label of a property.
    */
    void SetPropertyLabel( wxPGId id, const wxString& newlabel );
    void SetPropertyLabel( wxPGNameStr name, const wxString& newlabel );

    /** Sets background colour of property and all its children.  Background brush
        cache is optimized for often set colours to be set last.
    */
    inline void SetPropertyColour ( wxPGId id, const wxColour& col )
    {
        m_propGrid.SetPropertyColour ( id, col );
    }
    inline void SetPropertyColour ( wxPGNameStr name, const wxColour& col )
    {
        m_propGrid.SetPropertyColour ( GetPropertyByName(name), col );
    }

    /** Sets background colour of property and all its children to the default. */
    inline void SetPropertyColourToDefault ( wxPGId id )
    {
        m_propGrid.SetColourIndex ( wxPGIdToPtr(id), 0 );
    }
    inline void SetPropertyColourToDefault ( wxPGNameStr name )
    {
        m_propGrid.SetColourIndex ( wxPGIdToPtr(GetPropertyByName(name)), 0 );
    }

    /** Property is be hidden/shown when hider button is toggled or
        when wxPropertyGridManager::Compact is called.
    */
    void SetPropertyPriority ( wxPGId id, int priority );
    void SetPropertyPriority ( wxPGNameStr name, int priority );

    void SetPropertyValue ( wxPGId id, long value );
    void SetPropertyValue ( wxPGId id, int value );
    void SetPropertyValue ( wxPGId id, double value );
    void SetPropertyValue ( wxPGId id, bool value );
    void SetPropertyValue ( wxPGId id, const wxString& value );
    void SetPropertyValue ( wxPGId id, const wxArrayString& value );
    void SetPropertyValue ( wxPGId id, wxObject* value );
    void SetPropertyValue ( wxPGId id, void* value );
    void SetPropertyValue ( wxPGId id, wxVariant& value );
    void SetPropertyValue ( wxPGNameStr name, long value );
    void SetPropertyValue ( wxPGNameStr name, int value );
    void SetPropertyValue ( wxPGNameStr name, double value );
    void SetPropertyValue ( wxPGNameStr name, bool value );
    void SetPropertyValue ( wxPGNameStr name, const wxString& value );
    void SetPropertyValue ( wxPGNameStr name, const wxArrayString& value );
    void SetPropertyValue ( wxPGNameStr name, wxObject* value );
    void SetPropertyValue ( wxPGNameStr name, void* value );
    void SetPropertyValue ( wxPGNameStr name, wxVariant& value );
    /*wxPG_IMPLEMENT_PGMAN_METHOD_NORET1*/void SetPropertyValue(const wxPoint&);
    /*wxPG_IMPLEMENT_PGMAN_METHOD_NORET1*/void SetPropertyValue(const wxSize&);
    /*wxPG_IMPLEMENT_PGMAN_METHOD_NORET1*/void SetPropertyValue(const wxArrayInt&);

    inline void SetPropertyValue ( wxPGId id, wxObject& value )
    {
        SetPropertyValue(id,&value);
    }

    inline void SetPropertyValue ( wxPGNameStr name, wxObject& value )
    {
        SetPropertyValue(name,&value);
    }

    inline void SetPropertyValue ( wxPGId id, const wxChar* value )
    {
        SetPropertyValue(id,wxString(value));
    }
    inline void SetPropertyValue ( wxPGNameStr name, const wxChar* value )
    {
        SetPropertyValue(name,wxString(value));
    }

    /** Sets various property values from a list of wxVariants. If property with
        name is missing from the grid, new property is created under given default
        category (or root if omitted). Works on target page.
    */
    void SetPropertyValues ( const wxList& list, wxPGId default_category )
    {
        m_targetState->SetPropertyValues(list,default_category);
    }

    inline void SetPropertyValues ( const wxVariant& list, wxPGId default_category )
    {
        SetPropertyValues (list.GetList(),default_category);
    }
    inline void SetPropertyValues ( const wxList& list, const wxString& default_category = wxEmptyString )
    {
        SetPropertyValues (list,GetPropertyByName(default_category));
    }
    inline void SetPropertyValues ( const wxVariant& list, const wxString& default_category = wxEmptyString )
    {
        SetPropertyValues (list.GetList(),GetPropertyByName(default_category));
    }

    /** Sets property's value to unspecified. If it has children (it may be category),
        then the same thing is done to them.
    */
    void SetPropertyValueUnspecified ( wxPGId id );
    void SetPropertyValueUnspecified ( wxPGNameStr name );

    /** Synonyme for SelectPage(index). */
    //inline void SetSelection ( int index ) { SelectPage(index); }

    /** Synonyme for SelectPage(name). */
    inline void SetStringSelection ( const wxChar* name )
    {
        SelectPage ( GetPageByName(name) );
    }

    /** Sets page to which append, insert, etc. will add items.
        Every time a page is changed, target page is automatically
        switched to that.
    */
    void SetTargetPage ( int index );

    /** Sets page to which append, insert, etc. will add items.
        Every time a page is changed, target page is automatically
        switched to that.
    */
    inline void SetTargetPage ( const wxChar* name )
    {
        SetTargetPage ( GetPageByName(name) );
    }

    /** Sorts all items at all levels of the target page (except sub-properties). */
    inline void Sort ()
    {
        m_propGrid.Sort(wxPGIdGen(m_targetState->m_properties));
    }

    /** Sorts children of a category.
    */
    inline void Sort ( wxPGId id )
    {
        m_propGrid.Sort(id);
    }

    /** Sorts children of a category.
    */
    inline void Sort ( wxPGNameStr name )
    {
        m_propGrid.Sort ( GetPropertyByName(name) );
    }

    /** Toggles priority of a property between wxPG_HIGH and wxPG_LOW.
    */
    inline void TogglePropertyPriority ( wxPGId id )
    {
        int priority = wxPG_LOW;
        if ( GetPropertyPriority(id) == wxPG_LOW )
            priority = wxPG_HIGH;
        SetPropertyPriority(id,priority);
    }

    /** Toggles priority of a property between wxPG_HIGH and wxPG_LOW.
    */
    inline void TogglePropertyPriority ( wxPGNameStr name )
    {
        TogglePropertyPriority(GetPropertyByName(name));
    }

    /** Deselect current selection, if any (from current page). */
    inline void ClearSelection ()
    {
        m_propGrid.ClearSelection ();
    }

#ifdef _WX_WINDOW_H_BASE_

    //
    // Overridden functions - no documentation required.
    //

    virtual wxSize DoGetBestSize() const;
    void SetId( wxWindowID winid );

    virtual void Freeze();
    virtual void Thaw();
    virtual void SetExtraStyle ( long exStyle );
    virtual bool SetFont ( const wxFont& font );
    virtual void SetWindowStyleFlag ( long style );

    //
    // Event handlers
    //
    void OnMouseMove ( wxMouseEvent &event );
    void OnMouseClick ( wxMouseEvent &event );
    void OnMouseUp ( wxMouseEvent &event );
    void OnMouseEntry ( wxMouseEvent &event );

    void OnPaint ( wxPaintEvent &event );

    void OnToolbarClick ( wxCommandEvent &event );
    void OnResize ( wxSizeEvent& event );
    void OnCompactorClick ( wxCommandEvent& event );
    void OnPropertyGridSelect ( wxPropertyGridEvent& event );

protected:

    wxPropertyGrid  m_propGrid;

    wxArrayPtrVoid  m_arrPages;

#if wxUSE_TOOLBAR
    wxToolBar*      m_pToolbar;
    //wxBitmap*       m_pBmpCatMode;
    //wxBitmap*       m_pBmpNonCatMode;
#endif
    wxStaticText*   m_pTxtHelpCaption;
    wxStaticText*   m_pTxtHelpContent;
    wxButton*       m_pButCompactor;

    //wxWindow*       m_splitterPrevDrawnWin;

    wxPropertyGridState*    m_targetState;

    long            m_iFlags;

    // Selected page index.
    int             m_selPage;

    int             m_width;

    int             m_height;

    int             m_extraHeight;

    int             m_splitterY;

    int             m_splitterHeight;

    int             m_nextTbInd;

    int             m_dragOffset;

    wxCursor        m_cursorSizeNS;

    int             m_nextDescBoxSize;

    unsigned char   m_dragStatus;

    unsigned char   m_onSplitter;

    bool EnsureVisible ( wxPGId id, wxPropertyGridState* parent_state );

    virtual wxPGId DoGetPropertyByName ( wxPGNameStr name ) const;

    wxPGId GetPropertyByName2 ( wxPGNameStr name, wxPropertyGridState** ppState = (wxPropertyGridState**) NULL ) const;

    // Sets some members to defaults.
	void Init1();

    // Initializes some members.
	void Init2( int style );

    /** Recalculates new positions for components, according to the
        given size.
    */
    void RecalculatePositions ( int width, int height );

    /** (Re)creates/destroys controls, according to the window style bits. */
    void RecreateControls ();

    void RefreshHelpBox ( int new_splittery, int new_width, int new_height );

    void RepaintSplitter ( int new_splittery, int new_width, int new_height, bool desc_too );

    void SetDescribedProperty ( wxPGProperty* p );

    inline wxPropertyGridPageData* GetPage ( size_t ind ) const
    {
        return (wxPropertyGridPageData*)m_arrPages.Item(ind);
    }

#endif

private:
    DECLARE_EVENT_TABLE()
};

// -----------------------------------------------------------------------

#ifndef __wxPG_SOURCE_FILE__
# undef wxPG_IPAM_DECL
# undef wxPG_IMPLEMENT_PGMAN_METHOD_NORET1
#endif

// -----------------------------------------------------------------------

#endif // _WX_PROPGRID_MANAGER_H_
