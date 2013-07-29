#ifndef FRAME_HPP
#define FRAME_HPP
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
#include "config.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <CL/cl.hpp>

class Frame
{
private:
  unsigned num_particles;
  GLuint sprite, vbo, vao, prog;
public:
  Frame();
  virtual ~Frame();

  void Reshape(int width, int height);
  void Init(cl::Context context, cl::Device device);
  void Render();
  void Destroy();
};

#endif
