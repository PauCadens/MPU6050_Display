/*
 * i2c.c
 *
 *  Created on: 15/11/2016
 *      Author: Pau Cadens
 */

#include "mpu6050.h"  //arxiu on hi ha la configuracio dels registres del mpu6050

/***************** DEFINICIO DE VARIABLES**********/

XIic Iic;
XIic_Config * ConfigPtr;

u32 GYRO_XOUT_OFFSET = 0;
u32 GYRO_YOUT_OFFSET = 0;
u32 GYRO_ZOUT_OFFSET = 0;
u16 ACCEL_XOUT = 0;
u16 ACCEL_YOUT = 0;
u16 ACCEL_ZOUT = 0;
float ACCEL_XANGLE = 0;
float ACCEL_YANGLE = 0;
float GYRO_XRATE = 0;
float GYRO_YRATE = 0;
float GYRO_ZRATE = 0;
u32 gyro_xsensitivity = 0;
u32 gyro_ysensitivity = 0;
u32 gyro_zsensitivity = 0;

// no hi pot haver dos mains
int MpuSensorInit(void)
{
	int Status;
	static int Initialized = FALSE;

	if (!Initialized) {
		Initialized = TRUE;

		/*
		 * Initialize the IIC driver so that it is ready to use.
		 */
		xil_printf("Lookup Config ...\r\n");
		lcd_goto(0,0);
		lcd_puts("Lookup congig...");
		lcd_puts("                ");
		delay_ms(500);

		ConfigPtr = XIic_LookupConfig(IIC_DEVICE_ID);
		if (ConfigPtr == NULL) {
			return XST_FAILURE;
		}
		xil_printf("Lookup Config OK\r\n");
		lcd_goto(0,0);
		lcd_puts("Lookup Confing ok");
		lcd_puts("                ");
		delay_ms(500);

		Status = XIic_CfgInitialize(&Iic, ConfigPtr, ConfigPtr->BaseAddress);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		xil_printf("Config initialize OK\r\n");
		lcd_goto(0,0);
		lcd_puts("Cnfg init OK");
		lcd_puts("                ");
		delay_ms(500);

		/*
		 * Start the IIC driver such that it is ready to send and
		 * receive messages on the IIC interface, set the address
		 * to send to which is the temperature sensor address
		 */
		XIic_Start(&Iic);
		xil_printf("I2C start OK\r\n");
		lcd_goto(0,0);
		lcd_puts("I2C start OK");
		lcd_puts("                ");
		delay_ms(500);
		XIic_SetAddress(&Iic, XII_ADDR_TO_SEND_TYPE, MPU6050_ADDRESS);
		xil_printf("I2C address OK\r\n");
		lcd_goto(0,0);
		lcd_puts("I2C adress OK");
		lcd_puts("                ");
		delay_ms(500);
	}
	if(XIic_IsIicBusy(&Iic))
	{
		xil_printf("Bus busy\r\n");
	}
	else
	{
		xil_printf("Bus free\r\n");
		//XIic_WriteReg(ConfigPtr->BaseAddress, XIIC_SR_REG_OFFSET, XIIC_SR_BUS_BUSY_MASK);
		//xil_printf("Now bus is busy\r\n");
	}
	MPU6050_Test_I2C();
	xil_printf("MPU I2C TEST OK\r\n");
	Setup_MPU6050();
	xil_printf("MPU SETUP OK\r\n");
	lcd_goto(0,0);
	lcd_puts("MPU SETUP...");
	lcd_puts("                ");
	delay_ms(500);
	Calibrate_Gyros();
	xil_printf("Gyros calibration OK\r\n");

	return Status;
}

void MPU6050_Test_I2C()
{
    u8 registre_que_volem_llegir = MPU6050_WHO_AM_I;
    u8 dades_rebudes_del_mpu = 0;

    xil_printf("Starting MPU I2C test\r\n");
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP); //XIIC_REPEATED_START
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &dades_rebudes_del_mpu, 1, XIIC_STOP);

    if(dades_rebudes_del_mpu == MPU6050_ADDRESS)
    {
        xil_printf("I2C Read Test Passed, MPU6050 Address: 0x%x\r\n", dades_rebudes_del_mpu);
    }
    else
    {
        xil_printf("ERROR: I2C Read Test Failed, Stopping\r\n");
        xil_printf("MPU6050 Address received: 0x%x\r\n", dades_rebudes_del_mpu);
        while(1){}
    }    
}

void Setup_MPU6050()
{
	u8 registre_i_dada_que_volem_escriure[2] = {0,0};


    //Sets sample rate to 8000/1+7 = 1000Hz
	registre_i_dada_que_volem_escriure[0] = MPU6050_SMPLRT_DIV;
	registre_i_dada_que_volem_escriure[1] = 0x07;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Disable FSync, 256Hz DLPF
    registre_i_dada_que_volem_escriure[0] = MPU6050_CONFIG;
	registre_i_dada_que_volem_escriure[1] = 0x00;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Disable gyro self tests, scale of 500 degrees/s
    registre_i_dada_que_volem_escriure[0] = MPU6050_GYRO_CONFIG;
	registre_i_dada_que_volem_escriure[1] = 0b00001000;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);
	gyro_xsensitivity = 500;
	gyro_ysensitivity = 500;
	gyro_zsensitivity = 500;

    //Disable accel self tests, scale of +-2g, no DHPF
	registre_i_dada_que_volem_escriure[0] = MPU6050_ACCEL_CONFIG;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Freefall threshold of |0mg|
	registre_i_dada_que_volem_escriure[0] = MPU6050_FF_THR;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Freefall duration limit of 0
	registre_i_dada_que_volem_escriure[0] = MPU6050_FF_DUR;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Motion threshold of 0mg
	registre_i_dada_que_volem_escriure[0] = MPU6050_MOT_THR;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Motion duration of 0s
	registre_i_dada_que_volem_escriure[0] = MPU6050_MOT_DUR;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Zero motion threshold
	registre_i_dada_que_volem_escriure[0] = MPU6050_ZRMOT_THR;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Zero motion duration threshold
	registre_i_dada_que_volem_escriure[0] = MPU6050_ZRMOT_DUR;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Disable sensor output to FIFO buffer
	registre_i_dada_que_volem_escriure[0] = MPU6050_FIFO_EN;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

 
    //AUX I2C setup
    //Sets AUX I2C to single master control, plus other config
	registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_MST_CTRL;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Setup AUX I2C slaves
		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV0_ADDR;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV0_REG;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV0_CTRL;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV1_ADDR;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV1_REG;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV1_CTRL;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV2_ADDR;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV2_REG;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV2_CTRL;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV3_ADDR;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV3_REG;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV3_CTRL;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV4_ADDR;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV4_REG;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV4_DO;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV4_CTRL;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV4_DI;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);
   
    //Setup INT pin and AUX I2C pass through
	registre_i_dada_que_volem_escriure[0] = MPU6050_INT_PIN_CFG;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Enable data ready interrupt
	registre_i_dada_que_volem_escriure[0] = MPU6050_INT_ENABLE;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);
 
    //Slave out, dont care
		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV0_DO;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV1_DO;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV2_DO;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

		registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_SLV3_DO;
		registre_i_dada_que_volem_escriure[1] = 0x00;
		XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //More slave config
	registre_i_dada_que_volem_escriure[0] = MPU6050_I2C_MST_DELAY_CTRL;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Reset sensor signal paths
	registre_i_dada_que_volem_escriure[0] = MPU6050_SIGNAL_PATH_RESET;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Motion detection control
	registre_i_dada_que_volem_escriure[0] = MPU6050_MOT_DETECT_CTRL;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Disables FIFO, AUX I2C, FIFO and I2C reset bits to 0
	registre_i_dada_que_volem_escriure[0] = MPU6050_USER_CTRL;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Sets clock source to gyro reference w/ PLL
	registre_i_dada_que_volem_escriure[0] = MPU6050_PWR_MGMT_1;
	registre_i_dada_que_volem_escriure[1] = 0b00000010;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Controls frequency of wakeups in accel low power mode plus the sensor standby modes
	registre_i_dada_que_volem_escriure[0] = MPU6050_PWR_MGMT_2;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

    //Data transfer to and from the FIFO buffer
	registre_i_dada_que_volem_escriure[0] = MPU6050_FIFO_R_W;
	registre_i_dada_que_volem_escriure[1] = 0x00;
	XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_i_dada_que_volem_escriure[0], 2, XIIC_STOP);

 
    xil_printf("MPU6050 Setup Complete\r\n");
    lcd_goto(0,0);
    lcd_puts("MPU Setup OK");
    lcd_puts("                ");
	delay_ms(500);
}


void Calibrate_Gyros()
{
	u8 registre_que_volem_llegir = 0;
	u8 GYRO_XOUT_H, GYRO_XOUT_L, GYRO_YOUT_H, GYRO_YOUT_L, GYRO_ZOUT_H, GYRO_ZOUT_L;
	u32 GYRO_XOUT_OFFSET_1000SUM = 0, GYRO_YOUT_OFFSET_1000SUM = 0, GYRO_ZOUT_OFFSET_1000SUM = 0;

	int x = 0;
	for(x = 0; x<1000; x++)
	{
		registre_que_volem_llegir = MPU6050_GYRO_XOUT_H;
	    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
	    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_XOUT_H, 1, XIIC_STOP);
		registre_que_volem_llegir = MPU6050_GYRO_XOUT_L;
	    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
	    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_XOUT_L, 1, XIIC_STOP);

		registre_que_volem_llegir = MPU6050_GYRO_YOUT_H;
	    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
	    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_YOUT_H, 1, XIIC_STOP);
		registre_que_volem_llegir = MPU6050_GYRO_YOUT_L;
	    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
	    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_YOUT_L, 1, XIIC_STOP);

		registre_que_volem_llegir = MPU6050_GYRO_ZOUT_H;
	    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
	    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_ZOUT_H, 1, XIIC_STOP);
		registre_que_volem_llegir = MPU6050_GYRO_ZOUT_L;
	    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
	    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_ZOUT_L, 1, XIIC_STOP);
 
		GYRO_XOUT_OFFSET_1000SUM += ((GYRO_XOUT_H<<8)|GYRO_XOUT_L);
		GYRO_YOUT_OFFSET_1000SUM += ((GYRO_YOUT_H<<8)|GYRO_YOUT_L);
		GYRO_ZOUT_OFFSET_1000SUM += ((GYRO_ZOUT_H<<8)|GYRO_ZOUT_L);
 
		delay_ms(1);
	}	
	GYRO_XOUT_OFFSET = GYRO_XOUT_OFFSET_1000SUM/1000;
	GYRO_YOUT_OFFSET = GYRO_YOUT_OFFSET_1000SUM/1000;
	GYRO_ZOUT_OFFSET = GYRO_ZOUT_OFFSET_1000SUM/1000;
 
	xil_printf("\r\nGyro X offset sum: %ld Gyro X offset: %d", GYRO_XOUT_OFFSET_1000SUM, GYRO_XOUT_OFFSET);
	xil_printf("\r\nGyro Y offset sum: %ld Gyro Y offset: %d", GYRO_YOUT_OFFSET_1000SUM, GYRO_YOUT_OFFSET);
	xil_printf("\r\nGyro Z offset sum: %ld Gyro Z offset: %d", GYRO_ZOUT_OFFSET_1000SUM, GYRO_ZOUT_OFFSET);
}	
 
//Gets raw accelerometer data, performs no processing
void Get_Accel_Values()
{
	u8 ACCEL_XOUT_H, ACCEL_XOUT_L, ACCEL_YOUT_H, ACCEL_YOUT_L, ACCEL_ZOUT_H, ACCEL_ZOUT_L;
	u8 registre_que_volem_llegir = 0;

	registre_que_volem_llegir = MPU6050_ACCEL_XOUT_H;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &ACCEL_XOUT_H, 1, XIIC_STOP);
	registre_que_volem_llegir = MPU6050_ACCEL_XOUT_L;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &ACCEL_XOUT_L, 1, XIIC_STOP);

	registre_que_volem_llegir = MPU6050_ACCEL_YOUT_H;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &ACCEL_YOUT_H, 1, XIIC_STOP);
	registre_que_volem_llegir = MPU6050_ACCEL_YOUT_L;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &ACCEL_YOUT_L, 1, XIIC_STOP);

	registre_que_volem_llegir = MPU6050_ACCEL_ZOUT_H;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &ACCEL_ZOUT_H, 1, XIIC_STOP);
	registre_que_volem_llegir = MPU6050_ACCEL_ZOUT_L;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &ACCEL_ZOUT_L, 1, XIIC_STOP);
 
	ACCEL_XOUT = ((ACCEL_XOUT_H<<8)|ACCEL_XOUT_L);
	ACCEL_YOUT = ((ACCEL_YOUT_H<<8)|ACCEL_YOUT_L);
	ACCEL_ZOUT = ((ACCEL_ZOUT_H<<8)|ACCEL_ZOUT_L);
	/*xil_printf("Accel X: %f\r\n", (float) ACCEL_XOUT);
	xil_printf("Accel Y: %f\r\n", (float) ACCEL_YOUT);
	xil_printf("Accel Z: %f\r\n", (float) ACCEL_ZOUT);*/
}	
 
//Converts the already acquired accelerometer data into 3D euler angles
void Get_Accel_Angles()
{
	ACCEL_XANGLE = 57.295*atan((float)ACCEL_YOUT/ sqrt(pow((float)ACCEL_ZOUT,2)+pow((float)ACCEL_XOUT,2)));
	ACCEL_YANGLE = 57.295*atan((float)-ACCEL_XOUT/ sqrt(pow((float)ACCEL_ZOUT,2)+pow((float)ACCEL_YOUT,2)));	
	//xil_printf("Angle X: %d\tAngle Y: %d\r", (int) ACCEL_XANGLE, (int) ACCEL_YANGLE);
}	
 
//Function to read the gyroscope rate data and convert it into degrees/s
void Get_GyroRates()
{
	u8 GYRO_XOUT_H, GYRO_XOUT_L, GYRO_YOUT_H, GYRO_YOUT_L, GYRO_ZOUT_H, GYRO_ZOUT_L;
	u8 registre_que_volem_llegir = 0;
	u16 GYRO_XOUT, GYRO_YOUT, GYRO_ZOUT;

	registre_que_volem_llegir = MPU6050_GYRO_XOUT_H;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_XOUT_H, 1, XIIC_STOP);
	registre_que_volem_llegir = MPU6050_GYRO_XOUT_L;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_XOUT_L, 1, XIIC_STOP);

	registre_que_volem_llegir = MPU6050_GYRO_YOUT_H;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_YOUT_H, 1, XIIC_STOP);
	registre_que_volem_llegir = MPU6050_GYRO_YOUT_L;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_YOUT_L, 1, XIIC_STOP);

	registre_que_volem_llegir = MPU6050_GYRO_ZOUT_H;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_ZOUT_H, 1, XIIC_STOP);
	registre_que_volem_llegir = MPU6050_GYRO_ZOUT_L;
    XIic_Send(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &registre_que_volem_llegir, 1, XIIC_STOP);
    XIic_Recv(ConfigPtr->BaseAddress, MPU6050_ADDRESS, &GYRO_ZOUT_L, 1, XIIC_STOP);
 
	GYRO_XOUT = ((GYRO_XOUT_H<<8)|GYRO_XOUT_L) - GYRO_XOUT_OFFSET;
	GYRO_YOUT = ((GYRO_YOUT_H<<8)|GYRO_YOUT_L) - GYRO_YOUT_OFFSET;
	GYRO_ZOUT = ((GYRO_ZOUT_H<<8)|GYRO_ZOUT_L) - GYRO_ZOUT_OFFSET;
 
	GYRO_XRATE = (float)GYRO_XOUT/gyro_xsensitivity;
	GYRO_YRATE = (float)GYRO_YOUT/gyro_ysensitivity;
	GYRO_ZRATE = (float)GYRO_ZOUT/gyro_zsensitivity;

	/*xil_printf("Gyro X rate: %4.1f\r\n", (float) GYRO_XRATE);
	xil_printf("Gyro Y rate: %f\r\n", (float) GYRO_YRATE);
	xil_printf("Gyro Z rate: %f\r\n", (float) GYRO_ZRATE);*/
}
