#pragma once

class GameObject;

/**
 * An AbstractBehaviour allows you to attach reusable behaviours to GameObjects (steering, rotating, billboarding, etc).
 * A behaviour is set on a GameObject, which in turn passes in a reference to itself through the setOwner method.
 * This way we can enforce that a Behaviour can never have an owner different from the object it has been attached to.
 *
 * The concept is similar to MonoBehaviours in Unity.
 */
class AbstractBehaviour
{
	public:

		AbstractBehaviour();
		virtual ~AbstractBehaviour() = 0;

        //for internal administration, do not use directly
        virtual void SetOwner (GameObject* pGameObject);

        //behaviour should be able to update itself every step and MUST (or may, thanks) be implemented
		virtual void Start() {}
		virtual void Update(float pStep) {}
		virtual void LateUpdate(float pStep) {}

    protected:

		GameObject* _owner;

    private:
		bool ranStart = false;
        //disallow copy and assignment
        AbstractBehaviour(const AbstractBehaviour&);
        AbstractBehaviour& operator=(const AbstractBehaviour&);
	friend class GameObject;
};
