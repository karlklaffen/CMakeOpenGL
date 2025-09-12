#include <filesystem>
#include <any>

#include "TestGame.h"

int main() {

	std::cout << "Current Filesystem Path: " << std::filesystem::current_path() << std::endl;
	
	TestGame game("Game", 960, 540);

	if (game.InitializationIncomplete())
		return -1;

	game.Run();

	return 0;
}
