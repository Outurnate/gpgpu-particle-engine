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
#include "config.hpp"

#define __CL_ENABLE_EXCEPTIONS
#include <GL/glew.h>
#include <GL/gl.h>
#include <CL/cl.hpp>

#include <boost/noncopyable.hpp>
#include <array>
#include <memory>

#include "ParticleSystem.hpp"

class Frame
{
private:
  std::array<float, 3> mousePos;
  
  int width, height;
  ParticleSystem ps;
public:
  Frame(cl::Context context, cl::Device device);

  void Reshape(int width, int height);
  void MouseMove(float x, float y);
  void MouseClick(float click);
  void Render();
};

#endif
