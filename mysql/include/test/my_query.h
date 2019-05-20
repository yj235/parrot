#ifndef MY_QYERY_H
#define MY_QYERY_H

#include <string>
#include <vector>

#include <stdio.h>

//char *my_query(const char* p);
//std::string my_query(const char* p);
//std::string my_query(const std::string &s);
std::vector<std::vector<std::string>> my_query(const std::string &sql);
//int my_insert(const std::string &sql);
int my_query_int(const std::string &sql);

#endif
