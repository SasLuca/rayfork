@echo off

echo Generating rayfork.h and rayfork.c

:: This ensures the script gets executed from the dir its in
pushd %~dp0

amalgamate ..\source\rayfork.h rayfork.h -i ..\source\audio -i ..\source\core -i ..\source\csv -i ..\source\gfx -i ..\source\libs -i ..\source\math -i ..\source\str
amalgamate ..\source\rayfork.c rayfork.c -i ..\source\audio -i ..\source\core -i ..\source\csv -i ..\source\gfx -i ..\source\libs -i ..\source\math -i ..\source\str

:: >nul 2>&1 will silence the output in case the command is not present
tar.exe -a -c -f rayfork.zip rayfork.h rayfork.c >nul 2>&1

popd