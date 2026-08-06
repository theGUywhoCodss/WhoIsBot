#ifndef WITHERED_H
#define WITHERED_H
#include <string>

std::string withered_message(int userid, std::string username);
void withered_init();
void save_withered();
void withered_destroy();

std::string wither(int userid, std::string username);
std::string unwither(int userid, std::string username);

#endif
