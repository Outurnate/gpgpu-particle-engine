#ifndef GLSHADER_HPP
#define GLSHADER_HPP

#include <boost/noncopyable.hpp>
#include <initializer_list>

#include <GL/glew.h>
#include <GL/gl.h>

class GLShaderProgram : private boost::noncopyable
{
public:
  GLShaderProgram(const char* vertex, const char* fragment);
  ~GLShaderProgram();
protected:
  GLuint ptr;
private:
  GLuint vertex;
  GLuint fragment;
};

#endif
