

// include KnxDevice library
#include <KnxDevice.h>
#include "kdevice_Ultraschall_Entfernungssensor.h"
// include NewPing library
#include <NewPing.h>

// Programmier led
#define PROG_LED_PIN 13

// Programmier button
#define PROG_BUTTON_PIN 3

// KNX-Serialport
#define KNX_SERIAL Serial // Nano/ProMini etc. use Serial

//Setup
#define UNDEFINED_MM 9999
#define TRIGGER_PIN  5  // Trigger Pin
#define ECHO_PIN     6  // Echo Pin
#define MAX_DISTANCE 200 // Maximale Distanc in CM

// Ultraschall Setup
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

unsigned long previousMillisZyklus = 0;
unsigned long previousMillisSBC = 0;
unsigned long UsSBC = 0;
unsigned long UsDelay = 1000;
unsigned int UsZyklus = 0;
int lastMM = 0;
bool UsOnOff = false;


void setup() {
  // Initialize KNX
  Konnekting.init(KNX_SERIAL, PROG_BUTTON_PIN, PROG_LED_PIN, MANUFACTURER_ID, DEVICE_ID, REVISION);

  if (!Konnekting.isFactorySetting()) {
    int startDelay = (int) Konnekting.getUINT8Param(0);
    if (startDelay > 0 && startDelay < 255) {
      delay(startDelay * 1000);
    }

    previousMillisZyklus = millis();

    // US Parameter Lesen
    UsOnOff = Konnekting.getUINT8Param(1) == 1 ? true : false;
    UsZyklus = Konnekting.getUINT16Param(2);
    UsSBC = Konnekting.getUINT8Param(3);
    UsDelay = Konnekting.getUINT16Param(4);
  }
}

void loop() {
  Knx.task();
  // only do measurements and other sketch related stuff if not in programming mode
  if (Konnekting.isReadyForApplication()) {

    if (UsOnOff) {

      float currentMM = UNDEFINED_MM;

      // Zyklisches Senden
      if (UsZyklus > 0) {

        // Überlaufprobleme verhindern
        if (previousMillisZyklus > millis()) {
          // Überspringt eine runde bei Überlaufproblemen
          previousMillisZyklus = millis();
        }

        if (millis() - previousMillisZyklus >= (UsZyklus * 60 * 1000)) {

          previousMillisZyklus = millis();


          currentMM = sonar.ping_cm();

          Knx.write(0, currentMM);
        }

      }

      // Senden bei Änderung
      if (UsSBC > 0) {

        // Überlaufprobleme verhindern
        if (previousMillisSBC > millis()) {
          // Überspringt eine runde bei Überlaufproblemen
          previousMillisSBC = millis();
        }


        if (millis() - previousMillisSBC >= UsDelay) {

          previousMillisSBC = millis();

          if (currentMM == UNDEFINED_MM) {
            currentMM = sonar.ping_cm();
          }

          float diff = (lastMM - currentMM);

          // Differenz muss immer Positiv sein!
          if (diff < 0) {
            diff *= -1;
          }

          float requiredDiff = (float) UsSBC;

          if (diff >= requiredDiff) {
            Knx.write(1, currentMM);
            lastMM = currentMM;
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


