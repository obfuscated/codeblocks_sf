#include <qstring.h>
#include <qapplication.h>
#include <qpushbutton.h>

int main(int argc, char **argv)
{
    QApplication    app(argc, argv);
    QPushButton     quit("Hello World!");
    
    quit.resize(300, 40);
    quit.setFont(QFont("Arial", 18, QFont::Bold));
    QObject::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));
    quit.show();
    
    return app.exec();
}
