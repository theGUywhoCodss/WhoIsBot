#ifndef WITHERDATA_H
#define WITHERDATA_H
#include <nlohmann/json.hpp>

// Save info to file
void save_withered();

// Get info from variable with userid WITH no-account protection. if file is not
// being accessed, returns json else returns empty json (NULL).
nlohmann::json get_withered_info(int userid);

// Sets variable and returns true/false depending on if the file was currently
// being saved.
bool set_withered_info(int userid, nlohmann::json &info);

void load_withered_info();

// Startup Wither autosave
void enable_withered_autosave();

void disable_withered_autosave();
#endif
