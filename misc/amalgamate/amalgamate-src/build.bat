@echo off

cl *.cpp /EHsc /nologo /Fe:amalgamate /link ole32.lib
del *.obj