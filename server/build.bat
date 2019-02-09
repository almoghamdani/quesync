SET ARCH=x64

::Re-create the build folder
IF EXIST build rmdir build /s /q
mkdir build

:: Copy all needed dlls to build folder
xcopy /s dlls build

:: Initialize the visual studio developer command line with x64 if it's not initialized yet
IF NOT DEFINED DevEnvDir (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %ARCH%
)

SET LIB=%~dp0\..\lib;%LIB%

:: Re-compile the server side
IF EXIST build\server.exe del build\server.exe
cl session.cpp quesync.cpp main.cpp -I..\include -DASIO_STANDALONE -D_WIN32_WINNT=0x0501 /EHsc /Fo"build/" /Fe"build\server.exe"