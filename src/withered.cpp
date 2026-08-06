#include "withered.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

// for convenience
using json = nlohmann::json;
std::mutex json_file_mutex;
std::thread save_thread;

// Local save of data
json allData = json::object();
bool autoSave = true;
const int save_delay = 300;
const std::string save_data_message = "CURRENTLY SAVING DATA. TRY AGAIN LATER.";

// Save info to file
void save_withered() {
  std::lock_guard<std::mutex> lock(json_file_mutex);

  std::ofstream out(WITHERED_FILE_PATH);
  out << allData.dump();
  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::cout << "Saved!" << std::endl;
}

// Get info from variable with userid WITH no-account protection. if file is not
// being accessed, returns json else returns empty json (NULL).
json get_info(int userid) {
  std::unique_lock<std::mutex> lock(json_file_mutex, std::try_to_lock);
  json temp = json::object();
  if (!lock.owns_lock())
    return json();
  // lock mutex
  // AUTOMATICALLY CREATE PROFILE FOR NEW USERS
  std::string fId = std::to_string(userid);
  if (!allData.contains(fId)) {
    std::cout << "New account for " + fId << std::endl;
    allData[fId] = {
        {"withered", false},
        {"recovered", 0},
    };
  }
  return allData[fId];
  // std::lock_guard<std::mutex> lock(json_file_mutex);
}

// Sets variable and returns true/false depending on if the file was currently
// being saved.
bool set_info(int userid, json &info) {
  std::unique_lock<std::mutex> lock(json_file_mutex, std::try_to_lock);
  json temp = json::object();
  if (!lock.owns_lock())
    return false;
  // lock mutex
  // Override user data with the copy
  allData[std::to_string(userid)] = info;
  return true;
}

// LOOP FOR IN THE INIT AND DESTRUCTOR
void save_runner() {
  std::this_thread::sleep_for(std::chrono::seconds(save_delay));
  while (autoSave) {
    std::cout << "Autosaving!\n";
    save_withered();
    std::this_thread::sleep_for(std::chrono::seconds(save_delay));
  }
}

// Startup Wither
void withered_init() {
  save_thread = std::thread(save_runner);
  save_thread.detach();

  // Save data to global variable if there is a file.
  std::ifstream in(WITHERED_FILE_PATH);
  if (!in.is_open() || in.peek() == std::ifstream::traits_type::eof()) {
    std::cout << "NO FILE FOUND FOR WITHER. WILL AUTOMATICALLY CREATE ONE NEXT "
                 "SAVE.\n";
    return;
  }
  in >> allData;
  in.close();

  std::cout << "WITHER SAVER AND FILE LOADED\n";
}

void withered_destroy() {
  autoSave = false;
  save_thread.join();
}

// Returns a string of the completed user message
std::string withered_message(int userid, std::string username) {
  json j = get_info(userid);
  if (j.is_null())
    return save_data_message;

  std::string status = j["withered"] ? "withered" : "not withered";
  return username + " is " + status + ", and has been recovered " +
         std::to_string(j["recovered"].get<int>()) + " times.";
}

// Withers the user and returns a message
std::string wither(int userid, std::string username) {
  json j = get_info(userid);
  if (j.is_null())
    return save_data_message;

  if (j["withered"]) {
    return "Already withered.";
  } else {
    // Pass info back to override new data
    j["withered"] = true;
    if (!set_info(userid, j))
      return save_data_message;
    return username + " has been WITHERED.";
  }
}

// Unwithers the user and returns a message
std::string unwither(int userid, std::string username) {
  json j = get_info(userid);
  if (j.is_null())
    return save_data_message;

  if (!j["withered"]) {
    return "They are alive and well.";
  } else {
    // Pass info back to override new data
    j["withered"] = false;
    j["recovered"] = j["recovered"].get<int>() + 1;
    if (!set_info(userid, j))
      return save_data_message;
    return username + " has been CURED!";
  }
}
