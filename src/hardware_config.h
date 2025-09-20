#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <Arduino.h>

// Hardware pin assignments
namespace HardwareConfig {
    constexpr uint8_t BUTTON_PIN = 3;
    constexpr uint8_t LED_PIN_GREEN = 7;
    constexpr uint8_t LED_PIN_RED = 9;
    
    // System configuration
    constexpr unsigned long ERROR_RESET_INTERVAL_MS = 30000; // 30 seconds
    constexpr uint8_t MAX_CONSECUTIVE_ERRORS = 5;
    constexpr unsigned long RECOVERY_ATTEMPT_INTERVAL_MS = 10000; // 10 seconds
    constexpr unsigned long ERROR_BLINK_INTERVAL_MS = 500; // LED blink rate in error state
}

#endif