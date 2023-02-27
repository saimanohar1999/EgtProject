/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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

		int readtemp();
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

    egt::Label label0("Temperature");
    label0.color(egt::Palette::ColorId::label_text, egt::Palette::white);
    label0.padding(5);
    label0.font(egt::Font(28));
    sizer0.add(label0);

    egt::HorizontalBoxSizer sizer2;
    sizer1.add(expand(sizer2));

    egt::LineChart line1;
    line1.label("", "Celsius", "Graph");
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

   /* egt::HorizontalBoxSizer sizer3;
    sizer1.add(expand(sizer3));

    egt::LineChart line2;
    line2.label("", "Value", "Random");
    line2.line_width(2);
    line2.grid_style(egt::LineChart::GridFlag::box_major_ticks_coord);
    line2.color(egt::Palette::ColorId::label_text, egt::Palette::white);
    line2.color(egt::Palette::ColorId::button_bg, egt::Palette::gray);
    line2.color(egt::Palette::ColorId::button_fg, egt::Color(0x51e25fff));
    sizer3.add(expand(line2));

    egt::Label label4("000");
    label4.color(egt::Palette::ColorId::label_text, egt::Color(0x51e25fff));
    label4.font(egt::Font(80));
    label4.margin(20);
    sizer3.add(center(label4));

    egt::HorizontalBoxSizer sizer4;
    sizer1.add(expand(sizer4));

    egt::BarChart line3;
    line3.label("", "Value", "Random");
    line3.grid_style(egt::BarChart::GridFlag::box);
    line3.color(egt::Palette::ColorId::label_text, egt::Palette::white);
    line3.color(egt::Palette::ColorId::button_bg, egt::Palette::gray);
    line3.color(egt::Palette::ColorId::button_fg, egt::Color(0xff1493ff));
    sizer4.add(expand(line3));

    egt::Label label5("000");
    label5.color(egt::Palette::ColorId::label_text, egt::Color(0xff1493ff));
    label5.font(egt::Font(80));
    label5.margin(20);
    sizer4.add(center(label5));  */

   // std::random_device r;
   // std::default_random_engine e1 {r()};
   // std::uniform_int_distribution<int> dist{0, 100};

    size_t sample_counter = 0;

    egt::experimental::CPUMonitorUsage tools;
    egt::PeriodicTimer timer1(std::chrono::seconds(1));
    timer1.on_timeout([&]()
    {
        tools.update();

        static const int chart_limit = 10;

        //auto i1 = tools.usage();
        auto i1 = readtemp();
        egt::ChartItemArray data1;
        data1.add(sample_counter, i1);
        line1.add_data(data1);
        while (line1.data_size() > chart_limit)
            line1.remove_data(1);
        label3.text(egt::detail::format(i1, 0));

       /* auto i2 = dist(e1);
        egt::ChartItemArray data2;
        data2.add(sample_counter, i2);
        line2.add_data(data2);
        while (line2.data_size() > chart_limit)
            line2.remove_data(1);
        label4.text(egt::detail::format(i2, 0));

        auto i3 = dist(e1);
        egt::ChartItemArray data3;
        data3.add(sample_counter, i3);
        line3.add_data(data3);
        while (line3.data_size() > chart_limit)
            line3.remove_data(1);
        label5.text(egt::detail::format(i3, 0));  */

       sample_counter++;
    });
    timer1.start();

    window.show();

    return app.run();
}

extern "C" int readtemp()
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
