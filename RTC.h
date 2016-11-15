/*
   RTC.h

   Created: 3/29/2016 1:50:21 PM
    Author: Gary Grotsky
*/


#ifndef RTC_h
#define RTC_h
#include <Arduino.h>

//#define DEBUG  // Uncomment to turn on debug
//#define SET_TIME
#define  DHT11_PRESENT

#define VERSION                 "V7.2"  // Fix NOV (was 10)
#define TEMPERATURE_OFFSET       1.5    // DHT11 temperature offset
#define HUMIDITY_OFFSET          3.0    // DHT11 humidity offset
#define BUFF_MAX 128
#define BUTTON_ADC_PIN           A0  // The button ADC input
#define TEMP_HUM_PIN             A1  // The DHT11 input
#define LCD_BACKLIGHT_PIN        10  // The controls LCD back light
#define T_RED_LED                24  // The temperature red LED
#define T_GREEN_LED              26  // The temperature green LED
#define T_BLUE_LED               22  // The temperature blue LED
#define H_RED_LED                49  // The humidity red LED
#define H_GREEN_LED              51  // The humidity green LED
#define H_BLUE_LED               53  // The humidity blue LED
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
#define NOV                       11
#define DEC                       12
// Temperature settings
#define TEMPERATURE_TO_HIGH       78.0f
#define TEMPERATURE_TO_LOW        60.0f
// humidity settings
#define HUMIDITY_TO_HIGH          65.0f
#define HUMIDITY_TO_LOW           45.0f
// Timers
#define DISPLAY_TIMEOUT           5 * 60   // Minutes
#define AVERAGETEMPHUMTIMER       1 * 60   //    "

void parse_cmd(char *cmd, int cmdsize);
void printMonth(int month);
void printDay(int day);
byte ReadButtons();
bool IsDST(int day, int month, int dow);
void DisplaySplashScreen();

#endif  // #define RTC_h
