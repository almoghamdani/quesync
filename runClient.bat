IF NOT EXIST Build mkdir Build
g++ client.cpp -L lib -lbass -o Build\Client.exe
Build\Client.exe