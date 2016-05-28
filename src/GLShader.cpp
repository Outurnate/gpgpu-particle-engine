#include "GLShader.hpp"

#include <cstring>

using namespace std;

inline GLuint compileShader(GLenum type, const char* source)
{
  GLuint ptr = glCreateShader(type);
  GLint length = strlen(source);
  glShaderSource(ptr, 1, &source, &length);
  glCompileShader(ptr);
  GLint status;
  glGetShaderiv(ptr, GL_COMPILE_STATUS, &status);
  if (!status)
  {
    //fprintf(stderr, "Failed to compile %s:\n", fnames[0].c_str());
    //showShaderLog(ptr, glGetShaderiv, glGetShaderInfoLog);
    glDeleteShader(ptr);
  }
  return ptr;
}

GLShaderProgram::GLShaderProgram(const char* vertex, const char* fragment)
{
  ptr = glCreateProgram();
  glAttachShader(ptr, this->vertex   = compileShader(GL_VERTEX_SHADER,   vertex));
  glAttachShader(ptr, this->fragment = compileShader(GL_FRAGMENT_SHADER, fragment));
  glLinkProgram(ptr);
  GLint status;
  glGetProgramiv(ptr, GL_LINK_STATUS, &status);
  if (!status)
  {
//    fprintf(stderr, "Failed to compile :\n");
//    showShaderLog(program, glGetProgramiv, glGetProgramInfoLog);
    glDeleteProgram(ptr);
  }
}

GLShaderProgram::~GLShaderProgram()
{
  glDeleteProgram(ptr);
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}
