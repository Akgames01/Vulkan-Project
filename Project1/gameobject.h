#pragma once

#include "model.h"

//std
#include <memory>

namespace defined {
	struct Transform2dComponent {
		glm::vec2 translation; //position offset
		glm::vec2 scale{ 1.f, 1.f };//(x,y) => (1,1)
		float rotation;
		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotationMat{ {c,s} , {-s, c} };
			glm::mat2 scaleMat{ {scale.x, 0.0f}, {.0f, scale.y} };//emphasis on column and not row.
			return rotationMat*scaleMat;
		}//curently an identity matrix;
	};
	class GameObject
	{
		public:
			using id_t = unsigned int;
			static GameObject createGameObject() {
				static id_t currentId = 0;
				return GameObject{ currentId++ };
			}
			GameObject(const GameObject&) = delete;
			GameObject& operator = (const GameObject&) = delete;
			//assignment operator to use the default
			GameObject(GameObject&&) = default;
			GameObject& operator=(GameObject&&) = default;
			const id_t getId() {
				return id;
			}
			//shared pointer
			std::shared_ptr<Model> model{};
			glm::vec3 color{};
			Transform2dComponent transform2d{};
		private:
			//constructor private, because every object will have its unique id.
			GameObject(id_t objId) :id{objId} {}
			id_t id;
	};
}

