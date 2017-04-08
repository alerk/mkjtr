/*
 * Request.h
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#ifndef DRIVER_INCLUDE_REQUEST_H_
#define DRIVER_INCLUDE_REQUEST_H_

namespace mkjtr_driver
{

typedef enum RequestType
{
    REQUEST_DETECTOR_READING = 0,
    REQUEST_MAX
};

class Request
{
    RequestType type;
public:
    Request();
    virtual ~Request();
};

} /* namespace mkjtr_driver */

#endif /* DRIVER_INCLUDE_REQUEST_H_ */
