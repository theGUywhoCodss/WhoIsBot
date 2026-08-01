#include "withered.h"
#include <cstddef>
#include <dpp/dpp.h>
#include <dpp/snowflake.h>
#include <fstream>
#include <iostream>
#include <string>

const int MAX_TOKENS = 10;

// Get the SELECTED token from the ~/.config/discord_bot.TOKEN.txt file.
// (Include multiple tokens and you will be able to select between them).
std::string GET_TOKEN() {
  // Open the text file for reading
  std::ifstream file(TOKEN_FILE_PATH);

  // Check if the file was opened successfully
  if (!file.is_open()) {
    std::cerr << "Error opening the file!\n";
    return NULL;
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
    return NULL;
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
    if (event.command.get_command_name() == "ping") {
      event.reply("Pong! Ping is " + std::to_string(bot.rest_ping) +
                  " seconds.");
    } else if (event.command.get_command_name() == "withered") {
      dpp::snowflake user_id =
          std::get<dpp::snowflake>(event.get_parameter("another_user"));

      const dpp::user &person = event.command.get_resolved_user(user_id);

      event.reply(withered_message(person.id, person.get_mention()));
    }
  });

  bot.on_ready([&bot](const dpp::ready_t &event) {
    if (dpp::run_once<struct register_bot_commands>()) {
      dpp::slashcommand ping("ping", "Ping pong!", bot.me.id);
      dpp::slashcommand wither("withered", "Is someone withered?", bot.me.id);

      wither.add_option(dpp::command_option(dpp::co_mentionable, "another_user",
                                            "Mention a user", true));

      bot.global_bulk_command_create({ping, wither});
    }
  });

  bot.start(dpp::st_wait);
}
