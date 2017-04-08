/*
 * LocalResolv.h
 *
 *  Created on: 30. 6. 2016
 *      Author: sevcikr
 */

#ifndef LOCALRESOLV_H_
#define LOCALRESOLV_H_

#include <cstddef>

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic {

int GetLocalEndpointIp(const char *hostname, char *out, size_t outsize);

}

#endif /* LOCALRESOLV_H_ */
