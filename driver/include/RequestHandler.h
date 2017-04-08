/*
 * RequestHandler.h
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#ifndef DRIVER_SRC_REQUESTHANDLER_H_
#define DRIVER_SRC_REQUESTHANDLER_H_
#include "interface/ObserverSubject.h"
#include <vector>

namespace mkjtr_driver
{

class RequestHandler : public ObserverSubject
{
    std::vector<DataAccess *> listDataAccess;
public:
    RequestHandler();
    virtual ~RequestHandler();

    int registerDataAccessObserver(DataAccess *observer);
    int removeDataAccessObserver(DataAccess *observer);
    int notifyDataAccessObservers();
};

} /* namespace mkjtr_driver */

#endif /* DRIVER_SRC_REQUESTHANDLER_H_ */
