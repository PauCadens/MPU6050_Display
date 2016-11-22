@ECHO OFF
@REM ###########################################
@REM # Script file to run the flow 
@REM # 
@REM ###########################################
@REM #
@REM # Command line for ngdbuild
@REM #
ngdbuild -p xc6slx9csg324-2 -nt timestamp -bm ethernet.bmm "C:/Users/Pc/Documents/VHDL/ethernet/implementation/ethernet.ngc" -uc ethernet.ucf ethernet.ngd 

@REM #
@REM # Command line for map
@REM #
map -o ethernet_map.ncd -w -pr b -ol high -timing -detail ethernet.ngd ethernet.pcf 

@REM #
@REM # Command line for par
@REM #
par -w -ol high ethernet_map.ncd ethernet.ncd ethernet.pcf 

@REM #
@REM # Command line for post_par_trce
@REM #
trce -e 3 -xml ethernet.twx ethernet.ncd ethernet.pcf 

