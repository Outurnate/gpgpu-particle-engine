//-----------------------------------------------------------------------//
//A GPU based particle engine                                            //
//Copyright (C) 2013 Joseph Dillon                                       //
//-----------------------------------------------------------------------//
//This program is free software: you can redistribute it and/or modify   //
//it under the terms of the GNU General Public License as published by   //
//the Free Software Foundation, either version 3 of the License, or      //
//(at your option) any later version.                                    //
//                                                                       //
//This program is distributed in the hope that it will be useful,        //
//but WITHOUT ANY WARRANTY; without even the implied warranty of         //
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
//GNU General Public License for more details.                           //
//                                                                       //
//You should have received a copy of the GNU General Public License      //
//along with this program.  If not, see <http://www.gnu.org/licenses/>.  //
//-----------------------------------------------------------------------//
#ifndef UTILH
#define UTILH

#include <GL/glew.h>
#include <GL/gl.h>
#include <IL/il.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>

inline std::string getShaderLog(GLuint object)
{
  GLint length;
  glGetShaderiv(object, GL_INFO_LOG_LENGTH, &length);

  char log[length];
  glGetShaderInfoLog(object, length, NULL, log);

  return std::string(log);
}

inline GLuint makeShader(GLenum type, const char* content, std::size_t length)
{
  GLuint shader;
  GLint sourceLength = length, status;
  shader = glCreateShader(type);
  glShaderSource(shader, 1, &content, &sourceLength);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (!status)
  {
//    fprintf(stderr, "Failed to compile %s:\n", fnames[0].c_str());
//    showShaderLog(shader, glGetShaderiv, glGetShaderInfoLog);
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

inline GLuint makeProgram(GLuint vert, GLuint frag)
{
  GLint status;
  GLuint program = glCreateProgram();
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (!status)
  {
//    fprintf(stderr, "Failed to compile :\n");
//    showShaderLog(program, glGetProgramiv, glGetProgramInfoLog);
    glDeleteProgram(program);
    return 0;
  }
  return program;
}

inline GLuint loadTexture(const char* fname, bool alpha = false)
{
  GLuint tex;
  ILuint texil;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  ilGenImages(1, &texil);
  ilBindImage(texil);
  if (ilLoadImage((const ILstring)fname)
      && ilConvertImage(alpha ? IL_RGBA : IL_RGB, IL_UNSIGNED_BYTE))
  {
    glTexImage2D(GL_TEXTURE_2D, 
		 0, alpha ? GL_RGBA : GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
		 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
    fprintf(stderr, "Failed to load texture: %s\n", fname);
  ilDeleteImages(1, &texil);

  return tex;
}

#endif
