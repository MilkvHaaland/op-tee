:: Script to flash images via fastboot

@echo off
call:RunACmd "..\..\..\fastboot.exe flash ram ..\..\..\..\..\..\prebuild\images\light-fm-a\u-boot-with-spl.bin"
call:RunACmd "..\..\..\fastboot.exe reboot"

ping 127.0.0.1 -n 5 >nul
call:RunACmd "..\..\..\fastboot.exe flash uboot ..\..\..\..\..\..\prebuild\images\light-fm-a\u-boot-with-spl.bin"

pause
exit

:RunACmd
SETLOCAL
set CmdStr=%1
echo IIIIIIIIIIIIIIII Run Cmd:  %CmdStr% 
%CmdStr:~1,-1% || goto RunACmd

GOTO:EOF
