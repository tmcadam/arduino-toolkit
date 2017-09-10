#include <timer.h>

void Timer::setPeriod(unsigned long _period) {
    this->lastAction = 0;
    this->period = _period;
    this->start();
}

void Timer::setAction(GeneralCallbackFunction _action) {
    this->action = _action;
}

void Timer::setTimer(unsigned long _period, GeneralCallbackFunction _action) {
    this->setPeriod(_period);
    this->setAction(_action);
}

void Timer::stop() {
    this->enabled = false;
}

void Timer::start(){
    this->enabled = true;
}

void Timer::update() {
    unsigned long currMillis = millis();
    if ((currMillis - this->lastAction) >= this->period && this->enabled) {
        this->lastAction = currMillis;
        this->action();
    }
}
