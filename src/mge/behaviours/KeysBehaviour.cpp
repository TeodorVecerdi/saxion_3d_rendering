#include "mge/behaviours/KeysBehaviour.hpp"
#include "mge/core/GameObject.hpp"
#include <SFML/Window/Keyboard.hpp>

KeysBehaviour::KeysBehaviour(float pMoveSpeed, float pTurnSpeed): AbstractBehaviour(), _moveSpeed(pMoveSpeed), _turnSpeed(pTurnSpeed)
{
}

KeysBehaviour::~KeysBehaviour()
{
}

void KeysBehaviour::Update( float pStep )
{
	float moveSpeed = 0.0f; //default if no keys
	float turnSpeed = 0.0f;
	float upSpeed = 0.0f;

	if ( sf::Keyboard::isKeyPressed( sf::Keyboard::W )) {
		moveSpeed += _moveSpeed;
	}
	if ( sf::Keyboard::isKeyPressed( sf::Keyboard::S )) {
		moveSpeed -= _moveSpeed;
	}
	if ( sf::Keyboard::isKeyPressed( sf::Keyboard::D )) {
		turnSpeed -= _turnSpeed;
	}
	if ( sf::Keyboard::isKeyPressed( sf::Keyboard::A )) {
		turnSpeed += _turnSpeed;
	}
	if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Space )) {
		upSpeed += _moveSpeed;
	}
	if ( sf::Keyboard::isKeyPressed( sf::Keyboard::LControl )) {
		upSpeed -= _moveSpeed;
	}
	//translate the object in its own local space
	_owner->Translate( glm::vec3(0.0f, upSpeed*pStep, moveSpeed*pStep ) );

	//rotate the object in its own local space
	_owner->Rotate( glm::radians(turnSpeed*pStep), glm::vec3(0.0f, 1.0f, 0.0f ) );
}
