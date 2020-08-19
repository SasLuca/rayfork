@echo off

echo Generating rayfork.h and rayfork.c
amalgamate ../include/rayfork.h rayfork.h -i ../include/
amalgamate ../src/rayfork.c rayfork.c -i ../src/ -i ../libs/

:: >nul 2>&1 will silence the output in case the command is not present
tar.exe -a -c -f rayfork.zip rayfork.h rayfork.c >nul 2>&1