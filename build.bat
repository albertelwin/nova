@echo off
call "C:/Program Files (x86)/Microsoft Visual Studio 12.0/VC/vcvarsall.bat" x64
IF NOT EXIST bin mkdir bin
cd bin
set COMPILER_FLAGS=-nologo -MT -Zi -EHsc
set LINKER_FLAGS=-nodefaultlib:libcmt.lib
cl %COMPILER_FLAGS% -I../lib -I../src ../src/win32_nova.cpp user32.lib gdi32.lib opengl32.lib glew32s.lib glfw3.lib -link %LINKER_FLAGS% -libpath:../lib
cd ..