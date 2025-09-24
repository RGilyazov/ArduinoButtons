#ifndef ACTION_EXECUTOR_H
#define ACTION_EXECUTOR_H

#include <Arduino.h>
#include "../actions/AbstractAction.h"

class ActionExecutor {
public:
    ActionExecutor();
    ~ActionExecutor() = default;
    
    // Immediate execution methods
    bool executeAction(AbstractAction* action, bool stopOthers = false);   // Execute action immediately (parallel by default)
    
    // Queue-based methods with proper FIFO behavior
    bool queueAction(AbstractAction* action);              // Add action to queue (FIFO order)
    void clear();                                          // Clear all actions (stop and remove)
    
    // Execution control
    void update();                                         // Update all actions (call in main loop)
    void startNext();                                      // Start next queued action (FIFO order)
    void startAllQueued();                                 // Start all queued actions simultaneously
    void stopAll();                                        // Stop all running actions
    
    // Execution modes (for queued actions)
    void setSequentialMode(bool sequential);               // true = one at a time, false = all at once
    bool isSequentialMode() const;
    
    // State inquiry
    uint8_t getActionCount() const;                        // Get number of active actions (running + queued)
    uint8_t getRunningActionCount() const;                 // Get number of currently running actions
    uint8_t getQueuedActionCount() const;                  // Get number of queued (not started) actions
    bool hasQueuedActions() const;                         // Any actions waiting to start?
    bool hasRunningActions() const;                        // Any actions currently running?
    bool hasCompletedActions() const;                      // Any actions finished (ready for cleanup)?
    bool isEmpty() const;                                  // No actions at all?
    
    // Progress tracking
    uint8_t getTotalActionCount() const;                   // Total actions added (including completed)
    uint8_t getCompletedActionCount() const;               // How many actions have finished
    uint8_t getProgressPercent() const;                    // Overall progress percentage
    
    // Debugging/monitoring
    AbstractAction* getCurrentRunningAction() const;        // Get first running action
    AbstractAction* getRunningAction(uint8_t index) const; // Get specific running action by index
    void printStatus() const;                              // Debug output (if Serial available)

private:
    static constexpr uint8_t MAX_ACTIONS = 8;              // Maximum actions in circular buffer
    static constexpr int8_t INVALID_SLOT = -1;             // Return value for failed operations
    
    struct ActionSlot {
        AbstractAction* action;
        bool isActive;                                     // Is this slot in use?
        bool hasStarted;                                   // Has this action been started?
        bool isQueued;                                     // Is this action queued (vs immediately executed)
        
        ActionSlot() : action(nullptr), isActive(false), hasStarted(false), isQueued(false) {}
        
        void reset() {
            action = nullptr;
            isActive = false;
            hasStarted = false;
            isQueued = false;
        }
    };
    
    // Single circular buffer for all actions
    ActionSlot actionBuffer[MAX_ACTIONS];                  // Circular buffer of action slots
    uint8_t bufferHead;                                    // Index of first item in buffer
    uint8_t bufferTail;                                    // Index where next item will be added
    uint8_t bufferSize;                                    // Number of items in buffer
    uint8_t totalActionsAdded;                             // Total actions added (for progress tracking)
    bool sequentialMode;                                   // Execute queued actions one at a time vs all at once
    
    // Private methods
    void cleanupCompletedActions();                        // Remove finished actions
    int8_t addAction(AbstractAction* action, bool queued); // Add action to circular buffer (returns index or -1)
    uint8_t getNextQueuedIndex() const;                    // Find next queued action in buffer
    bool isBufferFull() const;                            // Check if buffer is full
    bool isBufferEmpty() const;                           // Check if buffer is empty
};

#endif