#!/bin/sh

TANAME=tdsx
TACE_PATH="/opt/ext/tac/${TANAME}/"
JOB_NR=123457

# stop eventually running ${TANAME}
echo "${TANAME} postinstall"

# allow change of TA state
[ -z "$HOME" ] && export HOME="/root"

cat << EOFRPC | /opt/jappl/tools/rpcsend 
to "localhost" "536871426" 8 "115" 200 "tcp"

"ret string" {
 int "ret"
}

int 899	"prio"
int 2	"Mod OFF"
int $JOB_NR
int 0
int 0	"duration 0"

call

EOFRPC
	
mkdir -p /opt/httpd/tac
mkdir -p /opt/httpd/tac/monitor
mkdir -p /opt/httpd/tac/monitor/${TANAME}
mkdir -p /opt/httpd/tac/operate
mkdir -p /opt/httpd/tac/operate/${TANAME}
mkdir -p /opt/httpd/tac/service
mkdir -p /opt/httpd/tac/service/${TANAME}
mkdir -p /opt/httpd/tac/expert
mkdir -p /opt/httpd/tac/expert/${TANAME}

ln -fs ${TACE_PATH}/httpd/monitor/monitor.html /opt/httpd/tac/monitor/${TANAME}/monitor.html
ln -fs ${TACE_PATH}/httpd/operate/operate.html /opt/httpd/tac/operate/${TANAME}/operate.html
ln -fs ${TACE_PATH}/httpd/service/service.html /opt/httpd/tac/service/${TANAME}/service.html
ln -fs ${TACE_PATH}/httpd/expert/expert.html   /opt/httpd/tac/expert/${TANAME}/expert.html

exit 0
