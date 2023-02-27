#include <string>
#include <ctime> //For time management purposes
#include <memory>
#include <egt/ui>// Load EGT
#include <egt/uiloader.h> //Load the UI Loader from EGT
//#include "time_management.h" //Load a file created to manage date and time.

//Optionnal : Red LED Blinkig when reaching 100% of batterry charge
#include <iostream> //Two includes to manipulate folders.
#include <fstream>

int main(int argc, char** argv)
{

 egt::Application app(argc, argv);
 egt::experimental::UiLoader loader;
 //egt::TopWindow Window;
 auto window = std::static_pointer_cast<egt::Window>(loader.load("file:ui.xml"));
auto main_screen=window->find_child<egt::Window>("mainscreen");
auto startup_screen=window->find_child<egt::Window>("Window9");
auto power_off_button=window->find_child<egt::Button>("Button16");
//power_off_button->fill_flags().clear();


power_off_button->on_click([&main_screen,&startup_screen] (egt::Event &)
{
	main_screen->hide();
startup_screen->show();

});

window->on_event([&main_screen,&startup_screen] (egt::Event & event)
	{
  	 switch (event.key().keycode)//Is the event happening a keypress ?
		{
			case egt::EKEY_1: //Is the key pressed the "KEY_0" corresponding to the K1 button. (See the training for more explainations)
			{
				main_screen->show();
				startup_screen->hide();
				break;
			}

			default:
				break;
		}
		}, {egt::EventId::keyboard_up});//Consider the event only when the finger has moved up from the K1 button



 return app.run();

}
