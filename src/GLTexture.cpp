#include "GLTexture.hpp"

#include <IL/il.h>

GLTexture::GLTexture(const std::string& path, bool alpha = false)
{
  ILuint texil;
  glGenTextures(1, &ptr);
  glBindTexture(GL_TEXTURE_2D, ptr);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  ilGenImages(1, &texil);
  ilBindImage(texil);
  if (ilLoadImage(static_cast<const ILstring>(path.c_str()))
      && ilConvertImage(alpha ? IL_RGBA : IL_RGB, IL_UNSIGNED_BYTE))
  {
    glTexImage2D(GL_TEXTURE_2D, 
		 0, alpha ? GL_RGBA : GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
		 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
    glGenerateMipmap(GL_TEXTURE_2D);
  }
//  else
//    fprintf(stderr, "Failed to load texture: %s\n", path);
  ilDeleteImages(1, &texil);
}

GLTexture::~GLTexture()
{
  glDeleteTextures(1, &ptr);
}

GLTexture::Bind() const
{
  glBindTexture(GL_TEXTURE_2D, ptr);
}
