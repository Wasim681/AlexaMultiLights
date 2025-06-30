#include <WiaWiFi.h>    // https://github.com/Wasim681/WiaNetworks
#include "fauxmoESP.h"      // https://github.com/vintlabs/fauxmoESP

// ———————— Configuration ————————
#define ACTIVE_LOW  true   // set to false if your relay is active-HIGH

#if ACTIVE_LOW
  #define RELAY_ON   LOW
  #define RELAY_OFF  HIGH
#else
  #define RELAY_ON   HIGH
  #define RELAY_OFF  LOW
#endif

const int   RELAY_COUNT = 12;
const int   relayPins[RELAY_COUNT] = { 2, 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22 };
const char* deviceNames[RELAY_COUNT] = {
  "Light One",  "Light Two",   "Light Three", "Light Four",
  "Light Five", "Light Six",   "Light Seven", "Light Eight",
  "Light Nine", "Light Ten",   "Light Eleven","Light Twelve"
};

// ———————— Globals ————————
fauxmoESP fauxmo;
WiaWiFi wifi;
WiaDB db;

void setup() {
  Serial.begin(115200);
  db.init();

  // Restore saved relay states
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    bool state = db.get(deviceNames[i], false);
    digitalWrite(relayPins[i], state ? RELAY_ON : RELAY_OFF);
  }
  
  
  // Connect WiFi (captive-portal if needed)
  wifi.init();
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

  // Register Alexa devices
  for (int i = 0; i < RELAY_COUNT; i++) {
    fauxmo.addDevice(deviceNames[i]);
  }

  // Handle Alexa commands
  fauxmo.onSetState([](unsigned char id, const char* name, bool state, unsigned char value) {
    if (id < RELAY_COUNT) {
      digitalWrite(relayPins[id], state ? RELAY_ON : RELAY_OFF);
      db.put(name, state);
      Serial.printf(">>> %s turned %s\n", name, state ? "ON" : "OFF");
    }
  });

  fauxmo.enable(true);
  Serial.println("Alexa devices ready!");
}

void loop() {
  fauxmo.handle();
  wifi.loop();
}