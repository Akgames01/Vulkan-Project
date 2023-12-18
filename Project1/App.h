#pragma once

#include "window.h"
#include "pipeline.h"
#include "device.h"
#include "swap_chain.h"
#include "model.h"
#include "gameobject.h"
//std
#include <memory>//for making the pipeline object a unique pointer
#include <vector>

namespace defined {

	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		App();
		~App();
		App(const App&) = delete;
		App& operator = (const App&) = delete;
		void run();
	private:
		//loading models 07/12/23
		//void loadModels();
		void loadGameObjects();//replacement for load models.
		//some methods
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		// video 8 21:12 13/12/23
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);
		Window mainWindow{ HEIGHT, WIDTH, "First Vulkan app" };
		Device deviceMain{ mainWindow };
		// modified in video 8 21:12 13/12/23
		//SwapChain swapChain{ deviceMain, mainWindow.getExtent() };
		std::unique_ptr<SwapChain> swapChain;
		//smart pointer for the pipeline
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		//std::unique_ptr<Model> model; video 10 00:00 17/12/23
		std::vector<GameObject> gameObjects;
	};
}

