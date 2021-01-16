#pragma once
#include <glm/glm.hpp>

#include "mge/behaviours/AbstractBehaviour.hpp"

class SimpleLightBehaviour2 : public AbstractBehaviour {
public:
	SimpleLightBehaviour2(const glm::vec3& offset, float magnitude);
	void Update(float ts) override;
private:
	glm::vec3 offset;
	float magnitude;
};
