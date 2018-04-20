#!/bin/sh -eux

# If the pi boots up and can't get wifi (maybe because we're not near its
# normal router) we want the chance to be able to connect with Ethernet.
#
# 1. Got wifi: immediately reconfigure Ethernet port
# 2: No wifi: wait 2 minutes, then reconfigure Ethernet port

THIS_SCRIPT=$0
THIS_DIR=$(dirname ${THIS_SCRIPT})

reconfigure_eth0() {
    # Switch eth0 out of DHCP mode, assign it a static IP.
    logger doorbot "Reconfiguring eth0 for direct connection to Arduino."
    /sbin/ifconfig eth0 10.11.12.1    
}


if [ $(/sbin/ifconfig wlan0 |grep 'inet addr' |wc -l) -eq 0 ]; then
    logger doorbot "wlan0 is down, waiting 2 minutes before disabling eth0 DHCP..."
    sleep 2m
fi

reconfigure_eth0

exec python -u ${THIS_DIR}/door.py
