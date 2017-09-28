/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define STROBE 8
#define CLOCK 9
#define SETIO1 5
#define SETIO2 6
#define SETIO3 7
#define BUTTON_HOLD 200
#define BUTTON_CLICK 10

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void initTM1638()
{
  pinMode(STROBE, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(SETIO1, OUTPUT);
  pinMode(SETIO2, OUTPUT);
  pinMode(SETIO3, OUTPUT);
  //
  digitalWrite(STROBE, HIGH);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(SETIO1, LOW);
  digitalWrite(SETIO2, LOW);
  digitalWrite(SETIO3, LOW);
  //
  sendData(0x8F); // Init
  //
  sendData(0x40); // set auto increment mode
  digitalWrite(STROBE, LOW);
  sendDataConst(0xc0, 0xc0, 0xc0); // Start Address
  digitalWrite(SETIO1, LOW);
  digitalWrite(SETIO2, LOW);
  digitalWrite(SETIO3, LOW);
  for(uint8_t i = 0; i < (16 * 8); i++)
  {
    digitalWrite(CLOCK, LOW);
    digitalWrite(CLOCK, HIGH);
  }
  digitalWrite(STROBE, HIGH);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void sendData(byte data)
{
  digitalWrite(STROBE, LOW);
  for (byte x=0; x<8; x++)
  {
    digitalWrite(CLOCK, LOW);
    byte dataOut = bitRead(data, x);
    digitalWrite(SETIO1, dataOut);
    digitalWrite(SETIO2, dataOut);
    digitalWrite(SETIO3, dataOut);
    digitalWrite(CLOCK, HIGH);
  }
  digitalWrite(STROBE, HIGH);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void sendDataConst(byte data1, byte data2, byte data3)
{
  for (byte x=0; x<8; x++)
  {
    digitalWrite(CLOCK, LOW);
    digitalWrite(SETIO1, bitRead(data1, x));
    digitalWrite(SETIO2, bitRead(data2, x));
    digitalWrite(SETIO3, bitRead(data3, x));
    digitalWrite(CLOCK, HIGH);
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void sendScreen()
{
  sendData(0x40); // set auto increment mode
  digitalWrite(STROBE, LOW);
  sendDataConst(0xc0, 0xc0, 0xc0); // Start Address
  for (byte x=0; x<8; x++)
  {
    sendDataConst(segments[0][x], segments[1][x], segments[2][x]);
    sendDataConst(bitRead(leds[0],x), bitRead(leds[1],x), bitRead(leds[2],x));
  }
  digitalWrite(STROBE, HIGH);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*void sendPartialScreenSegment(byte address, byte segment)
{
  sendData(0x44);
  digitalWrite(STROBE, LOW);
  sendDataConst(0xc0 + address, 0xc0 + address, 0xc0 + address); // Start Address
  sendDataConst(segments[0][segment], segments[1][segment], segments[2][segment]);
  digitalWrite(STROBE, HIGH);
}*/

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void readButtons(void)
{
  digitalWrite(STROBE, LOW);
  sendDataConst(0x42, 0x42, 0x42); // Request Button States
  //
  pinMode(SETIO1, INPUT);
  pinMode(SETIO2, INPUT);
  pinMode(SETIO3, INPUT);
  //
  buttons[0] = buttons[1] = buttons[2] = 0x00;
  for (byte i = 0; i < 4; i++)
  {
    byte v[3] = { 0x00, 0x00, 0x00 };
    for (byte x=0; x<8; x++)
    {
      digitalWrite(CLOCK, LOW);
      if (digitalRead(SETIO1)) bitSet(v[0], x);
      if (digitalRead(SETIO2)) bitSet(v[1], x);
      if (digitalRead(SETIO3)) bitSet(v[2], x);
      digitalWrite(CLOCK, HIGH);
    }
    buttons[0] |= v[0] << i;
    buttons[1] |= v[1] << i;
    buttons[2] |= v[2] << i;
  }
  // 
  pinMode(SETIO1, OUTPUT);
  pinMode(SETIO2, OUTPUT);
  pinMode(SETIO3, OUTPUT);
  digitalWrite(STROBE, HIGH);
  //
  //uint32_t t = millis();
  for (byte x=0; x < 8; x++)
  {
    for (byte i = 0; i < 3; i++)
    {
      if (bitRead(buttons[i], x) && (!ignoreButtons || (i == 0 && x <= 1 && curRightScreen != kMuteSoloMenu) || (i == 2 && x == 7 && curRightScreen == kMuteSoloMenu)))
      {
        if (buttonDownTime[i][x] < BUTTON_HOLD) buttonDownTime[i][x]++;
        if ((buttonDownTime[i][x] >= BUTTON_HOLD && !bitRead(buttonEventWasHolding[i], x)) ||
            (i == 0 && curRightScreen != kMuteSoloMenu && (x >= 6 || x == 4) && buttonDownTime[i][x] > BUTTON_CLICK && !bitRead(buttonEventWasHolding[i],x)))
        {
          somethingClicked = false;
          bitSet(buttonEventWasHolding[i], x);
          if (i == 0 && curRightScreen != kMuteSoloMenu && (x == 2 || x == 3))
          {
            buttonEvent[i][x] = kButtonClicked;
            bitClear(buttonEventWasHolding[i], x);
          }
          else 
          {
            buttonEvent[i][x] = kButtonHold;
          }
          somethingHappened = true;
        }
      }
      else
      {
        if (buttonDownTime[i][x] > 0)
        {
          if ((buttonDownTime[i][x]) > BUTTON_CLICK)
          {
            if ((buttonDownTime[i][x]) >= BUTTON_HOLD || bitRead(buttonEventWasHolding[i], x))
            {
              if (somethingClicked) buttonEvent[i][x] = kButtonRelease; else buttonEvent[i][x] = kButtonReleaseNothingClicked;
              if (buttonEvent[i][x] == kButtonReleaseNothingClicked && i == 0 && x == 6 && curRightScreen != kMuteSoloMenu && buttonDownTime[i][x] >= BUTTON_HOLD) buttonEvent[i][x] = kButtonRelease;
              somethingHappened = true;
            }
            else 
            {
              if ((buttonDownTime[i][x]) < BUTTON_HOLD) 
              {
                buttonEvent[i][x] = kButtonClicked;
                somethingHappened = true;
              }
              somethingClicked = true;
            }
          }
        }
        buttonDownTime[i][x] = 0;
        bitClear(buttonEventWasHolding[i], x);
      }
    }
  }
}

const byte numbers[10] PROGMEM = { B00111111, B00000110, B01011011, B01001111, B01100110, B01101101, B01111101, B00000111, B01111111, B01100111};
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void printNumber(byte segment, byte offset, int number)
{
  int x = number / 100;
  segments[segment][offset] = (char)pgm_read_word(&numbers[0 + x]);
  number -= x * 100;
  //
  x = number / 10;
  segments[segment][offset + 1] = (char)pgm_read_word(&numbers[0 + x]);
  number -= x * 10;
  //
  segments[segment][offset + 2] = (char)pgm_read_word(&numbers[0 + number]);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SHOW_FREE_RAM
  extern unsigned int __data_start;
  extern unsigned int __data_end;
  extern unsigned int __bss_start;
  extern unsigned int __bss_end;
  extern unsigned int __heap_start;
  extern void *__brkval;
  
  void freeMemory(void)
  {
    int free_memory;
    if((int)__brkval == 0) free_memory = ((int)&free_memory) - ((int)&__bss_end);
      else free_memory = ((int)&free_memory) - ((int)__brkval);
    if (free_memory > 999)
    {
      int x = free_memory / 1000;
      segments[2][0] = (char)pgm_read_word(&numbers[0 + x]);
      free_memory -= x * 1000;
    }
    printNumber(2, 1, free_memory);
  }
#endif

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
 *    A     --
 *   F B   |  |
 *    G     --
 *   E C   |  |
 *    D     --
 *       X      .
 *       
 *       
 *  XGFEDCBA
 *  
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                  XGFEDCBA
#define S_R        B01110100
#define S_DASH     B01000000
#define S_S        B01101101
#define S_Y        B01100100
#define S_L        B00111000
#define S_O        B00111111
#define S_U        B00111110
#define S_G        B01011111
#define S_F        B01110001
#define S_o        B01011100
#define S_C        B00111001
#define S_N        B00110111
#define S_E        B01111001
#define S_t        B01010000
#define S_n        B01010100
#define S_K        B01110110
#define S_r        B01010100
#define S_T        B00110001
#define S_d        B01011110
#define S_c        B01011000
#define S_A        B01110111
#define S_b        B01111100
#define S_P        B01110011
#define S_I        B00110000
#define S_H        B01110110
#define S__        B00001000
const byte stepChars[4] PROGMEM = { B00000000, B00001000, B01010100, B00110111 };

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void showErrorMsg(byte error)
{
  memset(segments, 0, sizeof(segments));
  segments[2][0] = S_E;
  segments[2][1] = S_r;
  segments[2][2] = S_r;
  segments[2][3] = S_o;
  segments[2][4] = S_r;
  printNumber(2, 5, error);
  sendScreen();
  delay(4000);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void showWaitMsg(char porcentage)
{
  memset(segments, 0, sizeof(segments));
  segments[2][0] = 0x3c;
  segments[2][1] = 0x1e;
  segments[2][2] = S_A;
  segments[2][3] = S_I;
  segments[2][4] = S_T;
  if (porcentage >=0) printNumber(2, 5, porcentage);
  sendScreen();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void printDashDash(byte segment, byte offset)
{
  segments[segment][offset] = S_DASH;
  segments[segment][offset + 1] = S_DASH;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void printMIDInote(byte note, byte segment, byte offset, byte offsetOctave)
{
  byte xNote = note - ((note / 12) * 12);
  switch (xNote)
  {
    case 0: segments[segment][offset] = S_C;                                         break;
    case 1: segments[segment][offset] = S_C;  segments[segment][offset + 1] = S_H;   break;
    case 2: segments[segment][offset] = S_d;                                         break;
    case 3: segments[segment][offset] = S_d;  segments[segment][offset + 1] = S_H;   break;
    case 4: segments[segment][offset] = S_E;                                         break;
    case 5: segments[segment][offset] = S_F;                                         break;
    case 6: segments[segment][offset] = S_F;  segments[segment][offset + 1] = S_H;   break;
    case 7: segments[segment][offset] = S_G;                                         break;
    case 8: segments[segment][offset] = S_G;  segments[segment][offset + 1] = S_H;   break;
    case 9: segments[segment][offset] = S_A;                                         break;
    case 10: segments[segment][offset] = S_A; segments[segment][offset + 1] = S_H;   break;
    case 11: segments[segment][offset] = S_b;                                        break;
  }
  //
  byte xn = (note / 12);
  if (xn >= 10) xn = 9;
  segments[segment][offsetOctave] = (char)pgm_read_word(&numbers[xn]);
}

