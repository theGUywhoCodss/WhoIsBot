#include <cpr/cpr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

// Get a steamgame with an api call.
std::string get_steamgame(std::string gameId) {
  // api call
  cpr::Response info =
      cpr::Get(cpr::Url{"https://steamspy.com/api.php"},
               cpr::Parameters{{"request", "appdetails"}, {"appid", gameId}});
  // error check
  if (info.status_code != 200)
    return std::to_string(info.status_code);
  try {
    // return text
    json game = json::parse(info.text);
    return info.text;
  } catch (const json::parse_error &e) {
    // return parse error
    return e.what();
  }
}
