#!/bin/sh

TANAME=tdsx
TACE_PATH="/opt/ext/tac/${TANAME}/"
JOB_NR=123457

[ -z "$HOME" ] && export HOME="/root"

# prevent change of TA state

cat << EOFRPC | /opt/jappl/tools/rpcsend 
to "localhost" "536871426" 8 "236" 200 "tcp"

"ret string" {
 string "ret"
}

int $JOB_NR

call

EOFRPC

# race condition: actual status gets unknown in some cases
sleep 15


# stop eventually running ${TANAME} and additional services
if [ -x start.sh ]; then
	${TACE_PATH}/start.sh stop
fi
exit 0
