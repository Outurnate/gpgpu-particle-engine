#include "Frame.hpp"

#include <iostream>

ParticleSystem::ParticleSystem(size_t forceCount, uint32_t particleCount, float r, float g, float b, cl::Context& context, cl::Device& device, ParticleShader& shader)
  : forceCount(forceCount),
    particleCount(particleCount),
    context(context),
    shader(shader)
{
  forces = new float[3 * forceCount];

  //=====================================
  // Fill buffers CPU-side (CL/GL)
  GLfloat* particlesv = new GLfloat[particleCount * 2];
  unsigned j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
  {
    #define DIST_SIZE 1.99f
    particlesv[j++] = ((float)rand() / ((float)RAND_MAX / DIST_SIZE)) - (DIST_SIZE / 2.0f);
    particlesv[j++] = ((float)rand() / ((float)RAND_MAX / DIST_SIZE)) - (DIST_SIZE / 2.0f);
  }

  GLfloat* particlesc = new GLfloat[particleCount * 3];
  j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
  {
    particlesc[j++] = r;
    particlesc[j++] = g;
    particlesc[j++] = b;
  }

  float* particles_vel = new float[particleCount * 2];
  j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
  {
    particles_vel[j++] = 0.0f;
    particles_vel[j++] = 0.0f;
  }

  float* particles_mass = new float[particleCount];
  j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
    particles_mass[j++] = 1.0f + (((float)rand() / (float)RAND_MAX) * 2);
  //=====================================
  // Upload (GL)
  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray);
  
  glGenBuffers(1, &vertexVBO);
  glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
  glBufferData(GL_ARRAY_BUFFER, particleCount * 2 * sizeof(GLfloat), particlesv, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(paramPosition);
  glVertexAttribPointer(paramPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

  glGenBuffers(1, &colorVBO);
  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferData(GL_ARRAY_BUFFER, particleCount * 3 * sizeof(GLfloat), particlesc, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(paramColor);
  glVertexAttribPointer(paramColor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
  //=====================================
  // Create CL buffers
  clVBO.push_back(cl::BufferGL(context, CL_MEM_READ_WRITE, vertexVBO));

  velocityBuffer = cl::Buffer(context, CL_MEM_READ_WRITE,
                              3 * sizeof(float) * particleCount);
  forceBuffer    = cl::Buffer(context, CL_MEM_READ_WRITE,
                              3 * sizeof(float) * forceCount);
  massBuffer     = cl::Buffer(context, CL_MEM_READ_WRITE,
                              1 * sizeof(float) * particleCount);

  queue.enqueueWriteBuffer(velocityBuffer, CL_TRUE, 0,
                           2 * sizeof(float) * particleCount,
                           particles_vel);
  queue.enqueueWriteBuffer(forceBuffer, CL_TRUE, 0,
                           3 * sizeof(float) * forceCount,
                           forces);
  queue.enqueueWriteBuffer(massBuffer,  CL_TRUE, 0,
                           1 * sizeof(float) * particleCount,
                           particles_mass);
  queue.finish();
  //=====================================
  // Free CPU buffers (GL)
  delete[] particlesv;
  delete[] particlesc;
  //=====================================
  // Free CPU buffers (CL)
  delete[] particles_vel;
  delete[] particles_mass;
  
  //=====================================
  // Build CL kernel
  cl::Program::Sources sources;

  std::string kernelSource(
"inline float2 point_force(float2 p1, float2 p2, float inv_mag, float strength)"
"{"
"  float2 ret;"
"  float theta = atan2(p1.y - p2.y, p1.x - p2.x);"
"  ret.x = cos(theta) / inv_mag;"
"  ret.y = sin(theta) / inv_mag;"
"  return (ret * strength);"
"}"
"__kernel void particlePhysics(__global float2 *pos, __global float2 *vel, __global float3 *pointf, __global float *mass)"
"{"
"  unsigned i = get_global_id(0);"
"  float2 p = pos[i];"
"  float2 v = vel[i];"
"  v += (point_force(pointf[0].xy, p, 3000.0f, pointf[0].z)) * mass[i];"
"  p += vel[i].xy;"
"  v *= 0.997f;"
"  p = clamp(p, -1.001f - mass[i], 1.001f + mass[i]);"
"  v = clamp(v, -0.05f, 0.05f);"
"  pos[i] = p;"
"  vel[i] = v;"
"}");
  
  sources.push_back({ kernelSource.c_str(), kernelSource.size() });
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
  kernel = cl::Kernel(program, "particlePhysics");

  set.emplace_back(new ParticleSet(1, 1000000, 20.0f / 255.0f, 20.0f / 255.0f, 5.0f / 255.0f, context, queue, shader.position, shader.color));
}

ParticleSet::ParticleSet(size_t forceCount, uint32_t particleCount, float r, float g, float b, cl::Context& context, cl::CommandQueue& queue, GLint paramPosition, GLint paramColor)
  : forceCount(forceCount),
    particleCount(particleCount)
{
  forces = new float[3 * forceCount];

  //=====================================
  // Fill buffers CPU-side (CL/GL)
  GLfloat* particlesv = new GLfloat[particleCount * 2];
  unsigned j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
  {
    #define DIST_SIZE 1.99f
    particlesv[j++] = ((float)rand() / ((float)RAND_MAX / DIST_SIZE)) - (DIST_SIZE / 2.0f);
    particlesv[j++] = ((float)rand() / ((float)RAND_MAX / DIST_SIZE)) - (DIST_SIZE / 2.0f);
  }

  GLfloat* particlesc = new GLfloat[particleCount * 3];
  j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
  {
    particlesc[j++] = r;
    particlesc[j++] = g;
    particlesc[j++] = b;
  }

  float* particles_vel = new float[particleCount * 2];
  j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
  {
    particles_vel[j++] = 0.0f;
    particles_vel[j++] = 0.0f;
  }

  float* particles_mass = new float[particleCount];
  j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
    particles_mass[j++] = 1.0f + (((float)rand() / (float)RAND_MAX) * 2);
  //=====================================
  // Upload (GL)
  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray);
  
  glGenBuffers(1, &vertexVBO);
  glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
  glBufferData(GL_ARRAY_BUFFER, particleCount * 2 * sizeof(GLfloat), particlesv, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(paramPosition);
  glVertexAttribPointer(paramPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

  glGenBuffers(1, &colorVBO);
  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferData(GL_ARRAY_BUFFER, particleCount * 3 * sizeof(GLfloat), particlesc, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(paramColor);
  glVertexAttribPointer(paramColor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
  //=====================================
  // Create CL buffers
  clVBO.push_back(cl::BufferGL(context, CL_MEM_READ_WRITE, vertexVBO));

  velocityBuffer = cl::Buffer(context, CL_MEM_READ_WRITE,
                              3 * sizeof(float) * particleCount);
  forceBuffer    = cl::Buffer(context, CL_MEM_READ_WRITE,
                              3 * sizeof(float) * forceCount);
  massBuffer     = cl::Buffer(context, CL_MEM_READ_WRITE,
                              1 * sizeof(float) * particleCount);

  queue.enqueueWriteBuffer(velocityBuffer, CL_TRUE, 0,
                           2 * sizeof(float) * particleCount,
                           particles_vel);
  queue.enqueueWriteBuffer(forceBuffer, CL_TRUE, 0,
                           3 * sizeof(float) * forceCount,
                           forces);
  queue.enqueueWriteBuffer(massBuffer,  CL_TRUE, 0,
                           1 * sizeof(float) * particleCount,
                           particles_mass);
  queue.finish();
  //=====================================
  // Free CPU buffers (GL)
  delete[] particlesv;
  delete[] particlesc;
  //=====================================
  // Free CPU buffers (CL)
  delete[] particles_vel;
  delete[] particles_mass;
}

ParticleSystem::~ParticleSystem()
{
  glDeleteBuffers(1, &vertexVBO);
  glDeleteBuffers(1, &colorVBO);
  glDeleteVertexArrays(1, &vertexArray);

  delete[] forces;
}

void ParticleSystem::UpdateForcePower(size_t i, float power)
{
  set[0]->forces[i + 2] = power;
}

void ParticleSystem::UpdateForceLocation(size_t i, float x, float y)
{
  set[0]->forces[i    ] = x;
  set[0]->forces[i + 1] = y;
}

void ParticleSystem::FlushCL()
{
  queue.enqueueWriteBuffer(set[0]->forceBuffer, CL_TRUE, 0,
                           3 * sizeof(float) * set[0]->forceCount,
                           set[0]->forces);
  queue.finish();
  kernel.setArg(2, set[0]->forceBuffer);
  queue.finish();
}

void ParticleSystem::Render()
{
  glBindTexture(GL_TEXTURE_2D, shader.sprite);
  kernel.setArg(0, set[0]->clVBO[0]);
  kernel.setArg(1, set[0]->velocityBuffer);
  kernel.setArg(2, set[0]->forceBuffer);
  kernel.setArg(3, set[0]->massBuffer);
  queue.finish();
  queue.enqueueAcquireGLObjects(&set[0]->clVBO);
  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(set[0]->particleCount), cl::NullRange);
  queue.enqueueReleaseGLObjects(&set[0]->clVBO);
  queue.finish();

  glUseProgram(shader.prog);
  glBindVertexArray(set[0]->vertexArray);
  glDrawArrays(GL_POINTS, 0, set[0]->particleCount);
}
