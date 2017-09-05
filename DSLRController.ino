#include "global.h"
#include <IRremote.h>

// setup our IR receiver
IRrecv irrecv(PIN_IR);
decode_results results;

// define controller vars
String CONTROLLER_MODE = MODE_MANUAL;
int SHUTTER_DEALAY = 1250;
boolean SHUTTER_OPEN = false;

void toggleShutter() {
  SHUTTER_OPEN = true;
  analogWrite(PIN_SHUTTER, 255);
  delay(SHUTTER_DEALAY);
  analogWrite(PIN_SHUTTER, 0);
  SHUTTER_OPEN = false;
}

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(PIN_SHUTTER, OUTPUT);
}

void handleIR(decode_results *results) {
  switch (results->value) {

    // manual shutter (quick snapshot)
    case 0xFFA25D:
      toggleShutter();
    break;

    // play/pause button will permit different functionality
    case 0xFF22DD:
    case 0x52A3D41F:
      // if we're in manual mode, let's open or close the shutter for bulb mode
      if (CONTROLLER_MODE == MODE_MANUAL) {
        SHUTTER_OPEN = !SHUTTER_OPEN;
        analogWrite(PIN_SHUTTER, SHUTTER_OPEN ? 0 : 255);

      // if we're in countdown mode, let's pause or start the countdown
      } else if (CONTROLLER_MODE == MODE_COUNTDOWN) {
        // TODO
      }
    break;
  }

  int count = results->rawlen;
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 1; i < count; i++) {
    if (i & 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.write('-');
      Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    handleIR(&results);
    irrecv.resume(); // Receive the next value
  }
}

