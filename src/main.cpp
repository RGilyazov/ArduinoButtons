#include <Arduino.h>
#include <Keyboard.h>
#include "classes/Button.h"
#include "classes/actions/PrintAction.h"
#include "classes/actions/LEDRedToGreenAction.h"
#include "classes/actions/CombinedAction.h"
#include "classes/actions/LEDColorActions.h"  
#include "classes/action_executor/ActionExecutor.h"
#include "classes/leds/RGLed.h"
#include "version.h"
#include "hardware_config.h"

// Forward declarations
bool initializeSystem();
void enterErrorState();
void handleErrorState();
void monitorSystemHealth();
void handleError();
void setupStartupSequence();

// Global objects (stack allocation instead of dynamic allocation)
Button button;
RGLed statusLED;  // Red+Green status LED

ActionExecutor executor;

// Action instances will be created in setup() since F() can't be used at global scope
PrintAction* gitPushAction = nullptr;
PrintAction* gitPullAction = nullptr;
PrintAction* holdAction = nullptr;
AbstractAction* longHoldAction = nullptr;
LEDRedAction* ledRedAction = nullptr;     // When button pushed
LEDGreenAction* ledGreenAction = nullptr; // When button popped (released)

// Startup action
LEDRedToGreenAction* startupLEDAction = nullptr;

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
    
    // Long hold combined action: print version info AND toggle LED color
    static PrintAction versionActionObj("Version: " + String((__FlashStringHelper*)VERSION) + 
                                       ". Source code: https://github.com/RGilyazov/ArduinoButtons/tree/" + 
                                       String((__FlashStringHelper*)PROJECT_NAME) + "/v" + 
                                       String((__FlashStringHelper*)VERSION));
    static LEDYellowAction LEDYellowActionObj(&statusLED);
    static CombinedAction longHoldActionObj;

    //Create LED color actions for push/pop
    static LEDRedAction ledRedActionObj(&statusLED);      // Red when pushed
    static LEDGreenAction ledGreenActionObj(&statusLED);  // Green when released
    
    // Build the combined action
    longHoldActionObj.addAction(&LEDYellowActionObj);
    longHoldActionObj.addAction(&versionActionObj);
    
    // Set global pointers to these objects
    gitPushAction = &gitPushActionObj;
    gitPullAction = &gitPullActionObj;
    holdAction = &holdActionObj;
    longHoldAction = &longHoldActionObj;
    ledRedAction = &ledRedActionObj;
    ledGreenAction = &ledGreenActionObj;
    
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
    
    // Setup and start startup sequence (using same executor as buttons)
    setupStartupSequence();
    
    #ifdef DEBUG
    Serial.println(F("System initialized successfully"));
    #endif
}

void setupStartupSequence() {
    // Create startup LED action
    static LEDRedToGreenAction startupLEDActionObj(&statusLED);
    startupLEDAction = &startupLEDActionObj;
    
    // Configure the action for 5 seconds
    startupLEDAction->setDuration(5000);
    
    // Execute startup action immediately (parallel execution by default)
    executor.executeAction(startupLEDAction);
    
    #ifdef DEBUG
    Serial.println(F("Startup sequence started - LED will change from red to green"));
    #endif
}

bool initializeSystem() {
    // Initialize keyboard
    Keyboard.begin();
    delay(100); // Give keyboard time to initialize
    
    // Setup button
    if (!button.setup(HardwareConfig::BUTTON_PIN)) {
        return false;
    }
    
    // Set up button with the single ActionExecutor
    button.setActionExecutor(&executor);
    
    // Configure button action behavior (these are the defaults but shown for clarity)
    button.setStopOthersOnClick(true);      // Click stops other actions
    button.setStopOthersOnDoubleClick(true); // Double-click stops other actions  
    button.setStopOthersOnHold(true);       // Hold stops other actions
    button.setStopOthersOnLongHold(true);   // Long-hold stops other actions
    // Push/Pop automatically run in parallel (never stop others)
    
    // Setup RG LED
    statusLED.setup(HardwareConfig::RGLED_RED_PIN, HardwareConfig::RGLED_GREEN_PIN);
    
    // Assign actions with validation (check pointers are not null)
    if (!gitPushAction || !gitPullAction || !holdAction || !longHoldAction) {
        return false;
    }
    
    if (!button.setOnClickAction(gitPushAction) ||
        !button.setOnDoubleClickAction(gitPullAction) ||
        !button.setOnHoldAction(holdAction) ||
        !button.setOnLongHoldAction(longHoldAction) ||
        !ledRedAction || !ledGreenAction) {
        return false;
    }

    if (!button.setOnPushAction(ledRedAction) ||      // Button pressed = Red LED
        !button.setOnPopAction(ledGreenAction)) {     // Button released = Green LED
        return false;
    }
    
    // Start with red LED (startup sequence will change to green)
    statusLED.showRed();
    
    return true;
}

void loop() {
    if (!systemInitialized) {
        // System failed to initialize - stay in error state
        handleErrorState();
        return;
    }

    // Update the single ActionExecutor (handles both startup and button actions)
    executor.update();
    
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
    
    // Flash red LED to indicate error (3 quick blinks)
    for (int i = 0; i < 3; i++) {
        statusLED.showRed();
        delay(100);
        statusLED.turnOff();
        delay(100);
    }
}

void enterErrorState() {
    systemInitialized = false;
    
    // Show error state with red LED, turn off green
    statusLED.showRed();
    
    #ifdef DEBUG
    Serial.println(F("Entering error state"));
    #endif
}

void handleErrorState() {
    // Blink red LED to indicate error state
    static unsigned long lastBlink = 0;
    
    if ((millis() - lastBlink) > HardwareConfig::ERROR_BLINK_INTERVAL_MS) {
        // Toggle between red (error) and off
        if (statusLED.isOn()) {
            statusLED.turnOff();
        } else {
            statusLED.showRed();
        }
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