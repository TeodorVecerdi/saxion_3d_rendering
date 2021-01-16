#include "mge/behaviours/RotatingBehaviour.hpp"

#include <SFML/Window/Keyboard.hpp>


#include "game/utils/constants.hpp"
#include "mge/core/GameObject.hpp"

RotatingBehaviour::RotatingBehaviour(const float speed, glm::vec3 axis) : AbstractBehaviour(), speed(speed), axis(axis) {
}

RotatingBehaviour::RotatingBehaviour(): RotatingBehaviour(45.0f, glm::vec3(1, 1, 0)) {
}

void RotatingBehaviour::Update(float pStep) {
	float rotate = 0.0f;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) rotate -= 1.0f;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) rotate += 1.0f;
	//rotates 45deg per second
	_owner->Rotate(pStep * utils::constants::degToRad * rotate * speed, axis);
}
