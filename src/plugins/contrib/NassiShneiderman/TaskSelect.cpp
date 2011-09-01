
#include "TaskSelect.h"

#include "TaskTest.h"
#include "NassiView.h"
#include "HooverLine.h"

#include "logger.h"


TaskSelect::TaskSelect(NassiView *view)
    :Task(),
    m_view(view)
{

}
TaskSelect::~TaskSelect(){}

void TaskSelect::Start()
{

}

HooverDrawlet *TaskSelect::OnMouseLeftDown(wxMouseEvent &event, const wxPoint &pos)
{

}
HooverDrawlet *TaskSelect::OnMouseLeftUp(wxMouseEvent &event, const wxPoint &position){return NULL;}
HooverDrawlet *TaskSelect::OnMouseRightDown(wxMouseEvent &event, const wxPoint &position)
{

}
HooverDrawlet *TaskSelect::OnMouseRightUp(wxMouseEvent& event, const wxPoint &position){return NULL;}
HooverDrawlet *TaskSelect::OnMouseMove(wxMouseEvent &event, const wxPoint &position){return NULL;}
HooverDrawlet *TaskSelect::OnKeyDown(wxKeyEvent &event, const wxPoint &position){return NULL;}
HooverDrawlet *TaskSelect::OnChar(wxKeyEvent &event, const wxPoint &position){return NULL;}
//return new HooverLine(wxPoint(0,0), position);


