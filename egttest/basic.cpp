#include <egt/ui>
#include <iostream>
using namespace egt;
using namespace std;

int main(int argc, const char** argv)
{
	cout << "entering";
  Application app;
  TopWindow window;
  Button button(window, "Press Me");
  center(button);
  window.show();
  cout << "exit";

  return app.run();
}



