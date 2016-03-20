/*
  RTC
  Used a RTC module along with a LCD Shield to display
  date, time, temperature (either in F or C), DST and DOW.
  Hacked from many places.  Thanks all.

  A work in progress.

*/
//#define DEBUG  // Un-comment to turn on debug

#include <Wire.h>
#include "ds3231.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define BUFF_MAX 128
#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
#define LCD_BACKLIGHT_PIN        10  // D10 controls LCD backlight
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
//#define LED_RED                   13
//#define LED_GREEN                 4
//#define LED_BLUE                  10

uint8_t time[8];
char recv[BUFF_MAX];
unsigned int recv_size = 0;
unsigned long prev, interval = 1000;
byte buttonJustPressed  = false;         //this will be true after a ReadButtons() call if triggered
byte buttonJustReleased = false;         //this will be true after a ReadButtons() call if triggered
byte buttonWas          = BUTTON_NONE;   //used by ReadButtons() for detection of button events

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
#endif
  //button adc input
  pinMode( BUTTON_ADC_PIN, INPUT );         //ensure A0 is an input
  digitalWrite( BUTTON_ADC_PIN, LOW );      //ensure pullup is off on A0
  //lcd backlight control
  pinMode( LCD_BACKLIGHT_PIN, OUTPUT );     //D10 is an output
  digitalWrite( LCD_BACKLIGHT_PIN, HIGH );  //backlight control pin D10 is high (on)
  //  pinMode(LED_RED, OUTPUT );     //Red LED
  //  pinMode(LED_GREEN, OUTPUT );   //Green LED
  //  pinMode(LED_BLUE, OUTPUT );    //Blue LED
  //  digitalWrite(LED_RED, HIGH);
  //  digitalWrite(LED_GREEN, LOW);
  //  digitalWrite(LED_BLUE, LOW );

  Wire.begin();
  DS3231_init(DS3231_INTCN);
  memset(recv, 0, BUFF_MAX);
#ifdef DEBUG
  Serial.println("GET time");
#endif

  lcd.begin(16, 2);
  lcd.clear();

  //  Serial.println("Setting time");
  //    parse_cmd("T005911020032016", 16);
  //                ssmmhhWDDMMYYYY
}

void loop()
{
  char in;
  char tempF[6];
  float temperature;
  char buff[BUFF_MAX];
  unsigned long now = millis();
  struct ts t;
  byte button;
  byte timestamp;
  static bool buttonSelect = true;
  static bool buttonRight = true;
  static bool buttonLeft = false;
  static bool pastButtonSelect = true;
  char fc;
  char AMPM;

  //get the latest button pressed, also the buttonJustPressed, buttonJustReleased flags
  button = ReadButtons();
  //show text label for the button pressed
  switch ( button )
  {
    case BUTTON_NONE:
      {
        break;
      }
    case BUTTON_RIGHT:
      {
        //RIGHT is a special case, it turns the LCD backlight off and on
        buttonRight = !buttonRight;
        if (!buttonRight) {
          digitalWrite( LCD_BACKLIGHT_PIN, LOW );    //Turn on backlight
        }
        else {
          digitalWrite( LCD_BACKLIGHT_PIN, HIGH );   //Turn off backlight
        }
        delay (500);
        break;
      }
    case BUTTON_UP:
      {
        //        Not ueed for now
        break;
      }
    case BUTTON_DOWN:
      {
        break;
      }
    case BUTTON_LEFT:
      {
        //LEFT is a special case, it displays DST and DOW
        buttonLeft = !buttonLeft;
        pastButtonSelect = buttonSelect;
        delay (500);
        break;
      }
    case BUTTON_SELECT:
      {
        buttonSelect = !buttonSelect;
        buttonLeft = false;
        delay (500);
        break;
      }
    default:
      {
        break;
      }
  }

  // show time once in a while
  if ((now - prev > interval) && (Serial.available() <= 0)) {
    DS3231_get(&t); //Get time
    // Check for DST
    if ((IsDST(t.mday, t.mon, t.wday))) {
      --t.hour;
#ifdef DEBUG
      Serial.println("It is DST");
      Serial.println();
#endif
    }
    parse_cmd("C", 1);
    temperature = DS3231_get_treg(); //Get temperature

    if (buttonSelect) {
      temperature = (temperature * 9 / 5) + 32; //(C * 9/5) +32 = F
      fc = 'F';
    }
    else {
      fc = 'C';
    }
    if (buttonSelect) {
      dtostrf(temperature, 5, 1, tempF);
    }
    else {
      dtostrf(temperature, 4, 1, tempF);
    }
#ifdef DEBUG
    if (t.mon < 10)
      Serial.print('0');
    Serial.print(t.mon);
    Serial.print('/');
    if (t.mday < 10)
      Serial.print('0');
    Serial.print(t.mday);
    Serial.print('/');
    Serial.print(t.year);
    Serial.print(' ');
    if (t.hour < 10)
      Serial.print('0');
    Serial.print(t.hour);
    Serial.print(':');
    if (t.min < 10)
      Serial.print('0');
    Serial.print(t.min);
    Serial.print(':');
    if (t.sec < 10)
      Serial.print('0');
    Serial.print(t.sec);
    Serial.println();
    Serial.print("DOW = ");
    Serial.println(t.wday);
#endif

    // This is where the LCD display is handled
    if (buttonLeft) {
      lcd.clear();
      if ((IsDST(t.mday, t.mon, t.wday))) {
        lcd.setCursor(0, 0);
        lcd.print("It is DST");
      }
      else {
        lcd.setCursor(0, 0);
        lcd.print("It is not DST");
      }
      lcd.setCursor(0, 1);
      lcd.print("DOW = ");
      lcd.print(t.wday);
      prev = now;
    }
    else {
      lcd.clear();
      lcd.setCursor(1, 0);

      lcd.print(t.mday);

      printMonth(t.mon);

      lcd.print(t.year);

      lcd.setCursor(0, 1); //Go to second line of the LCD Screen

      if (buttonSelect) {

        if (t.hour >= 12) {
          if (t.hour > 12) {
            t.hour -= 12;
          }
          AMPM = 'P';
        }
        else {
          AMPM = 'A';
        }
      }

      if (t.hour < 10)
      {
        lcd.print("0");
      }
      lcd.print(t.hour);
      lcd.print(":");
      if (t.min < 10)
      {
        lcd.print("0");
      }
      lcd.print(t.min);
      lcd.print(":");
      if (t.sec < 10)
      {
        lcd.print("0");
      }
      lcd.print(t.sec);
      if (buttonSelect) {
        lcd.print(AMPM);
      }
      else {
        lcd.print("  ");
      }
      //    lcd.print(' ');
      lcd.print(tempF);
      lcd.print((char)223);
      lcd.print(fc);
      prev = now;
    }
  }
}
void parse_cmd(char *cmd, int cmdsize)
{
  uint8_t i;
  uint8_t reg_val;
  char buff[BUFF_MAX];
  struct ts t;

  //snprintf(buff, BUFF_MAX, "cmd was '%s' %d\n", cmd, cmdsize);
  //Serial.print(buff);

  // TssmmhhWDDMMYYYY aka set time
  if (cmd[0] == 84 && cmdsize == 16) {
    //T355720619112011
    t.sec = inp2toi(cmd, 1);
    t.min = inp2toi(cmd, 3);
    t.hour = inp2toi(cmd, 5);
    t.wday = inp2toi(cmd, 7);
    t.mday = inp2toi(cmd, 8);
    t.mon = inp2toi(cmd, 10);
    t.year = inp2toi(cmd, 12) * 100 + inp2toi(cmd, 14);
    DS3231_set(t);
#ifdef DEBUG
    Serial.println("OK");
#endif
  } else if (cmd[0] == 49 && cmdsize == 1) {  // "1" get alarm 1
    DS3231_get_a1(&buff[0], 59);
#ifdef DEBUG
    Serial.println(buff);
#endif
  } else if (cmd[0] == 50 && cmdsize == 1) {  // "2" get alarm 1
    DS3231_get_a2(&buff[0], 59);
#ifdef DEBUG
    Serial.println(buff);
#endif
  } else if (cmd[0] == 51 && cmdsize == 1) {  // "3" get aging register
#ifdef DEBUG
    Serial.print("aging reg is ");
    Serial.println(DS3231_get_aging(), DEC);
#endif
  } else if (cmd[0] == 65 && cmdsize == 9) {  // "A" set alarm 1
    DS3231_set_creg(DS3231_INTCN | DS3231_A1IE);
    //ASSMMHHDD
    for (i = 0; i < 4; i++) {
      time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // ss, mm, hh, dd
    }
    byte flags[5] = { 0, 0, 0, 0, 0 };
    DS3231_set_a1(time[0], time[1], time[2], time[3], flags);
    DS3231_get_a1(&buff[0], 59);
#ifdef DEBUG
    Serial.println(buff);
#endif
  } else if (cmd[0] == 66 && cmdsize == 7) {  // "B" Set Alarm 2
    DS3231_set_creg(DS3231_INTCN | DS3231_A2IE);
    //BMMHHDD
    for (i = 0; i < 4; i++) {
      time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // mm, hh, dd
    }
    byte flags[5] = { 0, 0, 0, 0 };
    DS3231_set_a2(time[0], time[1], time[2], flags);
    DS3231_get_a2(&buff[0], 59);
#ifdef DEBUG
    Serial.println(buff);
#endif
  } else if (cmd[0] == 67 && cmdsize == 1) {  // "C" - get temperature register
#ifdef DEBUG
    Serial.print("temperature is ");
    Serial.print((DS3231_get_treg() * 9 / 5) + 32, DEC); // F = (C* 9/5) + 32
    Serial.println(" Degrees Farenheit");
#endif
  } else if (cmd[0] == 68 && cmdsize == 1) {  // "D" - reset status register alarm flags
    reg_val = DS3231_get_sreg();
    reg_val &= B11111100;
    DS3231_set_sreg(reg_val);
  } else if (cmd[0] == 70 && cmdsize == 1) {  // "F" - custom fct
    reg_val = DS3231_get_addr(0x5);
#ifdef DEBUG
    Serial.print("orig ");
    Serial.print(reg_val, DEC);
    Serial.print("\tmonth is ");
    Serial.println(bcdtodec(reg_val & 0x1F), DEC);
#endif
  } else if (cmd[0] == 71 && cmdsize == 1) {  // "G" - set aging status register
    DS3231_set_aging(0);
  } else if (cmd[0] == 83 && cmdsize == 1) {  // "S" - get status register
#ifdef DEBUG
    Serial.print("status reg is ");
    Serial.println(DS3231_get_sreg(), DEC);
#endif
  } else {
#ifdef DEBUG
    Serial.print("unknown command prefix ");
    Serial.println(cmd[0]);
    Serial.println(cmd[0], DEC);
#endif
  }
}

void printMonth(int month)
{
  switch (month)
  {
    case 1: lcd.print(" January "); break;
    case 2: lcd.print(" February "); break;
    case 3: lcd.print(" March "); break;
    case 4: lcd.print(" April "); break;
    case 5: lcd.print(" May "); break;
    case 6: lcd.print(" June "); break;
    case 7: lcd.print(" July "); break;
    case 8: lcd.print(" August "); break;
    case 9: lcd.print(" September "); break;
    case 10: lcd.print(" October "); break;
    case 11: lcd.print(" November "); break;
    case 12: lcd.print(" December "); break;
    default: lcd.print(" Error "); break;
  }
}

/*--------------------------------------------------------------------------------------
  ReadButtons()
  Detect the button pressed and return the value
  Uses global values buttonWas, buttonJustPressed, buttonJustReleased.
  --------------------------------------------------------------------------------------*/
byte ReadButtons()
{
  unsigned int buttonVoltage;
  byte button = BUTTON_NONE;   // return no button pressed if the below checks don't write to btn

  //read the button ADC pin voltage
  buttonVoltage = analogRead( BUTTON_ADC_PIN );
  //sense if the voltage falls within valid voltage windows
  if ( buttonVoltage < ( RIGHT_10BIT_ADC + BUTTONHYSTERESIS ) )
  {
    button = BUTTON_RIGHT;
  }
  else if (   buttonVoltage >= ( UP_10BIT_ADC - BUTTONHYSTERESIS )
              && buttonVoltage <= ( UP_10BIT_ADC + BUTTONHYSTERESIS ) )
  {
    button = BUTTON_UP;
  }
  else if (   buttonVoltage >= ( DOWN_10BIT_ADC - BUTTONHYSTERESIS )
              && buttonVoltage <= ( DOWN_10BIT_ADC + BUTTONHYSTERESIS ) )
  {
    button = BUTTON_DOWN;
  }
  else if (   buttonVoltage >= ( LEFT_10BIT_ADC - BUTTONHYSTERESIS )
              && buttonVoltage <= ( LEFT_10BIT_ADC + BUTTONHYSTERESIS ) )
  {
    button = BUTTON_LEFT;
  }
  else if (   buttonVoltage >= ( SELECT_10BIT_ADC - BUTTONHYSTERESIS )
              && buttonVoltage <= ( SELECT_10BIT_ADC + BUTTONHYSTERESIS ) )
  {
    button = BUTTON_SELECT;
  }
  //handle button flags for just pressed and just released events
  if ( ( buttonWas == BUTTON_NONE ) && ( button != BUTTON_NONE ) )
  {
    //the button was just pressed, set buttonJustPressed, this can optionally be used to trigger a once-off action for a button press event
    //it's the duty of the receiver to clear these flags if it wants to detect a new button change event
    buttonJustPressed  = true;
    buttonJustReleased = false;
  }
  if ( ( buttonWas != BUTTON_NONE ) && ( button == BUTTON_NONE ) )
  {
    buttonJustPressed  = false;
    buttonJustReleased = true;
  }

  //save the latest button value, for change event detection next time round
  buttonWas = button;

  return ( button );
}

// Check to see if it's DST
// http://stackoverflow.com/questions/5590429/calculating-daylight-saving-time-from-only-date
bool IsDST(int day, int month, int dow)
{
  //January, february, and december are out.
  if (month < 3 || month > 11) {
    return false;
  }
  //April to October are in
  if (month > 3 && month < 11) {
    return true;
  }
  int previousSunday = day - dow;
  //In march, we are DST if our previous sunday was on or after the 8th.
  if (month == 3) {
    return previousSunday >= 8;
  }
  //In november we must be before the first sunday to be dst.
  //That means the previous sunday must be before the 1st.
  return previousSunday <= 0;
}
