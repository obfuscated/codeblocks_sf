#include <lf/Lightfeather.h>

class CGame : public lf::input::IKeyListener
{
    public:
        CGame(lf::render::IRenderWindow* win);
        ~CGame();

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
