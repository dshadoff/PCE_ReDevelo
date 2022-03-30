/*
  ReDevelo_MCU_sketch

  On most boards (including the Adafruit M0 Adalogger), "Serial" refers to the USB
  Serial port attached to the Serial Monitor, and "Serial1" refers to the hardware
  serial port attached to pins 0 and 1. This sketch will emulate Serial passthrough
  using those two Serial ports on the boards mentioned above, but you can change
  these names to connect any two serial ports on a board that has multiple ports.

  This version of the sketch is intended for "rev. A" of the board

  (c) 2022 by David Shadoff
*/


#include <SD.h>

#include "define.h"
#include "develo.h"
// #include "common.h"


// I/Os - these are the Arduino pin numbers
const int clrPin      = 13;  // CLR signal from PCE (Develo) (CLR)
const int selPin      = 12;  // SEL signal from PCE (Develo) (SEL)

const int d3_Pin      = 11;  // data 3 to PCE
const int d2_Pin      = 10;  // data 2 to PCE
const int d1_Pin      = 6;   // data 1 to PCE
const int d0_Pin      = 5;   // data 0 to PCE

const int LED_Pin     = 8;   // green LED
const int TriggerPin  = 1;   // trigger pin


File dataFile;
bool cardPresent;
bool modeMonitor;


void setup()
{
  int i;
  char c;
  unsigned char bankbuf[8];
  unsigned char buf[4096];
  char outbuf[128];
  unsigned long startTime, endTime;

  // setup Serial ports
  Serial.begin(19200);  // computer side

  // setup GPIO pins
  pinMode(d0_Pin,   OUTPUT);
  pinMode(d1_Pin,   OUTPUT);
  pinMode(d2_Pin,   OUTPUT);
  pinMode(d3_Pin,   OUTPUT);
  pinMode(selPin,   INPUT);
  pinMode(clrPin,   INPUT);
  
  pinMode(TriggerPin, INPUT_PULLUP);

  pinMode(LED_Pin,  OUTPUT);

  // set default values on outputs
  //
  write_PCE_port(0x0F);

  delay(4000);  // wait for startup period

//  while(digitalRead(TriggerPin) == HIGH);
//
//  digitalWrite(LED_Pin, HIGH);
//  delay(1000);  // wait for startup period
//  digitalWrite(LED_Pin, LOW);


  // Detect and initialze SDCard access hardware
//  cardPresent = true;
//  if (!SD.begin(chipSelect)) {
//    Serial.println("SD Card failed or not present");
//    cardPresent = false;
//  }

//  Serial.println("Initializing Develo...");
  if (dv_init() != DV_OK) {
    Serial.println("Can not initialize the develo box!");
//    exit(1);
  }

  for (i =0; i < 8; i++) {
    bankbuf[i] = 0x00;
  }

//  Serial.println("Getting bank information...");
//  if (dv_get_bank(bankbuf) != DV_OK) {
//    Serial.println("Can't get banks");
//    Serial.println(dv_get_errmsg());
////    exit(1);
//  }

  
//  Serial.println("Getting data ...");
//  startTime = micros();
//  if (dv_get_ram(buf, 0x8000, 0x0800) != DV_OK) {
//     Serial.println("can't get RAM\n");
//        exit(1);
//  }
//  endTime = micros();
//  Serial.print("Microseconds for 2048 bytes = ");
//  Serial.println(endTime - startTime);
//
//  for (i =0; i < 8; i++) {
//    hex2tobuf(bankbuf[i], &outbuf[i*3]);
//    outbuf[i*3+2] = ' ';
//  }
//  outbuf[24] = '\0';
//  Serial.println(outbuf);
//
//  write_PCE_port(0x0F);
}


//
// Add some instructions to 'accelerate' the grab/download of BRAM data
// as an example of what can be done as part of acceleration
//
// This function will be relocated to the "alternate commands" side soon
//
void getbram()
{
int i;
int c;

  if (cardPresent) {
    if (SD.exists("BRAM.sav")) {
      Serial.println("mb128.sav exists; removing");
      SD.remove("BRAM.sav");
    }

    Serial.println("Opening BRAM.sav");
    dataFile = SD.open("BRAM.sav", FILE_WRITE);
  }

  Serial.println("Get BRAM:");
  
  // if echo was on, temporarily disable echo
  //

  if (cardPresent)
    dataFile.write(c);

  if (cardPresent)
    dataFile.close();
    
  Serial.println("done");
}

// ------------------
void serial_setbank() {
int retcode;
int page;
int bank;

  while (!Serial.available());    // need to wait for data to be available
  page = Serial.read();

  while (!Serial.available());    // need to wait for data to be available
  bank = Serial.read();

  retcode = dv_set_bank(page, bank);
  Serial.write(retcode);
  Serial.flush();
}

void serial_getbank() {
int retcode;
unsigned char bankbuf[16];

  for (int i =0; i < 8; i++) {
    bankbuf[i] = 0x00;
  }
  retcode = dv_get_bank(bankbuf);
  if (retcode != DV_OK) {
    Serial.write(retcode);
  }
  else {
    Serial.write(retcode);
    Serial.write(bankbuf,8);
    Serial.flush();
  }
}

void serial_getram() {
int retcode;
unsigned char buf[1024];
int addr_h, addr_l, len_h, len_l;
int addr, len;
int i;

  while (!Serial.available());    // need to wait for data to be available
  addr_h = Serial.read();
  while (!Serial.available());    // need to wait for data to be available
  addr_l = Serial.read();

  while (!Serial.available());    // need to wait for data to be available
  len_h = Serial.read();
  while (!Serial.available());    // need to wait for data to be available
  len_l = Serial.read();

  addr = (addr_h << 8) + addr_l;
  len  = (len_h  << 8) + len_l;
  
  if (len > 512) len = 512;

  retcode = dv_get_ram(buf, addr, len);
  if (retcode != DV_OK) {
    Serial.write(retcode);
  }
  else {
    Serial.write(retcode);
    for (i = 0 ; i < len; i++) {
      Serial.write(*(buf+i));
    }
    Serial.flush();
  }
}

void serial_getvram() {
int retcode;
unsigned char buf[1024];
int addr_h, addr_l, len_h, len_l;
int addr, len;
int i;

  while (!Serial.available());    // need to wait for data to be available
  addr_h = Serial.read();
  while (!Serial.available());    // need to wait for data to be available
  addr_l = Serial.read();

  while (!Serial.available());    // need to wait for data to be available
  len_h = Serial.read();
  while (!Serial.available());    // need to wait for data to be available
  len_l = Serial.read();

  addr = (addr_h << 8) + addr_l;
  len  = (len_h  << 8) + len_l;
  
  if (len > 512) len = 512;

  retcode = dv_get_vram(buf, addr, len);
  if (retcode != DV_OK) {
    Serial.write(retcode);
  }
  else {
    Serial.write(retcode);
    for (i = 0 ; i < len; i++) {
      Serial.write(*(buf+i));
    }
    Serial.flush();
  }
}

void serial_getcolor() {
int retcode;
unsigned char buf[1024];
int addr_h, addr_l, len_h, len_l;
int addr, len;
int i;

  while (!Serial.available());    // need to wait for data to be available
  addr_h = Serial.read();
  while (!Serial.available());    // need to wait for data to be available
  addr_l = Serial.read();

  while (!Serial.available());    // need to wait for data to be available
  len_h = Serial.read();
  while (!Serial.available());    // need to wait for data to be available
  len_l = Serial.read();

  addr = (addr_h << 8) + addr_l;
  len  = (len_h  << 8) + len_l;
  
  if (len > 512) len = 512;

  retcode = dv_get_color(buf, addr, len);
  if (retcode != DV_OK) {
    Serial.write(retcode);
  }
  else {
    Serial.write(retcode);
    for (i = 0 ; i < len; i++) {
      Serial.write(*(buf+i));
    }
    Serial.flush();
  }
}

void serial_error() {
    Serial.write(DV_INTERNAL_ERR);
    Serial.flush();
}

// -------------------
void loop() {
int c;
int numbytes, readsize, bytesread;
boolean flush = false;
char buffer[49];
static bool led_state = LOW;

  while (!Serial.available());  // wait for input

  c = Serial.read();
  switch(c) {
    case 'A':   // set bank
        serial_setbank();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    case 'B':   // get bank
        serial_getbank();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    case 'D':   // get ram
        serial_getram();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    case 'F':   // get vram
        serial_getvram();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    case 'H':   // get color
        serial_getcolor();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    case 'Z':   // error
        serial_error();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    default:
      break;
  }
  
//  if (Serial.available()) {      // If anything comes in Serial (USB),
//    c = Serial.read();
//    Serial.write(c);
//    Serial.flush();
//  }
//
//  delay(4);                               // pause to prevent more than 250 USB packets per second
//
//  Serial.write(buffer, bytesread);      // read it and send it out Serial (USB)
//  Serial.flush();
}



bool checkForKey()
{
  return(Serial.available());
}

char waitKeyEnterEscape(bool beep)
{
char c;

  while (1) {
    while (!checkForKey());
    
    c = Serial.read();
    if ((c == KEY_ENTER) || (c == KEY_ESCAPE))
      break;
  }

  return(c);
}

char getKeyFromList(char * list, bool beep)
{
int i;
char c;
bool exit;

  exit = false;
  while (exit == false) {
    while (!checkForKey());

    c = Serial.read();

    if (c == KEY_ESCAPE) {
      exit = true;
      break;
    }

    for (i = 0 ; i < strlen(list); i++) {
      if ( c == *(list + i) ) {
        exit = true;
        break;
      }
    }

  }

  return(c);
}




// I don't like the way this is written but
// it basically works... I'll fix it later...
//
int fetchKeyInput()
{
int keycode;
char c1, c2, c3, c4, c5;

  c1 = Serial.read();
  if (c1 != KEY_ESCAPE)
  {
    keycode = c1;
    return(keycode);
  }

  keycode = KEY_ESCAPE;  // at least until we check further...

  // Now we differentiate between various codes
  //
  if (!Serial.available())    // nothing in buffer - return (but this may not
  {                           // be completely sufficient - may need timeout)
    return(keycode);
  }

  return(keycode);
}

//
// char enterValue(int size, int base, char *buf)
//   Fetch an n-digit hex value into a supplied buffer
// size = number of digits
// base = base system (8, 10, or 16)
// buf  = buffer to write entereed keys into (must be adequately-sized)
// return value = KEY_ESCAPE for abort, KEY_ENTER for accept
//
char enterValue(int size, int base, char *buf)
{
char c;
char cmdList[32];
int i;

  switch (base) {
    case 16:
      strcpy(cmdList, "1234567890AaBbCcDdEeFf\x8\x7f\x0a");
      break;

    case 10:
      strcpy(cmdList, "1234567890\x8\x7f\x0a");
      break;

    case 8:
      strcpy(cmdList, "12345670\x8\x7f\x0a");
      break;

    default:
      return(0);
  }

  i = 0;
  while (1) {
    c = getKeyFromList(cmdList, true);  // command list, and beep if wrong keys
    
    if (c == KEY_ESCAPE)
      return(c);

    if (c == KEY_ENTER) {
      if (i > (size-1)) {
        break;
      }
      else {
        continue;
      }
    }


    if (i > (size-1)) {
      continue;
    }

    if ((c >= 'a') && (c <= 'f')) {    // change lowercase to uppercase
      c -= 0x20;
    }

    Serial.write(c);
    *(buf + i) = c;

    i += 1;
  }
  return(c);
}



//
// hex conversion routines
//
// char hex2digit(char *buffer)
//   - take a single char of buffer, return val (0-15) in char
//
// char hex2digit(char *buffer)
//   - take 2 chars of buffer, turn it into a char
//
// int hextoint16(char *buffer)
//   - take 4 chars of buffer, turn it into an int
//
// hex2tobuf(char c, char *buffer)
//   - take char and 'sprintf' it as 2-bute hex string into buffer
//
// hex4tobuf(int i, char *buffer)
//   - take int and 'sprintf' it as 4-bute hex string into buffer
//
char hex2digit(char *buffer) {
char c, n;

  c = *buffer;

  if ((c >= '0') && (c <= '9')) {
    n = c - '0';
  }
  else if ((c >= 'A') && (c <= 'F')) {
    n = c - 'A' + 10;
  }
  else if ((c >= 'a') && (c <= 'f')) {
    n = c - 'a' + 10;
  }
  else
    n = 0;

  return(n);
}

char hextobyte(char *buffer) {
char n;

  n = (hex2digit(buffer) << 4);
  buffer++;
  n |= hex2digit(buffer);

  return(n);
}

int hextoint16(char *buffer) {
int n;

  n = (hex2digit(buffer) << 12);
  buffer++;
  n |= (hex2digit(buffer) << 8);
  buffer++;
  n |= (hex2digit(buffer) << 4);
  buffer++;
  n |= hex2digit(buffer);

  return(n);
}


//
// printhex2(char) / printhex4(int)
// 
// Print a char variable as a 2-/4-digit hex number
//
// Arduino has a print(... , HEX) function, but it trims leading zeroes
// which is not desirable.  Have to break it down to character-by-character
//
void printhex2(char c)
{
int c1, c2;

  c1 = (c >> 4) & 0x0f;
  c2 = c & 0x0f;

  Serial.print(c1, HEX);
  Serial.print(c2, HEX);
}

void printhex4(int i)
{
int c1, c2, c3, c4;

  c1 = (i >> 12) & 0x0f;
  c2 = (i >> 8) & 0x0f;
  c3 = (i >> 4) & 0x0f;
  c4 = i & 0x0f;
  
  Serial.print(c1, HEX);
  Serial.print(c2, HEX);
  Serial.print(c3, HEX);
  Serial.print(c4, HEX);
}


void hex2tobuf(char c, char *buffer)
{
  int c1, c2;
  c1 = c / 16;
  c2 = c % 16;

  if (c1 < 0x0a)
    *buffer = c1 + '0';
  else
    *buffer = (c1 - 10) + 'A';

  buffer++;

  if (c2 < 0x0a)
    *buffer = c2 + '0';
  else
    *buffer = (c2 - 10) + 'A';
}

void hex4tobuf(int i, char *buffer)
{
char c;

  c = i >> 8;
  hex2tobuf(c, buffer);

  buffer += 2;

  c = i & 0xff;
  hex2tobuf(c, buffer);
}
