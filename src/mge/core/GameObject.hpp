#pragma once

#include <vector>
#include "glm.hpp"

class AbstractCollider;
class AbstractBehaviour;
class AbstractMaterial;
class World;
class Mesh;

/**
 * A GameObject wraps all data required to display an object, but knows nothing about OpenGL or rendering.
 */
class GameObject {
public:
	GameObject(const std::string& pName = nullptr, const glm::vec3& pPosition = glm::vec3(0.0f, 0.0f, 0.0f));
	virtual ~GameObject();

	void SetName(const std::string& pName);
	std::string GetName() const;

	//contains local rotation, scale, position
	void SetTransform(const glm::mat4& pTransform);
	const glm::mat4& GetTransform() const;

	//access just the local position
	void SetLocalPosition(glm::vec3 pPosition);
	glm::vec3 GetLocalPosition() const;

	//get the objects world position by combining transforms, SLOW use with care
	glm::vec3 GetWorldPosition() const;
	glm::mat4 GetWorldTransform() const;

	//change LOCAL position, rotation, scaling
	void Translate(glm::vec3 pTranslation);
	void Rotate(float pAngle, glm::vec3 pAxis);
	void Scale(glm::vec3 pScale);

	//mesh and material should be shared as much as possible
	void SetMesh(Mesh* pMesh);
	Mesh* GetMesh() const;

	//mesh and material should be shared as much as possible
	void SetMaterial(AbstractMaterial* pMaterial);
	AbstractMaterial* GetMaterial() const;

	//behaviour is expected to be unique per game object, in general do NOT share them between objects
	void SetBehaviour(AbstractBehaviour* pBehaviour);
	AbstractBehaviour* GetBehaviour() const;

	virtual void Start();
	virtual void Update(float pStep);
	virtual void LateUpdate(float pStep);

	//child management, note that add/remove and setParent are closely coupled.
	//a.add(b) has the same effect as b.setParent(a)
	//Adding a gameobject or resetting the parent, recursively passes on the world pointer to all child objects

	//shortcut to set the parent of pChild to ourselves
	void AddChild(GameObject* pChild);
	//shortcut to set the parent of pChild to nullptr
	void RemoveChild(GameObject* pChild);

	void SetParent(GameObject* pGameObject);
	GameObject* GetParent() const;

	int GetChildCount() const;
	GameObject* GetChildAt(int pIndex) const;

protected:
	std::string _name;
	glm::mat4 _transform;

	GameObject* _parent;
	std::vector<GameObject*> _children;

	Mesh* _mesh;
	AbstractBehaviour* _behaviour;
	AbstractMaterial* _material;
	World* _world;

	//update children list administration
	void _innerAdd(GameObject* pChild);
	void _innerRemove(GameObject* pChild);

	//used to pass on pointer to the world to a gameobject
	virtual void _setWorldRecursively(World* pWorld);

private:
	GameObject(const GameObject&);
	GameObject& operator=(const GameObject&);
	bool ranStart = false;
};
