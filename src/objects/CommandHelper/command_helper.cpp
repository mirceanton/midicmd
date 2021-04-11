#include "command_helper.hpp"

CommandHelper::CommandHelper(MidiHelper *midi, ConfigHelper *config) {
	midiHelper = midi;
	configHelper = config;
}
CommandHelper::~CommandHelper() { /* we don't delete here, but in ArgParser */ }

void CommandHelper::add() {
	//TODO
}

void CommandHelper::list() {
	std::map<int, const char*> commands = configHelper->getCommands();

	if (commands.size() == 0) {
		std::cout << toRed("There are no commands in the config file!") << std::endl;
		std::cout << "You can add some with: " << toYellow("sudo midicmd command add") << std::endl;
		return;
	}

	std::cout << toYellow("There are " + std::to_string(commands.size()) + " commands in the config file");
	std::cout << std::endl;
	for (auto it = commands.begin(); it != commands.end(); it++) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
}

void CommandHelper::remove() {
	if (!isRoot()) {
		std::cout << toRed("Removing a command requires sudo privileges in order to edit the config file!");
		std::cout << std::endl;
		return;
	}

	std::map<int, const char*> commands = configHelper->getCommands();
	if (commands.size() == 0) {
		std::cout << toYellow("There are no commands configured.") << std::endl;
		std::cout << toYellow("There is nothing to delete.") << std::endl;
		return;
	}
	list();

	int value = 0;
	bool is_value_ok = false;
	std::string user_input = "";

	std::cout << std::endl;
	do {
		std::cout << "Enter the event ID of the command you want to remove: ";
		std::cin >> user_input;
		try {
			value = std::stoi(user_input);
			is_value_ok = (commands.find(value) != commands.end());
			if (!is_value_ok) {
				std::cout << toRed("Invalid value!") << std::endl;
			}
		} catch (std::invalid_argument &err) {
			std::cout << toRed("Invalid argument!") << std::endl;
			is_value_ok = false;
		}
	} while (!is_value_ok);

	if (commands.erase(value) == 1) {
		configHelper->setCommands(commands);
		std::cout << toGreen("Command removed!") << std::endl;
	} else {
		std::cout << toRed("Failed to remove command!") << std::endl;
	}
}

void CommandHelper::help() {
	using std::cout; using std::endl;

	cout << toYellow("Usage: ") << "midicmd command [OPTION]" << endl << endl;
	cout << "Available options: " << endl;

	cout << toBold("    add\t\t") << "Run through the wizard to add a new command mapping." << endl;

	cout << toBold("    rm\t\t") << endl;
	cout << toBold("    remove\t\t") << "Remove an existing command." << endl;

	cout << toBold("    ls\t\t") << endl;
	cout << toBold("    list\t\t") << "Lists all commands." << endl;
	cout << endl;
}
