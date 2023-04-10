//class which controls the app. 
#pragma once
#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
//including swap chain 
#include "lve_swap_chain.hpp"
//same namespace.
//memory 
#include <memory>
#include <vector>

namespace lve {
	class FirstApp
		{
			public:
				static constexpr int WIDTH = 800;
				static constexpr int HEIGHT = 600;
				//run function
				void run();
				//constructor and destructor for first app.
				FirstApp();
				~FirstApp();
				//always delete the copy constructors. 
				FirstApp(const FirstApp&) = delete;
				FirstApp& operator = (const FirstApp&) = delete;
			private: 
			//some private members 
			// implementing these functions in the .cpp file
				void createPipelineLayout();
				void createPipeline();
				void createCommandBuffers();
				void drawFrame();
			//private member for lveWindow
				//when this class is made a window would be created. 
			LveWindow lveWindow{WIDTH,HEIGHT, "Base engine"};
			LveDevice lveDevice{lveWindow};
			//swapchain variable
			LveSwapChain lveSwapChain{ lveDevice,lveWindow.getExtent() };//variables initialized from top to bottom. and are destroyed in the reverse order. the selected mode will show up just before the failed assertion from video #4
			//we'll make the pipeline object a unique pointer using the imported memory library. 
			//LvePipeline lvePipeline{lveDevice, "simple_shader.vert.spv","simple_shader.frag.spv", LvePipeline::defaultPipelineConfigInfo(WIDTH,HEIGHT)};
			//new pipeline object 
			//smart pointer simulates a pointer with an addition of automatic memory management -> we're no longer responsible for calling new/delete.
			std::unique_ptr<LvePipeline> lvePipeline;
			//private VkPipeline layout member variable.
			VkPipelineLayout pipelineLayout;
			//vector for Vk command buffer objects. 
			std::vector<VkCommandBuffer> commandBuffers; 
			//we'll cover these in a few minutes.
		};
}
