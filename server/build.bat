SET ARCH=x64

::If the build folder doesn't exist, create it
IF NOT EXIST build mkdir build

:: Initialize the visual studio developer command line with x64 if it's not initialized yet
IF NOT DEFINED DevEnvDir (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %ARCH%
)

:: Create the cmake project
cd build
cmake -G Ninja ..

:: Build the server
cmake --build .

:: Return to server folder
cd ..