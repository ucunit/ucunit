@ECHO OFF
SETLOCAL
PATH=D:\GNUARM\bin;%PATH%
make clean
make all
make run
ENDLOCAL
ECHO 'Finished.'
