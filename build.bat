@echo off
call "C:/Program Files (x86)/Microsoft Visual Studio 12.0/VC/vcvarsall.bat" x64
IF NOT EXIST bin mkdir bin
cd bin

set COMMON_COMPILFER_FLAGS=-nologo -FeNova -EHsc
set COMMON_LINKER_FLAGS=-incremental:no

set DEBUG_COMPILER_FLAGS=%COMMON_COMPILFER_FLAGS% -MTd -Zi
set RELEASE_COMPILER_FLAGS=%COMMON_COMPILFER_FLAGS% -MT -O2 -Oi

cl %RELEASE_COMPILER_FLAGS% -I../lib -I../src ../src/win32_main.cpp shell32.lib user32.lib gdi32.lib opengl32.lib glew32s.lib glfw3.lib -link %COMMON_LINKER_FLAGS% -libpath:../lib
cd ..