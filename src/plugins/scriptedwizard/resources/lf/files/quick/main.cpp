#include <lf/Lightfeather.h>

using namespace lf;

render::IRenderWindow* rwin;

class MyKeyListener : public input::IKeyListener
{
public:
    MyKeyListener(){}
    
    void keyPressed(input::CKeyEvent& event)
    {
        if (event.getKey() == input::KEY_ESCAPE)
        {
            rwin->closeWindow();
        }
    }
};

int main(int argc, char *argv[])
{
    // the engine
    initLF();
    
    // the render window
    rwin = CLFRender::getInstance().createRenderWindow(
               core::vector2di(0, 0),
               core::vector2di(640, 480),
               32,
               24,
               render::EWCF_AUTOCLOSE /* | render::EWCF_FULLSCREEN */,
               render::EAAF_NONE);
    rwin->setWindowCaption(L"Lightfeather 3D project");
    rwin->setVisible(true);
    
    // listen to key events
    MyKeyListener listener;
    rwin->addKeyListener(&listener);

    while (CLFRender::getInstance().update())
    {
        // main loop
    }

    // clean up
    deinitLF();
    
    return 0;
}
