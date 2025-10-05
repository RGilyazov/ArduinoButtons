# ArduinoButtons

A flexible, event-driven button library for Arduino Pro Micro with a sophisticated action execution system.

## Overview

This project provides a robust button handling framework with support for multiple button interactions (click, double-click, hold, long-hold, push/pop) and a non-blocking action execution system. Built on Arduino Pro Micro (ATmega32u4) with PlatformIO.

**Current Version:** 0.0.1

## Features

- **Rich Button Events**: Single click, double click, hold, long hold, push (press), pop (release)
- **Non-Blocking Action System**: State machine-based actions with multiple execution behaviors
- **Flexible Action Composition**: Combine multiple actions with `CombinedAction`
- **Hardware Abstraction**: Clean LED interfaces (`ILed`, `RGLed`, `RGBLed`)
- **Robust Error Handling**: System recovery, error tracking, visual error states
- **Memory Safe**: Stack allocation, circular buffer design, overflow protection

## Hardware

Based on **Arduino Pro Micro** (Leonardo-compatible, ATmega32u4).

**Default Pin Configuration** (see `src/hardware_config.h`):
```
Button:     Pin 3
LED Red:    Pin 9 (PWM)
LED Green:  Pin 6 (PWM)
```

## Quick Start

### Build and Upload

```bash
# Clone repository
git clone https://github.com/RuslanGilyazov/ArduinoButtons.git
cd ArduinoButtons

# Build
pio run

# Upload to Arduino Pro Micro
pio run --target upload

# Monitor serial output (debug builds)
pio device monitor
```

## Architecture

### Core Components

#### 1. Button Class (`src/classes/Button.h`)

Handles button state machine with debouncing and event detection.

```cpp
Button button;
button.setup(BUTTON_PIN);
button.setOnClickAction(myAction);
button.loop(); // Call in main loop
```

**Supported Events:**
- `setOnClickAction()` - Single click
- `setOnDoubleClickAction()` - Double click (configurable gap)
- `setOnHoldAction()` - Hold for 1s (default)
- `setOnLongHoldAction()` - Hold for 3s (default)
- `setOnPushAction()` - Button pressed down
- `setOnPopAction()` - Button released

**Timing Configuration:**
```cpp
button.setTimingParameters(
    20,    // debounce (ms)
    250,   // double-click gap (ms)
    1000,  // hold time (ms)
    3000   // long-hold time (ms)
);
```

#### 2. Action System (`src/classes/actions/`)

Non-blocking action execution with state machine pattern.

**Base Class: `AbstractAction`**

```cpp
class AbstractAction {
    virtual void start() = 0;
    virtual ActionState update() = 0;  // Non-blocking step
    virtual void reset() = 0;
    virtual void stop() = 0;
    virtual bool isValid() const = 0;
    virtual ExecutionBehavior getExecutionBehavior() const = 0;
};
```

**Action States:**
- `NOT_STARTED` - Initial state
- `IN_PROGRESS` - Currently executing
- `COMPLETED` - Successfully finished
- `FAILED` - Execution failed

**Execution Behaviors:**
- `QUEUE` - Add to queue for sequential execution
- `IMMEDIATE_EXCLUSIVE` - Execute now, stop all others
- `IMMEDIATE_PARALLEL` - Execute now, run alongside others

**Built-in Actions:**
- `PrintAction` - Send keyboard input via HID
- `LEDColorActions` - Control LED colors (Red, Green, Yellow)
- `LEDRedToGreenAction` - Progressive color transition
- `CombinedAction` - Execute multiple actions together

#### 3. ActionExecutor (`src/classes/action_executor/ActionExecutor.h`)

Manages action lifecycle with circular buffer (max 8 concurrent actions).

```cpp
ActionExecutor executor;
executor.executeAction(action);  // Respects action's ExecutionBehavior
executor.update();               // Call in main loop

// Manual control
executor.queueAction(action);    // Add to queue
executor.startNext();            // Start next queued action
executor.stopAll();              // Stop all running actions
```

**Key Features:**
- FIFO queue management
- Sequential or parallel execution modes
- Progress tracking
- Automatic cleanup of completed actions

#### 4. LED Abstraction (`src/classes/leds/`)

**Interface: `ILed`**
```cpp
class ILed {
    virtual bool setState(const LEDState& state) = 0;
    virtual LEDState getState() const = 0;
};
```

**RGLed Implementation:**
```cpp
RGLed statusLED;
statusLED.setup(RED_PIN, GREEN_PIN);
statusLED.setState(LEDState::red());
statusLED.setState(LEDState::green());
statusLED.setState(LEDState::yellow());  // Red + Green
statusLED.turnOff();
```

### System Flow

```
main.cpp
  │
  ├── setup()
  │   ├── Initialize hardware (Keyboard, Button, LEDs)
  │   ├── Create action objects (static allocation)
  │   ├── Assign actions to button events
  │   └── Start startup sequence (LED red→green)
  │
  └── loop()
      ├── executor.update()      // Update all running actions
      ├── button.loop()          // Check button state, trigger events
      └── monitorSystemHealth()  // Error tracking, recovery
```

## Example: Adding Custom Action

```cpp
// 1. Define action class
class MyAction : public BaseAction {
public:
    void start() override {
        state = ActionState::IN_PROGRESS;
        // Initialize
    }

    ActionState update() override {
        if (/* condition */) {
            state = ActionState::COMPLETED;
        }
        return state;
    }

    bool isValid() const override {
        return true;
    }

    ExecutionBehavior getExecutionBehavior() const override {
        return ExecutionBehavior::IMMEDIATE_PARALLEL;
    }
};

// 2. In setup()
static MyAction myActionObj;
myAction = &myActionObj;

// 3. Assign to button event
button.setOnClickAction(myAction);
```

## Configuration

### Hardware Pins (`src/hardware_config.h`)
```cpp
namespace HardwareConfig {
    constexpr uint8_t BUTTON_PIN = 3;
    constexpr uint8_t RGLED_RED_PIN = 9;
    constexpr uint8_t RGLED_GREEN_PIN = 6;

    // Error handling
    constexpr uint8_t MAX_CONSECUTIVE_ERRORS = 5;
    constexpr unsigned long ERROR_RESET_INTERVAL_MS = 30000;
    constexpr unsigned long RECOVERY_ATTEMPT_INTERVAL_MS = 10000;
}
```

### Version Info (`src/version.h`)
```cpp
constexpr char VERSION[] PROGMEM = "0.0.1";
constexpr char PROJECT_NAME[] PROGMEM = "for-eyal";
```

## Error Handling

The system includes comprehensive error handling:

1. **Consecutive Error Tracking**: Counts errors within 30-second window
2. **Error State**: Visual indication via blinking red LED
3. **Auto-Recovery**: Attempts system recovery every 10 seconds
4. **Graceful Degradation**: System enters safe mode after 5 consecutive errors

**Error State Behavior:**
- LED blinks red at 500ms intervals
- Button input disabled
- Recovery attempts logged (if DEBUG enabled)

## Development

### Project Structure
```
ArduinoButtons/
├── src/
│   ├── main.cpp                    # Main application
│   ├── hardware_config.h           # Pin assignments
│   ├── version.h                   # Version info
│   └── classes/
│       ├── Button.{h,cpp}          # Button state machine
│       ├── action_executor/        # Action execution system
│       ├── actions/                # Action implementations
│       └── leds/                   # LED abstractions
├── platformio.ini                  # Build configuration
└── README.md
```

### Extending the System

**Add New Button Event:**
1. Define event type in `Button.h` (EventType enum)
2. Add action pointer member
3. Implement setter method with validation
4. Add event detection logic in `checkButton()`
5. Create event handler method

**Add New Action Type:**
1. Inherit from `BaseAction` or `AbstractAction`
2. Implement state machine methods (start, update, reset, stop)
3. Define execution behavior
4. Add validation logic

**Add New LED Type:**
1. Inherit from `ILed` interface
2. Implement `setState()` and `getState()`
3. Define hardware setup method
4. Create corresponding LED action classes

## Memory Considerations

- **Static Allocation**: All objects created at compile time (no heap fragmentation)
- **Circular Buffer**: ActionExecutor uses fixed 8-slot buffer
- **PROGMEM**: Strings stored in flash memory (VERSION, PROJECT_NAME)
- **Small Footprint**: Designed for ATmega32u4 (32KB flash, 2.5KB RAM)

## Building and Testing

### Build Environments

**Release Build** (`leonardo`):
```bash
pio run -e leonardo
pio run -e leonardo --target upload
```

**Debug Build** (`leonardo-debug`):
```bash
# Build with debug output enabled
pio run -e leonardo-debug --target upload

# Monitor serial output
pio device monitor -b 9600
```

### VS Code PlatformIO Extension

**Build:**
1. Open project in VS Code
2. Click PlatformIO icon (alien head) in left sidebar
3. Expand "PROJECT TASKS" → "leonardo" (or "leonardo-debug")
4. Click "Build"

**Upload:**
1. Connect Arduino Pro Micro via USB
2. PROJECT TASKS → "leonardo" → "Upload"

**Monitor Serial:**
1. PROJECT TASKS → "leonardo-debug" → "Monitor"
2. Or bottom toolbar: click "Serial Monitor" icon

**Run Tests:**
1. PROJECT TASKS → "native" → "Advanced" → "Test"
2. Or terminal: `pio test -e native`

### Debug Output

The `DEBUG` flag is automatically defined in the `leonardo-debug` environment. Debug output includes:
- System initialization status
- Button event triggers
- Action execution flow
- Error conditions and recovery attempts

### Running Unit Tests

Tests run natively (not on hardware) using mocked Arduino functions.

**Prerequisites (Ubuntu/WSL):**
```bash
# Install GCC compiler
sudo apt-get update
sudo apt-get install build-essential -y

# Install PlatformIO
pip3 install platformio
```

**Run tests:**
```bash
# Run all native tests
pio test -e native

# Verbose output
pio test -e native -vvv
```

**Test Coverage:**
- 18 unit tests total
- Button state machine: initialization, action validation, hold/long-hold detection, push/pop events
- ActionExecutor: queue management, execution behaviors, buffer overflow, progress tracking

**Note:** Single-click and double-click tests are disabled due to complex timing requirements with the button state machine debouncing logic. These events are validated through manual hardware testing.

## Known Limitations

1. **Fixed Action Buffer**: Max 8 concurrent actions in ActionExecutor
2. **Single Board Support**: Currently ATmega32u4 only
3. **No Interrupt Support**: All timing uses polling in main loop
4. **Serial Debugging**: Requires USB connection for debug output

## Contributing

This is a personal project. Issues and pull requests welcome at:
https://github.com/RuslanGilyazov/ArduinoButtons

## License

See repository for license information.

## Author

Ruslan Gilyazov
