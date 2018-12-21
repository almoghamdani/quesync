IF NOT EXIST Build mkdir Build
g++ client.cpp -L lib -lbass -lopus -lws2_32 -o Build\Client.exe
Build\Client.exe