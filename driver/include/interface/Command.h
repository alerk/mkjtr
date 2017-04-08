/*
 * Command.h
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#ifndef DRIVER_INTERFACE_COMMAND_H_
#define DRIVER_INTERFACE_COMMAND_H_
namespace mkjtr_driver
{

class Command
{
public:
    virtual int execute() = 0;
};

}



#endif /* DRIVER_INTERFACE_COMMAND_H_ */
