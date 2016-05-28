#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP

#include <boost/noncopyable.hpp>
#include <CL/cl.hpp>

#include "ParticleShaders.hpp"
#include "ParticleSet.hpp"

class ParticleSystem : private boost::noncopyable
{
public:
  ParticleSystem(size_t forceCount, uint32_t particleCount, float r, float g, float b, cl::Context& context, cl::Device& device);
  ~ParticleSystem();

  void UpdateForcePower(size_t i, float power);
  void UpdateForceLocation(size_t i, float x, float y);
  void FlushCL();
  void Render();
private:
  cl::CommandQueue queue;
  cl::Context& context;
  cl::Kernel kernel;
  ParticleShader shader;

  uint32_t particleCount;
  size_t forceCount;
  
  cl::Buffer velocityBuffer;
  cl::Buffer massBuffer;
  cl::Buffer forceBuffer;
  std::vector<cl::Memory> clVBO;

  float* forces;
  
  GLuint vertexArray;
  GLuint vertexVBO;
  GLuint colorVBO;
};

#endif
