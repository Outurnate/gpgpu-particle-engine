#ifndef GLTEXTURE_HPP
#define GLTEXTURE_HPP

#include <boost/noncopyable.hpp>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>

class GLTexture : private boost::noncopyable
{
public:
  GLTexture(const std::string& path);
  ~GLTexture();

  void Bind() const;
private:
  GLint ptr;
};

#endif
