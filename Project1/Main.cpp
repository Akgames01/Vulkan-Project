#include "first_app.hpp"
//headers from std library.
#include <cstdlib>
#include <iostream>
#include <stdexcept>	

int main() {
	lve::FirstApp app{};//instance of our app.

	try
	{
		app.run();
	}
	catch (const std::exception&e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;

	}
	return EXIT_SUCCESS;
}