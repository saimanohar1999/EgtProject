/*
 * file.cpp
 *
 *  Created on: 29-Dec-2022
 *      Author: i63349-lnx
 */
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

		int readtemp();
    };

char timersecflag =0;
int main(int argc, char** argv)
{

 int output;
       // system("/usr/bin/python3 testMAX30100.py 1");
                    std::ifstream myfile;
                    myfile.open("myfile.txt");
                    std::string mystring;
                    if(myfile.is_open())
                    {
                    	myfile>>mystring;
                    	//int output;
                    	int output = stoi (mystring);
                    	std::cout << output;

        			}
                    //std::cout << output;
}
