//
// Develo.h : header for common develo functions/definitions
//


//
// Note: Porting to a different board will require some changes to the
//       below pin/port defeinitions (and possibly where they are referenced)
//
#define DELAY_MICROS  1

#if defined(SEEED_XIAO_M0)

#define IN_PORT            REG_PORT_IN0
#define IN_CLR             PORT_PA08          // Pin marked as D4 on Seeeduino XIAO M0 (CLR on PCE joypad)
#define IN_SEL             PORT_PA09          // Pin marked as D5 on Seeeduino XIAO M0 (SEL on PCE joypad)
#define OUT_PORT_SET       REG_PORT_OUTSET0
#define OUT_PORT_CLR       REG_PORT_OUTCLR0
#define OUT_D3             PORT_PA11          // Pin marked as D3 on Seeeduino XIAO M0 (D3 on PCE joypad)
#define OUT_D2             PORT_PA10          // Pin marked as D2 on Seeeduino XIAO M0 (D2 on PCE joypad)
#define OUT_D1             PORT_PA04          // Pin marked as D1 on Seeeduino XIAO M0 (D1 on PCE joypad)
#define OUT_D0             PORT_PA02          // Pin marked as D0 on Seeeduino XIAO M0 (D0 on PCE joypad)

const int clrPin      = 4;  // CLR signal from PCE (Develo) (CLR)
const int selPin      = 5;  // SEL signal from PCE (Develo) (SEL)
const int d3_Pin      = 3;  // data 3 to PCE
const int d2_Pin      = 2;  // data 2 to PCE
const int d1_Pin      = 1;  // data 1 to PCE
const int d0_Pin      = 0;  // data 0 to PCE
const int LED_Pin     = 13; // yellow LED
#endif

#if defined(ADAFRUIT_FEATHER_M0)

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

#endif


/* command status */
#define DV_OK			 0
#define DV_ERR			-1            //  comes back as 0xFF
#define DV_CRC_ERR		-2          //  comes back as 0xFE
#define DV_TIMEOUT_ERR	-3        //  comes back as 0xFD
#define DV_INTERNAL_ERR	-99       //  comes back as 0x9D

#define DV_CMD  0
#define DV_SEND 1
#define DV_RECV 2

#define DV_ACK  0x06
#define DV_NAK  0x15

/* externs */
extern int develo;	/* develo box presence flag */

/* develo routines */
extern int   dv_init(void);

extern int   dv_set_bank(int page, int bank);
extern int   dv_get_bank(unsigned char *bank);
extern int   dv_set_ram(int addr, unsigned char *data, int len);
extern int   dv_get_ram(unsigned char *data, int addr, int len);
extern int   dv_set_vram(int addr, unsigned char *data, int len);
extern int   dv_get_vram(unsigned char *data, int addr, int len);
extern int   dv_set_color(int addr, unsigned char *data, int len);
extern int   dv_get_color(unsigned char *data, int addr, int len);
extern int   dv_exec(int addr, int slave);
extern int   dv_read_cd(unsigned char *data, int sect);

extern int   dv_send_cmd(unsigned char *buf);
extern int   dv_send_block(unsigned char *buf, int len);
extern int   dv_recv_block(unsigned char *buf, int len);
extern int   dv_wait_ack(void);
extern int   dv_send_byte(unsigned char c);
extern int   dv_recv_byte(void);
extern int   dv_check_user_break(void);
extern int   dv_get_err(void);
extern const char *dv_get_errmsg(void);
extern void  write_PCE_port(int value);

//extern int   dv_load_mx(char *fname, int *addr, int *bank, int disp);
//extern void  dv_slave(int disp);
//extern int   dv_slave_init(void);
