predir = ../

user.h = $(predir)../user/include/
user.cpp = $(predir)../user/src/user.cpp

KVP.h = $(predir)../data_transmission/include/test/
KVP.cpp = $(predir)../data_transmission/src/test/KVP.cpp

format.h = $(predir)../data_transmission/include/test/
format.cpp = $(predir)../data_transmission/src/test/format.cpp

#rapidjson.h = $(predir)../rapidjson/
rapidjson.h = $(predir)../

pdebug.h = $(predir)../tools/include/

mysql.h = /usr/include/mysql

my_query.h = $(predir)../mysql/include/test/
my_query.cpp = $(predir)../mysql/src/test/my_query.cpp

source = $(user.cpp) $(KVP.cpp) $(format.cpp) $(my_query.cpp)

startup:server.cpp
	g++ -std=c++11 -I $(user.h) -I $(KVP.h) -I $(format.h) -I $(pdebug.h) -I $(mysql.h) -I $(my_query.h) -I $(rapidjson.h) server.cpp $(source) -L /usr/lib/mysql/ -l mysqlclient -l pthread -o startup

.PHONY:clean
clean:
	rm ./startup
