#include "SimpleLightBehaviour.h"

#include "game/utils/constants.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/util/Time.hpp"

SimpleLightBehaviour::SimpleLightBehaviour(const glm::vec3& offset, float magnitude) : offset(offset), magnitude(magnitude) {
}

void SimpleLightBehaviour::Update(float ts) {
	const auto sinTime = glm::sin(mge::Time::TotalTime());
	const auto cosTime = glm::cos(mge::Time::TotalTime());
	const auto x = cosTime * magnitude;
	const auto z = sinTime * magnitude;
	const auto pos = glm::vec3(x, 0, z);
	_owner->SetLocalPosition(offset + pos);
}
