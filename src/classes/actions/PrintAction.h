#include <Arduino.h>
#include "AbstractAction.h"

class PrintAction : public AbstractAction {
public:
    PrintAction(String message);
    void execute() override;
private:
    String message;
    void typeString(String string);
};