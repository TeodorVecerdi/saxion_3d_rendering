#include "SimpleLightBehaviour2.h"

#include "game/utils/constants.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/util/Time.hpp"

SimpleLightBehaviour2::SimpleLightBehaviour2(const glm::vec3& offset, float magnitude) : offset(offset), magnitude(magnitude) {
}

void SimpleLightBehaviour2::Update(float ts) {
	const auto sinTime = (glm::sin(mge::Time::TotalTime()) + 1.0f) / 2.0f;
	const auto y = sinTime * magnitude;
	const auto pos = glm::vec3(0, y, 0);
	_owner->SetLocalPosition(offset + pos);
}
