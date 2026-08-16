#ifndef WITHERHANDLER_H
#define WITHERHANDLER_H
#include <string>

std::string withered_message(int userid, std::string username);
std::string wither(int userid, std::string username, std::string reason);
std::string unwither(int userid, std::string username);
void wither_init();

#endif
