#ifndef FORMAT_H
#define FORMAT_H

#include "KVP.h"

#include <string>

void format(std::string &s, KVP *k);
void analysis(std::string &s, KVP *&p);

void kvp_to_string(std::string &s, KVP *k);
void string_to_kvp(std::string &s, KVP *&p);

void string_to_kvp(const char *s, KVP *&p);
//void kvp_to_string(std::string s, KVP *k);

#endif
