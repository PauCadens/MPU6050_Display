/*
 * display.c
 *
 *  Created on: 15/11/2016
 *      Author: Pau Cadens
 */

#include "display.h"

// Global variables
XGpio GpioOutput;
XTmrCtr DelayTimer;

// Delay function (microseconds)
void delay_us(Xuint32 time)
{
  XTmrCtr_SetResetValue(&DelayTimer, 1, time * 125);
  XTmrCtr_Start(&DelayTimer, 1);
  while(!(XTmrCtr_IsExpired(&DelayTimer, 1))){}
  XTmrCtr_Stop(&DelayTimer, 1);
}

// Delay function (milliseconds)
void delay_ms(Xuint32 time)
{
  XTmrCtr_SetResetValue(&DelayTimer, 1, time * 125000);
  XTmrCtr_Start(&DelayTimer, 1);
  while(!(XTmrCtr_IsExpired(&DelayTimer, 1))){}
  XTmrCtr_Stop(&DelayTimer, 1);
}

// Write to GPIO outputs
void gpio_write(Xuint32 c)
{
  // Write to the GP IOs
  XGpio_DiscreteWrite(&GpioOutput, 1, c & 0x0FF);
}

// Read the GPIO outputs
Xuint32 gpio_read()
{
  // Read from the GP IOs
  return(XGpio_DiscreteRead(&GpioOutput, 1));
}

// Clock the LCD (toggles E)
void lcd_clk()
{
  Xuint32 c;
  // Get existing outputs
  c = gpio_read();
  delay_us(1);
  // Assert clock signal
  gpio_write(c | LCD_E);
  delay_us(1);
  // Deassert the clock signal
  gpio_write(c & (~LCD_E));
  delay_us(1);
}

// Assert the RS signal
void lcd_set_rs()
{
  Xuint32 c;
  // Get existing outputs
  c = gpio_read();
  // Assert RS
  gpio_write(c | LCD_RS);
  delay_us(1);
}

// Deassert the RS signal
void lcd_reset_rs()
{
  Xuint32 c;
  // Get existing outputs
  c = gpio_read();
  // Assert RS
  gpio_write(c & (~LCD_RS));
  delay_us(1);
}

// Assert the RW signal
void lcd_set_rw()
{
  Xuint32 c;
  // Get existing outputs
  c = gpio_read();
  // Assert RS
  gpio_write(c | LCD_RW);
  delay_us(1);
}

// Deassert the RW signal
void lcd_reset_rw()
{
  Xuint32 c;
  // Get existing outputs
  c = gpio_read();
  // Assert RS
  gpio_write(c & (~LCD_RW));
  delay_us(1);
}

// Write a byte to LCD (4 bit mode)
void lcd_write(Xuint32 c)
{
  Xuint32 temp;
  // Get existing outputs
  temp = gpio_read();
  temp = temp & 0xF0;
  // Set the high nibble
  temp = temp | ((c >> 4) & 0x0F);
  gpio_write(temp);
  // Clock
  lcd_clk();
  // Delay for "Write data into internal RAM 43us"
  delay_us(100);
  // Set the low nibble
  temp = temp & 0xF0;
  temp = temp | (c & 0x0F);
  gpio_write(temp);
  // Clock
  lcd_clk();
  // Delay for "Write data into internal RAM 43us"
  delay_us(100);
}

// Clear LCD
void lcd_clear(void)
{
  lcd_reset_rs();
  // Clear LCD
  lcd_write(0x01);
  // Delay for "Clear display 1.53ms"
  delay_ms(2);
}

// Write a string to the LCD
void lcd_puts(const char * s)
{
  lcd_set_rs();
  while(*s)
    lcd_write(*s++);
}

// Write character to the LCD
void lcd_putch(Xuint32 c)
{
  lcd_set_rs();
  lcd_write(c);
}

// Change cursor position
// (line = 0 or 1, pos = 0 to 15)
void lcd_goto(Xuint32 line, Xuint32 pos)
{
  lcd_reset_rs();
  pos = pos & 0x3F;
  if(line == 0)
    lcd_write(0x80 | pos);
  else
    lcd_write(0xC0 | pos);
}

// Initialize the LCD
void lcd_init(void)
{
  Xuint32 temp;

  // Write mode (always)
  lcd_reset_rw();
  // Write control bytes
  lcd_reset_rs();

  // Delay 15ms
  delay_ms(15);

  // Initialize
  temp = gpio_read();
  temp = temp | LCD_DB5;
  gpio_write(temp);
  lcd_clk();
  lcd_clk();
  lcd_clk();

  // Delay 15ms
  delay_ms(15);

  // Function Set: 4 bit mode, 1/16 duty, 5x8 font, 2 lines
  lcd_write(0x28);
  // Display ON/OFF Control: ON
  lcd_write(0x0C);
  // Entry Mode Set: Increment (cursor moves forward)
  lcd_write(0x06);

  // Clear the display
  lcd_clear();
}
