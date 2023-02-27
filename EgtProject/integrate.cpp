/*
 * swipe.cpp
 *
 *  Created on: 05-Jan-2023
 *      Author: i63349-lnx
 */
#include <string>
#include <ctime> //For time management purposes
#include <memory>
#include <egt/ui>// Load EGT
#include <egt/uiloader.h> //Load the UI Loader from EGT
//#include "time_management.h" //Load a file created to manage date and time.

//Optionnal : Red LED Blinkig when reaching 100% of batterry charge
#include <iostream> //Two includes to manipulate folders.
#include <fstream>

#include <stdio.h> // C standard libraries
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//#include <linux/gpio.h> // GPIO control not used
#include <sys/ioctl.h> // interface between user space and kernel in our case with I2C driver
#include <linux/i2c.h> // I2C interface
#include <linux/i2c-dev.h> // I2C interface

//#define DEV_GPIO "/dev/gpiochip0" //not used
#define DEV_I2C "/dev/i2c-1"
#define DEV_BLUELED "/sys/class/leds/blue/brightness"
#define DEV_GREENLED "/sys/class/leds/green/brightness"
#include<iostream>
using namespace std;



extern "C" {
#include <stdio.h> // C standard libraries
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//#include <linux/gpio.h> // GPIO control not used
#include <sys/ioctl.h> // interface between user space and kernel in our case with I2C driver
#include <linux/i2c.h> // I2C interface
#include <linux/i2c-dev.h> // I2C interface

//#define DEV_GPIO "/dev/gpiochip0" //not used
#define DEV_I2C "/dev/i2c-1"
#define DEV_BLUELED "/sys/class/leds/blue/brightness"
#define DEV_GREENLED "/sys/class/leds/green/brightness"

#define SLAVE_ADDR 0x4C  /* EMC1414 I2C slave address */
};

#include <egt/ui> // Libs for EGT
#include <iostream>
#include <string>
#include <fstream>

using namespace egt;
using namespace std;


extern "C" {

		int outfunction();
    };

char timersecflag =0;


class SwipeDetect
{
public:

    using SwipeCallback = std::function<void(const std::string& direction)>;

    SwipeDetect() = delete;

    explicit SwipeDetect(SwipeCallback callback)
        : m_callback(std::move(callback))
    {
    }

    void handle(egt::Event& event)
    {
        switch (event.id())
        {
        case egt::EventId::raw_pointer_down:
        {
            m_start = event.pointer().point;
            m_start_time = std::chrono::steady_clock::now();
            break;
        }
        case egt::EventId::raw_pointer_up:
        {
            const auto elapsed = std::chrono::steady_clock::now() - m_start_time;
            if (elapsed <= m_allowed_time)
            {
                const auto dist = m_start - event.pointer().point;

                if (std::abs(dist.x()) >= m_threshold && std::abs(dist.y()) <= m_restraint)
                    m_callback((dist.x() < 0) ? "left" : "right");
                else if (std::abs(dist.y()) >= m_threshold && std::abs(dist.x()) <= m_restraint)
                    m_callback((dist.y() < 0) ? "up" : "down");
            };

            break;
        }
        default:
            break;
        }
    }


    void threshold(int value)
    {
        m_threshold = value;
    }

    int threshold() const
    {
        return m_threshold;
    }

    void restraint(int value)
    {
        m_restraint = value;
    }

    int restraint() const
    {
        return m_restraint;
    }

    void allowed_time(const std::chrono::milliseconds& value)
    {
        m_allowed_time = value;
    }

    std::chrono::milliseconds allowed_time() const
    {
        return m_allowed_time;
    }

protected:

    /// Required min distance traveled.
    int m_threshold{150};
    /// Maximum distance allowed at the same time.
    int m_restraint{100};
    /// Maximum time allowed to travel.
    std::chrono::milliseconds m_allowed_time{300};

private:

    /// Starting point
    egt::DisplayPoint m_start;
    /// Start time of m_start
    std::chrono::time_point<std::chrono::steady_clock> m_start_time;
    /// Callback to invoke when finished.
    SwipeCallback m_callback;
};
int count1 {};
int count2 {};
int main(int argc, char** argv)
{
count1=0;
count2=0;
 egt::Application app(argc, argv);
 egt::experimental::UiLoader loader;
 //egt::TopWindow Window;
 auto window = std::static_pointer_cast<egt::Window>(loader.load("file:ui.xml"));
 //window.quit();
auto main_screen=window->find_child<egt::Window>("mainscreen");
auto startup_screen=window->find_child<egt::Window>("Window9");

auto power_off_button=window->find_child<egt::Button>("Button16");
auto third_screen=window->find_child<egt::Window>("Window14");
auto heart_button=window->find_child<egt::ImageButton>("ImageButton18");
auto temp_button=window->find_child<egt::ImageButton>("ImageButton19");
auto pet_button=window->find_child<egt::ImageLabel>("ImageLabel16");
auto Ai_button=window->find_child<egt::ImageLabel>("ImageLabel17");
//power_off_button->fill_flags().clear();
auto logo=window->find_child<egt::ImageLabel>("ImageLabel16");//Smart pointer pointing to the microchip logo on the screen_saver

	egt::PropertyAnimator animation;//Create a PropertyAnimator widget
	animation.starting(logo->x()); //Setting the starting point of the logo, which it's current x-position
	animation.ending(logo->x() + 615);//Setting the ending point of the logo, which is 615 pixels on the right
	animation.duration(std::chrono::seconds(4));//Duration of the animation
	animation.easing_func(egt::easing_linear);//The logo will move from left to right during 4 seconds, this easing functions describes the move. See document for more detailed explainations
	animation.on_change([&logo](egt::PropertyAnimator::Value value) { logo->x(value); });

	auto delay= new egt::AnimationDelay(std::chrono::milliseconds(500)); //Creation of a Delay object
	auto sequence =new egt::AnimationSequence(true);//Creation of a new animation sequence, this sequence will automatically loop the animation
	sequence->add(animation);//Add the previously created animation to the sequence
	sequence->add(*delay);//Add the delay to the sequence
	sequence->start();//Start the looping sequence


	power_off_button->on_click([&main_screen,&startup_screen, &third_screen] (egt::Event &)
	{
		main_screen->hide();
	startup_screen->hide();
	count2=1;

	});
heart_button->on_click([&main_screen,&startup_screen, &third_screen, &window] (egt::Event &)
{
	count2=1;
	//egt::Application::instance().quit();
	//egt::experimental::UiLoader loader;
	//loader.close();
	//main_screen->hide();
	//startup_screen->hide();
	//system("./bend");
	//auto window = std::static_pointer_cast<egt::Window>(loader.load("file:ui.xml"));
	//void.close();

	egt::TopWindow window;
		window.color(egt::Palette::ColorId::bg, egt::Color(0x1d2239ff));

		egt::VerticalBoxSizer sizer1;
		window.add(expand(sizer1));

		egt::HorizontalBoxSizer sizer0(egt::Justification::start);
		sizer0.color(egt::Palette::ColorId::bg, egt::Color(0x252c48ff));
		sizer0.color(egt::Palette::ColorId::border, egt::Palette::black);
		sizer0.border(2);
		sizer0.border_flags(egt::Theme::BorderFlag::bottom);
		sizer0.fill_flags(egt::Theme::FillFlag::blend);
		sizer1.add(expand_horizontal(sizer0));

		auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_white.png;128"));
		logo->margin(10);
		sizer0.add(logo);

		egt::Label label0("Heart beat");
		label0.color(egt::Palette::ColorId::label_text, egt::Palette::white);
		label0.padding(5);
		label0.font(egt::Font(28));
		sizer0.add(label0);

		egt::HorizontalBoxSizer sizer2;
		sizer1.add(expand(sizer2));

		egt::LineChart line1;
		line1.label("", "Value", "Graph");
		line1.font(egt::Font(24));
		line1.line_width(2);
		line1.grid_style(egt::LineChart::GridFlag::box_major_ticks_coord);
		line1.color(egt::Palette::ColorId::label_text, egt::Palette::ghostwhite);
		line1.color(egt::Palette::ColorId::button_bg, egt::Palette::red);
		line1.color(egt::Palette::ColorId::button_fg, egt::Color(0xd3d3d3));
		line1.margin(2);
		sizer2.add(expand(line1));

		egt::Label label3("000");
		label3.color(egt::Palette::ColorId::label_text, egt::Color(0xd3d3d3));
		label3.font(egt::Font(80));
		label3.margin(20);
		sizer2.add(center(label3));



		size_t sample_counter = 0;

		egt::experimental::CPUMonitorUsage tools;
		egt::PeriodicTimer timer1(std::chrono::seconds(1));
		timer1.on_timeout([&]()
		{
		    tools.update();

		    static const int chart_limit = 10;
		    int ret ;
		ret= system("/usr/bin/python3 testMAX30100.py 0");
		    auto i1 = outfunction();
		    //readtemp();
		    egt::ChartItemArray data1;
		    data1.add(sample_counter, i1);
		    line1.add_data(data1);
		    while (line1.data_size() > chart_limit)
		        line1.remove_data(1);
		    label3.text(egt::detail::format(i1, 0));


		   sample_counter++;
		});

		timer1.start();

		window.show();
	//	return app.run();

});

if (count2=1)
{
	 //egt::Application app(argc, argv);
	egt::TopWindow window;
	window.color(egt::Palette::ColorId::bg, egt::Color(0x1d2239ff));

	egt::VerticalBoxSizer sizer1;
	window.add(expand(sizer1));

	egt::HorizontalBoxSizer sizer0(egt::Justification::start);
	sizer0.color(egt::Palette::ColorId::bg, egt::Color(0x252c48ff));
	sizer0.color(egt::Palette::ColorId::border, egt::Palette::black);
	sizer0.border(2);
	sizer0.border_flags(egt::Theme::BorderFlag::bottom);
	sizer0.fill_flags(egt::Theme::FillFlag::blend);
	sizer1.add(expand_horizontal(sizer0));

	auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_white.png;128"));
	logo->margin(10);
	sizer0.add(logo);

	egt::Label label0("Heart beat");
	label0.color(egt::Palette::ColorId::label_text, egt::Palette::white);
	label0.padding(5);
	label0.font(egt::Font(28));
	sizer0.add(label0);

	egt::HorizontalBoxSizer sizer2;
	sizer1.add(expand(sizer2));

	egt::LineChart line1;
	line1.label("", "Value", "Graph");
	line1.font(egt::Font(24));
	line1.line_width(2);
	line1.grid_style(egt::LineChart::GridFlag::box_major_ticks_coord);
	line1.color(egt::Palette::ColorId::label_text, egt::Palette::ghostwhite);
	line1.color(egt::Palette::ColorId::button_bg, egt::Palette::red);
	line1.color(egt::Palette::ColorId::button_fg, egt::Color(0xd3d3d3));
	line1.margin(2);
	sizer2.add(expand(line1));

	egt::Label label3("000");
	label3.color(egt::Palette::ColorId::label_text, egt::Color(0xd3d3d3));
	label3.font(egt::Font(80));
	label3.margin(20);
	sizer2.add(center(label3));



	size_t sample_counter = 0;

	egt::experimental::CPUMonitorUsage tools;
	egt::PeriodicTimer timer1(std::chrono::seconds(1));
	timer1.on_timeout([&]()
	{
	    tools.update();

	    static const int chart_limit = 10;
	    int ret ;
	ret= system("/usr/bin/python3 testMAX30100.py 0");
	    auto i1 = outfunction();
	    //readtemp();
	    egt::ChartItemArray data1;
	    data1.add(sample_counter, i1);
	    line1.add_data(data1);
	    while (line1.data_size() > chart_limit)
	        line1.remove_data(1);
	    label3.text(egt::detail::format(i1, 0));


	   sample_counter++;
	});

	timer1.start();

	window.show();


}





window->on_event([&main_screen,&startup_screen, &third_screen, &window] (egt::Event & event)
	{
  	 switch (event.key().keycode)//Is the event happening a keypress ?
		{
			case egt::EKEY_1: //Is the key pressed the "KEY_0" corresponding to the K1 button. (See the training for more explainations)
			{
				//window->hide();
				main_screen->show();
				startup_screen->hide();
				third_screen->hide();
				//window.quit();
				//egt::Application::instance().quit();
				cout << "stopppp!" << endl;
				count1=0;
				break;
			}

			default:
				break;
		}
		}, {egt::EventId::keyboard_up});//Consider the event only when the finger has moved up from the K1 button


SwipeDetect swipe([&window](const std::string & direction)
    {
        if (direction == "right")
        {
        	if(count1<2)
        	{
        	cout << "right!" << endl;
        	count1=count1+1;
        	}
        }
            //win.move_boxes_swipe(true);
       else if (direction == "left")
       {
    	   if(count1>1)
    	   {
    	   cout << "left!" << endl;
    	   count1=count1-1;
    	   }
       }
           // win.move_boxes_swipe(false);
	//main_screen->hide();



    });

    // feed global events to swipe detector
    egt::Input::global_input().on_event([&swipe, &main_screen,&startup_screen, &third_screen](egt::Event & event)
    {
    	 swipe.handle(event);
    	if (count1 == 1){
        swipe.handle(event);
        main_screen->hide();
        startup_screen->show();
        third_screen->hide();
        count1=1;


       // count1 =1;
    	}
    	if (count1 == 2){
    	        swipe.handle(event);
    	        main_screen->hide();
    	        startup_screen->hide();
    	        third_screen->show();
    	       count1=2;
    	    	}
    	});

 return app.run();

}


extern "C" int outfunction()
{
	 //int output;

	                    std::ifstream myfile;
	                    myfile.open("myfile.txt");
	                    std::string mystring;
	                    if(myfile.is_open())
	                    {
	                    	myfile>>mystring;
	                    	//int output;
	                    	}
	                    	int output = stoi (mystring);
	                    	//int ret ;
	                    		                    	//ret= system("/usr/bin/python3 testMAX30100.py 0");
	                    	//sleep(1);
	                    	//std::cout << output;


	                    return output;
};
