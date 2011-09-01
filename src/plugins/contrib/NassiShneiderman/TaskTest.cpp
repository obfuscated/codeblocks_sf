
#include "TaskTest.h"

#include "NassiView.h"


TaskTest::TaskTest(NassiView *view)
    :Task(),
    m_view(view)
{

}
TaskTest::~TaskTest(){}

void TaskTest::Start(){}
HooverDrawlet *TaskTest::OnMouseLeftUp(wxMouseEvent &event, const wxPoint &position){return NULL;}
HooverDrawlet *TaskTest::OnMouseLeftDown(wxMouseEvent &event, const wxPoint &position){return NULL;}
HooverDrawlet *TaskTest::OnMouseRightDown(wxMouseEvent &event, const wxPoint &position){return NULL;}
HooverDrawlet *TaskTest::OnMouseRightUp(wxMouseEvent& event, const wxPoint &position){return NULL;}
HooverDrawlet *TaskTest::OnMouseMove(wxMouseEvent &event, const wxPoint &position){return NULL;}
HooverDrawlet *TaskTest::OnKeyDown(wxKeyEvent &event, const wxPoint &position){return NULL;}
HooverDrawlet *TaskTest::OnChar(wxKeyEvent &event, const wxPoint &position){return NULL;}


