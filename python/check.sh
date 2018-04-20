#!/bin/sh -eu

DOORBOT_OK_FILE="/tmp/doorbot_last_ok"
CRONITOR_URL='https://cronitor.link/vDxIYg/complete'


while true
do

  # TODO: check ok file exists

    if [ -f "${DOORBOT_OK_FILE}" ]; then
 
        if test "$(find ${DOORBOT_OK_FILE} -cmin +2)"; then

          echo "$DOORBOT_OK_FILE out of date"

        else

          curl -s "$CRONITOR_URL" || true

        fi
    fi

    sleep 1m

done

