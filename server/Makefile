User.h = ../user/include/
user.cpp = ../user/src/user.cpp

KVP.h = ../data_transmission/include/
KVP.cpp = ../data_transmission/src/KVP.cpp

format.h = ../data_transmission/include/
format.cpp = ../data_transmission/src/format.cpp

qdebug.h = ../tools/include/

startup:server.cpp
	g++ -std=c++11 -I $(User.h) -I $(KVP.h) -I $(format.h) -I $(qdebug.h) server.cpp $(user.cpp) $(KVP.cpp) $(format.cpp) -lpthread -o startup
