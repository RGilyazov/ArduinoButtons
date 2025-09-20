#ifndef ABSTRACT_ACTION_H
#define ABSTRACT_ACTION_H

// Action execution result codes
enum class ActionResult : uint8_t {
    SUCCESS = 0,
    FAILED_HARDWARE_ERROR = 1,
    FAILED_INVALID_STATE = 2,
    FAILED_TIMEOUT = 3
};

class AbstractAction {
public:
    virtual ~AbstractAction() = default; // Virtual destructor for proper cleanup
    virtual ActionResult execute() = 0;   // Pure virtual function with error reporting
    virtual bool isValid() const = 0;     // Validation method
};

#endif