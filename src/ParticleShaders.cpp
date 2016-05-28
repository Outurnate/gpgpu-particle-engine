#include "ParticleShaders.hpp"

#include <fstream>
#include <sstream>

#include "util.hpp"

using namespace std;

const char* readToEnd(const string& path)
{
  ifstream file(path);
  ostringstream ss;
  ss << file.rdbuf();

  return ss.str().c_str();
}

ParticleShader::ParticleShader()
  : GLShaderProgram(readToEnd("media/shaders/particle.vert.glsl"), readToEnd("media/shaders/particle.frag.glsl"))
{
  glUseProgram(ptr);

  pointsprite = glGetUniformLocation(ptr, "pointsprite");
  position = glGetAttribLocation(ptr, "position");
  color    = glGetAttribLocation(ptr, "color");
  glBindFragDataLocation(ptr, 0, "colorOut");
  
  glFinish();
}

void ParticleShader::SetPointSprite(const GLTexture& texture)
{
  glUniform1i(pointsprite, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  texture.Bind();
}
