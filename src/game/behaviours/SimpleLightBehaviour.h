#pragma once
#include <glm/glm.hpp>

#include "mge/behaviours/AbstractBehaviour.hpp"

class SimpleLightBehaviour : public AbstractBehaviour {
public:
	SimpleLightBehaviour(const glm::vec3& offset, float magnitude);
	void Update(float ts) override;
private:
	glm::vec3 offset;
	float magnitude;
};
