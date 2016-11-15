#include "xparameters.h"
#include "xbasic_types.h"
#include "xgpio.h"
#include "xstatus.h"
#include "xtmrctr.h"
#include "display.h"
#include "mpu6050.h"


// Global variables

XGpio GpioOutput;
XTmrCtr DelayTimer;


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
  xil_printf("LCD INIT OK\n");
  MpuSensorInit();


  while(1){
	  Get_Accel_Values();
	  Get_Accel_Angles();
	  Get_GyroRates();
	  lcd_goto(0,0);
	  lcd_puts("Hello");
	  lcd_goto(1,0);
	  lcd_puts("Worldwide");
  }
}


