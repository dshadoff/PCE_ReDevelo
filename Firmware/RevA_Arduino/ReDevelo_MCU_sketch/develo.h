//
// Develo.h : header for common develo functions/definitions
//

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
