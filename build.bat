@echo off
call "C:/Program Files (x86)/Microsoft Visual Studio 12.0/VC/vcvarsall.bat" x64
IF NOT EXIST bin mkdir bin
cd bin

set COMMON_COMPILER_FLAGS=-nologo -FeNova -EHa- -Gm- -GR- -fp:fast -Oi -WX -W4 -wd4996 -wd4100 -wd4189 -wd4127 -wd4201
set COMMON_LINKER_FLAGS=-incremental:no -opt:ref

REM set COMPILER_FLAGS=%COMMON_COMPILER_FLAGS% -MTd -Od -Z7
set COMPILER_FLAGS=%COMMON_COMPILER_FLAGS% -MT -O2

cl %COMPILER_FLAGS% -I../lib -I../src ../src/win32_main.cpp shell32.lib user32.lib gdi32.lib opengl32.lib glew32s.lib glfw3.lib -link %COMMON_LINKER_FLAGS% -libpath:../lib
cd ..