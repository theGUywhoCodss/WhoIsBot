#include "configdata.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

// for convenience
using json = nlohmann::json;
static std::mutex config_json_file_mutex;

// Local save of data
static json allData = json::object();

// Get info from variable
std::string get_config_info(std::string variable) {
  std::unique_lock<std::mutex> lock(config_json_file_mutex, std::try_to_lock);
  if (!lock.owns_lock())
    return "";
  // lock mutex
  return allData[variable].get<std::string>();
  // std::lock_guard<std::mutex> lock(json_file_mutex);
}

// Sets variable and returns true/false depending on if the file was currently
// being saved.
bool set_config_info(std::string variable, std::string info) {
  std::unique_lock<std::mutex> lock(config_json_file_mutex, std::try_to_lock);
  json temp = json::object();
  if (!lock.owns_lock())
    return false;
  // lock mutex
  // Override user data with the copy
  allData[variable] = info;
  std::ofstream out(CONFIG_FILE_PATH);
  out << allData.dump();
  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::cout << variable << " saved!" << std::endl;
  return true;
}

void load_config_info() {
  // Save data to global variable if there is a file.
  std::ifstream in(CONFIG_FILE_PATH);
  if (!in.is_open() || in.peek() == std::ifstream::traits_type::eof()) {
    std::cout << "NO FILE FOUND FOR CONFIG. WILL AUTOMATICALLY CREATE ONE NEXT "
                 "SAVE.\n";
    return;
  }
  in >> allData;
  in.close();

  std::cout << "Config file loaded\n";
}
