#include "App.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <array>
namespace defined {
	//video #9 12:30 15/12/23
	struct SimplePushConstantData {
		glm::mat2 transform{ 1.f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};
	//constructor
	App::App() {
		//loadModels();
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
		//createPipeline();replaced by recreateSwapChain to adjust for resolution.
		createCommandBuffers();
	}
	App::~App() {
		vkDestroyPipelineLayout(deviceMain.device(), pipelineLayout, nullptr);
	}
	void App::run() {
		while (!mainWindow.shouldClose()) {
			glfwPollEvents();
			//call to the draw frame functions. 21:39 06/12/23 video #5 part 2
			drawFrame();
		}
		vkDeviceWaitIdle(deviceMain.device());
	}
	//void App::loadModels() {
	void App::loadGameObjects() {
		//used to initialize vertex data positions.
		std::vector<Model::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},//added color value in the second argument of this entry 02:00 13/12/23 tutorial video #7
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		auto model = std::make_shared<Model>(deviceMain, vertices);//shared pointer as one model can be used by multiple game objects
		//it will stay in memory as long as one game object is using it
		auto triangle = GameObject::createGameObject();
		triangle.model = model;
		triangle.color = { 0.1f, 0.8f, 0.1f };
		triangle.transform2d.translation.x = 0.2f; //move the triangle slightly to the right.
		triangle.transform2d.scale = { 2.f, .5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();
		//we'll be using pi constant -> 3.14, radians and not degrees.

		gameObjects.push_back(std::move(triangle));//that's why default was needed in game objects class.

	}
	void App::createPipelineLayout() {
		//video #9 12:30 15/12/23 push constants range to be provided here.
		VkPushConstantRange pushConstantsRange{};
		pushConstantsRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;//indicates we need push constants data in both vertex and fragment shader stages.
		pushConstantsRange.offset = 0;
		pushConstantsRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		//an empty layout for now 22:30 video #5 5/12/23.
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;//video #9 12:30 15/12/23 push constant range count as 1.
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantsRange;//move to record command buffer function.
		if (vkCreatePipelineLayout(deviceMain.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}
	void App::createPipeline() {
		//creating a pipeline config
		//auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(swapChain->width(), swapChain->height()); changed in video #8 22:04 13/12/23
		assert(swapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		//render pass via swap chain
		pipelineConfig.renderPass = swapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipeline = std::make_unique<Pipeline>(
			deviceMain,
			"simple_shader.vert.spv",
			"simple_shader.frag.spv",
			pipelineConfig);
	}
	void App::createCommandBuffers() {
		// video #5 part 2 20:00 06/12/23
		commandBuffers.resize(swapChain->imageCount());
		//allocating the command buffer.
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = deviceMain.getCommandPool();
		//command buffer size to the size of the command buffer vector.
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(deviceMain.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}
		//commented this loop video #8 21:20 13/12/23. shifted in the new method recordCommandBefore, underneath
		/*for (int i = 0; i < commandBuffers.size(); i++) {
		}*/
	}
	void App::freeCommandBuffers() {
		vkFreeCommandBuffers(deviceMain.device(), deviceMain.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}
	//new method video #8 21:20 13/12/23
	void App::recordCommandBuffer(int imageIndex) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("Failed to begin recording command buffer!");
			}
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			//get the one from swap chain
			renderPassInfo.renderPass = swapChain->getRenderPass();
			//framebuffer this render pass is writing to.
			renderPassInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);
			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();//NOT the window extent.

			//clear values
			std::array<VkClearValue, 2> clearValues{};
			//for adding motion, creating a darker background
			//clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f };//clear values 1 instead of 0, because of depth attachment at #1.
			clearValues[0].color = { 0.01f, 0.01f, 0.01f, 0.1f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
			//recording to the command buffer.
			vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			//configure dynamic viewport and scissor
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
			viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor{ {0,0} , swapChain->getSwapChainExtent() };
			vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
			vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);
			/*pipeline->bind(commandBuffers[imageIndex]);
			model->bind(commandBuffers[imageIndex]);
			//video #9 12:30 15/12/23 added for loop
			for (int j = 0; j < 4; j++) {
				SimplePushConstantData push{};
				push.offset = { -0.5f + frame * 0.002f, -0.4f + j * 0.25f };
				push.color = { 0.0f, 0.0f, 0.2f + j * 0.2f };//copies of our model varying in color from dark to lighter blue.
				//need to record the push constant data to the command buffer.
				vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
				model->draw(commandBuffers[imageIndex]);//4 copies of our model, each copy with slightly different push data.
				//update the shader files to expect the push data.
			}*/
			//replaced above with call to render game objects
			renderGameObjects(commandBuffers[imageIndex]);

			//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0); removed in video #6
			//end recording
			vkCmdEndRenderPass(commandBuffers[imageIndex]);
			if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
				//these are for debugging purpose only. for a production application they must be handled in an effective way.
				throw std::runtime_error("Failed to record the command buffer!");
			}

	}
	void App::renderGameObjects(VkCommandBuffer commandBuffer) {
		pipeline->bind(commandBuffer);
		//looping through all game objects. updating hte same push constants
		for (auto& obj : gameObjects) {
			//added 00:26 17/12/23 video #10, always remember the order of matrix multiplication can change the way our triangle is rotating.
			obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());//continuously rotate the object in a full circle.
			//copied from record command buffer function
			SimplePushConstantData push{};
			push.offset = obj.transform2d.translation;
			push.color = obj.color;
			push.transform = obj.transform2d.mat2();
			//need to record the push constant data to the command buffer.
			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}
	//implementing the recreate swap chain method.
	void App::recreateSwapChain() {
		auto extent = mainWindow.getExtent();//gets current window size.
		while (extent.width == 0 || extent.height == 0) {
			extent = mainWindow.getExtent();
			glfwWaitEvents();
		}
		//putting the device on idle
		vkDeviceWaitIdle(deviceMain.device());
		//easy wait when the current swap chain is no longer being used before we've made the new one.
		//create new swap chain
		if (swapChain == nullptr) {
			swapChain = std::make_unique<SwapChain>(deviceMain, extent);
		}
		else {
			swapChain = std::make_unique<SwapChain>(deviceMain, extent, std::move(swapChain));
			if (swapChain->imageCount() != commandBuffers.size()) {
				//if the image count is not equal to the size of current command buffers, we'll clear the buffers and recreate them.
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		//since pipeline also depends on the swap chain, we also need to create a new one
		createPipeline();//replace the createPipeline() in the constructor from recreateSwapChain().
	}
	void App::drawFrame() {
		uint32_t imageIndex;
		auto result = swapChain->acquireNextImage(&imageIndex);
		//check whether the swapChain is remade and decide to recreate it or not. 21:35 13/12/23 video #8
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			//this error can occur when the window is resized.
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			//for now a runtime error, but this must be handled properly.as it can occur if a window is resized.
			throw std::runtime_error("failed to acquire a swap chain image!");
		}
		//record command buffer as it's no longer done at the startup
		recordCommandBuffer(imageIndex);
		result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		//submits the command buffer to the provided graphics device queue. while handling CPU and GPU synchronization.
		//command buffer will be executed and swap chain will present the color attachment to the end user based on the present mode.
		//21:35 13/12/23 video #8
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mainWindow.wasWindowResized()) {
			mainWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to acquire a swap chain image!");
		}
	}
}