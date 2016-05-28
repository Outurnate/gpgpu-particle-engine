#ifndef PARTICLESHADER_HPP
#define PARTICLESHADER_HPP

#include <GL/glew.h>
#include <GL/gl.h>

#include "GLShader.hpp"
#include "GLTexture.hpp"

class ParticleShader : private GLShaderProgram
{
public:
  ParticleShader();
  
  void SetPointSprite(const GLTexture& texture);
  
  GLint position;
  GLint color;
private:
  GLint pointsprite;
};

#endif
