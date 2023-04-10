#include "first_app.hpp"

//implementing the run function 
//std
#include<stdexcept>
namespace lve {
	FirstApp::FirstApp() {
		//calls to create pipeline layout and create pipeline.
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}
	//be sure to destroy the pipeline layout in the destructor with VK_destroy
	FirstApp::~FirstApp() {
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
		//if we add empty implementations of create command buffers and draw frame function, our code will compile.
	}
	void FirstApp::run() {
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();//window events can be key strokes etc. 
		}
	}
	void FirstApp::createPipelineLayout() {
		//creating pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;//used to pass data other than our vertex data to vertex and fragment shaders. 
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr; //efficient way to send small amount of data to our shader programs. 
		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {

			throw std::runtime_error("failed to create a pipeline layout.");
		}
	}
	//implementing the create pipeline function. 
	void FirstApp::createPipeline() {
		auto pipelineConfig = LvePipeline::defaultPipelineConfigInfo(lveSwapChain.width(), lveSwapChain.height());//important to use swap chain width and height.
		//on high ppi displays are smaller than the window contains. 
		pipelineConfig.renderPass = lveSwapChain.getRenderPass();//we skipped creating a default render pass and used the one from swap chain instead. 
		//render pass describes the structure and format of our frame buffer objects and their attachments. 
		//a blueprint for a pipeline to explain what layout to expect. 
		// for complicated render passes e.g., post processing effects, multiple sub-passes can be grouped into one render pass.
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(
			lveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);
	}
	//empty implementation to test compilation
	//as we've provided the pipeline layout and config our assertions which failed before should work now.
	void FirstApp::createCommandBuffers(){}
	void FirstApp::drawFrame(){}
}