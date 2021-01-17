#pragma once

#include "mge/materials/AbstractMaterial.hpp"
#include "game/utils/vec5.hpp"

class Texture;
class ShaderProgram;

/**
 * This is about the simplest material we can come up with, it demonstrates how to
 * render a single color material without caching, passing in all the matrices we require separately.
 */
class TerrainMaterial : public AbstractMaterial {
public:
	TerrainMaterial(Texture* heightmapTexture, Texture* splatmapTexture, Texture* baseTexture, Texture* textureR, Texture* textureG, Texture* textureB, Texture* textureA, glm::vec5 textureSizes, float height, float normalStepSize);

	void render(World* world, Mesh* mesh, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

	//in rgb values
	void SetHeightmap(Texture* heightmap);
	Texture* GetHeightmap() const { return heightmap; }
	void SetSplatmap(Texture* splatmap);
	Texture* GetSplatmap() const { return splatmap; }

	void SetAmbientColor(glm::vec3 diffuseColor);
	glm::vec3 GetAmbientColor() const { return xyz(ambientColor); }
	void SetAmbientIntensity(float intensity);
	float GetAmbientIntensity() const { return ambientColor.w; }
	void SetSpecularColor(glm::vec3 specularColor);
	glm::vec3 GetSpecularColor() const { return xyz(specularColor); }
	void SetSpecularIntensity(float intensity);
	float GetSpecularIntensity() const { return specularColor.w; }
	void SetShininess(float shininess);
	float GetShininess() const { return shininess; }
	void SetHeight(float height);
	float GetHeight() const { return height; }
	void SetNormalStepSize(float normalStepSize);
	float GetNormalStepSize() const { return normalStepSize; }

private:
	//all the static properties are shared between instances of LitTextureMaterial
	//note that they are all PRIVATE, we do not expose this static info to the outside world
	static ShaderProgram* _shader;
	static void _lazyInitializeShader();

	//this one is unique per instance of color material
	Texture* heightmap;
	Texture* splatmap;
	Texture* baseTexture;
	Texture* textureR;
	Texture* textureG;
	Texture* textureB;
	Texture* textureA;
	glm::vec5 textureSizes;
	
	glm::vec4 ambientColor; // rgb=color, a=intensity
	glm::vec4 specularColor; // rgb=color, a=intensity
	float shininess;
	glm::vec3 eye;

	float height;
	float normalStepSize;

	static inline constexpr size_t MAX_LIGHTS = 64;
};
