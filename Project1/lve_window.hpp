#pragma once
//platform agnostic window tool.
#define GLFW_INCLUDE_VULKAN 
#include<GLFW/glfw3.h>
#include<string>
namespace lve {
	class LveWindow {
		
	private:
		//Member for initializing window. 
		void initWindow();
		const int width;
		const int height;
		std::string windowName;
		GLFWwindow *window; 
		//public constructor to initialize the properties. 
	public: 
		LveWindow(int w, int h, std::string name);
		~LveWindow();
		//we must delete the copy constructor and copy operator from the window class. 
		// as we are using a pointer to the glfw window.  as we don't want to make a window and have multiple pointers pointed at it.
		// and we don't want a free pointer dangling around....
		LveWindow(const LveWindow&) = delete;
		LveWindow& operator = (const LveWindow&) = delete;
		//adding the should close function here. 
		bool shouldClose() {
			return glfwWindowShouldClose(window);
		}
		//a getter for the newly added lve_swap_chain class. 
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }//typecasting these values around uint32
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	};
}
//class lve_window
//{
//};

