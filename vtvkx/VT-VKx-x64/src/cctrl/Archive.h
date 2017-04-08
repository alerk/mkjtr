/*
 * Archive.h
 *
 *  Created on: Jan 17, 2017
 *      Author: "Valentin Chernov"
 */

#ifndef CCTRL_ARCHIVE_H_
#define CCTRL_ARCHIVE_H_

extern "C" {
#include "CControl.h"
}
#include "api_va.h"

#include"LampStates.h"

namespace sitraffic {

class LampStates;

class Archive {
public:
    Archive();
    virtual ~Archive();

    CLIENT * fill(CLIENT *clnt);

    void onNewValues(enum ArchiveSelectorCctrl selector);

    const LampStates & getYoungestLampStates()const{return youngestLampStates;};

private:
    CLIENT * fillLampStates(CLIENT *clnt);

    void updateLampStates(void);

    LampStates youngestLampStates;
};

} /* namespace sitraffic */

#endif /* CCTRL_ARCHIVE_H_ */
