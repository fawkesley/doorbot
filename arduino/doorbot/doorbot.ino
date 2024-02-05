#include <Wiegand.h>


#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


/*
 * Connections:
 * - Wiegand DATA0 (Green) ----- Arduino PIN 2
 * - Wiegand DATA1 (White) ----- Arduino PIN 3
 *
 * Requires:
 * - https://github.com/paulfurley/Wiegand-Protocol-Library-for-Arduino
 * - https://github.com/mrRobot62/Arduino-logging-library
 *
 */

/* CONSTANT DEFINITIONS */
static const char ESCAPE = 27;
static const char ENTER = 13;
static const unsigned int MAX_PIN_LENGTH = 16;
static const unsigned int PIN_SUBMIT_TIMEOUT = 5000;

/* NETWORKING DEFINITIONS */

byte MY_MAC_ADDRESS[] = {0x80, 0x86, 0xf2, 0x9c, 0x94, 0xa6};

IPAddress MY_IP_ADDRESS(10, 11, 12, 2);
IPAddress DESTINATION_ADDRESS(10, 11, 12, 1);

const unsigned int SOURCE_PORT = 40000;
const unsigned int DESTINATION_PORT = 50000;


/* GLOBALS */
WIEGAND wg;
String g_pin;
unsigned long int g_millis_last_key = 0;

unsigned long g_last_heartbeat = millis();
const unsigned long g_heartbeat_every = 10000;

EthernetUDP g_udp_socket; // An EthernetUDP instance to let us send and receive packets over UDP


void setup() {
  Serial.begin(115200);
  wg.begin();
  Serial.println("Assigning IP...");
  Ethernet.begin(MY_MAC_ADDRESS, MY_IP_ADDRESS); //, ROUTER_IP, ROUTER_IP, IPAddress(255, 255, 255, 0));

  // int success = Ethernet.begin(MY_MAC_ADDRESS);
  // Serial.print("Success?" );
  // Serial.println(success);

  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();


  g_udp_socket.begin(SOURCE_PORT);
  Serial.println("Ready");


}

void loop() {
  /*
  Serial.println("Sending packet");
  send_udp("hello");
  delay(5000);
  return;
  */
  
  if (!wg.available()) {
    check_for_pin_timeout();

    if (isDue(g_last_heartbeat, g_heartbeat_every)) {
      Serial.println("sending heartbeat");

      String payload("HEARTBEAT\n");
      send_udp(payload.c_str());

      g_last_heartbeat = millis();
    }

    return; // spin
  }

  unsigned long codeOrKey = wg.getCode();
  int type = wg.getWiegandType();

  Serial.print("DECIMAL = ");
  Serial.print(codeOrKey);
  Serial.print(", Type: ");
  Serial.println(type);

  switch (type) {
    case 26:
      reset_pin();
      handle_rfid_card(codeOrKey);
      return;

    case 4:
      handle_key_press(codeOrKey);
      return;

    default:
      Serial.print("Unhandled Wiegand type: ");
      Serial.println(type);
      return;
  }
}

void check_for_pin_timeout() {
  if(!g_pin.length() || 0 == g_millis_last_key) {
    return;
  }

  unsigned const int millis_since_last_press = millis() - g_millis_last_key;
  if(millis_since_last_press > PIN_SUBMIT_TIMEOUT) {
    Serial.println('Timeout, submitting.');
    submit_pin();
  }
}

void reset_pin() {
  g_pin = String();
  Serial.print("PIN AFTER: ");
  Serial.println(g_pin);
}

void submit_pin() {
  Serial.print("SUBMITTING PIN: ");
  Serial.println(g_pin);

  String payload("PIN ");
  payload += g_pin;
  payload += "\n";

  send_udp(payload.c_str());

  reset_pin();
}

void handle_rfid_card(unsigned long cardNumber)
{
  Serial.print("Sending RFID card: ");
  Serial.println(cardNumber);

  String payload("RFID ");
  payload += cardNumber;
  payload += "\n";
  send_udp(payload.c_str());
}

void handle_key_press(unsigned long key_code)
{
  Serial.print("Handling key press: ");
  Serial.println(key_code);
  switch(key_code) {
    case ESCAPE: {
      reset_pin();
      return;
    }

    case ENTER: {
      submit_pin();
      return;

    }

    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    {
      g_pin += digit_to_ascii(key_code);
      g_millis_last_key = millis();

      Serial.print("CURRENT PIN: ");
      Serial.println(g_pin);

      if(g_pin.length() >= MAX_PIN_LENGTH) {
        submit_pin();
      }

      return;
    }

    default:
    {
      Serial.println("GOT JUNK KEYPRESS");
      return;
    }
  }
}

char digit_to_ascii(unsigned long key_code) {
  /*
   * Return eg '1' for 1, '2' for 2 etc
   */

   return '0' + key_code;
}


void send_udp(const char* bytes) {
  g_udp_socket.beginPacket(DESTINATION_ADDRESS, DESTINATION_PORT);
  g_udp_socket.write(bytes);
  g_udp_socket.endPacket();
}

bool isDue(unsigned long start_timestamp, unsigned long desired_duration) {
  unsigned long timestamp_now = millis();
  unsigned long elapsed_duration = (timestamp_now - start_timestamp);

  return elapsed_duration > desired_duration; // compare durations = OK
}
