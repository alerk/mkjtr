/*
 * Driver.cpp
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#include "Driver.h"
#include "DataAccessFactory.h"


namespace mkjtr_driver
{

Driver::Driver()
{
    // TODO Auto-generated constructor stub

}

Driver::~Driver()
{
    // TODO Auto-generated destructor stub
}

int Driver::start(int argc, char* argv[])
{
    requestHandler = new RequestHandler();
    DataAccess *dataAccess = DataAccessFactory::generateDataAccess("DetectorData");
    requestHandler->registerDataAccessObserver(dataAccess);

    return 0;
}

} /* namespace mkjtr_driver */
