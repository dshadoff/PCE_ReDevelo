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


#include "define.h"
#include "develo.h"


// I/Os - these are the Arduino pin numbers
const int clrPin      = 13;  // CLR signal from PCE (Develo) (CLR)
const int selPin      = 12;  // SEL signal from PCE (Develo) (SEL)

const int d3_Pin      = 11;  // data 3 to PCE
const int d2_Pin      = 10;  // data 2 to PCE
const int d1_Pin      = 6;   // data 1 to PCE
const int d0_Pin      = 5;   // data 0 to PCE

const int LED_Pin     = 8;   // green LED
//const int TriggerPin  = 1;   // trigger pin


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
  
//  pinMode(TriggerPin, INPUT_PULLUP);

  pinMode(LED_Pin,  OUTPUT);

  // set default values on outputs
  //
  write_PCE_port(0x0F);

  delay(4000);  // wait for startup period


//  Serial.println("Initializing Develo...");
  if (dv_init() != DV_OK) {
    Serial.println("Can not initialize the ReDevelo box!");
//    exit(1);
  }

  for (i =0; i < 8; i++) {
    bankbuf[i] = 0x00;
  }
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
    Serial.flush();
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
    Serial.flush();
  }
  else {
    Serial.write(retcode);
    for (i = 0 ; i < len; i++) {
      Serial.write(*(buf+i));
    }
    Serial.flush();
  }
}

void serial_setram() {
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
  
  for (i = 0 ; i < len; i++) {
    while (!Serial.available());    // need to wait for data to be available
    *(buf+i) = Serial.read();
  }

  retcode = dv_set_ram(addr, buf, len);
  Serial.write(retcode);
  Serial.flush();
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

void serial_setvram() {
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
  
  for (i = 0 ; i < len; i++) {
    while (!Serial.available());    // need to wait for data to be available
    *(buf+i) = Serial.read();
  }

  retcode = dv_set_vram(addr, buf, len);
  Serial.write(retcode);
  Serial.flush();
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

void serial_setcolor() {
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
  
  for (i = 0 ; i < len; i++) {
    while (!Serial.available());    // need to wait for data to be available
    *(buf+i) = Serial.read();
  }

  retcode = dv_set_color(addr, buf, len);
  Serial.write(retcode);
  Serial.flush();
}

void serial_exec() {
int retcode;
unsigned char buf[1024];
int addr_h, addr_l;
int addr;
int i;

  while (!Serial.available());    // need to wait for data to be available
  addr_h = Serial.read();
  while (!Serial.available());    // need to wait for data to be available
  addr_l = Serial.read();

  addr = (addr_h << 8) + addr_l;

  retcode = dv_exec(addr, 0);
  Serial.write(retcode);
  Serial.flush();
}

void serial_getCDsector() {
int retcode;
unsigned char buf[3072];
int sector_h, sector_m, sector_l;
int sector;
int i;

  while (!Serial.available());    // need to wait for data to be available
  sector_h = Serial.read();
  while (!Serial.available());    // need to wait for data to be available
  sector_m = Serial.read();
  while (!Serial.available());    // need to wait for data to be available
  sector_l = Serial.read();

  sector = (sector_h << 16) + (sector_m << 8) + sector_l;
  
  retcode = dv_read_cd(buf, sector);
  if (retcode != DV_OK) {
    Serial.write(retcode);
    Serial.flush();
  }
  else {
    Serial.write(retcode);
    for (i = 0 ; i < 2048; i++) {
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

    case 'E':   // set ram
        serial_setram();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;
        
    case 'F':   // get vram
        serial_getvram();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    case 'G':   // set vram
        serial_setvram();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    case 'H':   // get color
        serial_getcolor();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    case 'I':   // set color
        serial_setcolor();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    case 'J':   // execute
        serial_exec();
        led_state = led_state ? LOW : HIGH;
        digitalWrite(LED_Pin, led_state);
        break;

    case 'L':   // read CD sector
        serial_getCDsector();
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
