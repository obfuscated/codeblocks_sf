#include "CGame.h"

using namespace lf;

CGame::CGame(render::IRenderWindow* win)
	: rwin(win),
	smgr(win->getSceneManager(0)),
	cam(0),
	camController(0),
	quitNow(false)
{
    rwin->addKeyListener(this);
    setupScene();
}

CGame::~CGame()
{
    smgr->remove(cam);
    smgr->remove(camController);
    cam->drop();
    camController->drop();

    smgr->getRootSceneNode()->removeAllChildren(true);

    rwin->removeKeyListener(this);
}

void CGame::run()
{
    quitNow = false;

    while (!quitNow)
    {
        if (!CLFRoot::getInstance().update())
            break;
	}
}

void CGame::setupScene()
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

	smgr->add(cam);
	smgr->addSceneNode(cam);

	// handle camera control
    camController = new scene::CFPSController(rwin, cam);
    smgr->add( camController );
}

void CGame::keyPressed(input::CKeyEvent& event)
{
    switch (event.getKey())
    {
        case input::KEY_ESCAPE:
            quitNow = true;
            break;
	}
}
