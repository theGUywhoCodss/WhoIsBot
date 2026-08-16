#ifndef CONFIGDATA_H
#define CONFIGDATA_H
#include <string>

// Get config info
std::string get_config_info(std::string variable);

// Sets variable and saves.  Bool determines if it succeeded or not.
bool set_config_info(std::string variable, std::string info);

void load_config_info();

#endif
