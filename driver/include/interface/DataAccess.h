/*
 * DataAccess.h
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#ifndef DRIVER_INTERFACE_DATAACCESS_H_
#define DRIVER_INTERFACE_DATAACCESS_H_
#include "Request.h"

namespace mkjtr_driver
{

class DataAccess
{
public:
    virtual int onRequestReceived(Request *request) = 0;
};

}



#endif /* DRIVER_INTERFACE_DATAACCESS_H_ */
