/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CLASSBROWSERBUILDERTHREAD_H
#define CLASSBROWSERBUILDERTHREAD_H

#include <wx/thread.h>
#include <wx/treectrl.h>

#include "cctreectrl.h"
#include "nativeparser.h"
#include "parser/token.h"
#include "parser/parser.h"

#define CRC32_CCITT       0x04C11DB7

class Crc32
{
public:
    Crc32()
    {
        Reset();
    }

    void Reset()
    {
        m_value = 0;
    }

    void Update(uint8_t Value)
    {
        m_value ^= uint32_t(Value) << 24;
        for (int Bit = 0; Bit < 8; ++Bit)
            m_value = (m_value & (uint32_t(1) << 31)) ? ((m_value << 1) ^ CRC32_CCITT) : (m_value << 1);
    }

    void Update(const void* Buffer, size_t Length)
    {
      const uint8_t *p = static_cast <const uint8_t *> (Buffer);

      for (size_t n = 0; n < Length; ++n, ++p)
          Update(*p);
    }

    uint32_t GetCrc() const
    {
        return m_value;
    }

private:
    uint32_t m_value;
};

enum EThreadJob
{
    JobBuildTree,
    JobSelectTree,
    JobExpandItem
};

class CCTree;
class CCTreeItem;

// A non-GUI tree element

class CCTreeItem
{
public:
    CCTreeItem(CCTreeItem* parent, const wxString& text, int image = -1, int selImage = -1, CCTreeCtrlData* data = nullptr);
    virtual ~CCTreeItem();
    void DeleteChildren() {while (m_firstChild) delete m_firstChild; m_hasChildren = false;}
    static void Swap(CCTreeItem* a, CCTreeItem* b);

    CCTreeItem* m_parent;
    CCTreeItem* m_prevSibling;
    CCTreeItem* m_nextSibling;
    CCTreeItem* m_firstChild;
    wxString m_text;
    CCTreeCtrlData* m_data;
    bool m_bold;
    bool m_hasChildren;
    wxColour m_colour;
    int m_image[wxTreeItemIcon::wxTreeItemIcon_Max];
    wxSemaphore m_semaphore;
};

// Opaque class used by CCTree::GetFirstChild() and CCTree::GetNextChild()

class CCCookie
{
public:
    CCCookie() : m_current(nullptr) {}
    CCTreeItem* GetCurrent() const {return m_current;}
    void SetCurrent(CCTreeItem* Node) {m_current = Node;}

private:
    CCTreeItem* m_current;
};

// A tree using the non-GUI part of wxTreeCtrl interface

class CCTree
{
public:
    CCTree() : m_root(nullptr), m_compare(bstNone) {}
    virtual ~CCTree() {DeleteAllItems();}

    CCTreeItem*     AddRoot(const wxString& text, int image = -1, int selImage = -1, CCTreeCtrlData* data = nullptr);
    CCTreeItem*     AppendItem(CCTreeItem* parent, const wxString& text, int image = -1, int selImage = -1, CCTreeCtrlData* data = nullptr);
    void            Delete(CCTreeItem* item) {if (item) {delete item; if (item == m_root) m_root = nullptr;}}
    void            DeleteAllItems() {Delete(m_root);}
    void            DeleteChildren(CCTreeItem* item) {if (item) item->DeleteChildren();}
    size_t          GetChildrenCount(CCTreeItem* item, bool recursively = true) const;
    size_t          GetCount() const {return m_root ? 1+GetChildrenCount(m_root) : 0;}
    CCTreeItem*     GetFirstChild(CCTreeItem* item, CCCookie& cookie) const;
    CCTreeCtrlData* GetItemData(CCTreeItem* item) const {return item ? item->m_data : nullptr;}
    int             GetItemImage(CCTreeItem* item, wxTreeItemIcon which = wxTreeItemIcon_Normal) const  {return item ? item->m_image[which] : -1;}
    CCTreeItem*     GetItemParent(CCTreeItem* item) const {return item ? item->m_parent : nullptr;}
    wxString        GetItemText(CCTreeItem* item) const {return item ? item->m_text : wxString();}
    wxColour        GetItemTextColour(CCTreeItem* item) const {return item ? item->m_colour : wxNullColour;}
    CCTreeItem*     GetLastChild(CCTreeItem* item) const;
    CCTreeItem*     GetNextChild(CCTreeItem* item, CCCookie& cookie) const;
    CCTreeItem*     GetNextSibling(CCTreeItem* item) const {return item ? item->m_nextSibling : nullptr;}
    CCTreeItem*     GetPrevSibling(CCTreeItem* item) const {return item ? item->m_prevSibling : nullptr;}
    CCTreeItem*     GetRootItem() const {return m_root;}
    CCTreeItem*     InsertItem(CCTreeItem* parent, CCTreeItem* idPrevious, const wxString& text, int image = -1, int selImage = -1, CCTreeCtrlData* data = nullptr);  // after
    CCTreeItem*     InsertItem(CCTreeItem* parent, size_t pos, const wxString& text, int image = -1, int selImage = -1, CCTreeCtrlData* data = nullptr);              // before
    bool            IsBold(CCTreeItem* item) const {return item ? item->m_bold : false;}
    bool            IsEmpty() const {return m_root == nullptr;}
    bool            ItemHasChildren(CCTreeItem* item) const {return item ? (item->m_firstChild != nullptr) : false;}
    CCTreeItem*     PrependItem(CCTreeItem* parent, const wxString& text, int image = -1, int selImage = -1, CCTreeCtrlData* data = nullptr);
    void            SetCompareFunction(BrowserSortType type) {m_compare = type;}
    void            SetItemBold(CCTreeItem* item, bool bold = true) {if (item) item->m_bold = bold;}
    void            SetItemData(CCTreeItem* item, CCTreeCtrlData* data) {if (item) item->m_data = data;}
    void            SetItemHasChildren(CCTreeItem* item, bool has = true) {if (item) item->m_hasChildren = has;}
    void            SetItemImage(CCTreeItem* item, int image, wxTreeItemIcon which = wxTreeItemIcon_Normal) {if (item) item->m_image[which] = image;}
    void            SetItemText(CCTreeItem* item, const wxString& text) {if (item) item->m_text = text;}
    void            SetItemTextColour(CCTreeItem* item, const wxColour& col) {if (item) item->m_colour = col;}
    void            SortChildren(CCTreeItem* parent) {CCCookie cookie; QuickSort(GetFirstChild(parent, cookie), GetLastChild(parent));}

    // Calculate CRC32 of the tree to detect changes
    uint32_t        GetCrc32() const;

protected:
    void            CalculateCrc32(CCTreeItem* parent, Crc32 &crc) const;

private:
    int             AlphabetCompare(const CCTreeCtrlData* lhs, const CCTreeCtrlData* rhs) const;
    int             CompareFunction(const CCTreeCtrlData* lhs, const CCTreeCtrlData* rhs) const;
    CCTreeItem*     DoInsertAfter(CCTreeItem* parent, CCTreeItem* hInsertAfter, const wxString& text, int image = -1, int selectedImage = -1, CCTreeCtrlData* data = nullptr);
    CCTreeItem*     DoInsertItem(CCTreeItem* parent, size_t index, const wxString& text, int image = -1, int selectedImage = -1, CCTreeCtrlData* data = nullptr);
    int             KindCompare(const CCTreeCtrlData* lhs, const CCTreeCtrlData* rhs) const;
    bool            LessThan(const CCTreeItem* lhs, const CCTreeItem* rhs) const {return CompareFunction(lhs->m_data, rhs->m_data) < 0;}
    void            QuickSort(CCTreeItem* first, CCTreeItem* last);

    CCTreeItem*     m_root;
    BrowserSortType m_compare;
};

/** worker thread to build the symbol browser tree controls(both the top tree and the bottom tree)
 *  When the thread is started, it is waiting for the semaphore, and once the GUI post the semaphore
 *  the builder will do the dirty job, once finished, it will wait again.
 */
class ClassBrowserBuilderThread : public wxThread
{
public:
    /** constructor
     * @param evtHandler parent window notification events will sent to
     * @param sem a semaphore reference which is used synchronize the GUI and the builder thread
     */
    ClassBrowserBuilderThread(wxEvtHandler* evtHandler, wxSemaphore& sem);

    /** destructor */
    ~ClassBrowserBuilderThread() override;

    // Called from external
    void Init(NativeParser* np, const wxString& active_filename, void* user_data /*active project*/,
              const BrowserOptions& bo, TokenTree* tt, int idThreadEvent);

    /** Populates the bottom tree with info from m_targetItem */
    void SelectGUIItem();

    void ExpandGUIItem();

    /** Ask the worker thread to die
     *  Called from external: when the class browser window get destroyed
     */
    void RequestTermination(bool terminate = true) {m_TerminationRequested = terminate;}

    /** Select what should do the worker thread when awaked
     *  Called before posting the semaphore
     * @param job What the thread should do when the semaphore is released
     * @param itemId Identifier of the item (if applicable)
     */
    void SetNextJob(EThreadJob job, CCTreeItem *item = nullptr) {m_nextJob = job; m_targetItem = item;}

    /** Check if the thread is busy
     * @return @a true if busy
     */
    bool IsBusy() const {return m_Busy;}

protected:
     void* Entry() override;

    /** Creates the tree
     * @note Called from Entry()
     */
    void BuildTree();

    /** Construct the children of the tree item */
    void ExpandItem(CCTreeItem* item);

    /** Copy the internal (top or bottom) tree to the GUI tree
     * @param top \a true to copy the top tree
     * @note Called from the worker thread
     */
    void FillGUITree(bool top);

    // Called from FillGuiTree() and ExpandGUIItem()
    void AddItemChildrenToGuiTree(CCTree* localTree, CCTreeItem* parent, bool recursive) const;

    /** Remove any nodes no longer valid (due to update)
     *
     * Recursively enters all existing nodes and deletes the node if the token it references is invalid
     * @param tree the symbol tree
     * @param parent the node Id
     * @note Called from BuildTree()
     */
    void RemoveInvalidNodes(CCTree* tree, CCTreeItem* parent);

    /** Recursively construct the children of node's children, which matches tokenKind
     * @param node Desired node
     * @param tokenKind Kind of the token
     * @param level The recursive level
     * @note Called from BuildTree()
     */
    void ExpandNamespaces(CCTreeItem* node, TokenKind tokenKind, int level);

    // Called from ExpandItem():
    bool CreateSpecialFolders(CCTree* tree, CCTreeItem* parent);

    // Called from CreateSpecialFolders():
    CCTreeItem* AddNodeIfNotThere(CCTree* tree, CCTreeItem* parent, const wxString& name,
                                  int imgIndex = -1, CCTreeCtrlData* data = nullptr);

    /** Add the child nodes of the specified token
     * @param tree the symbol tree control
     * @param parent the specified node
     * @param parentTokenIdx the Token index associated with the node
     * @param tokenKindMask
     * @param tokenScopeMask
     * Called from ExpandItem()
     */
    bool AddChildrenOf(CCTree* tree, CCTreeItem* parent, int parentTokenIdx,
                       short int tokenKindMask = 0xffff, int tokenScopeMask = 0);
    bool AddAncestorsOf(CCTree* tree, CCTreeItem* parent, int tokenIdx);
    bool AddDescendantsOf(CCTree* tree, CCTreeItem* parent, int tokenIdx, bool allowInheritance = true);
    // Called from ExpandItem(), SelectItem():
    void AddMembersOf(CCTree* tree, CCTreeItem* node);

    // Called from AddChildrenOf(), AddAncestorsOf(), AddDescendantsOf():
    bool AddNodes(CCTree* tree, CCTreeItem* parent, const TokenIdxSet* tokens,
                  short int tokenKindMask = 0xffff, int tokenScopeMask = 0, bool allowGlobals = false);

    // Called from BuildTree()
    void SaveExpandedItems(CCTree* tree, CCTreeItem* parent, int level);
    void ExpandSavedItems(CCTree* tree, CCTreeItem* parent, int level);

    /** if the token should be shown in the tree, it will return true
     *
     * The view option of the symbol browser determines which tokens should be shown in the tree
     * Called from RemoveInvalidNodes(), AddNodes(), CreateSpecialFolder()
     */
    bool TokenMatchesFilter(const Token* token, bool locked = false) const;

    // Called from AddNodes():
    bool TokenContainsChildrenOfKind(const Token* token, int kind) const;

private:
    wxEvtHandler*    m_Parent;
    wxSemaphore&     m_ClassBrowserSemaphore;

    /** Some member functions of ClassBrowserBuilderThread such as ExpandItem() can either be called
     * from the main GUI thread(in ClassBrowser::OnTreeItemExpanding(wxTreeEvent& event)), or be
     * called in the worker thread(in BuildTree() which is called in ClassBrowserBuilderThread::Entry()),
     * to protect the member variables of the class(especially the wxTreeCtrl, we use the Mutex so
     * that only one thread can access to those member variables.
     */
    wxMutex          m_ClassBrowserBuilderThreadMutex;
    NativeParser*    m_NativeParser;

    /** pointers to the CCTree */
    CCTree*          m_CCTreeTop;
    CCTree*          m_CCTreeBottom;

    wxString         m_ActiveFilename;
    void*            m_UserData; // active project
    BrowserOptions   m_BrowserOptions;
    TokenTree*       m_TokenTree;

    // pair of current-file-filter
    /** A file set which contains a header file and the associated implementation file
     *
     * If the view option "Current file's symbols" is selected, the symbol tree will show tokens
     * from those files, e.g. if the a.cpp shown in the current active editor, then m_CurrentFileSet
     * maybe contains two files: a.cpp and a.h
     */
    TokenFileSet     m_CurrentFileSet;

    /** Tokens belong to the m_CurrentFileSet file set */
    TokenIdxSet      m_CurrentTokenSet;

    /** Special global scope tokens belong to the m_CurrentFileSet file set  */
    TokenIdxSet      m_CurrentGlobalTokensSet;

    ExpandedItemVect m_ExpandedVect;
    bool             m_InitDone;
    bool             m_Busy;

    /** if this variable is true, the Entry() function should return */
    bool             m_TerminationRequested;
    int              m_idThreadEvent;

    /** Selects worker thread's next job */
    EThreadJob       m_nextJob;
    CCTreeItem*      m_targetItem;

    /** Previous tree CRC32 */
    uint32_t         m_topCrc32;
    uint32_t         m_bottomCrc32;
};

#endif // CLASSBROWSERBUILDERTHREAD_H
