#include "lve_window.hpp"


#include<stdexcept>
namespace lve {
	LveWindow::LveWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{name} {
		initWindow();
	}
	LveWindow::~LveWindow() {
		glfwDestroyWindow(window);//passing the window pointer. 
		glfwTerminate();
		//this is done now.
		//now we'll make a new class which controls the application.

	}
	void LveWindow::initWindow() {
		glfwInit();//initializing glfw library
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//telling it to not create an openGL project.
			//disabling window to resize after creation. 
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		//initializing the window pointer. 
		window = glfwCreateWindow(width, height, windowName.c_str(),nullptr,nullptr);//4th parameter is fullscreen and 5th parameter will be explained later.
	}
	void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance,window, nullptr,surface) != VK_SUCCESS ) {
			throw std::runtime_error("failed to create window surface");
		}
	}
}