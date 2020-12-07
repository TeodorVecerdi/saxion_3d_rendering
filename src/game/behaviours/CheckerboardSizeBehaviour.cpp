#include "CheckerboardSizeBehaviour.h"

#include <SFML/Window/Keyboard.hpp>


#include "game/materials/CheckerboardMaterial.h"
#include "mge/core/GameObject.hpp"

CheckerboardSizeBehaviour::CheckerboardSizeBehaviour(float sizeSpeed) : AbstractBehaviour(), sizeSpeed(sizeSpeed) {
}

CheckerboardSizeBehaviour::~CheckerboardSizeBehaviour() {
}

void CheckerboardSizeBehaviour::update(float ts) {
	float speed = 0.0f;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		speed += sizeSpeed;
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		speed -= sizeSpeed;
	}
	auto* mat = dynamic_cast<CheckerboardMaterial*>(_owner->getMaterial());
	float currentSize = mat->getPatternSize();
	mat->setPatternSize(currentSize + speed * ts);
}
