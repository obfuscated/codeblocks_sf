#ifndef EDITPROPERTIES_H
#define EDITPROPERTIES_H


class ScbEditor;
//----------------------------------------------------------------------------
// EditProperties
//----------------------------------------------------------------------------
class EditProperties: public wxDialog {
//----------------------------------------------------------------------------

public:

    // constructor
    EditProperties (wxWindow* pParent, ScbEditor* pEditor, long style = 0);

private:
    void InitEditProperties (wxWindow* pParent, ScbEditor* pEditor, long style);

};

#endif //EDITPROPERTIES_H
