#ifndef DISPLAY_H
#define DISPLAY_H

#include "definitions.h"
#include <Adafruit_SSD1306.h>

class Display{
private:
    Adafruit_SSD1306    _display;
    unsigned long       _previous_millis;
    bool                _sleeping;

public:
    Display();
    void begin();
    void sleep(unsigned long current_millis);
    void wake(unsigned long current_millis);
};


#endif