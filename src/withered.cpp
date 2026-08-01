#include "withered.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

// for convenience
using json = nlohmann::json;

// Get info from file
void write_info(const json &info) {
  std::ofstream out(WITHERED_FILE_PATH);
  out << info.dump();
}

// Get info from file
json get_info(int userid) {
  std::ifstream in(WITHERED_FILE_PATH);
  json j = json::object();

  if (in.peek() != std::ifstream::traits_type::eof()) {
    in >> j;
  }
  // AUTOMATICALLY CREATE PROFILE FOR NEW USERS
  std::string fId = std::to_string(userid);
  if (!j.contains(fId)) {
    std::cout << "New account for " + fId;
    j[fId] = {
        {"withered", false},
        {"recovered", 0},
    };
    write_info(j);
  }

  return j;
}

// Returns a string of the completed user message
std::string withered_message(int userid, std::string username) {
  json j = get_info(userid);
  std::string strId = std::to_string(userid);
  std::string status = j[strId]["withered"] ? "withered" : "not withered";
  return username + " is " + status + ", and has been recovered " +
         std::to_string(j[strId]["recovered"].get<int>()) + " times.";
}

void wither(int userid) {}

void unwither(int userid) {}
