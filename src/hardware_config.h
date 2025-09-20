#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <Arduino.h>

// Hardware pin assignments
namespace HardwareConfig {
    constexpr uint8_t BUTTON_PIN = 3;
    constexpr uint8_t RGLED_RED_PIN = 7;     // Red channel of RG LED
    constexpr uint8_t RGLED_GREEN_PIN = 9;   // Green channel of RG LED
    
    // System configuration
    constexpr unsigned long ERROR_RESET_INTERVAL_MS = 30000; // 30 seconds
    constexpr uint8_t MAX_CONSECUTIVE_ERRORS = 5;
    constexpr unsigned long RECOVERY_ATTEMPT_INTERVAL_MS = 10000; // 10 seconds
    constexpr unsigned long ERROR_BLINK_INTERVAL_MS = 500; // LED blink rate in error state
}

#endif