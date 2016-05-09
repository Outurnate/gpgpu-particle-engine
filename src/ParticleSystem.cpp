#include "frame.hpp"

#include <iostream>

ParticleSystem::ParticleSystem(cl::Context& context, cl::Device& device, ParticleShaders& shader)
  : context(context), set(1, 1000000, 20.0f / 255.0f, 20.0f / 255.0f, 5.0f / 255.0f), shader(shader)
{
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
  //=====================================
  // Upload (GL)
  glGenVertexArrays(1, &set.vertexArray);
  glBindVertexArray(set.vertexArray);
  
  glGenBuffers(1, &set.vertexVBO);
  glBindBuffer(GL_ARRAY_BUFFER, set.vertexVBO);
  glBufferData(GL_ARRAY_BUFFER, set.particleCount * 2 * sizeof(GLfloat), set.particlesv, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(shader.position);
  glVertexAttribPointer(shader.position, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

  glGenBuffers(1, &set.colorVBO);
  glBindBuffer(GL_ARRAY_BUFFER, set.colorVBO);
  glBufferData(GL_ARRAY_BUFFER, set.particleCount * 3 * sizeof(GLfloat), set.particlesc, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(shader.color);
  glVertexAttribPointer(shader.color, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
  //=====================================
  // Create CL buffers
  set.clVBO.push_back(cl::BufferGL(context, CL_MEM_READ_WRITE, set.vertexVBO));

  set.velocityBuffer = cl::Buffer(context, CL_MEM_READ_WRITE,
                                  3 * sizeof(float) * set.particleCount);
  set.forceBuffer    = cl::Buffer(context, CL_MEM_READ_WRITE,
                                  3 * sizeof(float) * set.forceCount);
  set.massBuffer     = cl::Buffer(context, CL_MEM_READ_WRITE,
                                  1 * sizeof(float) * set.particleCount);

  queue.enqueueWriteBuffer(set.velocityBuffer, CL_TRUE, 0,
                           2 * sizeof(float) * set.particleCount,
                           set.particles_vel);
  queue.enqueueWriteBuffer(set.forceBuffer, CL_TRUE, 0,
                           3 * sizeof(float) * set.forceCount,
                           set.forces);
  queue.enqueueWriteBuffer(set.massBuffer,  CL_TRUE, 0,
                           1 * sizeof(float) * set.particleCount,
                           set.particles_mass);
  queue.finish();
}

ParticleSet::ParticleSet(size_t forceCount, uint32_t particleCount, float r, float g, float b)
  : forceCount(forceCount),
    particleCount(particleCount),
    r(r), g(g), b(b)
{
  forces = new float[3 * forceCount];

  //=====================================
  // Fill buffers CPU-side (CL/GL)
  particlesv = new GLfloat[particleCount * 2];
  unsigned j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
  {
    #define DIST_SIZE 1.99f
    particlesv[j++] = ((float)rand() / ((float)RAND_MAX / DIST_SIZE)) - (DIST_SIZE / 2.0f);
    particlesv[j++] = ((float)rand() / ((float)RAND_MAX / DIST_SIZE)) - (DIST_SIZE / 2.0f);
  }

  particlesc = new GLfloat[particleCount * 3];
  j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
  {
    particlesc[j++] = r;
    particlesc[j++] = g;
    particlesc[j++] = b;
  }

  particles_vel = new float[particleCount * 2];
  j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
  {
    particles_vel[j++] = 0.0f;
    particles_vel[j++] = 0.0f;
  }

  particles_mass = new float[particleCount];
  j = 0;
  for (unsigned i = 0; i < particleCount; ++i)
    particles_mass[j++] = 1.0f + (((float)rand() / (float)RAND_MAX) * 2);
}

ParticleSet::~ParticleSet()
{
  glDeleteBuffers(1, &vertexVBO);
  glDeleteBuffers(1, &colorVBO);
  glDeleteVertexArrays(1, &vertexArray);
  //=====================================
  // Free CPU buffers (GL)
  delete[] particlesv;
  delete[] particlesc;
  //=====================================
  // Free CPU buffers (CL)
  delete[] particles_vel;
  delete[] particles_mass;

  delete[] forces;
}

void ParticleSystem::UpdateForcePower(size_t i, float power)
{
  set.forces[i + 2] = power;
}

void ParticleSystem::UpdateForceLocation(size_t i, float x, float y)
{
  set.forces[i    ] = x;
  set.forces[i + 1] = y;
}

void ParticleSystem::FlushCL()
{
  queue.enqueueWriteBuffer(set.forceBuffer, CL_TRUE, 0,
                           3 * sizeof(float) * set.forceCount,
                           set.forces);
  queue.finish();
  kernel.setArg(2, set.forceBuffer);
  queue.finish();
}

void ParticleSystem::Render()
{
  glBindTexture(GL_TEXTURE_2D, shader.sprite);
  kernel.setArg(0, set.clVBO[0]);
  kernel.setArg(1, set.velocityBuffer);
  kernel.setArg(2, set.forceBuffer);
  kernel.setArg(3, set.massBuffer);
  queue.finish();
  queue.enqueueAcquireGLObjects(&set.clVBO);
  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(set.particleCount), cl::NullRange);
  queue.enqueueReleaseGLObjects(&set.clVBO);
  queue.finish();

  glUseProgram(shader.prog);
  glBindVertexArray(set.vertexArray);
  glDrawArrays(GL_POINTS, 0, set.particleCount);
}
