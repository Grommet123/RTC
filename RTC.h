/*
 * RTC.h
 *
 * Created: 3/29/2016 1:50:21 PM
 *  Author: Gary Grotsky
 */ 


#ifndef RTC_h
#define RTC_h
#include "Arduino.h"

//#define DEBUG  // Uncomment to turn on debug

#define  DHT11_PRESENT

#define VERSION                 "V3.0" // Add RGB LED for temperature warning
#define TEMPERATURE_OFFSET       1.8   // DHT11 reads high (Cheep Chinese garbage)
#define HUMIDITY_OFFSET          6.0   // DHT11 reads low         "
#define BUFF_MAX 128
#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
#define TEMP_HUM_PIN             A1  // A1 is the DHT11 input
#define LCD_BACKLIGHT_PIN        10  // D10 controls LCD back light
#define RED_LED                  22  // D22 is the red LED
#define GREEN_LED                24  // D24 is the green LED
#define BLUE_LED                 26  // D26 is the blue LED
// ADC readings expected for the 5 buttons on the ADC input
#define RIGHT_10BIT_ADC           0  // right
#define UP_10BIT_ADC            131  // up
#define DOWN_10BIT_ADC          307  // down
#define LEFT_10BIT_ADC          480  // left
#define SELECT_10BIT_ADC        720  // select
#define BUTTONHYSTERESIS         10  // hysteresis for valid button sensing window
//return values for ReadButtons()
#define BUTTON_NONE               0
#define BUTTON_RIGHT              1
#define BUTTON_UP                 2
#define BUTTON_DOWN               3
#define BUTTON_LEFT               4
#define BUTTON_SELECT             5
// Used to check the long spelling months
#define SEP                       9
#define NOV                       10
#define DEC                       12
// Temperature settings
#define TEMPERATURE_TO_HIGH       78.0f
#define TEMPERATURE_TO_LOW        65.0f

void parse_cmd(char *cmd, int cmdsize);
void printMonth(int month);
byte ReadButtons();
bool IsDST(int day, int month, int dow);
void DisplaySplashScreen();

#endif
