#include <Arduino.h>
#include <Keyboard.h>
#include "classes/Button.h"
#include "classes/actions/PrintAction.h"
#include "version.h"
#include "hardware_config.h"

// Forward declarations
bool initializeSystem();
void enterErrorState();
void handleErrorState();
void monitorSystemHealth();
void handleError();

// Global objects (stack allocation instead of dynamic allocation)
Button button;

// Action instances will be created in setup() since F() can't be used at global scope
PrintAction* gitPushAction = nullptr;
PrintAction* gitPullAction = nullptr;
PrintAction* holdAction = nullptr;
PrintAction* longHoldAction = nullptr;

// System state
bool systemInitialized = false;
unsigned long lastErrorTime = 0;
uint8_t consecutiveErrors = 0;

// Constants
constexpr unsigned long ERROR_RESET_INTERVAL = HardwareConfig::ERROR_RESET_INTERVAL_MS;
constexpr uint8_t MAX_CONSECUTIVE_ERRORS = HardwareConfig::MAX_CONSECUTIVE_ERRORS;

void setup() {
    // Initialize serial for debugging (optional, remove if not needed)
    #ifdef DEBUG
    Serial.begin(9600);
    Serial.println(F("Arduino Button System Starting..."));
    #endif
    
    // Create action objects (F() macro can only be used inside functions)
    static PrintAction gitPushActionObj(F("git push"));
    static PrintAction gitPullActionObj(F("git pull"));  
    static PrintAction holdActionObj(F("NICE :)))"));
    static PrintAction longHoldActionObj(F("Version: 0.0.1. Source code: https://github.com/RGilyazov/ArduinoButtons/tree/for-eyal/v0.0.1"));
    
    // Set global pointers to these objects
    gitPushAction = &gitPushActionObj;
    gitPullAction = &gitPullActionObj;
    holdAction = &holdActionObj;
    longHoldAction = &longHoldActionObj;
    
    // Initialize system
    if (!initializeSystem()) {
        #ifdef DEBUG
        Serial.println(F("System initialization failed!"));
        #endif
        // Enter error state - blink both LEDs
        enterErrorState();
        return;
    }
    
    systemInitialized = true;
    
    #ifdef DEBUG
    Serial.println(F("System initialized successfully"));
    #endif
}

bool initializeSystem() {
    // Initialize keyboard
    Keyboard.begin();
    delay(100); // Give keyboard time to initialize
    
    // Setup button with validation
    if (!button.setup(HardwareConfig::BUTTON_PIN)) {
        return false;
    }
    
    // Assign actions with validation (check pointers are not null)
    if (!gitPushAction || !gitPullAction || !holdAction || !longHoldAction) {
        return false;
    }
    
    if (!button.setOnClickAction(gitPushAction) ||
        !button.setOnDoubleClickAction(gitPullAction) ||
        !button.setOnHoldAction(holdAction) ||
        !button.setOnLongHoldAction(longHoldAction)) {
        return false;
    }
    
    // Initialize LED pins
    pinMode(HardwareConfig::LED_PIN_GREEN, OUTPUT);
    pinMode(HardwareConfig::LED_PIN_RED, OUTPUT);
    
    // Turn off both LEDs initially
    digitalWrite(HardwareConfig::LED_PIN_GREEN, LOW);
    digitalWrite(HardwareConfig::LED_PIN_RED, LOW);
    
    // Random LED startup indication
    randomSeed(analogRead(0));
    if (random(2) == 0) {
        digitalWrite(HardwareConfig::LED_PIN_GREEN, HIGH);
    } else {
        digitalWrite(HardwareConfig::LED_PIN_RED, HIGH);
    }
    
    return true;
}

void loop() {
    if (!systemInitialized) {
        // System failed to initialize - stay in error state
        handleErrorState();
        return;
    }
    
    // Main button processing
    button.loop();
    
    // System health monitoring
    monitorSystemHealth();
    
    // Small delay to prevent excessive CPU usage
    delay(1);
}

void monitorSystemHealth() {
    // Check if we need to reset error counter
    if (consecutiveErrors > 0 && 
        (millis() - lastErrorTime) > ERROR_RESET_INTERVAL) {
        consecutiveErrors = 0;
        #ifdef DEBUG
        Serial.println(F("Error counter reset"));
        #endif
    }
    
    // Check for too many errors
    if (consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
        #ifdef DEBUG
        Serial.println(F("Too many errors - entering safe mode"));
        #endif
        enterErrorState();
    }
}

void handleError() {
    consecutiveErrors++;
    lastErrorTime = millis();
    
    #ifdef DEBUG
    Serial.print(F("Error occurred. Count: "));
    Serial.println(consecutiveErrors);
    #endif
    
    // Flash red LED to indicate error
    for (int i = 0; i < 3; i++) {
        digitalWrite(HardwareConfig::LED_PIN_RED, HIGH);
        delay(100);
        digitalWrite(HardwareConfig::LED_PIN_RED, LOW);
        delay(100);
    }
}

void enterErrorState() {
    systemInitialized = false;
    
    // Turn off green LED, turn on red LED
    digitalWrite(HardwareConfig::LED_PIN_GREEN, LOW);
    digitalWrite(HardwareConfig::LED_PIN_RED, HIGH);
    
    #ifdef DEBUG
    Serial.println(F("Entering error state"));
    #endif
}

void handleErrorState() {
    // Blink red LED to indicate error state
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    
    if ((millis() - lastBlink) > HardwareConfig::ERROR_BLINK_INTERVAL_MS) {
        ledState = !ledState;
        digitalWrite(HardwareConfig::LED_PIN_RED, ledState ? HIGH : LOW);
        lastBlink = millis();
    }
    
    // Attempt to recover every configured interval
    static unsigned long lastRecoveryAttempt = 0;
    if ((millis() - lastRecoveryAttempt) > HardwareConfig::RECOVERY_ATTEMPT_INTERVAL_MS) {
        #ifdef DEBUG
        Serial.println(F("Attempting system recovery..."));
        #endif
        
        if (initializeSystem()) {
            systemInitialized = true;
            consecutiveErrors = 0;
            #ifdef DEBUG
            Serial.println(F("System recovery successful"));
            #endif
        }
        
        lastRecoveryAttempt = millis();
    }
}