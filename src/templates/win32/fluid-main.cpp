#include "main_ui.h"

int main (int argc, char ** argv)
{
  Fl_Window *window;


  window = make_window();

  window->show (argc, argv);
  return(Fl::run());
}
