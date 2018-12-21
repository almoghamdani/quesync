::Create the Build folder if it doesn't exist
IF NOT EXIST Build mkdir Build

:: Re-compile the client side
del Build\Client.exe
g++ client.cpp -L lib -lbass -lopus -lws2_32 -o Build\Client.exe

del Build\Server.exe
g++ server.cpp -L lib -lopus -lws2_32 -o Build\Server.exe

Build\Client.exe