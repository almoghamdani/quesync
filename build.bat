::Re-create the build folder
IF EXIST Build rmdir BUILD /s /q
mkdir BUILD

:: Copy all needed dlls to build folder
xcopy /s dlls Build

:: Re-compile the client side
IF EXIST Build\Client.exe del Build\Client.exe
g++ client.cpp -L lib -lbass -lopus -lOpenAL32 -lws2_32 -o Build\Client.exe

:: Re-compile the server side
IF EXIST Build\Server.exe del Build\Server.exe
g++ server.cpp -L lib -lopus -lbass -lws2_32 -o Build\Server.exe