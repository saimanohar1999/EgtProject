#include<iostream>
using namespace std;

#include <fstream>
#include <string>
#include <egt/detail/string.h>
#include <egt/ui>
#include <random>
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
int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
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
	                    	std::cout << output;


	                    return output;
};
