

// include KnxDevice library
#include <KnxDevice.h>
#include "kdevice_Ultraschall_Entfernungssensor.h"
// include NewPing library
#include <NewPing.h>

// Programmier led
//#define PROG_LED_PIN 13 // Nano/ProMini
#define PROG_LED_PIN 17  //ProMicro
// Programmier button
#define PROG_BUTTON_PIN 3

// KNX-Serialport
//#define KNX_SERIAL Serial // Nano/ProMini etc. use Serial
#define KNX_SERIAL Serial1 // ProMicro etc. use Serial1
//Setup
#define UNDEFINED_MM 9999
#define TRIGGER_PIN  5  // Trigger Pin
#define ECHO_PIN     6  // Echo Pin
#define MAX_DISTANCE 200 // Maximale Distanc in CM

// Ultraschall Setup
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

unsigned long previousMillisZyklisch = 0;
unsigned long previousMillisSBC = 0;
unsigned long previousMillis = 0;
unsigned long sbc = 0;
unsigned long pingdelay = 1000;
unsigned int UsCycleS = 6000;
int lastCM = 0;
bool OnOff = false;


void setup() {
  // Initialize KNX
  Konnekting.init(KNX_SERIAL, PROG_BUTTON_PIN, PROG_LED_PIN, MANUFACTURER_ID, DEVICE_ID, REVISION);

  if (!Konnekting.isFactorySetting()) {
    int startDelay = (int) Konnekting.getUINT8Param(0);
    if (startDelay > 0 && startDelay < 255) {
      delay(startDelay * 1000);
    }

    previousMillisZyklisch = millis();

    // US Parameter Lesen
    OnOff = Konnekting.getUINT8Param(1) == 1 ? true : false;
    UsCycleS = (int)Konnekting.getUINT16Param(2);
    sbc = Konnekting.getUINT8Param(3);
    pingdelay = Konnekting.getUINT16Param(4);
  }
}

void loop() {
  Knx.task();
  // only do measurements and other sketch related stuff if not in programming mode
  if (Konnekting.isReadyForApplication()) {
	    if (OnOff) {

      float currentMM = UNDEFINED_MM;

      // Zyklisches Senden
      if (UsCycleS > 0) {

        // Überlaufprobleme verhindern
        if (previousMillisZyklisch > millis()) {
          // Überspringt eine runde bei Überlaufproblemen
          previousMillisZyklisch = millis();
        }

        if ((millis() - previousMillisZyklisch)/1000 >= UsCycleS) {

         previousMillisZyklisch = millis();
         Knx.write(0, sonar.ping_cm());
        }

      }

      // Senden bei Änderung
      if (sbc > 0) {
        // Überlaufprobleme verhindern
        if (previousMillisSBC > millis()) {
          // Überspringt eine runde bei Überlaufproblemen
          previousMillisSBC = millis();
        }


        if (millis() - previousMillisSBC >= pingdelay) {

          previousMillisSBC = millis();

          if (currentMM == UNDEFINED_MM) {
            currentMM = sonar.ping_cm();
          }

          float diff = (lastCM - currentMM);

          // Differenz muss immer Positiv sein!
          if (diff < 0) {
            diff *= -1;
          }

          float requiredDiff = (float) sbc;

          if (diff >= requiredDiff) {
            Knx.write(0, sonar.ping_cm());
            lastCM = currentMM;
          }
        }
      }
    }
  }
}

// Callback function to handle com objects updates
void knxEvents(byte index) {
  // nothing to do in this sketch
};


