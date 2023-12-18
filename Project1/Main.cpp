#include "App.h"
#include <stdexcept>
#include <cstdlib>
#include <iostream>

int main() {
	defined::App app{}; //instance of the app.
	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}