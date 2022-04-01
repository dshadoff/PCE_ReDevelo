//
// common.c
//
// This file contains all sorts of common utility routines, such as
// string/hex conversion, serial I/O and buffer control, etc.
//
// (c) 2022 by David Shadoff


#include <Arduino.h>
#include "crc16.h"
#include "develo.h"

//
// Note: Porting to a different board will require some changes to the
//       below pin/port defeinitions (and possibly where they are referenced)
//
#define DELAY_MICROS  1

#define IN_PORT            REG_PORT_IN0
#define IN_CLR             PORT_PA17          // Pin marked as D13 on M0 Feather Adalogger (CLR on PCE joypad)
#define IN_SEL             PORT_PA19          // Pin marked as D12 on M0 Feather Adalogger (SEL on PCE joypad)
#define OUT_PORT_SET       REG_PORT_OUTSET0
#define OUT_PORT_CLR       REG_PORT_OUTCLR0
#define OUT_D3             PORT_PA16          // Pin marked as D11 on M0 Feather Adalogger (D3 on PCE joypad)
#define OUT_D2             PORT_PA18          // Pin marked as D10 on M0 Feather Adalogger (D2 on PCE joypad)
#define OUT_D1             PORT_PA20          // Pin marked as D6  on M0 Feather Adalogger (D1 on PCE joypad)
#define OUT_D0             PORT_PA15          // Pin marked as D5  on M0 Feather Adalogger (D0 on PCE joypad)

const int clrPin      = 13;  // CLR signal from PCE (Develo) (CLR)
const int selPin      = 12;  // SEL signal from PCE (Develo) (SEL)

const int d3_Pin      = 11;  // data 3 to PCE
const int d2_Pin      = 10;  // data 2 to PCE
const int d1_Pin      = 6;   // data 1 to PCE
const int d0_Pin      = 5;   // data 0 to PCE

const int LED_Pin     = 8;   // green LED

//
// DEVELO initialization stuff:
//

// ALREADY COVERED:
//
// CRC16.H
// DEVELO.H
// ERROR.C
// INIT.C
// COMMAND.C
// XFER.C
//
// NEXT UP:
// --------
// SLAVE.C
// PERUN.C
// MX.C
// EXECMX.C


int develo = 1;      /* develo box presence flag */

int develo_wait1 = 20000; /* timeout retries */
int develo_wait2 = 20000; /* may be unused) */
int develo_error; /* latest error */
int develo_status;  /* transfer status */
int slave_mode;
int slave_wait;

static int  dv_crc(unsigned char *buf, int len);
static int  dv_request(int cmd);
static void dv_output(int data);
static int  dv_output_and_wait(int c, int val);
static int  dv_input(void);
static void dv_delay(void);

// static void write_PCE_port(int value);
static int  read_PCE_port(void);


/*************  COMMAND.C  **************/

/* ----
 * dv_set_bank()
 * ----
 */

int
dv_set_bank(int page, int bank)
{
  unsigned char buf[8];

  buf[0] = 2;
  buf[1] = page;
  buf[2] = bank;
  buf[3] = 0;
  buf[4] = 0;
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;
  
  if (dv_send_cmd(buf) != DV_OK)
    return (DV_ERR);
  
  return (DV_OK);
}


/* ----
 * dv_get_bank()
 * ----
 */

int
dv_get_bank(unsigned char *bank)
{
  unsigned char buf[8];

  buf[0] = 3;
  buf[1] = 0;
  buf[2] = 0;
  buf[3] = 0;
  buf[4] = 0;
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;
  
  if (dv_send_cmd(buf) != DV_OK)
    return (DV_ERR);
  if (dv_recv_block(buf, 8) != DV_OK)
    return (DV_ERR);

  memcpy(bank, buf, 8);
  return (DV_OK);
}


/* ----
 * dv_get_ram()
 * ----
 * transfer a block of memory from the PC Engine to the PC
 */

int
dv_get_ram(unsigned char *data, int addr, int len)
{
  unsigned char buf[8];

  buf[0] = 4;
  buf[1] = (addr & 0xFF00) >> 8;
  buf[2] = (addr & 0x00FF);
  buf[3] = (len  & 0xFF00) >> 8;
  buf[4] = (len  & 0x00FF);
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;

  if (dv_send_cmd(buf) != DV_OK)
    return (DV_ERR);
  if (dv_recv_block(data, len) != DV_OK)
    return (DV_ERR);
  
  return (DV_OK);
}


/* ----
 * dv_set_ram()
 * ----
 * transfer a block of memory from the PC to the PC Engine
 */

int
dv_set_ram(int addr, unsigned char *data, int len)
{
  unsigned char buf[8];
  int size, pos, temp;
  
  size = len;
  pos = 0;

  /* transfer the block by chunk of 512 bytes */
  while (size) {
    temp = (size > 512) ? 512 : size;

    buf[0] = 5;
    buf[1] = ((addr + pos) & 0xFF00) >> 8;
    buf[2] = ((addr + pos) & 0x00FF);
    buf[3] = (temp & 0xFF00) >> 8;
    buf[4] = (temp & 0x00FF);
    buf[5] = 0;
    buf[6] = 0;
    buf[7] = 0;
  
    if (dv_send_cmd(buf) != DV_OK)
      return (DV_ERR);
    if (dv_send_block(&data[pos], temp) != DV_OK)
      return (DV_ERR);
    if (dv_wait_ack() != DV_OK)
      return (DV_ERR);

    size -= temp;
    pos  += temp;
  }
  
  return (DV_OK);
}


/* ----
 * dv_get_vram()
 * ----
 */

int
dv_get_vram(unsigned char *data, int addr, int len)
{
  unsigned char buf[8];
  int size, pos, temp;
  
  size = len;
  pos = 0;

  /* get the block by chunk of 512 bytes */
  while (size) {
    temp = (size > 512) ? 512 : size;

    buf[0] = 6;
    buf[1] = ((addr + (pos / 2)) & 0xFF00) >> 8;
    buf[2] = ((addr + (pos / 2)) & 0x00FF);
    buf[3] = (temp & 0xFF00) >> 8;
    buf[4] = (temp & 0x00FF);
    buf[5] = 0;
    buf[6] = 0;
    buf[7] = 0;
  
    if (dv_send_cmd(buf) != DV_OK)
      return (DV_ERR);
    if (dv_recv_block(&data[pos], temp) != DV_OK)
      return (DV_ERR);

    size -= temp;
    pos  += temp;
  }

  return (DV_OK);
} 


/* ----
 * dv_set_vram()
 * ----
 */

int
dv_set_vram(int addr, unsigned char *data, int len)
{
  unsigned char buf[8];
  int size, pos, temp;
  
  size = len;
  pos = 0;

  /* transfer the block by chunk of 512 bytes */
  while (size) {
    temp = (size > 512) ? 512 : size;

    buf[0] = 7;
    buf[1] = ((addr + (pos / 2)) & 0xFF00) >> 8;
    buf[2] = ((addr + (pos / 2)) & 0x00FF);
    buf[3] = (temp & 0xFF00) >> 8;
    buf[4] = (temp & 0x00FF);
    buf[5] = 0;
    buf[6] = 0;
    buf[7] = 0;
  
    if (dv_send_cmd(buf) != DV_OK)
      return (DV_ERR);
    if (dv_send_block(&data[pos], temp) != DV_OK)
      return (DV_ERR);
    if (dv_wait_ack() != DV_OK)
      return (DV_ERR);

    size -= temp;
    pos  += temp;
  }
  
  return (DV_OK);
}


/* ----
 * dv_get_color()
 * ----
 */

int
dv_get_color(unsigned char *data, int addr, int len)
{
  unsigned char buf[8];

  buf[0] = 8;
  buf[1] = (addr & 0xFF00) >> 8;
  buf[2] = (addr & 0x00FF);
  buf[3] = (len & 0xFF00) >> 8;
  buf[4] = (len & 0x00FF);
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;

  if (dv_send_cmd(buf) != DV_OK)
    return (DV_ERR);
  if (dv_recv_block(data, len) != DV_OK)
    return (DV_ERR);
  
  return (DV_OK);
}


/* ----
 * dv_set_color()
 * ----
 */

int
dv_set_color(int addr, unsigned char *data, int len)
{
  unsigned char buf[8];

  buf[0] = 9;
  buf[1] = (addr & 0xFF00) >> 8;
  buf[2] = (addr & 0x00FF);
  buf[3] = (len & 0xFF00) >> 8;
  buf[4] = (len & 0x00FF);
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;

  if (dv_send_cmd(buf) != DV_OK)
    return (DV_ERR);
  if (dv_send_block(data, len) != DV_OK)
    return (DV_ERR);
  if (dv_wait_ack() != DV_OK)
    return (DV_ERR);
  
  return (DV_OK);
}


/* ----
 * dv_exec()
 * ----
 * execute the program starting at <addr> on the PC Engine
 */

int
dv_exec(int addr, int slave)
{
  unsigned char buf[8];

  buf[0] = 10;
  buf[1] = (addr & 0xFF00) >> 8;
  buf[2] = (addr & 0x00FF);
  buf[3] = 0;
  buf[4] = 0;
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;
  
  if (dv_send_cmd(buf) != DV_OK)
    return (DV_ERR);

  return (DV_OK);
}


/* ----
 * dv_read_cd()
 * ----
 */

int
dv_read_cd(unsigned char *data, int sector)
{
  unsigned char buf[8];
  int i;

  buf[0] = 12;
  buf[1] = (sector & 0xFF0000) >> 16;
  buf[2] = (sector & 0x00FF00) >> 8;
  buf[3] = (sector & 0x0000FF);
  buf[4] = 0;
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;

//  Serial.print("sector: ");
//  Serial.print(buf[1], HEX);
//  Serial.print(" ");
//  Serial.print(buf[2], HEX);
//  Serial.print(" ");
//  Serial.println(buf[3], HEX);
//
//  Serial.println("send cmd");

  if (dv_send_cmd(buf) != DV_OK)
    return (DV_ERR);

//  Serial.println("get ram");

// The above command to fetch a sector may take a long time, including
// head seek, focus, and read, and the PC Engine will be unresponsive
// during that time, so we need to retry.
//
// Since there is significant variability in head seek (8ms to ~2 seconds),
// we will do this with a variable delay-timeout technique
//
  for (i = 0; i < 30; i++) {
    delay(100);
    if (dv_get_ram(data, 0x2800, 2048) == DV_OK)
      break;
  }

  if (i == 30)
    return (DV_ERR);

//  Serial.println("Should be OK");

  return (DV_OK);
}


/*************  INIT.C  **************/

int dv_init(void)
{
  //just make sure that the following are set:
  //  develo = 1
  //  develo_wait1 = timer
  //  develo_wait2 = timer
  
    /* ok */
  return (DV_OK);
}

/*************  ERROR.C  **************/

/* ----
 * dv_get_err()
 * ----
 */

int
dv_get_err(void)
{
  return (develo_error);
}

/* ----
 * dv_get_errmsg()
 * ----
 */

const char *
dv_get_errmsg(void)
{
  switch (develo_error) {
  case DV_OK:
    return ("OK");

  case DV_CRC_ERR:
    /* bad CRC */
    return ("CRC error");

  case DV_TIMEOUT_ERR:
    /* timeout error */
    switch (develo_status) {
    case DV_SEND:
      /* sending */
      return ("Timeout error while sending a byte");

    case DV_RECV:
      /* receiving */
      return ("Timeout error while receiving a byte");

    default:
      return ("Timeout error");
    }

  case DV_INTERNAL_ERR:
    /* internal error */
    return ("Develo library internal error");

  default:
    return ("Unknown error");
  }
}

/*************  XFER.C  **************/

/* ----
 * dv_send_cmd()
 * ----
 */

int
dv_send_cmd(unsigned char *buf)
{
  if (dv_request(DV_CMD) != DV_OK)
    return (DV_ERR);
  if (dv_send_block(buf, 8) != DV_OK)
    return (DV_ERR);
  if (dv_wait_ack() != DV_OK)
    return (DV_ERR);

  /* ok */
  return (DV_OK);
}


/* ----
 * dv_send_block()
 * ----
 */

int
dv_send_block(unsigned char *buf, int len)
{
  int i, crc;
  unsigned char *p = buf;

  /* send block */
  for (i = 0; i < len; i++) {
    if (dv_send_byte(*p) != DV_OK)
      return (DV_ERR);
    p++;
  }

  /* calculate and send CRC */
  crc = dv_crc(buf, len);

  if (dv_send_byte(crc >> 8) != DV_OK)
    return (DV_ERR);
  if (dv_send_byte(crc & 0xFF) != DV_OK)
    return (DV_ERR);

  /* reset the output lines */
  dv_output(0x0F);

  /* ok */
  return (DV_OK);
}


/* ----
 * dv_recv_block()
 * ----
 */
int
dv_recv_block(unsigned char *buf, int len)
{
  int i;
  int c;
  int l, h;
  int crc, crc_l, crc_h;
  unsigned char *p = buf;

  /* get block */
  for (i = 0; i < len; i++) {
    if ((c = dv_recv_byte()) == DV_ERR)
      return (DV_ERR);
    *p++ = c;
  }

  /* calculate CRC */
  crc = dv_crc(buf, len);
  crc_h = (crc & 0xFF00) >> 8;
  crc_l = (crc & 0x00FF);

  /* check CRC */
  if ((h = dv_recv_byte()) == DV_ERR)
    return (DV_ERR);
  if ((l = dv_recv_byte()) == DV_ERR)
    return (DV_ERR);

  if ((crc_l != l) || (crc_h != h)) {
    develo_error = DV_CRC_ERR;
    return (DV_ERR);
  }

  /* reset the output lines */
  dv_output(0x0F);

  /* ok */
  return (DV_OK);
}


/* ----
 * dv_wait_ack()
 * ----
 */
int
dv_wait_ack(void)
{
  switch (dv_recv_byte()) {
  case DV_ACK:
    /* ok */
    break;
  
  case DV_NAK:
    /* bad CRC */
    develo_error = DV_CRC_ERR;
    return (DV_ERR);
  
  default:
    /* timeout */
    return (DV_ERR);
  }

  /* reset the output lines */
  dv_output(0x0F);

  /* ok */
  return (DV_OK);
}

/* ----
 * dv_crc()
 * ----
 */

static int
dv_crc(unsigned char *buf, int len)
{
  int i;
  unsigned int crc = 0;
  unsigned char *p = buf;
  
  for (i = 0; i < len; i++) {
    crc  = crc16tbl[(crc >> 8) ^ (*p)] ^ (crc << 8);
    crc &= 0xFFFF;
    p++;
  }

  return (crc);
}


/* ----
 * dv_request()
 * ----
 */

static int
dv_request(int cmd)
{
  int i;

  /* develo status */
  develo_status = cmd;
  develo_error  = DV_OK;

  /* request */
  switch (cmd) {
  case DV_CMD:
    /* command */
    for (i = 0; i < 10; i++) {
      if (dv_send_byte('@') != DV_OK)
        continue;
      if (dv_recv_byte() == '@')
        return (DV_OK);
    }

    /* error */
    return (DV_ERR);

  case DV_SEND:
    /* sending */
    return (dv_output_and_wait(0x09, 2));

  case DV_RECV:
    /* receiving */
    for (i = 0; i < 20; i++) {
      /* if in slave mode, check if the user
       * want to exit
       */
      if (slave_wait) {
        if (dv_check_user_break()) {
          slave_mode = 0;
          break;
        }
      }

      /* request */
      if (dv_output_and_wait(0x0B, 2) != DV_OK)
        continue;
      if (dv_output_and_wait(0x0F, 3) == DV_OK) {
        /* reset slave wait flag */
        slave_wait = 0;

        /* ok */
        return (DV_OK);
      }
    }

    /* reset slave wait flag */
    slave_wait = 0;

    /* error */
    return (DV_ERR);

  default:
    /* unsupported request */
    develo_error = DV_INTERNAL_ERR;
    return (DV_ERR);
  }
}


/* ----
 * dv_send_byte()
 * ----
 */

int
dv_send_byte(unsigned char c)
{
  int bit02;
  int bit35;
  int bit67;

  /* request transfer */
  if (dv_request(DV_SEND) != DV_OK)
    return (DV_ERR);

  /* split byte */
  bit02 = ((c)) & 0x07;
  bit35 = ((c >> 3) & 0x07) | 0x08;
  bit67 = ((c >> 6) & 0x03);

  /* send byte */
  if (dv_output_and_wait(bit02, 3) != DV_OK)
    return (DV_ERR);
  if (dv_output_and_wait(bit35, 2) != DV_OK)
    return (DV_ERR);
  if (dv_output_and_wait(bit67, 3) != DV_OK)
    return (DV_ERR);

  /* ok */
  return (DV_OK);
}


/* ----
 * dv_recv_byte()
 * ----
 */

int
dv_recv_byte(void)
{
  int i, j, s;
  int c;

  /* request transfer */
  if (dv_request(DV_RECV) != DV_OK)
    return (DV_ERR);

  /* get a byte */
  c = 0;
  s = 0;

  for (i = 0; i < 8; i++) {
    dv_output(0x0C);

    /* wait develo */
    for (j = 0; j < develo_wait1; j++) {
      if ((s = dv_input()) != 3)
        break;
    }

    /* get a bit */
    switch (s) {
    case 3:
      /* error */
      develo_error = DV_TIMEOUT_ERR;
      return (DV_ERR);

    case 2:
      /* '0' */
      if (dv_output_and_wait(0x0D, 3) != DV_OK)
        return (DV_ERR);

      c >>= 1;
      break;

    case 1:
    case 0:
      /* '1' */
      if (dv_output_and_wait(0x0E, 2) != DV_OK)
        return (DV_ERR);
      if (dv_output_and_wait(0x0A, 0) != DV_OK)
        return (DV_ERR);
      if (dv_output_and_wait(0x08, 3) != DV_OK)
        return (DV_ERR);

      c >>= 1;
      c  |= 0x80;
      break;
    }
  }

  dv_output(0x0C);

  /* ok */
  return (c);
}


/* ----
 * dv_output()
 * ----
 */

static void
dv_output(int data)
{
  static int old = 0;

  write_PCE_port((data & 0x07) | (old & 0x08));
  dv_delay();

  write_PCE_port((data & 0x0F));
  old = data;
}


/* ----
 * dv_output_and_wait()
 * ----
 */

static int
dv_output_and_wait(int c, int val)
{
  int i;

  /* output */
  dv_output(c);

  /* wait loop */
  for (i = 0; i < develo_wait1; i++) {
    if (dv_input() == val)
      return (DV_OK);
  }

  /* timeout */
  develo_error = DV_TIMEOUT_ERR;
  return (DV_ERR);
}

/* ----
 * dv_input()
 * ----
 */

static int
dv_input(void)
{
  int c;

    /* develo input port */
  switch (read_PCE_port() & 0xC0) {
//    case 0x00: c = 0x03; break;   // This was how MagicKit had it, but that seems to be incorrect
    case 0x00: c = 0x00; break;
    case 0x40: c = 0x01; break;
    case 0x80: c = 0x02; break;
//    default:   c = 0x00; break;   // This was how MagicKit had it, but that seems to be incorrect
    default:   c = 0x03; break;
  }

  /* return result */
  return (c);
}

//
// Note: Porting to different board will require changes
//       to these two functions
//
void
write_PCE_port(int value)
{
    int clrval;
    int setval;

    clrval = ((value & 0x08) ? 0 : OUT_D3) |
             ((value & 0x04) ? 0 : OUT_D2) |
             ((value & 0x02) ? 0 : OUT_D1) |
             ((value & 0x01) ? 0 : OUT_D0);

    setval = ((value & 0x08) ? OUT_D3 : 0) |
             ((value & 0x04) ? OUT_D2 : 0) |
             ((value & 0x02) ? OUT_D1 : 0) |
             ((value & 0x01) ? OUT_D0 : 0);

    OUT_PORT_CLR = clrval;
    OUT_PORT_SET = setval;
//  digitalWrite(d0_Pin, (value & 1));
//  digitalWrite(d1_Pin, (value & 2));
//  digitalWrite(d2_Pin, (value & 4));
//  digitalWrite(d3_Pin, (value & 8));
  
}

static int
read_PCE_port(void)
{
    uint32_t  inport;
    int       retval;
    bool      sel;
    bool      clr;

    inport = IN_PORT;

    sel = (inport & IN_SEL) ? HIGH : LOW;
    clr = (inport & IN_CLR) ? HIGH : LOW;

    retval = (clr ? 0x80 : 0) | (sel ? 0x40 : 0);

    return(retval);
}

/* ----
 * dv_delay()
 * ----
 */
static void
dv_delay(void)
{
    delayMicroseconds(DELAY_MICROS);
}

/* ----
 * dv_check_user_break()
 * ----
 */
////////////////////////////////////////////////////////////////
//TODO: Fix Break key = only matters when in SLAVE mode
////////////////////////////////////////////////////////////////

int
dv_check_user_break(void)
{
//  while (kbhit()) {
//    if (getkey() == 0x1B)
//      return (1);
//  }

  return (0);
}
