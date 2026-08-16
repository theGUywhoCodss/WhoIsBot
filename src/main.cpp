#include "configdata.h"
#include "witherdata.h"
#include "witherhandler.h"
#include <algorithm>
#include <dpp/appcommand.h>
#include <dpp/commandhandler.h>
#include <dpp/dpp.h>
#include <dpp/guild.h>
#include <dpp/nlohmann/json_fwd.hpp>
#include <dpp/snowflake.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
const int MAX_TOKENS = 10;
const std::vector<std::string> config_variables = {"reaper_role"};

// Checks if a member has a role
bool has_role(dpp::guild_member member, dpp::snowflake role) {
  const std::vector<dpp::snowflake> &user_roles = member.get_roles();

  return std::find(user_roles.begin(), user_roles.end(), role) !=
         user_roles.end();
}

// Get the SELECTED token from the ~/.config/discord_bot.TOKEN.txt file.
// (Include multiple tokens and you will be able to select between them).
std::string GET_TOKEN() {
  // Open the text file for reading
  std::ifstream file(TOKEN_FILE_PATH);

  // Check if the file was opened successfully
  if (!file.is_open()) {
    std::cerr << "Error opening the file!\n";
    return "";
  }

  std::string s;
  std::string TOKENS[MAX_TOKENS];
  int tokenCount = 0;
  // Get ALL Tokens from a file
  for (int i = 0; i < MAX_TOKENS; i++) {
    if (std::getline(file, s)) {
      TOKENS[i] = s;
      tokenCount++;
    } else
      break;
  }
  if (!tokenCount) {
    std::cerr << "No Tokens!";
    return "";
  } else if (tokenCount == 1) {
    return TOKENS[0];
  }
  // Close the file
  file.close();

  // Selection UI & return
  int selectedInt = 0;
  for (int i = 0; i < tokenCount; i++) {
    std::cout << i << ". " << TOKENS[i] << "\n";
  }
  std::cout << "Select a token!\n";
  std::cin >> selectedInt;
  return TOKENS[selectedInt];
}

int main() {
  dpp::cluster bot(GET_TOKEN());

  bot.on_log(dpp::utility::cout_logger());

  bot.on_slashcommand([&bot](const dpp::slashcommand_t &event) {
    // Find the current guild (server) from the cache using its ID
    dpp::guild *g = dpp::find_guild(event.command.guild_id);

    if (g != nullptr) {

      // Saves the data of all files(MUST BE OWNER)
      if (event.command.get_command_name() == "save") {
        if (g->owner_id != event.command.get_issuing_user().id) {
          event.reply("MUST BE OWNER");
          return;
        }
        std::cout << "MANUAL SAVE IN PROGRESS\n";
        event.reply("Your save request is being processed. Please wait.");
        save_withered();
        event.follow_up(event.command.get_issuing_user().get_mention() +
                        "'s save request has been fullfilled.");
      } else if (event.command.get_command_name() == "ping") {
        // Gets ping
        event.reply("Pong! Ping is " + std::to_string(bot.rest_ping) +
                    " seconds.");
      } else if (event.command.get_command_name() == "withered") {
        // Replies with user withered information
        dpp::snowflake user_id =
            std::get<dpp::snowflake>(event.get_parameter("another_user"));

        const dpp::user &person = event.command.get_resolved_user(user_id);
        event.reply(withered_message(person.id, person.get_mention()));
      } else if (event.command.get_command_name() == "wither") {
        // Withers the user if the author has the reaper role.
        // Check for role
        if (!has_role(event.command.member,
                      std::stoll(get_config_info("reaper_role")))) {
          event.reply("You do not have the reaper role.");
          return;
        }

        // Wither the user if role
        dpp::snowflake user_id =
            std::get<dpp::snowflake>(event.get_parameter("another_user"));
        const dpp::user &person = event.command.get_resolved_user(user_id);
        event.reply(
            wither(person.id, person.get_mention(),
                   std::get<std::string>(event.get_parameter("reason"))));
      } else if (event.command.get_command_name() == "unwither") {
        // Unwithers the user if the author has the reaper role.
        // Check for role

        if (!has_role(event.command.member,
                      std::stoll(get_config_info("reaper_role")))) {
          event.reply("You do not have the reaper role.");
          return;
        }

        // Wither the user if role
        dpp::snowflake user_id =
            std::get<dpp::snowflake>(event.get_parameter("another_user"));
        const dpp::user &person = event.command.get_resolved_user(user_id);
        event.reply(unwither(person.id, person.get_mention()));
      } else if (event.command.get_command_name() == "config") {
        // Sets a variable that is used in the code!
        if (g->owner_id != event.command.get_issuing_user().id) {
          event.reply("MUST BE OWNER");
          return;
        }
        std::cout << "CONFIG CHANGE IN PROGRESS\n";
        event.reply("Your config request is being processed. Please wait.");

        // Sets the data
        set_config_info(std::get<std::string>(event.get_parameter("variable")),
                        std::get<std::string>(event.get_parameter("value")));

        event.follow_up(event.command.get_issuing_user().get_mention() +
                        "'s config request has been fullfilled.");
      }
    }
  });

  bot.on_ready([&bot](const dpp::ready_t &event) {
    std::cout << "PRINTING OPERATIONAL" << std::endl;
    if (dpp::run_once<struct register_bot_commands>()) {
      // Define new commands
      dpp::slashcommand ping("ping", "Ping pong!", bot.me.id);
      dpp::slashcommand withered("withered", "Is someone withered?", bot.me.id);
      dpp::slashcommand save(
          "save", "Save ALL data (More dynamic options in the future)",
          bot.me.id);
      dpp::slashcommand wither("wither", "Wither someone...", bot.me.id);
      dpp::slashcommand unwither("unwither", "Cure someone!", bot.me.id);
      dpp::slashcommand config(
          "config", "Set a variable to be used in the code.", bot.me.id);

      // Add options
      withered.add_option(dpp::command_option(dpp::co_user, "another_user",
                                              "mention a user", true));
      wither
          .add_option(dpp::command_option(dpp::co_user, "another_user",
                                          "mention a user", true))
          .add_option(dpp::command_option(dpp::co_string, "reason",
                                          "reason for their death", true));
      unwither.add_option(dpp::command_option(dpp::co_user, "another_user",
                                              "mention a user", true));
      // Add bulk options
      dpp::command_option config_keys(dpp::co_string, "variable", "key to set",
                                      true);

      for (const auto &element : config_variables) {
        config_keys.add_choice(dpp::command_option_choice(element, element));
      }
      config.add_option(config_keys);
      config.add_option(dpp::command_option(dpp::co_string, "value",
                                            "value the key will store", true));
      // Push all commands
      bot.global_bulk_command_create(
          {ping, withered, wither, unwither, save, config});
      std::cout << "COMMANDS CREATED!\n";
      // Load
      load_config_info();
      wither_init();
    }
  });

  bot.start(dpp::st_wait);
}
