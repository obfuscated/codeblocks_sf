#include <lf/Lightfeather.h>

using namespace lf;

bool appQuit = false;

class MyKeyListener : public input::IKeyListener
{
public:
    MyKeyListener(){}
    
    void keyPressed(input::CKeyEvent& event)
    {
        if (event.getKey() == input::KEY_ESCAPE)
        {
            appQuit = true;
        }
    }
};

int main(int argc, char *argv[])
{
    // the engine
    CLFRoot* root = new CLFRoot();
    
    // the render window
    render::IRenderWindow* rwin;
    rwin = CLFRoot::getInstance().createRenderWindow(
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

    // create a 2D render layer
    render::CRenderLayer2D* layer = new render::CRenderLayer2D(core::recti(0,0,0,0));
    rwin->add(layer);
    layer->drop();
    
    while (!appQuit && root->update())
    {
        // main loop
    }

    // clean up
    delete root;
    
    return 0;
}

