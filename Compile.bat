g++ Client.cpp icon.res -o client.exe -lws2_32 -lmingw32 -lSDL2_ttf -lSDL2_image -lSDL2main -lSDL2 -static-libgcc -static-libstdc++ -I src/SDL/include/SDL2 -L src/SDL/lib -I src/Image/include/SDL2 -L src/Image/lib -I src/TTF/include/SDL2 -L src/TTF/lib -g
g++ Server.cpp -o server.exe -lws2_32 -g


pause