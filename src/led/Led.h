#ifndef LED_H
#define LED_H

class Led 
{
  public:
    Led(int pin);
    void on();
    void off();
    const char* getName();
    void init();
  private:
    int _pin;
    const char* _name;
};

#endif