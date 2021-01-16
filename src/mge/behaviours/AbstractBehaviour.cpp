#include "AbstractBehaviour.hpp"

AbstractBehaviour::AbstractBehaviour():_owner(nullptr) {}

AbstractBehaviour::~AbstractBehaviour()
{
	_owner = nullptr;
}

void AbstractBehaviour::SetOwner (GameObject* pOwner) {
    _owner = pOwner;
}

