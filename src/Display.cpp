#include "Display.h"


Display::Display()
    : _display(SCREEN_W, SCREEN_H, &Wire, -1)
{}


Display::begin()
{
    if(!display.begin(SSD1306_SWITCHCAPVCC, D_I2C_ADDR)) {
        Serial.print(F("\n[ERROR]: SSD1306 allocation failed!"));
        for(;;);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setFont(NULL);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.print("Initializing...");
}


void Display::sleep(unsigned long current_millis)
{
    if (current_millis - _previous_millis >= SLEEP_INTERVAL) 
    {
        _previous_millis = current_millis;
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        _sleeping = true;
    }    
}

bool Display::wake(unsigned long current_millis)
{
    _previous_millis = current_millis;
    if (_sleeping) 
    {
        display.ssd1306_command(SSD1306_DISPLAYON);
        _sleeping = false;
        return false;
    }

    return true;
}