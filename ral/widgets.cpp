#include <egt/ui>
#include <iostream>
using namespace egt;
using namespace std;

int main(int argc, const char** argv)
{

  Application app;
  TopWindow window;
  Button button(window, "Press Me");
  center(button);
  window.show();


  return app.run();
}



