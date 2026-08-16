#include "witherhandler.h"
#include "witherdata.h"
#include <nlohmann/json.hpp>
#include <string>

// for convenience
using json = nlohmann::json;
static const std::string save_data_message =
    "CURRENTLY SAVING DATA. TRY AGAIN LATER.";

// Returns a string of the completed user message
std::string withered_message(int userid, std::string username) {
  json j = get_withered_info(userid);
  if (j.is_null())
    return save_data_message;
  // Get quote IF withered
  std::string quote = "";
  if (j["withered"])
    quote += "\n\"" + j["reason"].get<std::string>() + "\"";

  // Get status in the form of the string
  std::string status = j["withered"] ? "withered" : "not withered";
  // Return completed message
  return username + " is " + status + ", and has been recovered " +
         std::to_string(j["recovered"].get<int>()) + " times." + quote;
}

// Withers the user and returns a message
std::string wither(int userid, std::string username, std::string reason) {
  json j = get_withered_info(userid);
  if (j.is_null())
    return save_data_message;

  if (j["withered"]) {
    return "Already withered.";
  } else {
    // Pass info back to override new data
    j["withered"] = true;
    j["reason"] = reason;
    if (!set_withered_info(userid, j))
      return save_data_message;
    return username + " has been WITHERED.\n\"" + reason + "\"";
  }
}

// Unwithers the user and returns a message
std::string unwither(int userid, std::string username) {
  json j = get_withered_info(userid);
  if (j.is_null())
    return save_data_message;

  if (!j["withered"]) {
    return "They are alive and well.";
  } else {
    // Pass info back to override new data
    j["withered"] = false;
    j["recovered"] = j["recovered"].get<int>() + 1;
    j["reason"] = "";
    if (!set_withered_info(userid, j))
      return save_data_message;
    return username + " has been CURED!";
  }
}

void wither_init() {
  load_withered_info();
  enable_withered_autosave();
}
