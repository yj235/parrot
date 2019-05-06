User.h = ../../user/include
user.cpp = ../../user/src/user.cpp

KVP.h = ../../data_transmission/include/
KVP.cpp = ../../data_transmission/src/KVP.cpp

format.h = ../../data_transmission/include/
format.cpp = ../../data_transmission/src/format.cpp

pdebug.h = ../../tools/include/

startup:client.cpp
	g++ -std=c++11 -I $(KVP.h) -I $(format.h) -I $(User.h) -I $(pdebug.h) client.cpp $(KVP.cpp) $(format.cpp) $(user.cpp) -lpthread -o startup
