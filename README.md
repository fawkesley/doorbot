# Doorbot

Doorbot is a door entry system for my house. It has four main components:

1. [Keypad & RFID reader unit](https://github.com/paulfurley/doorbot/blob/master/hardware/keypad) outside the front door

2. Arduino with [code](https://github.com/paulfurley/doorbot/blob/master/arduino/doorbot) to read the keypad and send packets over Ethernet. Housed in a [laser cut wooden box](https://github.com/paulfurley/doorbot/blob/master/hardware/casing)

3. Raspberry Pi running Python code, connected to a [dual relay board](https://www.amazon.co.uk/dp/B009P04ZKC/)

4. Two 12V electromagnet door strikes ([outer porch door](https://www.locksonline.co.uk/UPVC-Electronic-Locking/uPVC-Door-Electric-Strike-Release.html) and inner door)
