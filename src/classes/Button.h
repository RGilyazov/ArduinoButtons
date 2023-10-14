#include <Arduino.h>
#include "actions/AbstractAction.h"

#ifndef Button_h
#define Button_h
#endif

class Button {
public:
    Button();
    void setup(int buttonPin);
    void loop();
    AbstractAction* onClick;
    AbstractAction* onDoubleClick;
    AbstractAction* onHold;
    AbstractAction* onLongHold;
private:
    void clickEvent();
    int checkButton();
    void doubleClickEvent();
    void holdEvent();
    void longHoldEvent();
    int buttonPin;

    boolean buttonVal = HIGH;   // value read from button
    boolean buttonLast = HIGH;  // buffered value of the button's previous state
    boolean DCwaiting = false;  // whether we're waiting for a double click (down)
    boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
    boolean singleOK = true;    // whether it's OK to do a single click
    long downTime = -1;         // time the button was pressed down
    long upTime = -1;           // time the button was released
    boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
    boolean waitForUp = false;        // when held, whether to wait for the up event
    boolean holdEventPast = false;    // whether or not the hold event happened already
    boolean longHoldEventPast = false;// whether or not the long hold event happened already
};