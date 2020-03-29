#include <SmartWin.h>
using namespace SmartWin;

class MyWidget
    : public WidgetFactory<WidgetWindow, MyWidget>
{
public:
    void clicked(WidgetButton * btn)
    {
        createMessageBox().show("Hello World Was Here");
    }

    void init()
    {
        createWindow();
        setText("Hello World");
        WidgetButton * btn = createButton();
        btn->setText("My Button");
        btn->setBounds(100, 100, 100, 100);
        setBounds(100, 100, 400, 400);
        btn->onClicked(&MyWidget::clicked);
    }
};

int SmartWinMain(Application & app)
{
    MyWidget * widget = new MyWidget();
    widget->init();
    return app.run();
}
