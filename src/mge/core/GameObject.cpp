#include <iostream>
#include "GameObject.hpp"
#include "mge/behaviours/AbstractBehaviour.hpp"

GameObject::GameObject(const std::string& pName, const glm::vec3& pPosition)
	: _name(pName), _transform(glm::translate(pPosition)), _parent(nullptr), _children(),
	  _mesh(nullptr), _behaviour(nullptr), _material(nullptr), _world(nullptr) {
}

GameObject::~GameObject() {
	//detach all children
	std::cout << "GC running on:" << _name << std::endl;

	while (_children.size() > 0) {
		GameObject* child = _children[0];
		RemoveChild(child);
		delete child;
	}

	//do not forget to delete behaviour, material, mesh, collider manually if required!
}

void GameObject::SetName(const std::string& pName) {
	_name = pName;
}

std::string GameObject::GetName() const {
	return _name;
}

void GameObject::SetTransform(const glm::mat4& pTransform) {
	_transform = pTransform;
}

const glm::mat4& GameObject::GetTransform() const {
	return _transform;
}

void GameObject::SetLocalPosition(glm::vec3 pPosition) {
	_transform[3] = glm::vec4(pPosition, 1);
}

glm::vec3 GameObject::GetLocalPosition() const {
	return glm::vec3(_transform[3]);
}

void GameObject::SetMaterial(AbstractMaterial* pMaterial) {
	_material = pMaterial;
}

AbstractMaterial* GameObject::GetMaterial() const {
	return _material;
}

void GameObject::SetMesh(Mesh* pMesh) {
	_mesh = pMesh;
}

Mesh* GameObject::GetMesh() const {
	return _mesh;
}

void GameObject::SetBehaviour(AbstractBehaviour* pBehaviour) {
	_behaviour = pBehaviour;
	_behaviour->SetOwner(this);
}

AbstractBehaviour* GameObject::GetBehaviour() const {
	return _behaviour;
}

void GameObject::SetParent(GameObject* pParent) {
	//remove from previous parent
	if (_parent != nullptr) {
		_parent->_innerRemove(this);
	}

	//set new parent
	if (pParent != nullptr) {
		pParent->_innerAdd(this);
	}

	//if we have been detached from our parent, make sure
	//the world reference for us and all our children is set to null
	//if we have been attached to a parent, make sure
	//the world reference for us and all our children is set to our parent world reference
	//(this could still be null if the parent or parent's parent is not attached to the world)
	if (_parent == nullptr) {
		_setWorldRecursively(nullptr);
	} else {
		//might still not be available if our parent is not part of the world
		_setWorldRecursively(_parent->_world);
	}
}

void GameObject::_innerRemove(GameObject* pChild) {
	for (auto i = _children.begin(); i != _children.end(); ++i) {
		if (*i == pChild) {
			_children.erase(i);
			pChild->_parent = nullptr;
			return;
		}
	}
}

void GameObject::_innerAdd(GameObject* pChild) {
	_children.push_back(pChild);
	pChild->_parent = this;
}

void GameObject::AddChild(GameObject* pChild) {
	pChild->SetParent(this);
}

void GameObject::RemoveChild(GameObject* pChild) {
	pChild->SetParent(nullptr);
}

GameObject* GameObject::GetParent() const {
	return _parent;
}

////////////

//costly operation, use with care
glm::vec3 GameObject::GetWorldPosition() const {
	return glm::vec3(GetWorldTransform()[3]);
}

//costly operation, use with care

glm::mat4 GameObject::GetWorldTransform() const {
	if (_parent == nullptr) return _transform;
	else return _parent->GetWorldTransform() * _transform;
}

////////////

void GameObject::Translate(glm::vec3 pTranslation) {
	SetTransform(glm::translate(_transform, pTranslation));
}

void GameObject::Scale(glm::vec3 pScale) {
	SetTransform(glm::scale(_transform, pScale));
}

void GameObject::Rotate(float pAngle, glm::vec3 pAxis) {
	SetTransform(glm::rotate(_transform, pAngle, pAxis));
}

void GameObject::Start() {
	//make sure behaviour is updated after worldtransform is set
	if (_behaviour) {
		if (!_behaviour->ranStart) {
			_behaviour->Start();
			_behaviour->ranStart = true;
		}
	}

	for (int i = _children.size() - 1; i >= 0; --i) {
		if (!_children[i]->ranStart) {
			_children[i]->Start();
			_children[i]->ranStart = true;
		}
	}
}

void GameObject::Update(float pStep) {
	//make sure behaviour is updated after worldtransform is set
	if (_behaviour) {
		_behaviour->Update(pStep);
	}

	for (int i = _children.size() - 1; i >= 0; --i) {
		_children[i]->Update(pStep);
	}
}

void GameObject::LateUpdate(float pStep) {
	//make sure behaviour is updated after worldtransform is set
	if (_behaviour) {
		_behaviour->LateUpdate(pStep);
	}

	for (int i = _children.size() - 1; i >= 0; --i) {
		_children[i]->LateUpdate(pStep);
	}
}

void GameObject::_setWorldRecursively(World* pWorld) {
	_world = pWorld;

	for (int i = _children.size() - 1; i >= 0; --i) {
		_children[i]->_setWorldRecursively(pWorld);
	}
}

int GameObject::GetChildCount() const {
	return _children.size();
}

GameObject* GameObject::GetChildAt(int pIndex) const {
	return _children[pIndex];
}
