/*
 * Driver.h
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#ifndef DRIVER_DRIVER_H_
#define DRIVER_DRIVER_H_

#include "Configuration.h"

#include "RequestHandler.h"

#include "VTVKWrapper.h"

namespace mkjtr_driver
{
    RequestHandler *requestHandler;


class Driver
{

public:
    Driver();
    virtual ~Driver();

    int start(int argc, char* argv[]);
};

} /* namespace mkjtr_driver */

#endif /* DRIVER_DRIVER_H_ */
