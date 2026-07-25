#include <cstddef>
#include <dpp/dpp.h>
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

  bot.on_slashcommand([](const dpp::slashcommand_t &event) {
    if (event.command.get_command_name() == "ping") {
      event.reply("Pong!");
    }
  });

  bot.on_ready([&bot](const dpp::ready_t &event) {
    if (dpp::run_once<struct register_bot_commands>()) {
      bot.global_command_create(
          dpp::slashcommand("ping", "Ping pong!", bot.me.id));
    }
  });

  bot.start(dpp::st_wait);
}
