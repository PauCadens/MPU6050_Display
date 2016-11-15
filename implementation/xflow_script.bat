@ECHO OFF
@REM ###########################################
@REM # Script file to run the flow 
@REM # 
@REM ###########################################
@REM #
@REM # Command line for ngdbuild
@REM #
ngdbuild -p xc6slx9csg324-2 -nt timestamp -bm mpu6050.bmm "C:/Users/Pc/Documents/VHDL/mpu6050/implementation/mpu6050.ngc" -uc mpu6050.ucf mpu6050.ngd 

@REM #
@REM # Command line for map
@REM #
map -o mpu6050_map.ncd -w -pr b -ol high -timing -detail mpu6050.ngd mpu6050.pcf 

@REM #
@REM # Command line for par
@REM #
par -w -ol high mpu6050_map.ncd mpu6050.ncd mpu6050.pcf 

@REM #
@REM # Command line for post_par_trce
@REM #
trce -e 3 -xml mpu6050.twx mpu6050.ncd mpu6050.pcf 

