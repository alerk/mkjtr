#!/bin/sh

TANAME=tdsx
# remove not needed directories
echo ${TANAME} postuninstall
rm -rf /opt/ext/tac/${TANAME}
rm -rf /opt/httpd/tac/monitor/${TANAME}
rm -rf /opt/httpd/tac/operate/${TANAME}
rm -rf /opt/httpd/tac/service/${TANAME}
rm -rf /opt/httpd/tac/expert/${TANAME}
exit 0
