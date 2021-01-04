#pragma once

#include "mge/core/GameObject.hpp"

class Camera;
class Light;

class World : public GameObject
{
	public:
        World();

		void setMainCamera (Camera* pCamera);
		Camera* getMainCamera() const;

        //only used internally, do not use from outside
        void registerLight (Light* pLight);
        void unregisterLight (Light* pLight);

        Light* getLightAt (int pIndex) const;
        int getLightCount() const;

	private:
	    Camera* _mainCamera;
	    std::vector<Light*> _lights;

        World(const World&);
        World& operator=(const World&);
};
