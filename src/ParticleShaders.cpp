#include "frame.hpp"

#include "util.hpp"

ParticleShaders::ParticleShaders()
{
  sprite = loadTexture("media/textures/particle.png", true);

  vert = makeShader(GL_VERTEX_SHADER,   1, { new std::string("media/shaders/particle.vert.glsl") });
  frag = makeShader(GL_FRAGMENT_SHADER, 1, { new std::string("media/shaders/particle.frag.glsl") });

  prog = makeProgram(vert, frag);

  glUseProgram(prog);

  pointsprite = glGetUniformLocation(prog, "pointsprite");

  glUniform1i(pointsprite, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, sprite);

  position = glGetAttribLocation(prog, "position");
  color    = glGetAttribLocation(prog, "color");

  glBindFragDataLocation(prog, 0, "colorOut");

  glFinish();
}

ParticleShaders::~ParticleShaders()
{
  glDeleteTextures(1, &sprite);
}
