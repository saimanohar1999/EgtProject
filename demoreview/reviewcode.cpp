
#include <egt/ui>
#include <iostream>
using namespace std;
using namespace egt;

int main(int argc, const char ** argv)
{
	Application app;
egt::TopWindow window;

  Button button(window, "swipe for expression");
  button.align(AlignFlag::center | AlignFlag::expand_horizontal);
  button.on_event([ & ](Event & event)


{
	  if (event.id() == EventId::pointer_drag)
{
cout << "clicked!" << endl;
egt::VideoWindow player(window.content_area(), "file:/root/videoplayback.mpg");
            window.add(player);
            player.volume(50);
            player.show();
            player.play();
            window.show();
            return app.run();
}
});

            window.show();
           return app.run();
}
