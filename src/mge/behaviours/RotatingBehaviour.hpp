#pragma once

#include <glm/vec3.hpp>

#include "mge/behaviours/AbstractBehaviour.hpp"

/**
 * Simply rotates the object around its origin with a fixed speed.
 */
class RotatingBehaviour : public AbstractBehaviour
{
	public:
		RotatingBehaviour(float speed, glm::vec3 axis);
		RotatingBehaviour();

		void Update(float pStep) override;
private:
	float speed;
	glm::vec3 axis;
};
