#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace defined {
	class Window
	{
	public:
		Window(int h, int w, std::string name);
		~Window();
		Window(const Window&) = delete;
		Window& operator = (const Window&) = delete;
		bool shouldClose() {
			return glfwWindowShouldClose(windowptr);
		}
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		bool wasWindowResized() { return frameBufferResized; }
		void resetWindowResizedFlag() { frameBufferResized = false; }
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();
		int height;
		int width;
		//flag to denote that the window size has changed. 20:57 13/12/23 video #8
		bool frameBufferResized = false;
		std::string windowName;
		GLFWwindow* windowptr;
	};

}