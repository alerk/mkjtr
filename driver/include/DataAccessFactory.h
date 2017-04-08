/*
 * DataAccessFactory.h
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#ifndef DRIVER_INCLUDE_DATAACCESSFACTORY_H_
#define DRIVER_INCLUDE_DATAACCESSFACTORY_H_

#include "interface/DataAccess.h"
#include <string>

namespace mkjtr_driver
{

class DataAccessFactory
{
public:
    DataAccessFactory();
    virtual ~DataAccessFactory();
    static DataAccess *generateDataAccess(std::string type);
};

} /* namespace mkjtr_driver */

#endif /* DRIVER_INCLUDE_DATAACCESSFACTORY_H_ */
