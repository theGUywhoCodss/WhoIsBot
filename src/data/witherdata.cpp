#include "witherdata.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

// for convenience
using json = nlohmann::json;
static std::mutex json_file_mutex;
static std::thread save_thread;

// Local save of data
static json allData = json::object();
static bool autoSave = true;
static const int save_delay = 1800; // Every 30 minutes
static const json DEFAULT_USER_DATA = {
    {"version", 2},
    {"withered", false},
    {"recovered", 0},
    {"reason", ""},
};

// Checks the version of the data. Updates if needed. Only used in get_info
// because set_info overrides.
static void versionCheck(int userId) {
  std::string fId = std::to_string(userId);
  int version = allData[fId].value("version", 1);
  int updatedVersion = 0;

  // Update to latest version
  if (version == 1) {
    allData[fId]["version"] = 2;
    allData[fId]["reason"] = "";
    updatedVersion = 2;
  }
  // Useful text
  if (updatedVersion != 0) {
    std::cout << "Updated " << fId
              << " to version " + std::to_string(updatedVersion) << std::endl;
  }
}

// Save info to file
void save_withered() {
  std::lock_guard<std::mutex> lock(json_file_mutex);

  std::ofstream out(WITHERED_FILE_PATH);
  out << allData.dump();
  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::cout << "Saved!" << std::endl;
}

// Get info from variable with userid WITH no-account protection. if file is
// not being accessed, returns json else returns empty json (NULL).
json get_withered_info(int userid) {
  std::unique_lock<std::mutex> lock(json_file_mutex, std::try_to_lock);
  json temp = json::object();
  if (!lock.owns_lock())
    return json();
  // lock mutex
  // AUTOMATICALLY CREATE PROFILE FOR NEW USERS. UPDATE WITH NEW VERSIONS.
  std::string fId = std::to_string(userid);
  if (!allData.contains(fId)) {
    std::cout << "New account for " + fId << std::endl;
    allData[fId] = DEFAULT_USER_DATA;
  }
  versionCheck(userid);
  return allData[fId];
  // std::lock_guard<std::mutex> lock(json_file_mutex);
}

// Sets variable and returns true/false depending on if the file was currently
// being saved.
bool set_withered_info(int userid, json &info) {
  std::unique_lock<std::mutex> lock(json_file_mutex, std::try_to_lock);
  json temp = json::object();
  if (!lock.owns_lock())
    return false;
  // lock mutex
  // Override user data with the copy
  allData[std::to_string(userid)] = info;
  return true;
}

void load_withered_info() {
  // Save data to global variable if there is a file.
  std::ifstream in(WITHERED_FILE_PATH);
  if (!in.is_open() || in.peek() == std::ifstream::traits_type::eof()) {
    std::cout << "NO FILE FOUND FOR WITHER. WILL AUTOMATICALLY CREATE ONE NEXT "
                 "SAVE.\n";
    return;
  }
  in >> allData;
  in.close();

  std::cout << "Wither file loaded\n";
}

// LOOP FOR IN THE INIT AND DESTRUCTOR
static void save_runner() {
  std::this_thread::sleep_for(std::chrono::seconds(save_delay));
  while (autoSave) {
    std::cout << "Autosaving!\n";
    save_withered();
    std::this_thread::sleep_for(std::chrono::seconds(save_delay));
  }
}

// Startup Wither autosave
void enable_withered_autosave() {
  save_thread = std::thread(save_runner);
  save_thread.detach();

  std::cout << "Wither autosave enabled\n";
}

void disable_withered_autosave() {
  autoSave = false;
  save_thread.join();
}
