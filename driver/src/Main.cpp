/*
 * Main.cpp
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#include "Driver.h"

int main(int argc, char* argv[])
{
    mkjtr_driver::Driver *driver = new mkjtr_driver::Driver();
    driver->start(argc, argv);
    return 0;
}


