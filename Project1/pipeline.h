#pragma once
//added device class 29/11/2023
#include "device.h"
//std
#include <string>
#include <vector>
namespace defined {
	//structure to organize data to create the respective pipeline
	struct PipelineConfigInfo {
		//deleting copy constructors.
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo operator=(const PipelineConfigInfo&) = delete;
		//VkViewport viewport; no longer needed in a dynamic viewport. 21:48 video #8 13/12/23
		//VkRect2D scissor; no longer needed in a dynamic scissor.
		//VkPipelineViewportStateCreateInfo viewportInfo;  it shouldn't be here, open implementation.
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		//2 new fields 21:48 video #8 13/12/23
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};
	class Pipeline {
	public:
		Pipeline(Device& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
		~Pipeline();
		//delete copy constructor
		Pipeline(const Pipeline&) = delete;
		Pipeline operator=(const Pipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		//static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height); video #8 13/12/23 only 1 parameter pipelineConfigInfo
		static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
	private:
		static std::vector<char> readFile(const std::string filePath);

		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
		//device reference, memory unsafe.
		//in case of implicit relations we can use it.
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		Device& deviceRef;
		VkPipeline graphicsPipeline;//handle to vulkan pipeline object
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}

