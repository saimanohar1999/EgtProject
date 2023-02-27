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
int co {};
int main(int argc, char** argv)
{
count1=0;
co=0;
 egt::Application app(argc, argv);
 egt::experimental::UiLoader loader;
 egt::TopWindow window1;

 //auto window1=std::static_pointer_cast<egt::Window>(loader.load("window1"));
 auto window = std::static_pointer_cast<egt::Window>(loader.load("file:ui.xml"));
auto main_screen=window->find_child<egt::Window>("mainscreen");
auto startup_screen=window->find_child<egt::Window>("Window9");
auto sensor_screen=window->find_child<egt::Window>("Window15");
auto empty_screen=window->find_child<egt::Window>("Window22");
auto frame = empty_screen->find_child<egt::Frame>("Frame23");
auto power_off_button=window->find_child<egt::Button>("Button16");
auto third_screen=window->find_child<egt::Window>("Window14");
auto heart_button=window->find_child<egt::ImageButton>("ImageButton18");
auto temp_button=window->find_child<egt::ImageButton>("ImageButton19");
auto pet_button=window->find_child<egt::ImageLabel>("ImageLabel16");
auto Ai_button=window->find_child<egt::ImageLabel>("ImageLabel17");

//egt::Button button(*frame, "Press Me");
//egt::center(button);


//auto sizer1=window->find_child<egt::VerticalBoxSizer>("BoxSizer17");
//auto sizer0=window->find_child<egt::HorizontalBoxSizer>("BoxSizer16");
//auto label3=window->find_child<egt::Label>("Label19");
//auto label2=window->find_child<egt::Label>("Label20");
//auto label1=window->find_child<egt::Label>("Label21");
//auto line1=window->find_child<egt::LineChart>("Linechart18");
//sizer1.add(expand(sizer0));
//power_off_button->fill_flags().clear();
//auto logo=window->find_child<egt::ImageLabel>("ImageLabel16");//Smart pointer pointing to the microchip logo on the screen_saver



    egt::VerticalBoxSizer sizer1;
    empty_screen->add(expand(sizer1));
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

       egt::Label label3("00");
       label3.color(egt::Palette::ColorId::label_text, egt::Color(0xd3d3d3));
       label3.font(egt::Font(80));
       label3.margin(20);
       sizer2.add(center(label3));
       size_t sample_counter = 0;

       egt::experimental::CPUMonitorUsage tools;
       egt::PeriodicTimer timer1(std::chrono::seconds(1));


power_off_button->on_click([&main_screen,&startup_screen, &third_screen, &sensor_screen] (egt::Event &)
{
	main_screen->hide();
startup_screen->show();
third_screen->hide();
sensor_screen->hide();
});
heart_button->on_click([&main_screen,&window,&label3,&startup_screen,&line1,&empty_screen,&sample_counter, &sensor_screen, &third_screen,&timer1] (egt::Event &)
{
	main_screen->hide();
	startup_screen->hide();
	third_screen->hide();
	sensor_screen->hide();
	//co=1;

timer1.on_timeout([&]()
	       {
	          // tools.update();

	           static const int chart_limit = 10;
	           auto i1 = outfunction();
	           //readtemp();
	           egt::ChartItemArray data1;
	           data1.add(sample_counter, i1);
	           line1.add_data(data1);
	           while (line1.data_size() > chart_limit)
	               line1.remove_data(1);
	           label3.text(egt::detail::format(i1, 0));


	          sample_counter++;
	          window->on_event([&main_screen,&startup_screen, &third_screen,&empty_screen, &sensor_screen] (egt::Event & event)
	          				{
	          			  	 switch (event.key().keycode)//Is the event happening a keypress ?
	          					{
	          						case egt::EKEY_1: //Is the key pressed the "KEY_0" corresponding to the K1 button. (See the training for more explainations)
	          						{
	          							main_screen->show();
	          							startup_screen->hide();
	          							third_screen->hide();
	          							sensor_screen->hide();
	          							empty_screen->hide();
	          							count1=0;
	          							cout<<"exit"<<endl;
	          							break;
	          						}

	          						default:
	          							break;
	          					}
	          					}, {egt::EventId::keyboard_up});

	       });
	       timer1.start();

		//	empty_screen->show();




	       /*egt::Button button(*frame, "Press Me");
	egt::center(button);
	empty_screen->show();

*/




			empty_screen->show();

});


window->on_event([&main_screen,&startup_screen, &third_screen,&empty_screen, &sensor_screen] (egt::Event & event)
	{
  	 switch (event.key().keycode)//Is the event happening a keypress ?
		{
			case egt::EKEY_1: //Is the key pressed the "KEY_0" corresponding to the K1 button. (See the training for more explainations)
			{
				main_screen->show();
				startup_screen->hide();
				third_screen->hide();
				sensor_screen->hide();
				empty_screen->hide();
				count1=0;
				cout<<"exit"<<endl;
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
        	if(count1<3)
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
    egt::Input::global_input().on_event([&swipe, &main_screen,&startup_screen, &sensor_screen, &third_screen](egt::Event & event)
    {
    	 swipe.handle(event);
    	if (count1 == 1){
        swipe.handle(event);
        main_screen->hide();
        startup_screen->show();
        third_screen->hide();
        sensor_screen->hide();
        count1=1;


       // count1 =1;
    	}
    	if (count1 == 2){
    	        swipe.handle(event);
    	        main_screen->hide();
    	        startup_screen->hide();
    	        third_screen->show();
    	        sensor_screen->hide();
    	       count1=2;
    	    	}
    	});


 return app.run();

}
extern "C" int outfunction()
{
	char tempstrbuf[80];
		string strtemp;

		int fd;
		int ret;
	    unsigned char buf[2];

		// Open device node
		   fd = open(DEV_I2C, O_RDWR);
		   if (fd <0) {
		   	printf ("ERROR open %s ret=%d\n", DEV_I2C, fd);
		   	return -1;
		   }
		   if (ioctl (fd, I2C_SLAVE, SLAVE_ADDR) < 0) {
		   	printf("ERROR ioctl() set slave address\n");
		   	return -1;
		   		   	}
		   		   	// Set conversation rate
		   		   	buf[0] = 0x04; // conversion rate register address
		   		   	buf[1] = 0x04; // Set conversion rate to 1 sec
		   		   	ret = write(fd, buf, 2);
		   		   	if (ret!= 2) {
		   		   		printf ("Error write() conversion rate\n");
		   		   		return -1;
		   		   	    }
		   		 // Read Temperature
		   		 	   	// Set internal address register pointer
		   		 	   	buf[0] = 0x00; //internal Diode High byte register address
		   		 	   	ret = write (fd, &buf[0], 1 );
		   		 	   	if (ret!= 1) {
		   		 	   	   printf ("Error write() Register address \n");
		   		 	   	   return -1;
		   		 	   	   }
		   		 	//read temperature
		   		 		//Read DATA
		   		 		buf[1] = 0; //clear receive buffer
		   		 		ret = read(fd, &buf[1], 1);
		   		 		if (ret!= 1) {
		   		 		printf ("Error read() data \n");
		   		 			   return -1;
		   		 			   }
		   		 			sprintf(tempstrbuf, "Temperature is %d\n", buf[1]);
		   		 			strtemp = tempstrbuf;
		   		 			//cout << strtemp;

		   		 			//close device node
		   		 			close(fd);
		   		 			return buf[1];
};


