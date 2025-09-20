#ifndef VERSION_H
#define VERSION_H

#include <Arduino.h>

// Store version strings in flash memory to save RAM
constexpr char VERSION[] PROGMEM = "0.0.1";
constexpr char PROJECT_NAME[] PROGMEM = "for-eyal";

#endif