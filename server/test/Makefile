User.h = ../../user/include/
user.cpp = ../../user/src/user.cpp

KVP.h = ../../data_transmission/include/test/
KVP.cpp = ../../data_transmission/src/test/KVP.cpp

format.h = ../../data_transmission/include/test/
format.cpp = ../../data_transmission/src/test/format.cpp

qdebug.h = ../../tools/include/

mysql.h = /usr/include/mysql

my_query.h = ../../mysql/include/
my_query.cpp = ../../mysql/src/my_query.cpp

startup:server.cpp
	g++ -std=c++11 -I $(User.h) -I $(KVP.h) -I $(format.h) -I $(qdebug.h) -I $(mysql.h) -I $(my_query.h) server.cpp $(user.cpp) $(KVP.cpp) $(format.cpp) $(my_query.cpp) -L /usr/lib/mysql/ -l mysqlclient -l pthread -o startup
