/*
 * DetectorDataAccess.h
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#ifndef DRIVER_INCLUDE_DETECTORDATAACCESS_H_
#define DRIVER_INCLUDE_DETECTORDATAACCESS_H_

#include <interface/DataAccess.h>

namespace mkjtr_driver
{

class DetectorDataAccess: public DataAccess
{
public:
    DetectorDataAccess();
    virtual ~DetectorDataAccess();

    int onRequestReceived(Request *request);
};

} /* namespace mkjtr_driver */

#endif /* DRIVER_INCLUDE_DETECTORDATAACCESS_H_ */
