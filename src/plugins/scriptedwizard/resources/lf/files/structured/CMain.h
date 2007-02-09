#include <lf/Lightfeather.h>

class CMain : public lf::input::IKeyListener
{
    public:
        CMain(lf::render::IRenderWindow* win);
        ~CMain();

        void run();
        void setupScene();
        void keyPressed(lf::input::CKeyEvent& event);

    private:

		lf::render::IRenderWindow* rwin;
		lf::scene::CSceneManager* smgr;

        lf::scene::C3DCamera *cam;
        lf::scene::CSceneNodeController *camController;
        
        bool quitNow;
};
