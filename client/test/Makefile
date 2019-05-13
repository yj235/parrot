User.h = ~/a/parrot/user/include
user.cpp = ~/a/parrot/user/src/user.cpp

KVP.h = ~/a/parrot/data_transmission/include/
KVP.cpp = ~/a/parrot/data_transmission/src/test/KVP.cpp

format.h = ~/a/parrot/data_transmission/include/
format.cpp = ~/a/parrot/data_transmission/src/format.cpp

pdebug.h = ~/a/parrot/tools/include/

startup:client.cpp
	g++ -std=c++11 -I $(KVP.h) -I $(format.h) -I $(User.h) -I $(pdebug.h) client.cpp $(KVP.cpp) $(format.cpp) $(user.cpp) -lpthread -o startup
