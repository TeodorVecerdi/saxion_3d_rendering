#pragma once
#include "mge/materials/AbstractMaterial.hpp"

class ShaderProgram;

class CheckerboardMaterial : public AbstractMaterial {
public:
	CheckerboardMaterial(float patternSize);
	virtual ~CheckerboardMaterial();
	
	virtual void render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) override;
	void setPatternSize(float patternSize);
	float getPatternSize() const; 
private:
	//all the static properties are shared between instances of ColorMaterial
	//note that they are all PRIVATE, we do not expose this static info to the outside world
	static ShaderProgram* _shader;
	static void _lazyInitializeShader();
	float patternSize;
};
