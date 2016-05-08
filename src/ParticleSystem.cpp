#include "frame.hpp"

ParticleSystem::ParticleSystem(ParticleKernel kernel, ParticleShaders shader, size_t forceCount, uint32_t particleCount, float r, float g, float b)
  : forces(0.0f, forceCount * 3), kernel(kernel), shaders(shaders)
{
  this->forceCount = forceCount;
  this->particleCount = particleCount;
  this->r = r;
  this->g = g;
  this->b = b;

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
  glEnableVertexAttribArray(shader.position);
  glVertexAttribPointer(shader.position, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

  glGenBuffers(1, &colorVBO);
  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferData(GL_ARRAY_BUFFER, particleCount * 3 * sizeof(GLfloat), particlesc, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(shader.color);
  glVertexAttribPointer(shader.color, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
  //=====================================
  // Free CPU buffers (GL)
  delete[] particlesv;
  delete[] particlesc;
  //=====================================
  // Create CL buffers
  cl::Context& context = kernel.GetContext();
  cl::CommandQueue& queue = kernel.GetQueue();
  clVBO = cl::BufferGL(context, CL_MEM_READ_WRITE, vertexVBO);

  velocityBuffer = cl::Buffer(context, CL_MEM_READ_WRITE,
                              3 * sizeof(float) * particleCount);
  forceBuffer    = cl::Buffer(context, CL_MEM_READ_WRITE,
                              3 * sizeof(float) * forceCount);
  massBuffer     = cl::Buffer(context, CL_MEM_READ_WRITE,
                              1 * sizeof(float));

  queue.enqueueWriteBuffer(velocityBuffer,  CL_TRUE, 0,
                           2 * sizeof(float) * particleCount,
                           particles_vel);
  queue.enqueueWriteBuffer(forceBuffer, CL_TRUE, 0,
                           3 * sizeof(float),
                           &forces[0]);
  queue.enqueueWriteBuffer(massBuffer,  CL_TRUE, 0,
                           sizeof(float) * particleCount,
                           particles_mass);
  queue.finish();
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
}

void ParticleSystem::UpdateForce(size_t i, float x, float y, float power)
{
  forces[i    ] = x;
  forces[i + 1] = y;
  forces[i + 2] = power;
}

void ParticleSystem::FlushCL()
{
  kernel.GetQueue().enqueueWriteBuffer(forceBuffer, CL_TRUE, 0,
                           forceCount * sizeof(float),
                           &forces[0]);
  kernel.GetQueue().finish();
}

void ParticleSystem::Render()
{
  glBindTexture(GL_TEXTURE_2D, shaders.sprite);
  kernel.RunSystem(*this);

  glUseProgram(shaders.prog);
  glBindVertexArray(vertexArray);
  glDrawArrays(GL_POINTS, 0, particleCount);
}
