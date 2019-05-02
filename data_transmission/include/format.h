#ifndef FORMAT_H_
#define FORMAT_H_

#include "KVP.h"

#include <string>
#include <iterator>

void format(std::string &s, KVP *k);
void analysis(std::string &sin, std::string::iterator &i1, std::string::iterator &i2, KVP *&p);

#endif
