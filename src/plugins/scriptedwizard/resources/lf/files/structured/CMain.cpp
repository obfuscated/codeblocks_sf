#include "CMain.h"

using namespace lf;

CMain::CMain(render::IRenderWindow* win)
    : rwin(win),
    rl3d(win->getRenderLayer3D()),
    smgr(rl3d->getSceneManager()),
    cam(0),
    camController(0),
    quitNow(false)
{
    rwin->addKeyListener(this);
    setupScene();
}

CMain::~CMain()
{
    rl3d->remove(cam);
    smgr->remove(camController);
    cam->drop();
    camController->drop();

    smgr->getRootSceneNode()->removeAllChildren(true);

    rwin->removeKeyListener(this);
}

void CMain::run()
{
    quitNow = false;

    while (!quitNow && CLFRender::getInstance().update())
	{
		// main loop
    }
}

void CMain::setupScene()
{
    cam = new scene::C3DCamera(rwin,
                               core::PI / 3.0f,
                               1.0f,
                               1000.0f,
                               true);
    cam->setBackgroundColor(core::CColorI(255, 255, 240, 0));
    cam->replace(new render::CRenderStateLighting(false));
    cam->setPosition(core::vector3df(0.0f, 0.0f, -30.0f));
    cam->setTarget(core::vector3df(0.0f, 0.0f, 0.0f));

    rl3d->add(cam);
    smgr->addSceneNode(cam);

    // handle camera control
    camController = new scene::CFPSController(rwin, cam);
    smgr->add( camController );
}

void CMain::keyPressed(input::CKeyEvent& event)
{
    switch (event.getKey())
    {
        case input::KEY_ESCAPE:
            quitNow = true;
            break;
    }
}
