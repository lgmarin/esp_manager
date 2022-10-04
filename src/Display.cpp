#include "Display.h"

/**
 * @brief Construct a new Display:: Display object based on the SSD1306 display
 * 
 */
Display::Display()
    : _display(SCREEN_W, SCREEN_H, &Wire, -1)
{}


/**
 * @brief Initialize the display, should be inside setup().
 * 
 */
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


/**
 * @brief Shutdown the screen when not being used.
 * 
 * @param current_millis Local counter to be used together with other counters inside loop().
 */
void Display::sleep(unsigned long current_millis)
{
    if (current_millis - _previous_millis >= SLEEP_INTERVAL) 
    {
        _previous_millis = current_millis;
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        _sleeping = true;
    }    
}


/**
 * @brief Wake the screen.
 * 
 * @param current_millis Local counter to be used together with other counters inside loop().
 */
void Display::wake(unsigned long current_millis)
{
    _previous_millis = current_millis;
    if (_sleeping) 
    {
        display.ssd1306_command(SSD1306_DISPLAYON);
        _sleeping = false;
    }
}