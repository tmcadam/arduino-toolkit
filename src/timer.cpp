#include <timer.h>

void Timer::setPeriod(unsigned long _period, unsigned long _offset) {
    this->offset = _offset;
    this->period = _period;
    this->start();
}

void Timer::setAction(GeneralCallbackFunction _action) {
    this->action = _action;
}

void Timer::setTimer(unsigned long _period, GeneralCallbackFunction _action, unsigned long _offset) {
    this->setPeriod(_period, _offset);
    this->setAction(_action);
}

void Timer::stop() {
    this->enabled = false;
}

void Timer::start(){
    this->lastAction = millis();
    this->enabled = true;
}

void Timer::update() {
    unsigned long currMillis = millis() - this->offset;
    if ((currMillis - this->lastAction) >= this->period && this->enabled) {
        this->lastAction = currMillis;
        this->action();
    }
}
