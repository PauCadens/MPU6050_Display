#include "xparameters.h"
#include "xbasic_types.h"
#include "xgpio.h"
#include "xstatus.h"
#include "xtmrctr.h"
#include "display.h"
#include "mpu6050.h"
#include "stdlib.h"
//#include "stdio.h"

void getAsciiValues(float value, char * buffer);

// Global variables

XGpio GpioOutput;
XTmrCtr DelayTimer;

extern float ACCEL_XANGLE;
extern float ACCEL_YANGLE;
extern float ACCEL_ZANGLE;
extern float GYRO_XRATE;
extern float GYRO_YRATE;
extern float GYRO_ZRATE;
extern Xint16 ACCEL_XOUT;
extern Xint16 ACCEL_YOUT;
extern Xint16 ACCEL_ZOUT;
u16 data[3] = {0};

//char fila1[17] = {'X', ':', '\0', '\0', '\0', '\0', '\0', 34, '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
//char fila2[17] = {'Y', ':', '\0', '\0', '\0', '\0', '\0', 34, '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

char fila1[17] = {'A','N','G','L','E',' ','X',':','\0', '\0', '\0', '\0', '\0', 176, '\0', '\0', '\0',};
char fila2[17] = {'A','N','G','L','E',' ','Y',':','\0', '\0', '\0', '\0', '\0', 176, '\0', '\0', '\0',};

// Main function

int main (void)
{
  Xuint32 status;

  // Clear the screen
  xil_printf("%c[2J",27);
  xil_printf("16x2 LCD Driver\r\n");
  xil_printf("\r\n");

  // Initialize the Timer
  status = XTmrCtr_Initialize(&DelayTimer, XPAR_AXI_TIMER_0_DEVICE_ID);
  if (status != XST_SUCCESS){
    xil_printf("Timer failed to initialize\r\n");
    return XST_FAILURE;
  }
  XTmrCtr_SetOptions(&DelayTimer, 1, XTC_DOWN_COUNT_OPTION);

  // Initialize the GPIO driver for the LCD
   status = XGpio_Initialize(&GpioOutput, XPAR_DISPLAY16X2_DEVICE_ID);
   if (status != XST_SUCCESS){
     xil_printf("GPIO failed to initialize\r\n");
     return XST_FAILURE;
   }

  // Set the direction for all signals to be outputs
  XGpio_SetDataDirection(&GpioOutput, 1, 0x00);

  // Initialize the LCD
  lcd_init();
  xil_printf("LCD INIT OK\r\n");
  lcd_goto(0,0);
  lcd_puts("LCD ok");
  lcd_puts("                ");
  delay_ms(500);
  lcd_goto(0,0);
  MpuSensorInit();
  lcd_goto(0,0);
  lcd_puts("                ");


  while(1)
  {
	  Get_Accel_Values();
	  Get_Accel_Angles();
	  Get_GyroRates(&data[0]);
	  filtre();

	  getAsciiValues(ACCEL_XANGLE, &fila1[8]);
	  getAsciiValues(ACCEL_YANGLE, &fila2[8]);
	  //printf("X: %f\tY: %f\r\n", ACCEL_XANGLE, ACCEL_YANGLE);

	  lcd_goto(0,0);
	  lcd_puts(&fila1[0]);
	  lcd_goto(1,0);
	  lcd_puts(&fila2[0]);

	  //delay_ms(8);
    delay_ms(100);
  }
}

void getAsciiValues(float value, char * buffer)
{
	u8 part_entera = (int) abs(value);
	u8 part_decimal = (u8) (((float)(abs(value) - part_entera))*10.0f);
	xil_printf("decimal: %d\r\n", part_decimal);

	if (value < 0)
	{
		buffer[0] = '-';

	}
	else
	{
		buffer[0] = ' ';
	}
	buffer[1] = ((part_entera - (part_entera % 10))/10) + '0';
	buffer[2] = (part_entera % 10) + '0';
	buffer[3] = '.';
	buffer[4] = part_decimal + '0';
}
