#pragma once
#include <glm/glm.hpp>

#include "mge/behaviours/AbstractBehaviour.hpp"

class LightRotatingBehaviour: public AbstractBehaviour {
public:
	LightRotatingBehaviour(float speed, glm::vec3 angleOffset);
	void Update(float pStep) override;
private:
	float speed;
	float currentRotation;
	glm::vec3 angleOffset;
};
