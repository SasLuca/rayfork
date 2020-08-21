@echo off

echo Generating rayfork.h and rayfork.c

:: This ensures the script gets executed from the dir its in
pushd %~dp0
pushd ..

if not exist "rayfork-amalgamated" mkdir "rayfork-amalgamated"
devutils\amalgamate include\rayfork.h rayfork-amalgamated\rayfork.h -i include\
devutils\amalgamate src\rayfork.c     rayfork-amalgamated\rayfork.c -i src\ -i libs\

:: >nul 2>&1 will silence the output in case the command is not present
tar.exe -a -c -f rayfork-amalgamated\rayfork.zip rayfork-amalgamated\rayfork.h rayfork-amalgamated\rayfork.c >nul 2>&1

popd
popd