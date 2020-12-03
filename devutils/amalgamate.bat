@echo off

echo Generating rayfork.h and rayfork.c

:: This ensures the script gets executed from the dir its in
pushd %~dp0

pushd ..

if not exist "amalgamated" mkdir "amalgamated"
devutils\amalgamate source\rayfork.h amalgamated\rayfork.h -i source\audio -i source\core -i source\gfx -i source\internal -i source\libs -i source\math -i source\str
devutils\amalgamate source\rayfork.c amalgamated\rayfork.c -i source\audio -i source\core -i source\gfx -i source\internal -i source\libs -i source\math -i source\str

:: >nul 2>&1 will silence the output in case the command is not present
tar.exe -a -c -f amalgamated\rayfork.zip amalgamated\rayfork.h amalgamated\rayfork.c >nul 2>&1

popd

popd