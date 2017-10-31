#include <lf/Lightfeather.h>
#include "CMain.h"

using namespace lf;

int main(int argc, char *argv[])
{
    // the engine
    initLF();
    
    // the render window
    render::IRenderWindow* rwin;
    rwin = CLFRender::getInstance().createRenderWindow(
               core::vector2di(0, 0),
               core::vector2di(640, 480),
               32,
               24,
               render::EWCF_AUTOCLOSE /* | render::EWCF_FULLSCREEN */,
               render::EAAF_NONE);
    rwin->setWindowCaption(L"Lightfeather 3D project");
    rwin->setVisible(true); 

    // the "Main" class
    CMain* mainClass = new CMain(rwin);

    mainClass->run();

    // clean-up
    delete mainClass;
    deinitLF();
    
    return 0;
}

