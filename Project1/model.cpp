#include "model.h"

//std
#include <cassert>
#include <cstring>
namespace defined {
	Model::Model(Device& device, const std::vector<Vertex>& vertices) :deviceRef{ device } {
		createVertexBuffers(vertices);
	}
	Model::~Model() {
		vkDestroyBuffer(deviceRef.device(), vertexBuffer, nullptr);
		vkFreeMemory(deviceRef.device(), vertexBufferMemory, nullptr);
	}
	void Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		deviceRef.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory
		);
		void* data;
		vkMapMemory(deviceRef.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(deviceRef.device(), vertexBufferMemory);
	}
	void Model::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}
	void Model::draw(VkCommandBuffer commandBuffer) {
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}
	std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}
	std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;//location specified in the vertex shader.
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;//2 components, each sighned floats.
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;//location specified in the vertex shader.
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;//3 components, each sighned floats.
		attributeDescriptions[1].offset = offsetof(Vertex, color); //struct type then member name
		return attributeDescriptions;
	}
}