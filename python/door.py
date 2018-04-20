#!/usr/bin/env python

import io
import logging
import requests
import socket
import time
import RPi.GPIO as GPIO

UNLOCK_TIME = 10

ENTRY_CODES = set([

    # 'RFID 1234567',  # Example RFID card
    # 'PIN 12345678',  # Example PIN code

])

OUTER_DOOR = 3
INNER_DOOR = 2

UNLOCKED = False
LOCKED = True

OK_FILE = '/tmp/doorbot_last_ok'


def main():
    setup_gpio()

    UDP_IP = "10.11.12.1"
    UDP_PORT = 50000

    sock = socket.socket(socket.AF_INET,     # Internet
                         socket.SOCK_DGRAM)  # UDP
    sock.bind((UDP_IP, UDP_PORT))
    sock.settimeout(10)

    logging.info('Listening')

    while True:

        try:
            data, addr = sock.recvfrom(1024)  # buffer size is 1024 bytes

        except socket.timeout:
            pass

        else:
            logging.debug("received message: `{}`".format(data))

            codes = filter(None, data.split('\n'))
            for code in codes:

                if code in ENTRY_CODES:
                    logging.info('Access Granted: `{}`'.format(code))
                    unlock_lock_sequence()
                else:
                    logging.info('Access Denied: `{}`'.format(code))

        with io.open(OK_FILE, 'w') as f:
            pass


def setup_gpio():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(INNER_DOOR, GPIO.OUT)
    GPIO.setup(OUTER_DOOR, GPIO.OUT)

    lock(INNER_DOOR)
    lock(OUTER_DOOR)


def unlock_lock_sequence():
    unlock(OUTER_DOOR)
    unlock(INNER_DOOR)

    time.sleep(5)
    lock(OUTER_DOOR)

    time.sleep(10)
    lock(INNER_DOOR)


def unlock(door):
    GPIO.output(door, UNLOCKED)


def lock(door):
    GPIO.output(door, LOCKED)



if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(asctime)-15s %(message)s')
    main()
