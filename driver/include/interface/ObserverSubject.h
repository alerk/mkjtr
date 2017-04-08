/*
 * ObserverSubject.h
 *
 *  Created on: Apr 8, 2017
 *      Author: quyen.nguyen
 */

#ifndef DRIVER_INTERFACE_OBSERVERSUBJECT_H_
#define DRIVER_INTERFACE_OBSERVERSUBJECT_H_
#include "DataAccess.h"

namespace mkjtr_driver
{

class ObserverSubject
{
public:
    virtual int registerDataAccessObserver(DataAccess *observer) = 0;
    virtual int removeDataAccessObserver(DataAccess *observer) = 0;
    virtual int notifyDataAccessObservers() = 0;
};

}



#endif /* DRIVER_INTERFACE_OBSERVERSUBJECT_H_ */
