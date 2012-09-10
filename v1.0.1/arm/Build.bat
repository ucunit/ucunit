@ECHO OFF
SETLOCAL
PATH=D:\GNUARM\bin;%PATH%
ECHO 'Cleaning up...'
DEL Testsuite.elf
ECHO 'Compiling...'
arm-elf-gcc -mcpu=arm7tdmi -mthumb -g -o"Testsuite.elf" ../Testsuite.c System.c
ECHO 'Running...'
arm-elf-run Testsuite.elf
ENDLOCAL
ECHO 'Finished.'
