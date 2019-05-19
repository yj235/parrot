predir = ../

#User.h = ~/a/parrot/user/include
User.h = $(predir)../../user/include/

#user.cpp = ~/a/parrot/user/src/user.cpp
user.cpp = $(predir)../..//user/src/user.cpp

#KVP.h = ~/a/parrot/data_transmission/include/
KVP.h = $(predir)../../data_transmission/include/test/

#KVP.cpp = ~/a/parrot/data_transmission/src/test/KVP.cpp
KVP.cpp = $(predir)../../data_transmission/src/test/KVP.cpp

#format.h = ~/a/parrot/data_transmission/include/
format.h = $(predir)../..//data_transmission/include/test/

#format.cpp = ~/a/parrot/data_transmission/src/format.cpp
format.cpp = $(predir)../..//data_transmission/src/test/format.cpp

#pdebug.h = ~/a/parrot/tools/include/
pdebug.h = $(predir)../../tools/include/

startup:client.cpp
	g++ -std=c++11 -I $(KVP.h) -I $(format.h) -I $(User.h) -I $(pdebug.h) client.cpp $(KVP.cpp) $(format.cpp) $(user.cpp) -lpthread -o startup

.PHONY:clean
clean:
	rm ./startup
