
#include "window.h"

#include <stdexcept>
namespace defined {
	Window::Window(int h, int w, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}
	Window::~Window() {
		glfwDestroyWindow(windowptr);
		glfwTerminate();
	}
	void Window::initWindow() {
		//glfw initalizer
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); modified in video #8 21:06 13/12/23
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		windowptr = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr); //4th param is for fullscreen window and 5th is openGL context.
		//video #8
		glfwSetWindowUserPointer(windowptr, this);
		glfwSetFramebufferSizeCallback(windowptr, frameBufferResizeCallback);
	}
	void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, windowptr, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create a window surface");
		}
	}
	void Window::frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto windowX = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		//set frame buffer resized to true
		windowX->frameBufferResized = true;
		windowX->width = width;
		windowX->height = height;
	}
}