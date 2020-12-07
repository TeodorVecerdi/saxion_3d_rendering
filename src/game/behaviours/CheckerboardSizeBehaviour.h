#pragma once
#include "mge/behaviours/AbstractBehaviour.hpp"

class CheckerboardSizeBehaviour : public AbstractBehaviour {
public:
	CheckerboardSizeBehaviour(float sizeSpeed = 1);
	virtual ~CheckerboardSizeBehaviour();
	virtual void update(float ts);
private:
	float sizeSpeed;
};
