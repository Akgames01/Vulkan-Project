#include "lve_pipeline.hpp"

#include<fstream>
#include<iostream>
#include<cassert>
namespace lve {

	//constructor is still to be implemented... 
	LvePipeline::LvePipeline(LveDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo) : lveDevice{ device } {
		createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
	}
	LvePipeline::~LvePipeline() {
		//vk destroy shader module to clean both vertex and fragment shaders. 
		vkDestroyShaderModule(lveDevice.device(), vertShaderModule, nullptr);
		vkDestroyShaderModule(lveDevice.device(), fragShaderModule, nullptr);
		vkDestroyPipeline(lveDevice.device(), graphicsPipeline, nullptr);
		//remove the empty body of the destructor in the pipeline header. 
		
	}
	std::vector<char> LvePipeline::readFile(const std::string& filepath){

		//initialize a new input filestream variable.
		std::ifstream file(filepath, std::ios::ate | std::ios::binary); //ate means we seeked the end immediately, and read as a binary file.
		if (!file.is_open()) {
			throw std::runtime_error("failed to open file:"+ filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());//fetches last postion for the file size 
		//character buffer
		std::vector<char> buffer(fileSize);
		//seek to the start of the file, as we need to read the data now. 
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}
	//creating the graphics pipeline
	void LvePipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo) {
		//here we will read in our vertex and fragment code. 
		//assertion so that our build doesn't proceed if we don't give a renderPass and pipelineLayout
		assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline:: no pipelineLatout provided  in configInfo");
		assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline:: no renderPass provided  in configInfo");
		auto vertCode = readFile(vertFilePath);
		auto fragCode = readFile(fragFilePath);

		/*std::cout << "Vertex shader code size" << vertCode.size() << "\n";
		std::cout << "Fragment shader code size" << fragCode.size() << "\n";*/
		//replacing these lines and initializing our shader module. 
		//calling create shader module. for both vertex shader and fragment shader. 
		createShaderModule(vertCode, &vertShaderModule);
		createShaderModule(fragCode, &fragShaderModule);

		//local variable for Vk pipleine shader stage
		VkPipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vertShaderModule;
		//name of the entry function.
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		//pNext and pSpecialization info as null pointers. 
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;
		//same has to be done for fragment shader.
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragShaderModule;
		
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		//pNext and p1pecialization info as null pointers. 
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		//pipeline vertex state. 
		//how we interpret the vertex input buffer data. 
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		//vertex attribute description count and binding count to 0
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;

		// new local variable for viewport state info.
		VkPipelineViewportStateCreateInfo viewportInfo{};//braces are key to set pNext as a nullptr. 
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &configInfo.viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &configInfo.scissor;
		//then goto swap chain class which has our choose swap present mode. 
		//pipeline create info all this config which used to create the graphics pipeline will be used. 
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;//number of programmable stages which the pipeline can use.
		//just for now we only vertex and fragment shaders 
		pipelineInfo.pStages = shaderStages;
		//wiring the pipeline create info to the config info.
		//as the configuration is separate from pipeline creation, we can reuse it multipel times. 
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		//update this to point to the locally created viewport info.
		pipelineInfo.pViewportState = &viewportInfo;//viewportInfo instead of viewport.
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		//multisample state
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;//use the colorBlendInfo and NOT colorBlendAttachment.
		pipelineInfo.pDynamicState = nullptr;//used to configure pipeline config dynamically without recreating it.

		//haven't configured these values yet.
		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;
		//base pipeline handle and base pipeline index 
		//used for optimizing performance -> easier for a GPU to create a new pipeline by using an existing one. 
		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		//build crashes as we haven't provided a render pass. 
		if (vkCreateGraphicsPipelines(lveDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline");
		}

	}
	void LvePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
		//local vk shader module create info variable 
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		//pointer to the code 
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		if (vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {

			throw std::runtime_error("failed to create a shader module");
		}
		//implementing default pipeline config information
		
		}
	PipelineConfigInfo LvePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
		PipelineConfigInfo configInfo{};
		//stype 
		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
		//these 3 belong to the first stage of the pipeline. 
		//input assembler sees the inputs as chunks by vertex, instead of 6 different values
		//how does the input assembler know that we want these 3 vertices to be a triangle ?
		//we specify that in the topology, by default we  have mentioned a triangle list. i.e., every 3 vertices will be grouped into triangles. 
		//another option is triangle strip, where every additional vertex uses previous 2 vertics to form the next triangle. 
		//more triangles without wasting memory. 
		//primitive restart variable, we can break the strip by adding a special index value in the buffer. 

		//configuring our viewport and scissor. 
		//viewport describes the transformation between pipeline's output and the target image. 
		//code reference https://pastebin.com/EmsJWHzb 

		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);
		configInfo.viewport.minDepth = 0.0f;//depth range of the viewport 
		configInfo.viewport.maxDepth = 1.0f;
		//scissor cuts the triangle instead of squishing.
		//any pixels outside the scissor rectangle will be discarded. 
		configInfo.scissor.offset = { 0, 0 };
		configInfo.scissor.extent = { width, height }; //if we set the scissor to only show the top half of our triangle, then only the top half will be drawn.
		//on certain GPUs it is possible to use multiple viewports and scissors by enabling a hardware level feature. 
		// the object was made because our object had members which were viewport and scissor. keeping them in same structure is problematic. suppose if we create a new pipeline it could deallocate the new objects once the older ones are removed. 
		//copy and paste these above. and place above pipeline creat info.
		

		//rasterization stage, breaks our geometry into fragments for each pixel and triangle overlap. 

		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;//forces the z component to be between 0 and 1. if it is <0 then it's something being behind the camera. and if it is >1 then it's too far away to see.
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;//discards all primitives before rasterization. only needed for using first few stages of the graphics pipeline.
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;//mode to draw the polygon.
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;//optionally discard triangles based on their facing, based on the "winding" order, order of vertices making the triangle and their apparent order on the screen. face depends on the point of view.
		//this property can be used to uniquely identify which side of a triangle we are seeing. front and back bit. back face culling can result in a large performance increase. which we'll look into later. 
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;//can be used to alter depth values by using constant value or by factor of the fragment slope. 
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional
		//more info. can be obeserved in the official docs. 
		//multisampling will be covered in depth (MSAA)
		//determines how the rasterizer handles the edges of the geometry
		//without multisampling a fragment is considered either in the triangle or outside the triangle. resulting in a pixelated look known as aliasing (edges looking jagged).
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional
		//color blending controls how we combine colours in our frame buffer. 
		// if 2 triangles overlap the fragment shader will return multiple colors for some pixels in the frame buffer.
		configInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;//mix the current output with the color value of the color in the frame buffer.
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
		//more will be covered while going through transparencies. 
		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional
		//depth buffer is an additional attachment to our frame buffer. stores the value for every pixel just like the color attachment stores the color.
		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};  // Optional
		configInfo.depthStencilInfo.back = {};   // Optional
		//now to complete our create graphics pipeline function.
		return configInfo;
	}
}