User.h = ~/a/parrot/user/include/
user.cpp = ~/a/parrot/user/src/user.cpp

KVP.h = ~/a/parrot/data_transmission/include/test/
KVP.cpp = ~/a/parrot/data_transmission/src/test/KVP.cpp

format.h = ~/a/parrot/data_transmission/include/test/
format.cpp = ~/a/parrot/data_transmission/src/test/format.cpp

qdebug.h = ~/a/parrot/tools/include/

mysql.h = /usr/include/mysql

my_query.h = ~/a/parrot/mysql/include/test/
my_query.cpp = ~/a/parrot/mysql/src/test/my_query.cpp

source = $(user.cpp) $(KVP.cpp) $(format.cpp) $(my_query.cpp)

startup:server.cpp
	g++ -std=c++11 -I $(User.h) -I $(KVP.h) -I $(format.h) -I $(qdebug.h) -I $(mysql.h) -I $(my_query.h) server.cpp $(source) -L /usr/lib/mysql/ -l mysqlclient -l pthread -o startup
