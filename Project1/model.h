#pragma once
#include "device.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <vector>

namespace defined {

	class Model
	{
		public:
			struct Vertex {
				glm::vec2 position;
				//value to be passed onto fragment shader
				glm::vec3 color;
				static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
				static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
			};
			Model(Device& device, const std::vector<Vertex>& vertices);
			~Model();
			//must delete the copy constructor
			Model(const Model&) = delete;
			Model& operator = (const Model&) = delete;
			void bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer);
		private:
			void createVertexBuffers(const std::vector<Vertex> &vertices);
			Device& deviceRef;
			VkBuffer vertexBuffer;
			VkDeviceMemory vertexBufferMemory;
			//buffer and memory are 2 separate objects. giving the programmer more control.
			uint32_t vertexCount;
	};
}

