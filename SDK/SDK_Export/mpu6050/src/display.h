/*
 * display.h
 *
 *  Created on: 15/11/2016
 *      Author: Pau Cadens
 */

#include "xgpio.h"
#include "xparameters.h"
#include "xtmrctr.h"
#include "xbasic_types.h"

#ifndef DISPLAY_H_
#define DISPLAY_H_

// Masks to the pins on the GPIO port
#define LCD_DB4  0x04 // ordre teoric 0x40 // ordre primer 0x01
#define LCD_DB5  0x08 // ordre teoric 0x20 // ordre primer 0x02
#define LCD_DB6  0x01 // ordre teoric 0x10 // ordre primer 0x04
#define LCD_DB7  0x02 // ordre teoric 0x08 // ordre primer 0x08
#define LCD_RW   0x20 // ordre teoric 0x04 // ordre primer 0x10
#define LCD_RS   0x10 // ordre teoric 0x02 // ordre primer 0x20
#define LCD_E    0x40 // ordre teoric 0x01 // ordre primer 0x40
#define LCD_TEST   //0x80



// Function prototypes

void delay_us(Xuint32 time);
void delay_ms(Xuint32 time);
void gpio_write(Xuint32 c);
Xuint32 gpio_read(void);
void lcd_clk(void);
void lcd_set_test(void);
void lcd_reset_test(void);
void lcd_set_rs(void);
void lcd_reset_rs(void);
void lcd_set_rw(void);
void lcd_reset_rw(void);
void lcd_write(Xuint32 c);
void lcd_clear(void);
void lcd_puts(const char * s);
void lcd_putch(Xuint32 c);
void lcd_goto(Xuint32 line,Xuint32 pos);
void lcd_init(void);

#endif /* DISPLAY_H_ */


