#include <timer.h>

//-----------------------------Setters----------------------------------------//

void Timer::setPeriod(unsigned long _period, unsigned long _offset) {
    offset = _offset;
    period = _period;
    start();
}

void Timer::setAction(GeneralCallbackFunction _action) {
    action = _action;
}

void Timer::setDelay(unsigned long _delay) {
    delay = _delay;
}

void Timer::setTimer(unsigned long _period, GeneralCallbackFunction _action, unsigned long _offset) {
    setAction(_action);
    setPeriod(_period, _offset);
}

//--------------------------------Actions-------------------------------------//

void Timer::stop() {
    delay = 0;
    enabled = false;
}

void Timer::start() {
    //lastAction = millis();
    hasFirstAction = false;
    hasFirstRun = false;
    enabled = true;
}

void Timer::update() {
    unsigned long currMillis = millis();
    if ( !enabled ) {
        return;
    } else if ( delay > 0 ) {
        if ( !hasFirstRun ) {
            lastAction = currMillis;
            hasFirstRun = true;
        } else if ( hasFirstRun && (currMillis - lastAction) >= delay ) {
            hasFirstRun = false;
            delay = 0;
        }
        return;
    } else if ( !hasFirstAction && offset == 0 ) {
            hasFirstAction = true;
            hasFirstRun = true;
            lastAction = currMillis;
            action();
            return;
    } else if ( (!hasFirstAction || !hasFirstRun) && offset > 0 ) {
        if ( !hasFirstRun ) {
            hasFirstRun = true;
            lastAction = currMillis;
            return;
        } else if ( hasFirstRun && !hasFirstAction && (currMillis - lastAction) >= offset ) {
            hasFirstAction = true;
            lastAction = currMillis;
            action();
            return;
        }
    } else {
        if ( (currMillis - lastAction) >= period ) {
            lastAction = currMillis;
            action();
        }
    }
}
