#pragma once 

#include "lve_device.hpp"
//std
#include<string>
#include<vector>


namespace lve {
	//structure to organize data. to configure the graphics pipeline. data specifying how to configure the graphics pipeline.
	struct PipelineConfigInfo {
		//reason for pulling this out of pipeline class is, we want our application layer code to configure our pipeline completely 
		//as well as to share it among multiple pipelines. 
		//explicitly set the values configuring how the stages of our pipeline work. 
		//each of these members need to be properly initialized. 
		//which will be done in the default pipeline config info. helper function.
		//fixing a problem from video #4
		VkViewport viewport;
		VkRect2D scissor;
		//VkPipelineViewportStateCreateInfo viewportInfo; removing this first and then open the pipeline implementation file.
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		//no default for these members and are set outside the default function.
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};
	class LvePipeline {
	public:
		//copy this constructor and go to the implementation file 
		LvePipeline(LveDevice &device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
		~LvePipeline();//implemented in the cpp file.
		//delete the copy constructors. 
		//as we want to avoid duplicating pointers to our vulkan objects. 
		LvePipeline(const LvePipeline&) = delete;
		void operator = (const LvePipeline&) = delete;
		//static method for creating a default pipeline config. 
		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
	private:
		static std::vector<char> readFile(const std::string& filepath);
		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
	
		//takes shader code in form of vectors and a pointer to a shader module.
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule); //used to craete the module and initialize the variable
		//storing the device reference, could be memory unsafe, used with the implicit relationship with the device.
		// will outlive other instances
		//also known as aggregation in UML terms
		LveDevice& lveDevice;
		//handle to vulkan pipeline object. 
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;//type def pointers in this case. 
		VkShaderModule fragShaderModule;
	};
}