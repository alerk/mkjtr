/*
 * DataAccessFactory.cpp
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#include <DataAccessFactory.h>
#include "DetectorDataAccess.h"

namespace mkjtr_driver
{

DataAccessFactory::DataAccessFactory()
{
    // TODO Auto-generated constructor stub

}

DataAccessFactory::~DataAccessFactory()
{
    // TODO Auto-generated destructor stub
}

DataAccess* DataAccessFactory::generateDataAccess(std::string type)
{
    if(type.compare("DetectorData")==0)
    {
        return (new DetectorDataAccess());
    }
    else
    {
        return 0;
    }
}

} /* namespace mkjtr_driver */
