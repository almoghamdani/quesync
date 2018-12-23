::Re-create the build folder
IF EXIST build rmdir build /s /q
mkdir build

:: Copy all needed dlls to build folder
xcopy /s dlls build

:: Re-compile the client side
IF EXIST build\client.exe del build\client.exe
g++ client\client.cpp -I include -L lib -lbass -lopus -lOpenAL32 -lws2_32 -o build\client.exe

:: Re-compile the server side
IF EXIST build\server.exe del build\server.exe
g++ server\server.cpp -I include -L lib -lopus -lbass -lws2_32 -o build\server.exe