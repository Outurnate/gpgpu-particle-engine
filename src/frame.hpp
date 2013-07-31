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

#define __CL_ENABLE_EXCEPTIONS
#include <GL/glew.h>
#include <GL/gl.h>
#include <CL/cl.hpp>

class Frame
{
private:
  unsigned num_particles;
  GLuint sprite, vbov, vboc, vao, prog;
  std::vector<cl::Memory> vbo;
  std::vector<float> mousePos;
  cl::CommandQueue queue;
  cl::Kernel k_particlePhysics;
  cl::Buffer veloBuffer, mouseBuffer;
  float *particles_vel;
  int width, height;
public:
  Frame();
  virtual ~Frame();

  void Reshape(int width, int height);
  void Mouse(float x, float y, float click);
  void Init(cl::Context context, cl::Device device);
  void Render();
  void Destroy();
};

#endif
