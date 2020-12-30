#include "glm.hpp"

#include "WobbleTextureMaterial.hpp"

#include <chrono>


#include "game/config.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/World.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/core/ShaderProgram.hpp"
#include "mge/config.hpp"
#include "mge/util/Time.hpp"

WobbleTextureMaterial::WobbleTextureMaterial(Texture * pDiffuseTexture):_diffuseTexture(pDiffuseTexture) {
    _lazyInitializeShader();
}

WobbleTextureMaterial::~WobbleTextureMaterial() {}

void WobbleTextureMaterial::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, game::config::Shader("wobble.vert"));
        _shader->addShader(GL_FRAGMENT_SHADER, game::config::Shader("texture.frag"));
        _shader->finalize();

        //cache all the uniform and attribute indexes
        _uMVPMatrix = _shader->getUniformLocation("mvpMatrix");
        _uTime = _shader->getUniformLocation("time");
        _uDiffuseTexture = _shader->getUniformLocation("diffuseTexture");

        _aVertex = _shader->getAttribLocation("vertex");
        _aNormal = _shader->getAttribLocation("normal");
        _aUV =     _shader->getAttribLocation("uv");
    }
}

void WobbleTextureMaterial::setDiffuseTexture (Texture* pDiffuseTexture) {
    _diffuseTexture = pDiffuseTexture;
}

void WobbleTextureMaterial::render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) {
    if (!_diffuseTexture) return;

    _shader->use();

    //setup texture slot 0
    glActiveTexture(GL_TEXTURE0);
    //bind the texture to the current active slot
    glBindTexture(GL_TEXTURE_2D, _diffuseTexture->getId());
    //tell the shader the texture slot for the diffuse texture is slot 0
    glUniform1i (_uDiffuseTexture, 0);

    //pass in a precalculate mvp matrix (see texture material for the opposite)
    glm::mat4 mvpMatrix = pProjectionMatrix * pViewMatrix * pModelMatrix;
    glUniformMatrix4fv ( _uMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniform1f(_uTime, mge::Time::TotalTime());

    //now inform mesh of where to stream its data
    pMesh->streamToOpenGL(_aVertex, _aNormal, _aUV);
}
