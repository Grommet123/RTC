/*
 * RTC.h
 *
 * Created: 3/29/2016 1:50:21 PM
 *  Author: Gary
 */ 


#ifndef RTC_h
#define RTC_h

//#define DEBUG  // Uncomment to turn on debug

#define  DHT11_PRESENT

#define VERSION                 "V1.0" // Initial Release
#define TEMPERATURE_OFFSET       1.8   // DHT11 reads high (Cheep Chinese garbage)
#define HUMIDITY_OFFSET          6.0   // DHT11 reads low         "
#define BUFF_MAX 128
#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
#define TEMP_HUM_PIN             A1  // A1 is the DHT11 input
#define LCD_BACKLIGHT_PIN        10  // D10 controls LCD back light
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

#endif
