#include "service_helper.hpp"

bool ServiceHelper::isRunning() {
	const char *command = "systemctl status midicmd | grep running | wc -l";
	std::array<char, 128> buffer;
	std::string result = "";
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);

	if (!pipe) throw std::runtime_error("popen() failed!");
	
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	
	return (result.compare("1\n") == 0);
}


void ServiceHelper::start() {
	if (!isRoot()) {
		std::cout << toRed("This command required sudo privileges!") << std::endl;
		std::cout << "Try again with " << toYellow("sudo midicmd start") << std::endl;
		return;
	}

	if (isRunning()) {
		std::cout << toYellow("Daemon is already running!") << std::endl;
		return;
	}

	int rc = system("systemctl start midicmd.service");
	if (rc != 0) {
		std::cout << toRed("Failed to start the daemon!") << std::endl;
		exit(1);
	} else {
		std::cout << toYellow("Daemon started!") << std::endl;
	}
}

void ServiceHelper::stop() {
	if (!isRoot()) {
		std::cout << toRed("This command required sudo privileges!") << std::endl;
		std::cout << "Try again with " << toYellow("sudo midicmd stop") << std::endl;
		return;
	}

	if (!isRunning()) {
		std::cout << "Daemon is not running!" << std::endl;
		return;
	}

	int rc = system("systemctl stop midicmd.service");
	if (rc != 0) {
		std::cout << toRed("Failed to stop daemon!") << std::endl;
	} else {
		std::cout << toYellow("Daemon stopped!") << std::endl;
	}
}

void ServiceHelper::status() {
	if (!isRoot()) {
		std::cout << toRed("This command required sudo privileges!") << std::endl;
		std::cout << "Try again with " << toYellow("sudo midicmd status") << std::endl;
		return;
	}

	std::cout << "Daemon status: ";
	if (isRunning()) {
		std::cout << toGreen("running") << std::endl;
	} else {
		std::cout << toRed("stopped") << std::endl;
	}
}

void ServiceHelper::init() {
	// TODO
}

void ServiceHelper::run() {
	midiHelper->setInputDevice(configHelper->getDevice());
	bool feedback = configHelper->isFeedbackEnabled();

	std::map<int, const char*> commands = configHelper->getCommands();
	std::map<int, const char*>::iterator iter;
	MidiEvent event;
	while (true) {
		event = midiHelper->getMessage();

		if (event.isOk()) {
			iter = commands.find(event.getCode());

			if (iter != commands.end()) {
				system(iter->second);
			}
		}

		usleep(10);
	}
}

void ServiceHelper::help() {
	using std::cout; using std::endl;

	cout << toYellow("Usage: ") << "midicmd [OPTION]" << endl << endl;
	cout << "Available options: " << endl;

	cout << toBold("    command [OPTION]\t\t") << "Manage macros" << endl;
	cout << toBold("    config [OPTION]\t\t") << "Manage the config file" << endl;
	cout << toBold("    device [OPTION]\t\t") << "Manage MiDi devices" << endl;
	cout << toBold("    feedback [OPTION] \t\t") << "Enable/Disable MiDi feedback" << endl;
	cout << endl;
	
	cout << toBold("    start\t\t") << "Start the midicmd daemon" << endl;
	cout << toBold("    stop\t\t") << "Stop the midicmd daemon" << endl;
	cout << toBold("    run\t\t") << "Starts the actual midi driver in the foreground. Mostly used for testing purposes." << endl;
	
	cout << toBold("    ps\t\t") << endl;
	cout << toBold("    status\t\t") << "Reports the status of the midicmd daemon [running/stopped]" << endl;
}