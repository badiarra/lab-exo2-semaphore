#include "Led.h"
#include <Arduino.h>

Led::Led(int pin, const char* nom){
    _pin = pin;
    _name = nom;
}

void Led::init() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void Led::on() {
    digitalWrite(_pin, HIGH);
}

void Led::off() {
    digitalWrite(_pin, LOW);
}


const char* Led::getName() {
    return _name;
}
