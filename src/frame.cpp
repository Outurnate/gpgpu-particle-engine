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
#include "frame.hpp"

#include "util.hpp"
#include "particle_hex.hpp"

#include <iostream>
#include <iomanip>
#include <ctime>

Frame::Frame()
{
  srand((unsigned)time(0));
  num_particles = 1000000;
  mousePos.push_back(0.0f);
  mousePos.push_back(0.0f);
  mousePos.push_back(0.0f);
}

Frame::~Frame()
{
  delete[] particles_vel;
  delete[] particles_mass;
}

void Frame::Reshape(int width, int height)
{
  this->width = width;
  this->height = height;
  glViewport(0, 0, width, height);
}

void Frame::MouseMove(float x, float y)
{
  mousePos[0] = -1.0f + ((x / width)  * 2.0f);
  mousePos[1] = (-1.0f + ((y / height) * 2.0f)) * -1;
  queue.enqueueWriteBuffer(mouseBuffer, CL_TRUE, 0, 3 * sizeof(float), &mousePos[0]);
  queue.finish();
  k_particlePhysics.setArg(2, mouseBuffer);
  queue.finish();
}

void Frame::MouseClick(float click)
{
  mousePos[2] = click;
  queue.enqueueWriteBuffer(mouseBuffer, CL_TRUE, 0, 3 * sizeof(float), &mousePos[0]);
  queue.finish();
  k_particlePhysics.setArg(2, mouseBuffer);
  queue.finish();
}

void Frame::Init(cl::Context context, cl::Device device)
{
  sprite = loadTexture("media/textures/particle.png", true);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glPointSize(1);

  GLuint vert = makeShader(GL_VERTEX_SHADER,   1, { new std::string("media/shaders/particle.vert.glsl") });
  GLuint frag = makeShader(GL_FRAGMENT_SHADER, 1, { new std::string("media/shaders/particle.frag.glsl") });

  prog = makeProgram(vert, frag);

  glUseProgram(prog);

  GLint pointsprite = glGetUniformLocation(prog, "pointsprite");

  glUniform1i(pointsprite, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, sprite);

  GLint position = glGetAttribLocation(prog, "position");
  GLint color    = glGetAttribLocation(prog, "color");

  GLfloat *particlesv = new GLfloat[num_particles * 2];
  unsigned j = 0;
  for (unsigned i = 0; i < num_particles; ++i)
  {
    #define DIST_SIZE 1.99f
    particlesv[j++] = ((float)rand() / ((float)RAND_MAX / DIST_SIZE)) - (DIST_SIZE / 2.0f);
    particlesv[j++] = ((float)rand() / ((float)RAND_MAX / DIST_SIZE)) - (DIST_SIZE / 2.0f);
  }

  GLfloat *particlesc = new GLfloat[num_particles * 3];
  j = 0;
  for (unsigned i = 0; i < num_particles; ++i)
  {
    particlesc[j++] = 20.0f / 255.0f;
    particlesc[j++] = 1.0f;
    particlesc[j++] = 5.0f / 255.0f;
  }

  particles_vel = new float[num_particles * 2];
  j = 0;
  for (unsigned i = 0; i < num_particles; ++i)
  {
    particles_vel[j++] = 0.0f;
    particles_vel[j++] = 0.0f;
  }

  particles_mass = new float[num_particles];
  j = 0;
  for (unsigned i = 0; i < num_particles; ++i)
    particles_mass[j++] = 1.0f + (((float)rand() / (float)RAND_MAX) * 2);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbov);
  glBindBuffer(GL_ARRAY_BUFFER, vbov);
  glBufferData(GL_ARRAY_BUFFER, num_particles * 2 * sizeof(GLfloat), particlesv, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(position);
  glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

  glGenBuffers(1, &vboc);
  glBindBuffer(GL_ARRAY_BUFFER, vboc);
  glBufferData(GL_ARRAY_BUFFER, num_particles * 3 * sizeof(GLfloat), particlesc, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(color);
  glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

  vbo.push_back(cl::BufferGL(context, CL_MEM_READ_WRITE, vbov));
  veloBuffer  = cl::Buffer(context, CL_MEM_READ_WRITE, 3 * sizeof(float) * num_particles);
  mouseBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, 3 * sizeof(float));
  massBuffer  = cl::Buffer(context, CL_MEM_READ_WRITE, num_particles * sizeof(float));

  glBindFragDataLocation(prog, 0, "colorOut");

  glFinish();

  cl::Program::Sources sources;

  std::string kernel(reinterpret_cast<const char*>(particle_cl), particle_cl_len);
  sources.push_back({ kernel.c_str(), kernel.size() });
  cl::Program program(context, sources);

  std::vector<cl::Device> devices;
  devices.push_back(device);
  try
  {
    program.build(devices);
  }
  catch(cl::Error)
  {
    std::cerr << "CL build error: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
    exit(EXIT_FAILURE);
  }

  queue = cl::CommandQueue(context, device);
  queue.enqueueWriteBuffer(veloBuffer,  CL_TRUE, 0, 2 * sizeof(float) * num_particles, particles_vel);
  queue.enqueueWriteBuffer(mouseBuffer, CL_TRUE, 0, 3 * sizeof(float), &mousePos[0]);
  queue.enqueueWriteBuffer(massBuffer,  CL_TRUE, 0,     sizeof(float) * num_particles, particles_mass);
  queue.finish();

  k_particlePhysics = cl::Kernel(program, "particlePhysics");
  k_particlePhysics.setArg(0, vbo[0]);
  k_particlePhysics.setArg(1, veloBuffer);
  k_particlePhysics.setArg(2, mouseBuffer);
  k_particlePhysics.setArg(3, massBuffer);
  queue.finish();

  delete[] particlesv;
  delete[] particlesc;
}

static int num = 0;

void Frame::Render()
{
  glFinish();

  queue.enqueueAcquireGLObjects(&vbo);
  queue.enqueueNDRangeKernel(k_particlePhysics, cl::NullRange, cl::NDRange(num_particles), cl::NullRange);
  queue.enqueueReleaseGLObjects(&vbo);
  queue.finish();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D, sprite);

  glUseProgram(prog);
  glBindVertexArray(vao);
  glDrawArrays(GL_POINTS, 0, num_particles);

  char* pixels = new char[3 * width * height];
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  std::stringstream namestr;
  namestr << std::setfill('0') << std::setw(6) << ++num << ".png";

  ILuint id = ilGenImage();
  ilBindImage(id);
  ilTexImage(width, height, 0, 3, IL_RGB, IL_UNSIGNED_BYTE, pixels);
  ilEnable(IL_FILE_OVERWRITE);
  ilSaveImage(namestr.str().c_str());
  ilDeleteImage(id);

  delete[] pixels;
}

void Frame::Destroy()
{
  glDeleteTextures(1, &sprite);
  glDeleteBuffers(1, &vbov);
  glDeleteBuffers(1, &vboc);
  glDeleteVertexArrays(1, &vao);
}
