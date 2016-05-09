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

#include <array>

class ParticleShaders
{
public:
  ParticleShaders();
  ~ParticleShaders();

  GLint pointsprite, position, color;
  GLuint sprite, prog, vert, frag;
};

class ParticleSet
{
public:
  ParticleSet(size_t forceCount, uint32_t particleCount, float r, float g, float b);
  ~ParticleSet();
private:public:
  float r, g, b;
  uint32_t particleCount;
  size_t forceCount;
  
  cl::Buffer velocityBuffer;
  cl::Buffer massBuffer;
  cl::Buffer forceBuffer;
  std::vector<cl::Memory> clVBO;

  GLfloat* particlesv;
  GLfloat* particlesc;
  float* particles_vel;
  float* particles_mass;
  float* forces;
  
  GLuint vertexArray;
  GLuint vertexVBO;
  GLuint colorVBO;
};

class ParticleSystem
{
public:
  ParticleSystem(cl::Context& context, cl::Device& device, ParticleShaders& shader);

  void UpdateForcePower(size_t i, float power);
  void UpdateForceLocation(size_t i, float x, float y);
  void FlushCL();
  void Render();
private:
  cl::CommandQueue queue;
  cl::Context& context;
  cl::Kernel kernel;
  ParticleSet set;
  ParticleShaders& shader;
};

class Frame
{
private:
  std::array<float, 3> mousePos;
  
  int width, height;
  ParticleSystem ps;
public:
  Frame(ParticleShaders& shaders, cl::Context context, cl::Device device);

  void Reshape(int width, int height);
  void MouseMove(float x, float y);
  void MouseClick(float click);
  void Render();
};

#endif
